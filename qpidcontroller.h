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
#ifndef QPIDCONTROLLER_H
#define QPIDCONTROLLER_H

#include <QThread>
#include <QTimer>
#include <QTime>
#include <QMutex>
#include <QTextStream>
#include "PID/PID_v1.h"
#include "PID/PID_AutoTune_v0.h"
#include "qtempsensor.h"
#include "qpidparameters.h"
#include "qrecipe.h"
#include "qmysettings.h"
#include "qgpio.h"

#define dbgout qDebug
//#define dbgout printf

class QPidController : public QThread
{
    Q_OBJECT
public:
    explicit QPidController(QMySettings *pSettings, QObject *parent = 0);
    ~QPidController();
    void EndThread();

signals:
    void SetAktTemp(double temp);
    void SetSetTemp(double temp);
    void SetAktPower(int power);
    void SetAktAction(QString action);
    void SetRemainTime(QString t);
    void EnableContinueButton(bool state);

private slots:

public slots:
    void ATuneSingle(int Setpoint);
    void ATuneRecipe(QRecipe *pRecipe, bool overwrite);
    void SetRecipe(QRecipe *pRecipe);
    void ContinueClicked();
    void FlushCsvFile();
    void StopRecipe();

protected:
    void run();
    void InitTempSensor();
    bool ActivateRecipeStep(int idx);
    void StartAtune(bool single, double Setpoint = 0);
    void LogTemp();
    void DoAutotune();
    void SkipTunedRecipeSteps();
    void Beep(int msec);

    void Endbeep();

private:
    bool m_bEndThread;
    bool m_bAutoTuning;
    bool m_bOverwriteATuning;
    bool m_bRecipeTuning;
    bool m_bAutotuneComplete;
    bool m_bRecipeMode;
    bool m_bRecipeTempReached;
    bool m_bLogTemp;

    QTime       *m_pATuneTime;
    QTime       *m_pRecipeTime;
    QMutex      m_CtrlMutex;
    QFile       *m_pLogFile;
    QTextStream *m_pLogStream;

    PID         *m_pPID;
    PID_ATune   *m_pATune;   
    QTempSensor *m_pTempSensor;
    QRecipe     *m_pRecipe;
    QRecipeEntry *m_pAktRecipeStep;
    QGpio       m_beeperGpio;

    QMySettings *m_pSettings;

    double       m_AktTemp;
    int          m_AktPower;
    double       m_AktSetpoint;
    int          m_stepTime;
    int          m_recipeStep;

    double       m_kp;
    double       m_ki;
    double       m_kd;
};

#endif // QPIDCONTROLLER_H
