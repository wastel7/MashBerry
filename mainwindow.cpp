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
#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifndef Q_OS_WIN
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#endif

/* Translation:
 *
 C:\Qt\4.8.4\bin\lupdate.exe MashBerry.pro
 C:\Qt\4.8.4\bin\lrelease.exe MashBerry.pro
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_pSettings = new QMySettings(this);

    if(m_pSettings->GetLanguage() == "de")
    {
#ifndef Q_OS_WIN
        if(m_translator.load("gui_de", SETTINGS_DIR))
#else
        if(m_translator.load("gui_de"))
#endif
            qDebug()<<"translation loaded"<<endl;
        else
            qDebug()<<"failed loading translation"<<endl;
        qApp->installTranslator(&m_translator);
    }

//    ui = new Ui::MainWindow;
    ui->setupUi(this);
#ifndef Q_OS_WIN
//    showFullScreen();
    //QApplication::setStyle("plastique");
#endif
    m_pRecipeManager = new QRecipeManager(this);
    FillRecipeCBs();

    m_pPIDController = new QPidController(m_pSettings, this);
    m_pWebUi = new QWebUi(m_pRecipeManager->GetRecipeList(), m_pSettings, this);

    connect(m_pPIDController, SIGNAL(SetAktTemp(double)), this, SLOT(SetAktTemp(double)));
    connect(m_pPIDController, SIGNAL(SetSetTemp(double)), this, SLOT(SetSetTemp(double)));
    connect(m_pPIDController, SIGNAL(SetAktPower(int)), this, SLOT(SetAktPower(int)));
    connect(m_pPIDController, SIGNAL(SetAktAction(QString)), this, SLOT(SetAktAction(QString)));
    connect(m_pPIDController, SIGNAL(SetRemainTime(QString)), this, SLOT(SetRemainTime(QString)));
    connect(m_pPIDController, SIGNAL(EnableContinueButton(bool)), this, SLOT(EnableContinueButton(bool)));
    connect(m_pPIDController, SIGNAL(EnableContinueButton(bool)), m_pWebUi, SLOT(EnableContinueButton(bool)));

    connect(m_pPIDController, SIGNAL(SetAktTemp(double)), m_pWebUi, SLOT(SetAktTemp(double)));
    connect(m_pPIDController, SIGNAL(SetSetTemp(double)), m_pWebUi, SLOT(SetSetTemp(double)));
    connect(m_pPIDController, SIGNAL(SetAktPower(int)), m_pWebUi, SLOT(SetAktPower(int)));
    connect(m_pPIDController, SIGNAL(SetAktAction(QString)), m_pWebUi, SLOT(SetAktAction(QString)));
    connect(m_pPIDController, SIGNAL(SetRemainTime(QString)), m_pWebUi, SLOT(SetRemainTime(QString)));

    connect(m_pWebUi, SIGNAL(ModifyRecipe(QRecipe*)), m_pRecipeManager, SLOT(ModifyRecipe(QRecipe*)), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(stripRecipeEntries(QRecipe*, int)), m_pRecipeManager, SLOT(stripRecipeEntries(QRecipe*, int)), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(AddRecipeEntry(QRecipe*, QString, double, int, QPIDParameters*)), m_pRecipeManager,  SLOT(AddRecipeEntry(QRecipe*, QString, double, int, QPIDParameters*)), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(DeleteRecipe(int)), m_pRecipeManager, SLOT(DeleteRecipe(int)), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(SetRecipe(QRecipe*)), m_pPIDController, SLOT(SetRecipe(QRecipe*)), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(ATuneRecipe(QRecipe*, bool)), m_pPIDController, SLOT(ATuneRecipe(QRecipe*, bool)), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(ATuneSingle(int)), m_pPIDController, SLOT(ATuneSingle(int)), Qt::BlockingQueuedConnection);

    connect(m_pWebUi, SIGNAL(SetSensorName(QString,bool)), m_pSettings, SLOT(SetSensorName(QString,bool)), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(SetLanguage(QString,bool)), m_pSettings, SLOT(SetLanguage(QString,bool)), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(SetPIDParam(int,int,double,double,double,bool)), m_pSettings, SLOT(SetPIDParam(int,int,double,double,double,bool)), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(RemovePIDParam(int,bool)), m_pSettings, SLOT(RemovePIDParam(int,bool)), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(SetSinglePIDparam(bool,bool)), m_pSettings, SLOT(SetSinglePIDparam(bool,bool)), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(SaveSettings()), m_pSettings, SLOT(SaveSettings()), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(ContinueClicked()), m_pPIDController, SLOT(ContinueClicked()));
    connect(m_pWebUi, SIGNAL(FlushCsvFile()), m_pPIDController, SLOT(FlushCsvFile()), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(StopRecipe()), m_pPIDController, SLOT(StopRecipe()), Qt::BlockingQueuedConnection);

    connect(m_pWebUi, SIGNAL(SetAktRecipeText(QString)), this, SLOT(SetAktRecipeText(QString)), Qt::BlockingQueuedConnection);
    connect(m_pWebUi, SIGNAL(ChangeLanguage(QString)), this, SLOT(ChangeLanguage(QString)), Qt::BlockingQueuedConnection);

    connect(this, SIGNAL(ATuneSingle(int)), m_pPIDController, SLOT(ATuneSingle(int)));
    connect(this, SIGNAL(ATuneRecipe(QRecipe*, bool)), m_pPIDController, SLOT(ATuneRecipe(QRecipe*, bool)));
    connect(this, SIGNAL(SetRecipe(QRecipe*)), m_pPIDController, SLOT(SetRecipe(QRecipe*)));
    connect(this, SIGNAL(ContinueClicked()), m_pPIDController, SLOT(ContinueClicked()));
    connect(this, SIGNAL(StopRecipe()), m_pPIDController, SLOT(StopRecipe()));

    connect(this, SIGNAL(SetAktRecipe(QRecipe*)), m_pWebUi, SLOT(SetAktRecipe(QRecipe*)));

    connect(ui->autotuneButton, SIGNAL(clicked()), this, SLOT(AutoTuneStart()));
    connect(ui->holeautotuneButton, SIGNAL(clicked()), this, SLOT(AutoTuneRecipeStart()));

    InitLirc();

    m_pPIDController->start();

/*    QPIDParameters *pParams = m_pSettings->GetNearestPIDParam(63);
    qDebug()<<pParams->GetTemp()<<" "<<pParams->GetKp()<<" "<<pParams->GetKi()<<" "<<pParams->GetKd()<<endl;
*/


