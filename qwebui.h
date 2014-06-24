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
#ifndef QWEBUI_H
#define QWEBUI_H

#include "QtWebApp/lib/bfHttpServer/src/httplistener.h"
#include "QtWebApp/lib/bfHttpServer/src/staticfilecontroller.h"
#include "QtWebApp/lib/bfTemplateEngine/src/template.h"
#include "qrecipe.h"
#include "qmysettings.h"

#define LANG_DE_SETTINGS_ERR "Fehler in PID-parameter"
#define LANG_EN_SETTINGS_ERR "Error in PID-parameter"

class QWebUi : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit QWebUi(QList<QRecipe*>* pRecipeList, QMySettings *pMySettings, QObject *parent = 0);

    virtual void service(HttpRequest& request, HttpResponse& response);

signals:
    void ModifyRecipe(QRecipe *pRec);
    void stripRecipeEntries(QRecipe *pRec, int count);
    void AddRecipeEntry(QRecipe *pRec, QString name, double temp, int time, QPIDParameters *pParams);
    void DeleteRecipe(int idx);
    void SetRecipe(QRecipe *pRecipe);
    void SetAktRecipeText(QString rec);
    void ATuneRecipe(QRecipe *pRecipe, bool overwrite);
    void ATuneSingle(int Setpoint);

    void SetSensorName(QString name, bool saveSettings);
    void SetPIDParam(int idx, int temp, double kp, double ki, double kd, bool saveSettings);
    void RemovePIDParam(int idx, bool saveSettings);
    void SetLanguage(QString lang, bool saveSettings);
    void SetSinglePIDparam(bool single, bool saveSettings);

    void ContinueClicked();
    void FlushCsvFile();
    void StopRecipe();
    void SaveSettings();
    void ChangeLanguage(QString lang);

public slots:
    void SetAktRecipe(QRecipe* pRec);
    void SetAktTemp(double temp);
    void SetSetTemp(double temp);
    void SetAktPower(int power);
    void SetAktAction(QString action);
    void SetRemainTime(QString t);
    void EnableContinueButton(bool state);

protected:
    typedef enum
    {
        FB_NONE,
        FB_EDIT,
        FB_NEW,
        FB_NEW_STEP,
        FB_SAVE,
        FB_CANCEL,
        FB_DELETE,
        FB_DELETE_CONFIRM,
        FB_SHOW,
        FB_START,        
        FB_STOP_RECIPE,
        FB_ATUNE,
        FB_ATUNE_SINGLE,
        FB_NEW_PARAM,
        FB_SAVE_SETTINGS,
        FB_CANCEL_SETTINGS,
    }TFormButtons;

    void serviceHtml(HttpRequest& request, HttpResponse& response);
    void serviceStatus(HttpRequest& request, HttpResponse& response);
    void serviceRecipes(HttpRequest& request, HttpResponse& response);
    void serviceRecipeChoose(HttpRequest& request, HttpResponse& response);
    void serviceSettings(HttpRequest& request, HttpResponse& response);
    void serviceSettingsResp(HttpRequest& request, HttpResponse& response, int wrongEntry = -1);

    void serviceRecipeEdit(HttpRequest& request, HttpResponse& response, TFormButtons button);
    void serviceRecipeDelete(HttpRequest& request, HttpResponse& response);
    void serviceRecipeDeleteConfirm(HttpRequest& request, HttpResponse& response);
    void serviceRecipeShow(HttpRequest& request, HttpResponse& response);
    void serviceRecipeStart(HttpRequest& request, HttpResponse& response);
    void serviceRecipeStop(HttpRequest& request, HttpResponse& response);
    void serviceRecipeError(HttpRequest& request, HttpResponse& response);
    void serviceRecipeATune(HttpRequest& request, HttpResponse& response);

    void ReplaceGlobalTags(Template& t);
    void LocalizeSettingsTmpl(Template& t);

    QRecipe *GetRecipeFromRequest(HttpRequest& request, QRecipe *pRec, bool skipInvalid=false);
    TFormButtons GetButtonFromRequest(HttpRequest& request);
    TFormButtons GetSettingsButtonFromRequest(HttpRequest& request);
    int CheckSettings(HttpRequest& request, bool write);
    void Changelanguage(QString lang);
    int CheckRecipe(HttpRequest& request);

private:
    QSettings m_settings;
    HttpListener* m_pListener;
    StaticFileController *m_pStaticFileCtrlen;
    StaticFileController *m_pStaticFileCtrlde;
    StaticFileController *m_pStaticSettingsFileCtrl;
    QList<QRecipe*>* m_pRecipeList;
    QMySettings *m_pMySettings;

    QString m_aktRecipe;
    QString m_aktTemp;
    QString m_setTemp;
    QString m_aktPower;
    QString m_aktAction;
    QString m_remTime;

    bool m_bContinueButtonEnabled;
};

#endif // QWEBUI_H
