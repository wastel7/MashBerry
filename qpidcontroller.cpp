/*
 *
 * MashBerry Homebrewing-controller
 *
 * Copyright (C) 2013 Sebastian Duell
 * <mail@sebastian-duell.de>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 3. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include <QtDebug>
#include "qpidcontroller.h"
#include "qtempsensords18b20.h"
#include "qtempsensorpt1000.h"

//#define PID_CYCLE_TIME 5

/* 25L Einkocher:
kp:55.30430000 //45.304336

ki:0.044852

kd:15440.40000000 //11440.409587
*/


/* 5L Topf:
kp:18.00870000

ki:0.010955

kd:8503.73000000
*/

QPidController::QPidController(QMySettings *pSettings, QObject *parent) :
    QThread(parent)
{
    m_pSettings = pSettings;
    m_AktSetpoint = -1;
    m_AktTemp = 20;
    m_AktPower = 0;
    m_kp = 45.304336; //17.4616;
    m_ki = 0.044852;//0.074597;
    m_kd = 11440.409587; //1021.85;
    m_bEndThread = false;
    m_bAutoTuning = false;
    m_bOverwriteATuning = false;
    m_bRecipeTuning = false;
    m_bAutotuneComplete = false;
    m_bRecipeMode = false;
    m_bRecipeTempReached = false;
    m_bLogTemp = true;
    m_pRecipe = NULL;
    m_pAktRecipeStep = NULL;
    m_pLogFile = NULL;
    m_pATuneTime = new QTime();
    m_pRecipeTime = new QTime();
    *m_pATuneTime = QTime::currentTime();
    *m_pRecipeTime = QTime::currentTime();
    m_pPID = new PID( m_kp, m_ki, m_kd, DIRECT);
    m_pATune = new PID_ATune();
    InitTempSensor();
    m_beeperGpio.Open(7, true);
}

QPidController::~QPidController()
{
}

void QPidController::InitTempSensor()
{
    if(m_pSettings->GetSensorName() == "PT1000")
    {
        m_pTempSensor = new QTempSensorPT1000(this);
    }
    else
    {
        m_pTempSensor = new QTempSensorDS18B20(this);
    }
    m_pTempSensor->Open();
    m_pPID->SetSampleTime(m_pTempSensor->GetSampleTime() * 1000);
}

void QPidController::Beep(int msec)
{
    m_beeperGpio.Set(1);
    msleep(msec);
    m_beeperGpio.Set(0);
}

bool QPidController::ActivateRecipeStep(int idx)
{
    bool ret = false;
    QPIDParameters *pParams;

    m_pAktRecipeStep = m_pRecipe->GetEntry(idx);
    if(m_pAktRecipeStep != NULL)
    {
        m_AktSetpoint = m_pAktRecipeStep->GetTemp();
        m_stepTime = m_pAktRecipeStep->GetTime();
        pParams = m_pSettings->GetNearestPIDParam((int)m_AktSetpoint);
//        pParams = m_pAktRecipeStep->GetPidParams();
        if(pParams != NULL)
        {
            m_kp = pParams->GetKp();
            m_ki = pParams->GetKi();
            m_kd = pParams->GetKd();
            m_pPID->SetTunings(m_kp, m_ki, m_kd);
qDebug() << "Setting pid params:" << m_kp << m_ki << m_kd << "\n";
        }
        emit SetAktAction(m_pAktRecipeStep->GetName());
        emit EnableContinueButton(false);
        m_bRecipeTempReached = false;
        ret = true;
        m_pPID->SetSetpoint(m_AktSetpoint);
        m_pATune->SetSetpoint(m_AktSetpoint);
qDebug() << "Activating Recipe Step:" << m_pAktRecipeStep->GetName() << "at" << QTime::currentTime();
        if(idx > 0)
            Beep(500);
    }
    return ret;
}

void QPidController::SetRecipe(QRecipe *pRecipe)
{
    m_CtrlMutex.lock();
    m_pRecipe = pRecipe;
    m_recipeStep = 0;
    if(pRecipe != NULL)
    {
        m_bRecipeMode = true;
        m_bAutoTuning = false;
        ActivateRecipeStep(m_recipeStep++);
    }
    else
    {
        m_bRecipeMode = false;
        m_bAutoTuning = false;
        emit SetRemainTime("");
        emit SetAktAction("");
        emit EnableContinueButton(false);
    }
    m_CtrlMutex.unlock();
}

void QPidController::ContinueClicked()
{
dbgout("ContinueClicked\n");
    m_CtrlMutex.lock();
    if(m_bRecipeMode)
    {
        if(m_stepTime == -1)
            m_stepTime = 0;
    }
    m_CtrlMutex.unlock();
}