/*    QRecipe *pRec;

//    pRec = LoadRecipe(1);
    pRec = m_pRecipeManager->GetRecipe(1);
    if(pRec != NULL)
    {
//        emit ATuneRecipe(pRec);
        emit SetRecipe(pRec);
    }
*/

/*    QFile xo("rec-gen.xml");
    xo.open(QIODevice::WriteOnly);

    QXmlStreamWriter writer(&xo);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("Recipes");
    pRec->serialize(writer);
    writer.writeEndElement();
    writer.writeEndDocument();*/
}

MainWindow::~MainWindow()
{
    m_pPIDController->EndThread();
    m_pPIDController->wait(10000);
    delete ui;
#ifndef Q_OS_WIN
    lirc_freeconfig(m_pLircconfig);
    lirc_deinit();
#endif
}

void MainWindow::FillRecipeCBs()
{
    int i = 0;
    QRecipe *pRec;

    while((pRec = m_pRecipeManager->GetRecipe(i++)) != NULL)
    {
        ui->recipeCB->addItem(pRec->GetName());
        ui->recipePID_CB->addItem(pRec->GetName());
    }
}

void MainWindow::InitLirc()
{
#ifndef Q_OS_WIN
    int flags;
    m_socket = lirc_init("MashBerry",1);
    if(m_socket == -1)
        exit(EXIT_FAILURE);

    if(lirc_readconfig(NULL, &m_pLircconfig, NULL) != 0)
        exit(EXIT_FAILURE);

    fcntl(m_socket, F_SETOWN, getpid());
    flags = fcntl(m_socket, F_GETFL, 0);
    if(flags!=-1)
    {
        fcntl(m_socket, F_SETFL, flags|O_NONBLOCK);
    }
    m_pSN = new QSocketNotifier(m_socket, QSocketNotifier::Read, 0);
    connect(m_pSN, SIGNAL(activated(int)), this, SLOT(LircEvt()));
#endif
}

void MainWindow::LircEvt(void)
{
#ifndef Q_OS_WIN
    int ret;
    char *code;
    char *c;
    int lirccode;

    while((ret=lirc_nextcode(&code))==0 && code!=NULL)
    {
        if(code==NULL) continue;
    dbgout("got %s\n",code);
        while((ret=lirc_code2char(m_pLircconfig,code,&c))==0 && c!=NULL)
        {
            dbgout("decoded: %s\n", c);
            lirccode = atoi(c);
            switch(lirccode)
            {
                case LIRC_LEFT:
                    SwitchTab(false);
                    break;
                case LIRC_RIGHT:
                    SwitchTab(true);
                    break;
                case LIRC_UP:
                    SwitchRecipe(true);
                    break;
                case LIRC_DOWN:
                    SwitchRecipe(false);
                    break;
                case LIRC_VOL_PLUS:
                    ChangeATuneTemp(true);
                    break;
                case LIRC_VOL_MINUS:
                    ChangeATuneTemp(false);
                    break;
                case LIRC_CHAN_PLUS:
                    break;
                case LIRC_CHAN_MINUS:
                    break;
                case LIRC_STOP:
                    LIRCStop();
                    break;
                case LIRC_PLAY:
                    break;
                case LIRC_OK:
                    LircOKButton();
                    break;
                case LIRC_0:
                    break;
                case LIRC_1:
                    break;
                case LIRC_2:
                    break;
                case LIRC_3:
                    break;
                case LIRC_4:
                    break;
                case LIRC_5:
                    break;
                case LIRC_6:
                    break;
                case LIRC_7:
                    break;
                case LIRC_8:
                    break;
                case LIRC_9:
                    break;
                case LIRC_BACK:
                    break;
                case LIRC_MENU:
                    AutoTuneStart();
                    break;
               case LIRC_POWER:
                    break;
            }
        }
    }
#endif
}

