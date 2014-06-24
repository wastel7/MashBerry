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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSocketNotifier>
#include <QTranslator>
#include "qpidcontroller.h"
#include "qrecipemanager.h"
#include "qwebui.h"
#include "qmysettings.h"

#ifndef Q_OS_WIN
#include <lirc/lirc_client.h>
#endif

typedef enum
{
    LIRC_LEFT = 1,
    LIRC_RIGHT = 2,
    LIRC_UP = 3,
    LIRC_DOWN = 4,
    LIRC_VOL_PLUS = 5,
    LIRC_VOL_MINUS = 6,
    LIRC_CHAN_PLUS = 7,
    LIRC_CHAN_MINUS = 8,
    LIRC_STOP = 9,
    LIRC_PLAY = 10,
    LIRC_OK = 11,
    LIRC_0 = 12,
    LIRC_1 = 13,
    LIRC_2 = 14,
    LIRC_3 = 15,
    LIRC_4 = 16,
    LIRC_5 = 17,
    LIRC_6 = 18,
    LIRC_7 = 19,
    LIRC_8 = 20,
    LIRC_9 = 21,
    LIRC_BACK = 22,
    LIRC_MENU = 23,
    LIRC_POWER = 24,
}TLircCodes;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void ATuneSingle(int Setpoint);
    void ATuneRecipe(QRecipe *pRecipe, bool overwrite);
    void SetRecipe(QRecipe *pRecipe);
    void SetAktRecipe(QRecipe *pRecipe);
    void ContinueClicked();
    void StopRecipe();

private slots:
    void SetAktTemp(double temp);
    void SetSetTemp(double temp);
    void SetAktPower(int power);
    void SetAktAction(QString action);
    void SetAktRecipeText(QString rec);
    void SetRemainTime(QString t);
    void EnableContinueButton(bool state);
    void SaveRecipes();

    void AutoTuneStart();
    void AutoTuneRecipeStart();
    void LircEvt(void);

    void on_loadrecipeButton_clicked();

    void on_continueButton_clicked();

    void on_stopButton_clicked();

    void ChangeLanguage(QString lang);

protected:
    void InitLirc(void);
    void SwitchRecipe(bool up);
    void SwitchTab(bool right);
    void LircOKButton();
    void FillRecipeCBs();
    void LIRCStop();
    void ChangeATuneTemp(bool up);
    void StartATuneSingle();

private:
    Ui::MainWindow *ui;
    int m_socket;
    struct lirc_config *m_pLircconfig;
    QSocketNotifier *m_pSN;
    QPidController *m_pPIDController;
    QRecipeManager *m_pRecipeManager;
    QMySettings *m_pSettings;
    QWebUi *m_pWebUi;
    QTranslator m_translator;
};

#endif // MAINWINDOW_H