void QPidController::StartAtune(bool single, double Setpoint)
{
    m_CtrlMutex.lock();
    m_pATune->Cancel();
    m_pATune->SetNoiseBand(0.05);
    if(Setpoint != 0)
        m_pATune->SetSetpoint(Setpoint);
    m_pATune->SetOutputStep(50);
    m_pATune->SetControlType(1);
    m_pATuneTime->start();

    if(single)
    {
        m_bRecipeMode = false;
        m_AktSetpoint = Setpoint;
    }
    m_bAutoTuning = true;
    m_AktPower = 50;
    m_CtrlMutex.unlock();
    emit SetRemainTime(tr("PID-Autotuning"));
}

void QPidController::ATuneSingle(int Setpoint)
{
    StartAtune(true, Setpoint);
    emit SetAktAction("");
dbgout("Started Autotune\n");
}

void QPidController::SkipTunedRecipeSteps()
{
    while(m_pSettings->PIDTempExists((int)m_AktSetpoint))
    {
        if(!ActivateRecipeStep(m_recipeStep++))
        {
            m_bRecipeMode = false;
            break;
        }
    }
}

void QPidController::ATuneRecipe(QRecipe *pRecipe, bool overwrite)
{
    m_bOverwriteATuning = overwrite;

    SetRecipe(pRecipe);
    if(!overwrite)
    {
        SkipTunedRecipeSteps();
    }
    if(m_bRecipeMode)
    {
        StartAtune(false);
    }
    else
    {
        m_bAutoTuning = false;
        m_bAutotuneComplete = true;
        emit SetRemainTime(tr("Autotuning complete"));
        emit SetAktAction("");
    }
    emit EnableContinueButton(false);
}

void QPidController::DoAutotune()
{
    if(m_pATune->Runtime(m_pATuneTime->elapsed(), m_AktTemp, &m_AktPower))
    {
        m_kp = m_pATune->GetKp();
        m_ki = m_pATune->GetKi();
        m_kd = m_pATune->GetKd();
        m_pPID->SetTunings(m_kp, m_ki, m_kd);

        dbgout("Autotuning complete for sp:%f\n", m_AktSetpoint);
        dbgout("kp:%f\n",m_kp);
        dbgout("ki:%f\n",m_ki);
        dbgout("kd:%f\n",m_kd);

//        if(m_pAktRecipeStep)
        {
            m_pSettings->AddPIDParam((int)m_AktSetpoint, m_kp, m_ki, m_kd, true);
        }

        if(m_bRecipeMode)
        {
            if(ActivateRecipeStep(m_recipeStep))
            {
                m_recipeStep++;
                if(!m_bOverwriteATuning)
                    SkipTunedRecipeSteps();
            }
            else
                m_bRecipeMode = false;
        }
        if(!m_bRecipeMode)
        {
            m_bAutoTuning = false;
            m_bAutotuneComplete = true;
            emit SetRemainTime(tr("Autotuning complete"));
            emit SetAktAction("");
            Endbeep();
        }
    }
}

void QPidController::StopRecipe()
{
    m_CtrlMutex.lock();

    m_bAutoTuning = false;
    m_bAutotuneComplete = true;
    m_bRecipeMode = false;
    m_AktSetpoint = -1;
    m_pATune->Cancel();
//    emit SetAktAction(tr("Aborted"));
    emit SetAktAction("");
    emit SetRemainTime("");
    emit EnableContinueButton(false);

    m_CtrlMutex.unlock();
}

void QPidController::Endbeep()
{
    Beep(1500);
    msleep(500);
    Beep(1500);
    msleep(500);
    Beep(1500);
    msleep(500);
    Beep(1500);
}