void MainWindow::SwitchTab(bool right)
{
    int idx;

    idx = ui->tabWidget->currentIndex();
    if(right)
    {
        idx++;
        if(idx >= ui->tabWidget->count())
            idx = 0;
    }
    else
    {
        idx--;
        if(idx < 0)
            idx = ui->tabWidget->count() - 1;
    }
    ui->tabWidget->setCurrentIndex(idx);
}

void MainWindow::SwitchRecipe(bool up)
{
    QComboBox *pCb;
    int idx;

    idx = ui->tabWidget->currentIndex();

    if(idx == 1)
    {
        pCb = ui->recipeCB;
    }
    else if(idx == 2)
    {
        pCb = ui->recipePID_CB;
    }
    else
        return;

    idx = pCb->currentIndex();
    if(up)
    {
        idx--;
        if(idx < 0)
            idx = pCb->count() - 1;
    }
    else
    {
        idx++;
        if(idx >= pCb->count())
            idx = 0;
    }
    pCb->setCurrentIndex(idx);
}

void MainWindow::LircOKButton()
{
    int idx;
    idx = ui->tabWidget->currentIndex();

    if(idx == 0)
    {
        if(ui->continueButton->isEnabled())
            emit ContinueClicked();
    }
    else if(idx == 1)
    {
        on_loadrecipeButton_clicked();
    }
    else
    {
        AutoTuneRecipeStart();
    }
}

void MainWindow::AutoTuneStart()
{
    on_stopButton_clicked();
    emit ATuneSingle(ui->tuneTempSpinBox->value());
}

void MainWindow::AutoTuneRecipeStart()
{
    QRecipe *pRec;

    pRec = m_pRecipeManager->GetRecipe(ui->recipePID_CB->currentIndex());
    if(pRec != NULL)
    {
        emit ATuneRecipe(pRec, true);
        emit SetAktRecipe(pRec);
        SetAktRecipeText(pRec->GetName());
    }
}

void MainWindow::SetAktTemp(double temp)
{
    char degree[] = " C";
    char val[20];

    degree[0] = 0xB0;

    sprintf(val,"%.2f %s", temp, degree);

    ui->AktTempTextEdit->setPlainText(val);
}

void MainWindow::SetSetTemp(double temp)
{
    char degree[] = " C";
    char val[20];

    if(temp > 0)
    {
        degree[0] = 0xB0;

        sprintf(val,"%.2f %s", temp, degree);
    }
    else
        val[0] = 0;

    ui->SetTempTextEdit->setPlainText(val);
}

void MainWindow::SetAktPower(int power)
{
    ui->AktPowerTextEdit->setPlainText(QString("%1 %").arg(power));
}

void MainWindow::SetAktAction(QString action)
{
    ui->AktActionTextEdit->setPlainText(action);
}

void MainWindow::SetRemainTime(QString t)
{
//qDebug() <<  "SetRemainTime:" << t << endl;
    ui->AktTimeTextEdit->clear();
    ui->AktTimeTextEdit->setPlainText(t);
}

void MainWindow::EnableContinueButton(bool state)
{
    ui->continueButton->setEnabled(state);
}

void MainWindow::on_loadrecipeButton_clicked()
{
    QRecipe *pRec;

    pRec = m_pRecipeManager->GetRecipe(ui->recipeCB->currentIndex());
    if(pRec != NULL)
    {
        emit SetRecipe(pRec);
        emit SetAktRecipe(pRec);
        SetAktRecipeText(pRec->GetName());
    }
}

void MainWindow::on_continueButton_clicked()
{
    emit ContinueClicked();
}

void MainWindow::SaveRecipes()
{
    m_pRecipeManager->SaveRecipes();
}

void MainWindow::LIRCStop()
{
    on_stopButton_clicked();
}

void MainWindow::ChangeATuneTemp(bool up)
{
    if(up)
        ui->tuneTempSpinBox->setValue(ui->tuneTempSpinBox->value() + 1);
    else
        ui->tuneTempSpinBox->setValue(ui->tuneTempSpinBox->value() - 1);
}

void MainWindow::on_stopButton_clicked()
{
    emit StopRecipe();
    emit SetRecipe(NULL);
    emit SetAktRecipe(NULL);
    SetAktRecipeText("");
}

void MainWindow::SetAktRecipeText(QString rec)
{
    ui->AktRecipeTextEdit->setPlainText(rec);
}

void MainWindow::ChangeLanguage(QString lang)
{
    if(lang == "de")
    {
    #ifndef Q_OS_WIN
        if(m_translator.load("gui_de", SETTINGS_DIR))
    #else
        if(m_translator.load("gui_de"))
    #endif
            qDebug()<<"translation loaded"<<endl;
        else
            qDebug()<<"failed loading translation"<<endl;
        qApp->installTranslator(&m_translator);
    }
    else
        qApp->removeTranslator(&m_translator);

    ui->retranslateUi(this);
}