void QPidController::run()
{
    char power[8];
    int lastPower = -1;
    double tmin = 100;
    double tmax = 0;
    bool lastCycleSkipped = true;
    bool SkipCycles;
    int cyclewaittime;

    QFile ssr("/sys/devices/platform/ssr_plug.0/ssr_1");

    if(!ssr.open(QIODevice::WriteOnly))
        dbgout("Couldn't open ssr device!!\n");

    if(m_pTempSensor->GetSampleTime() > 5)
    {
        cyclewaittime = m_pTempSensor->GetSampleTime() / 2;
        SkipCycles = true;
    }
    else
    {
        cyclewaittime = m_pTempSensor->GetSampleTime();
        SkipCycles = false;
    }

    m_pPID->SetSetpoint(m_AktSetpoint);

//    ATuneSingle(60.0);

    while(!m_bEndThread)
    {
        m_CtrlMutex.lock();
        m_AktTemp = m_pTempSensor->ReadTemp();

        if(m_AktTemp > tmax)
            tmax = m_AktTemp;
        if(m_AktTemp < tmin)
            tmin = m_AktTemp;

        if(m_bAutoTuning)
        {
            DoAutotune();
        }
        else
        {
            if(m_bRecipeMode)
            {
                if(m_bRecipeTempReached)
                {
                    if(m_pRecipeTime->elapsed()/60000 >= m_stepTime && m_stepTime > -1)
                    {
                        if(ActivateRecipeStep(m_recipeStep))
                        {
                            m_recipeStep++;
                        }
                        else
                        {
                            m_bRecipeMode = false;
                            emit SetAktAction(tr("Finished"));
                            emit SetRemainTime("");
                            m_AktSetpoint = -1;
                            Endbeep();
                        }
                    }
                }
                else
                {
                    if(m_AktTemp >= m_AktSetpoint)
                    {
                        m_bRecipeTempReached = true;
                        m_pRecipeTime->start();
                        if(m_stepTime == -1)
                        {
                            emit EnableContinueButton(true);
                            Beep(3000);
                        }
                        else
                            emit EnableContinueButton(false);
                    }
                }
            }
            if(m_AktSetpoint > 0)
            {
                if(SkipCycles)
                {
                    if(lastCycleSkipped)
                    {
                        m_AktPower = m_pPID->Compute(m_AktTemp);
                        lastCycleSkipped = false;
                    }
                    else
                        lastCycleSkipped = true;
                }
                else
                {
                    m_AktPower = m_pPID->Compute(m_AktTemp);
                }
            }
            else
                m_AktPower = 0;
        }

        if(m_AktPower <= 100 && m_AktPower != lastPower)
        {
            sprintf(power, "%d\n", m_AktPower);
            ssr.write(power, strlen(power) + 1);
            ssr.flush();
            lastPower = m_AktPower;
        }

        if(!m_bAutoTuning)
        {
            if(m_bRecipeMode)
            {
                if(m_bRecipeTempReached)
                {
                    if(m_stepTime == -1)
                        emit SetRemainTime(QString(tr("Press continue")));
                    else
                        emit SetRemainTime(QString("%1 min").arg(m_stepTime - m_pRecipeTime->elapsed()/60000));
                }
                else
                {
                    emit SetRemainTime(QString(tr("Waiting for Setpoint...")));
                }
            }
//            else
//                emit SetRemainTime(QString("min:%1 max:%2").arg(tmin).arg(tmax));
        }

        emit SetAktTemp(m_AktTemp);
        emit SetSetTemp(m_AktSetpoint);
        emit SetAktPower(m_AktPower);
        m_CtrlMutex.unlock();

        if(m_bLogTemp)
            LogTemp();

        sleep(cyclewaittime);
    }
    if(m_pLogFile != NULL)
        m_pLogFile->close();
}

void QPidController::EndThread()
{
    m_bEndThread = true;
}

void QPidController::LogTemp()
{
    static int cycleCnt = 0;
    static double lastSetPoint = 0;

    if(m_pLogFile == NULL)
    {
        m_pLogFile = new QFile(CSV_FILE); //"/home/pi/templog.csv");

        if(!m_pLogFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            dbgout("Couldn't open logfile!!\n");

        m_pLogStream = new QTextStream(m_pLogFile);
    }
    if(lastSetPoint != m_AktSetpoint)
    {
        if(lastSetPoint != 0)
            (*m_pLogStream) << "\n";
        (*m_pLogStream) << QString("%1").arg(m_AktSetpoint) << "\n";
        m_pLogFile->flush();
        lastSetPoint = m_AktSetpoint;
    }
    (*m_pLogStream) << QString("%1;").arg(m_AktTemp);
    cycleCnt++;
    if(cycleCnt == 12)
    {
        cycleCnt = 0;
        m_pLogStream->flush();
        m_pLogFile->flush();
    }
//    m_pLogFile->write(QString("%1;").arg(m_AktTemp));
}

void QPidController::FlushCsvFile()
{
    if(m_pLogFile != NULL)
    {
        m_pLogFile->flush();
    }
}


/*
 *Autotuning complete for sp:50.000000

kp:7.545130

ki:0.027185

kd:523.526363

Autotuning complete for sp:60.000000

kp:5.456746

ki:0.025084

kd:296.758284

Autotuning complete for sp:65.000000

kp:8.793604

ki:0.046892

kd:412.266153

Autotuning complete for sp:70.000000

kp:7.736146

ki:0.053344

kd:280.479759

Autotuning complete for sp:76.000000

kp:6.866916

ki:0.056048

kd:210.332770

*/
