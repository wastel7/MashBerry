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
#include "qwebui.h"
#include <QTextCodec>

QWebUi::QWebUi(QList<QRecipe*>* pRecipeList, QMySettings *pMySettings, QObject *parent) :
    HttpRequestHandler(parent) , m_settings("MashBerry","MashBerry")
{
    m_pRecipeList = pRecipeList;
    m_pMySettings = pMySettings;
    m_bContinueButtonEnabled = false;
    m_settings.setValue("port", 80);
    m_settings.setValue("path", WEBUI_ROOT_DIR + "de/");
    m_pStaticFileCtrlde = new StaticFileController(&m_settings, this);
    m_settings.setValue("path", WEBUI_ROOT_DIR + "en/");
    m_pStaticFileCtrlen = new StaticFileController(&m_settings, this);

    m_settings.setValue("path", SETTINGS_DIR);
    m_pStaticSettingsFileCtrl = new StaticFileController(&m_settings, this);

    m_pListener = new HttpListener(&m_settings, this, this);
    Changelanguage(m_pMySettings->GetLanguage());
}

void QWebUi::Changelanguage(QString lang)
{
    if(lang == "de")
    {
//        m_LangSettingsError = tr(LANG_DE_SETTINGS_ERR);
    }
    else
    {
//        m_LangSettingsError = tr(LANG_EN_SETTINGS_ERR);
    }
}

void QWebUi::EnableContinueButton(bool state)
{
    m_bContinueButtonEnabled = state;
}

void QWebUi::service(HttpRequest& request, HttpResponse& response)
{
    QString path=request.getPath();
qDebug()<<path<<endl;
    if(path.startsWith("/html/-recipes-.html"))
    {
        serviceRecipes(request, response);
    }
    else if(path.startsWith("/html/status.html"))
    {
        serviceStatus(request, response);
    }
    else if(path.startsWith("/html/-settings-.html"))
    {
        serviceSettings(request, response);
    }
    else if(path.endsWith("templog.csv"))
    {
        emit FlushCsvFile();
        m_pStaticSettingsFileCtrl->service(request, response, true);
    }
    else if(path.endsWith(".xml"))
    {
        m_pStaticSettingsFileCtrl->service(request, response, true);
    }
    else if(path.endsWith(".html") || path == "/" || path.endsWith(".css"))
    {
        serviceHtml(request, response);
    }
    else if(path.startsWith("/assets"))
    {
        if(m_pMySettings->GetLanguage() == "de")
            m_pStaticFileCtrlde->service(request, response);
        else
            m_pStaticFileCtrlen->service(request, response);
    }
}

void QWebUi::ReplaceGlobalTags(Template& t)
{
    t.setVariable("Recipes", tr("Recipes"));
    t.setVariable("Settings", tr("Settings"));
}

void QWebUi::serviceHtml(HttpRequest& request, HttpResponse& response)
{
    QFile tfile;

    if( request.getPath() == "/")
        tfile.setFileName(WEBUI_ROOT_DIR + "index.html");
    else
        tfile.setFileName(WEBUI_ROOT_DIR + request.getPath());

    Template t(tfile, QTextCodec::codecForName("ISO 8859-1"));

    ReplaceGlobalTags(t);

    t.setVariable("Welcome to MashBerry !!", tr("Welcome to MashBerry !!"));

    response.write(t.toLatin1(),true);
}

void QWebUi::serviceStatus(HttpRequest& request, HttpResponse& response)
{
    QFile tfile(WEBUI_TEMPLATE_DIR + "status.html");
    Template t(tfile, QTextCodec::codecForName("ISO 8859-1"));

    ReplaceGlobalTags(t);

    if (request.getParameter("continue")!="")
    {
        emit ContinueClicked();
    }
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
    t.enableWarnings();

    if(m_bContinueButtonEnabled)
        t.setVariable("continueenabled\" value=\"", "\" value=\"");
    else
        t.setVariable("continueenabled\" value=\"", "\" disabled=\"disabled\" value=\"");

    t.setVariable("actrecipetext", tr("Actual recipe"));
    t.setVariable("acttemptext", tr("Actual temperature"));
    t.setVariable("setpointtext", tr("SetPoint"));
    t.setVariable("actpowertext", tr("Actual power"));
    t.setVariable("brewsteptext", tr("Brewing step"));
    t.setVariable("remaintimetext", tr("Remaining time for step"));
    t.setVariable("ContinueBtntext", tr("Continue"));

    t.setVariable("actrecipe", m_aktRecipe);
    t.setVariable("acttemp", m_aktTemp);
    t.setVariable("actsetpoint", m_setTemp);
    t.setVariable("actpower", m_aktPower);
    t.setVariable("actaction", m_aktAction);
    t.setVariable("acttime", m_remTime);
    response.write(t.toLatin1(),true);
}

QWebUi::TFormButtons QWebUi::GetButtonFromRequest(HttpRequest& request)
{
    TFormButtons ret;

    if (request.getParameter("edit")!="")
    {
        ret = FB_EDIT;
    }
    else if(request.getParameter("new")!="")
    {
        ret = FB_NEW;
    }
    else if(request.getParameter("newStep")!="")
    {
        ret = FB_NEW_STEP;
    }
    else if(request.getParameter("saveRecipe")!="")
    {
        ret = FB_SAVE;
    }
    else if(request.getParameter("cancelRecipe")!="")
    {
        ret = FB_CANCEL;
    }
    else if (request.getParameter("delete")!="")
    {
        ret = FB_DELETE;
    }
    else if (request.getParameter("deleteRecipe")!="")
    {
        ret = FB_DELETE_CONFIRM;
    }
    else if (request.getParameter("show")!="")
    {
        ret = FB_SHOW;
    }
    else if (request.getParameter("start")!="")
    {
        ret = FB_START;
    }
    else if (request.getParameter("stop")!="")
    {
        ret = FB_STOP_RECIPE;
    }
    else if (request.getParameter("atune")!="")
    {
        ret = FB_ATUNE;
    }
    else
    {
        ret = FB_NONE;
    }

    return ret;
}

QWebUi::TFormButtons QWebUi::GetSettingsButtonFromRequest(HttpRequest& request)
{
    TFormButtons ret;

    if (request.getParameter("newParamater")!="")
    {
        ret = FB_NEW_PARAM;
    }
    else if (request.getParameter("saveSettings")!="")
    {
        ret = FB_SAVE_SETTINGS;
    }
    else if (request.getParameter("cancelSettings")!="")
    {
        ret = FB_CANCEL_SETTINGS;
    }
    else if (request.getParameter("atune")!="")
    {
        ret = FB_ATUNE;
    }
    else if (request.getParameter("atuneSingle")!="")
    {
        ret = FB_ATUNE_SINGLE;
    }
    else
    {
        ret = FB_NONE;
    }

    return ret;
}

void QWebUi::serviceRecipes(HttpRequest& request, HttpResponse& response)
{
    TFormButtons button = GetButtonFromRequest(request);

    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");

    switch(button)
    {
        case FB_NONE:
            serviceRecipeChoose(request, response);
            break;
        case FB_EDIT:
        case FB_NEW:
        case FB_NEW_STEP:
        case FB_SAVE:
            serviceRecipeEdit(request, response, button);
            break;
        case FB_CANCEL:
            serviceRecipeChoose(request, response);
            break;
        case FB_DELETE:
            serviceRecipeDelete(request, response);
            break;
        case FB_DELETE_CONFIRM:
            serviceRecipeDeleteConfirm(request, response);
            break;
        case FB_SHOW:
            serviceRecipeShow(request, response);
            break;
        case FB_START:
            serviceRecipeStart(request, response);
            break;
        case FB_STOP_RECIPE:
            serviceRecipeStop(request, response);
            break;
        case FB_ATUNE:
            serviceRecipeATune(request, response);
            break;
    }
}

void QWebUi::serviceRecipeChoose(HttpRequest& request, HttpResponse& response)
{
    int i, count;
    QString strStep;
    QFile tfile(WEBUI_TEMPLATE_DIR + "-recipes-.html");
    Template t(tfile, QTextCodec::codecForName("ISO 8859-1"));
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");

    t.enableWarnings();

    ReplaceGlobalTags(t);
    t.setVariable("Choose recipe and select action.", tr("Choose recipe and select action."));
    t.setVariable("Recipe", tr("Recipe"));
    t.setVariable("Show", tr("Show"));
    t.setVariable("Edit", tr("Edit"));
    t.setVariable("New", tr("New"));
    t.setVariable("Delete", tr("Delete"));
    t.setVariable("Start brewing recipe", tr("Start brewing recipe"));
    t.setVariable("Stop recipe", tr("Stop recipe"));

    count = m_pRecipeList->count();
    t.loop("recipe", count);

    for(i=0;i<count;i++)
    {
        strStep = QString("recipe%1.").arg(i);
        t.setVariable(strStep + "num", QString("%1").arg(i));
        t.setVariable(strStep + "name", (*m_pRecipeList)[i]->GetName());
    }

    response.write(t.toLatin1(),true);
}

QRecipe *QWebUi::GetRecipeFromRequest(HttpRequest& request, QRecipe *pOldRec, bool skipInvalid)
{
    int step = 0;
    int stepcnt;
    int validEntries = 0;
    bool valid = true;
    QRecipeEntry *pRecEntry;
    QString strStepname;
    QString strSteptemp;
    QString strSteptime;
    QRecipe *pRec;

    if(pOldRec != NULL)
    {
        pRec = pOldRec;
        pRec->SetName(request.getParameter("Recipename"));
    }
    else
        pRec = new QRecipe(request.getParameter("Recipename"));// this->parent());

//    while((strStepname = request.getParameter(QString("recipestep%1.stepnamename").arg(step).toUtf8())) != "")
//    while(!strStepname.isNull())
    stepcnt = request.getParameter("entrycnt").toInt();
    for(step=0;step<stepcnt;step++)
    {
        strStepname = request.getParameter(QString("recipestep%1.stepnamename").arg(step).toUtf8());
        strSteptemp = request.getParameter(QString("recipestep%1.steptempname").arg(step).toUtf8());
        strSteptime = request.getParameter(QString("recipestep%1.steptimename").arg(step).toUtf8());
        if(skipInvalid)
        {
            valid = true;
            if(strStepname.length() == 0 ||
                strSteptemp.length() == 0 ||
                strSteptime.length() == 0)
            {
                valid = false;
            }
            if(valid)
            {
                strSteptemp.toDouble(&valid);
                if(valid)
                {
                    strSteptime.toInt(&valid);
                }
            }
        }
        if(valid)
        {
            if(pRec->GetEntryCount() <= validEntries)
            {
                if(pOldRec == NULL)
                    pRec->AddEntry(strStepname, strSteptemp.toDouble(), strSteptime.toInt(), NULL);
                else
                    emit AddRecipeEntry(pOldRec, strStepname, strSteptemp.toDouble(), strSteptime.toInt(), NULL);
            }
            else
            {
                pRecEntry = pRec->GetEntry(validEntries);
                pRecEntry->SetName(strStepname);
                pRecEntry->SetTemp(strSteptemp.toDouble());
                pRecEntry->SetTime(strSteptime.toInt());
            }
            validEntries++;
        }
//        step++;
    }
    if(pOldRec)
        emit stripRecipeEntries(pOldRec, validEntries);

    return pRec;
}

int QWebUi::CheckRecipe(HttpRequest& request)
{
    int step;
    int stepcnt;
    bool valid = true;
    QString strStepname;
    QString strSteptemp;
    QString strSteptime;

    stepcnt = request.getParameter("entrycnt").toInt();
    for(step=0;step<stepcnt;step++)
    {
        strStepname = request.getParameter(QString("recipestep%1.stepnamename").arg(step).toUtf8());
        strSteptemp = request.getParameter(QString("recipestep%1.steptempname").arg(step).toUtf8());
        strSteptime = request.getParameter(QString("recipestep%1.steptimename").arg(step).toUtf8());
        valid = true;
        if(strStepname.length() == 0 ||
            strSteptemp.length() == 0 ||
            strSteptime.length() == 0)
        {
            valid = false;
        }
        if(valid)
        {
            strSteptemp.toDouble(&valid);
            if(valid)
            {
                strSteptime.toInt(&valid);
            }
        }
        if(!valid)
        {
            return step;
        }
    }
    return -1;
}

void QWebUi::serviceRecipeEdit(HttpRequest& request, HttpResponse& response, TFormButtons button)
{
    int i;
    int recnum;
    int entrycount;
    int recError = -1;
    QString strStep;
    QRecipe *pRec;
    QRecipe *pOrigRec = NULL;

    QRecipeEntry *pRecEntry;
    QFile tfile(WEBUI_TEMPLATE_DIR + "recipenew.html");
    Template t(tfile, QTextCodec::codecForName("ISO 8859-1"));
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");

    ReplaceGlobalTags(t);
    t.setVariable("New recipe", tr("New recipe"));
    t.setVariable("Edit recipe", tr("Edit recipe"));
    t.setVariable("Recipename", tr("Recipename"));
    t.setVariable("Name step", tr("Name step"));
    t.setVariable("Temperature", tr("Temperature"));
    t.setVariable("Time", tr("Time"));
    t.setVariable("New step", tr("New step"));
    t.setVariable("Save", tr("Save"));
    t.setVariable("Cancel", tr("Cancel"));

    if(button == FB_EDIT)
    {
        recnum = QString(request.getParameter("recipes")).toInt();
        if(recnum >= 0 && recnum < m_pRecipeList->count())
        {
            pRec = (*m_pRecipeList)[recnum];
            pOrigRec = pRec;
        }
        else
        {
            serviceRecipeError(request, response);
            return;
        }
    }
    else if(button != FB_SAVE)
    {
        pRec = GetRecipeFromRequest(request, NULL);
    }

    if(button == FB_NEW_STEP || button == FB_SAVE)
    {
        if(sizeof(QRecipe *) == 4)  // Check for 64Bit systems
            pOrigRec = (QRecipe*)request.getParameter("recipeptr").toUInt();
        else
            pOrigRec = (QRecipe*)request.getParameter("recipeptr").toULongLong();
    }

    if(button == FB_SAVE)
    {
//        recError = CheckRecipe(request);
        if(recError == -1)
        {
            pRec = GetRecipeFromRequest(request, pOrigRec, true);

            emit ModifyRecipe(pRec);
            serviceRecipeChoose(request, response);
        }
        else
        {
            pRec = GetRecipeFromRequest(request, NULL);
        }
    }
    if(button == FB_NEW || button == FB_NEW_STEP || button == FB_EDIT || recError != -1)
    {
         entrycount = pRec->GetEntryCount();
         if(recError != -1)
             entrycount--;

         t.setVariable("Recipenamecont", pRec->GetName());
         t.loop("recipestep", entrycount + 1);
         t.setVariable("entrycnt", QString("%1").arg(entrycount + 1));
         t.setVariable("-1 to wait for user input", tr("-1 to wait for user input"));
         for(i=0;i<entrycount + 1;i++)
         {
             pRecEntry = pRec->GetEntry(i);
             strStep = QString("recipestep%1.").arg(i);
             t.setVariable(strStep + "num", QString("%1").arg(i+1));
             if(pRecEntry != NULL)
             {
                 t.setVariable(strStep + "stepname", pRecEntry->GetName());
                 t.setVariable(strStep + "steptemp", QString("%1").arg(pRecEntry->GetTemp()));
                 t.setVariable(strStep + "steptime",  QString("%1").arg(pRecEntry->GetTime()));
             }
             else
             {
                 t.setVariable(strStep + "stepname", "");
                 t.setVariable(strStep + "steptemp", 0);
                 t.setVariable(strStep + "steptime", 0);
             }
/*             if(recError != -1 && i == recError)
             {
                 t.setVariable(strStep + "error", tr("Error in Recipeentry"));
             }
             else
                 t.setVariable(strStep + "error", "");
*/
             t.setVariable(strStep + "stepnamename", strStep + "stepnamename");
             t.setVariable(strStep + "steptempname", strStep + "steptempname");
             t.setVariable(strStep + "steptimename", strStep + "steptimename");
         }

         if(sizeof(QRecipe *) == 4)  // Check for 64Bit systems
             t.setVariable("recipeptr", QString("%1").arg((uint)pOrigRec));
         else
             t.setVariable("recipeptr", QString("%1").arg((qulonglong)pOrigRec));

         response.write(t.toLatin1(),true);
    }
    if(pOrigRec == NULL)
        delete pRec;
}

void QWebUi::serviceRecipeDelete(HttpRequest& request, HttpResponse& response)
{
    QRecipe *pRec = NULL;
    int recnum;
    QFile tfile(WEBUI_TEMPLATE_DIR + "recipedelete.html");
    Template t(tfile, QTextCodec::codecForName("ISO 8859-1"));
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");

    ReplaceGlobalTags(t);
    t.setVariable("Delete recipe", tr("Delete recipe"));
    t.setVariable("Really delete recipe", tr("Really delete recipe"));
    t.setVariable("Delete", tr("Delete"));
    t.setVariable("Cancel", tr("Cancel"));

    recnum = QString(request.getParameter("recipes")).toInt();
    if(recnum >= 0 && recnum < m_pRecipeList->count())
        pRec = (*m_pRecipeList)[recnum];

    if(pRec != NULL)
    {
        t.setVariable("recipeidx", QString("%1").arg(recnum));
        t.setVariable("Recipename", pRec->GetName());
        response.write(t.toLatin1(),true);
    }
    else
        serviceRecipeError(request, response);
}

void QWebUi::serviceRecipeDeleteConfirm(HttpRequest& request, HttpResponse& response)
{
    QRecipe *pRec = NULL;
    int recnum;
    QFile tfile(WEBUI_TEMPLATE_DIR + "recipedeleted.html");
    Template t(tfile, QTextCodec::codecForName("ISO 8859-1"));
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");

    ReplaceGlobalTags(t);
    t.setVariable("Recipe Deleted", tr("Recipe Deleted"));
    t.setVariable("Recipe has been successfully deleted!!", tr("Recipe has been successfully deleted!!"));

    recnum = QString(request.getParameter("recipeidx")).toInt();
    if(recnum >= 0 && recnum < m_pRecipeList->count())
        pRec = (*m_pRecipeList)[recnum];

    if(pRec != NULL)
    {
        emit DeleteRecipe(recnum);
        t.setVariable("Recipename", pRec->GetName());
        response.write(t.toLatin1(),true);
    }
    else
        serviceRecipeError(request, response);
}

void QWebUi::serviceRecipeShow(HttpRequest& request, HttpResponse& response)
{
    QRecipe *pRec = NULL;
    QRecipeEntry *pRecEntry;
    int recnum;
    int i;
    int entrycount;
    QString strStep;
    QFile tfile(WEBUI_TEMPLATE_DIR + "recipeshow.html");
    Template t(tfile, QTextCodec::codecForName("ISO 8859-1"));
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");

    ReplaceGlobalTags(t);
    t.setVariable("Recipe display", tr("Recipe display"));
    t.setVariable("Name", tr("Name"));

    recnum = QString(request.getParameter("recipes")).toInt();
    if(recnum >= 0 && recnum < m_pRecipeList->count())
        pRec = (*m_pRecipeList)[recnum];

    if(pRec != NULL)
    {
        entrycount = pRec->GetEntryCount();
        t.setVariable("recipename", pRec->GetName());
        t.loop("recipestep", entrycount);
        for(i=0;i<entrycount;i++)
        {
            pRecEntry = pRec->GetEntry(i);
            if(pRecEntry != NULL)
            {
                strStep = QString("recipestep%1.").arg(i);
                t.setVariable(strStep + "num", QString("%1").arg(i+1));
                t.setVariable(strStep + "stepname", pRecEntry->GetName());
                t.setVariable(strStep + "steptemp", QString("%1").arg(pRecEntry->GetTemp()));
                t.setVariable(strStep + "steptime",  QString("%1").arg(pRecEntry->GetTime()));

                t.setVariable(strStep + "Step", tr("Step"));
                t.setVariable(strStep + "Temperature", tr("Temperature"));
                t.setVariable(strStep + "Time", tr("Time"));
            }
        }
        response.write(t.toLatin1(),true);
    }
    else
        serviceRecipeError(request, response);
}

void QWebUi::serviceRecipeStart(HttpRequest& request, HttpResponse& response)
{
    QRecipe *pRec = NULL;
    int recnum;

    recnum = QString(request.getParameter("recipes")).toInt();
    if(recnum >= 0 && recnum < m_pRecipeList->count())
        pRec = (*m_pRecipeList)[recnum];

    if(pRec != NULL)
    {
        emit SetRecipe(pRec);
        m_aktRecipe = pRec->GetName();
        emit SetAktRecipeText(m_aktRecipe);
        serviceStatus(request, response);
    }
    else
        serviceRecipeError(request, response);
}

void QWebUi::serviceRecipeStop(HttpRequest& request, HttpResponse& response)
{
    m_aktRecipe = "";
    emit StopRecipe();
    emit SetAktRecipeText("");
    serviceStatus(request, response);
}

void QWebUi::serviceRecipeATune(HttpRequest& request, HttpResponse& response)
{
    QRecipe *pRec = NULL;
    int recnum;
    bool overwrite = false;

    recnum = QString(request.getParameter("recipes")).toInt();
    if(recnum >= 0 && recnum < m_pRecipeList->count())
        pRec = (*m_pRecipeList)[recnum];

    if(QString(request.getParameter("PIDOverwrite")) == "PIDOverwriteactivated")
        overwrite = true;

    if(pRec != NULL)
    {
        emit ATuneRecipe(pRec, overwrite);
        m_aktRecipe = pRec->GetName();
        emit SetAktRecipeText(m_aktRecipe);
        serviceStatus(request, response);
    }
    else
        serviceRecipeError(request, response);
}

void QWebUi::serviceRecipeError(HttpRequest& request, HttpResponse& response)
{
    QFile tfile(WEBUI_TEMPLATE_DIR + "recipeerror.html");
    Template t(tfile, QTextCodec::codecForName("ISO 8859-1"));
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");

    ReplaceGlobalTags(t);
    t.setVariable("Error", tr("Error"));
    t.setVariable("Recipe not found!!", tr("Recipe not found!!"));

    response.write(t.toLatin1(),true);
}

void QWebUi::LocalizeSettingsTmpl(Template& t)
{
    t.setVariable("Language", tr("Language"));
    t.setVariable("Tempsensor", tr("Tempsensor"));
    t.setVariable("MultiPIDparams", tr("MultiPIDparams"));
    t.setVariable("Recipe", tr("Recipe"));
    t.setVariable("Temperature", tr("Temperature"));
    t.setVariable("parameters", tr("parameters"));
    t.setVariable("No", tr("No"));
    t.setVariable("Yes", tr("Yes"));
    t.setVariable("Autotune PID for hole recipe", tr("Autotune PID for hole recipe"));
    t.setVariable("Overwrite existing PID-parameters", tr("Overwrite existing PID-parameters"));
    t.setVariable("Autotune PID for temperature", tr("Autotune PID for temperature"));
    t.setVariable("New paramater", tr("New paramater"));
    t.setVariable("Save", tr("Save"));
    t.setVariable("Cancel", tr("Cancel"));
}

void QWebUi::serviceSettingsResp(HttpRequest& request, HttpResponse& response, int wrongEntry)
{
    int i;
    int entrycount;
    QString strStep;
    QFile tfile(WEBUI_TEMPLATE_DIR + "-settings-.html");
    Template t(tfile, QTextCodec::codecForName("ISO 8859-1"));

    ReplaceGlobalTags(t);
    LocalizeSettingsTmpl(t);

    if(request.getParameter("tempsensor") == "PT1000")
    {
        t.setVariable("pt1000", "PT1000\" selected=\"selected");
        t.setVariable("dallas", "dallas");
    }
    else
    {
        t.setVariable("pt1000", "PT1000");
        t.setVariable("dallas", "dallas\" selected=\"selected");
    }
    if(request.getParameter("langsel") == "eng")
    {
        t.setVariable("langeng", "eng\" selected=\"selected");
        t.setVariable("langde", "de");
    }
    else
    {
        t.setVariable("langeng", "eng");
        t.setVariable("langde", "de\" selected=\"selected");
    }
    if(request.getParameter("multipids") == "single")
    {
        t.setVariable("singlePIDparams", "single\" selected=\"selected");
        t.setVariable("multiPIDparams", "multi");

        t.setVariable("recipesdisabled", "\" disabled=\"disabled");
        t.setVariable("atuneRecipeenabled\" value=\"", "\" disabled=\"disabled\" value=\"");
        t.setVariable("PIDOverwriteenabled\" value=\"", "\" disabled=\"disabled\" value=\"");
    }
    else
    {
        t.setVariable("singlePIDparams", "single");
        t.setVariable("multiPIDparams", "multi\" selected=\"selected");

        t.setVariable("recipesdisabled", "");
        t.setVariable("atuneRecipeenabled\" value=\"", "\" value=\"");
        t.setVariable("PIDOverwriteenabled\" value=\"", "\" value=\"");
    }

    entrycount = m_pRecipeList->count();
    t.loop("recipe", entrycount);

    for(i=0;i<entrycount;i++)
    {
        strStep = QString("recipe%1.").arg(i);
        t.setVariable(strStep + "num", QString("%1").arg(i));
        t.setVariable(strStep + "name", (*m_pRecipeList)[i]->GetName());
    }

    entrycount = request.getParameter("entrycnt").toInt();
    if(wrongEntry == -1)
       entrycount++;

    t.setVariable("entrycnt", QString("%1").arg(entrycount));
    t.loop("pidstep", entrycount);

    for(i=0;i<entrycount;i++)
    {
        strStep = QString("pidstep%1.").arg(i);
        t.setVariable(strStep + "steptemp", request.getParameter(QString(strStep + "steptempname").toUtf8()));
        t.setVariable(strStep + "stepkp",  request.getParameter(QString(strStep + "stepkpname").toUtf8()));
        t.setVariable(strStep + "stepki",  request.getParameter(QString(strStep + "stepkiname").toUtf8()));
        t.setVariable(strStep + "stepkd",  request.getParameter(QString(strStep + "stepkdname").toUtf8()));
        if(wrongEntry == i)
            t.setVariable(strStep + "error",  tr("Error in PID-parameter"));
        else
            t.setVariable(strStep + "error", "");

        t.setVariable(strStep + "steptempname", strStep + "steptempname");
        t.setVariable(strStep + "stepkpname", strStep + "stepkpname");
        t.setVariable(strStep + "stepkiname", strStep + "stepkiname");
        t.setVariable(strStep + "stepkdname", strStep + "stepkdname");
    }
    t.setVariable("entrycnt", QString("%1").arg(entrycount));

    response.write(t.toLatin1(),true);
}

int QWebUi::CheckSettings(HttpRequest& request, bool write)
{
    QString strStep;
    QString strSteptemp;
    QString strStepkp;
    QString strStepki;
    QString strStepkd;
    int i;
    int diffidx = 0;
    bool valid;
    bool empty;
    int entrycount;

    if(write)
    {
        emit SetLanguage(request.getParameter("langsel"), false);
        emit SetSensorName(request.getParameter("tempsensor"), false);
        emit SetSinglePIDparam(request.getParameter("multipids") == "single", false);
        Changelanguage(m_pMySettings->GetLanguage());
        emit ChangeLanguage(m_pMySettings->GetLanguage());
    }

    entrycount = request.getParameter("entrycnt").toInt() + 1;

    for(i=0;i<entrycount;i++)
    {
        strStep = QString("pidstep%1.").arg(i);
        strSteptemp = request.getParameter(QString(strStep + "steptempname").toUtf8());
        strStepkp = request.getParameter(QString(strStep + "stepkpname").toUtf8());
        strStepki = request.getParameter(QString(strStep + "stepkiname").toUtf8());
        strStepkd = request.getParameter(QString(strStep + "stepkdname").toUtf8());

        valid = true;
        empty = false;
        if(strSteptemp.length() == 0 &&
            strStepkp.length() == 0 &&
            strStepki.length() == 0 &&
            strStepkd.length() == 0)
        {
            valid = false;
            empty = true;
        }
        if(valid)
        {
            strSteptemp.toInt(&valid);
            if(valid)
            {
                strStepkp.toDouble(&valid);
                if(valid)
                {
                    strStepki.toDouble(&valid);
                    if(valid)
                    {
                        strStepkd.toDouble(&valid);
                    }
                }
            }
        }

    //    add index based deleting fo params

        if(valid)
        {
            if(write)
                emit SetPIDParam(i-diffidx, strSteptemp.toInt(), strStepkp.toDouble(), strStepki.toDouble(), strStepkd.toDouble(), false);
        }
        else if(!write && !empty)
        {
            return i;
        }
        else if(write && empty)
        {
            emit RemovePIDParam(i-diffidx, false);
            diffidx++;
        }
    }
    if(write)
        emit SaveSettings();
    return -1;
}

void QWebUi::serviceSettings(HttpRequest& request, HttpResponse& response)
{
    int i;
    int temp;
    bool valid;
    int entry;
    int entrycount;
    QString strStep;
    QPIDParameters *pParams;
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
    TFormButtons button = GetSettingsButtonFromRequest(request);

    if(button == FB_NONE)
    {
        QFile tfile(WEBUI_TEMPLATE_DIR + "-settings-.html");
        Template t(tfile, QTextCodec::codecForName("ISO 8859-1"));

        ReplaceGlobalTags(t);
        LocalizeSettingsTmpl(t);

        if(m_pMySettings->GetSensorName() == "PT1000")
        {
            t.setVariable("pt1000", "PT1000\" selected=\"selected");
            t.setVariable("dallas", "dallas");
        }
        else
        {
            t.setVariable("pt1000", "PT1000");
            t.setVariable("dallas", "dallas\" selected=\"selected");
        }
        if(m_pMySettings->GetLanguage() == "eng")
        {
            t.setVariable("langeng", "eng\" selected=\"selected");
            t.setVariable("langde", "de");
        }
        else
        {
            t.setVariable("langeng", "eng");
            t.setVariable("langde", "de\" selected=\"selected");
        }
        if(m_pMySettings->IsSinglePIDparam())
        {
            t.setVariable("singlePIDparams", "single\" selected=\"selected");
            t.setVariable("multiPIDparams", "multi");

            t.setVariable("recipesdisabled", "\" disabled=\"disabled");
            t.setVariable("atuneRecipeenabled\" value=\"", "\" disabled=\"disabled\" value=\"");
            t.setVariable("PIDOverwriteenabled\" value=\"", "\" disabled=\"disabled\" value=\"");
        }
        else
        {
            t.setVariable("singlePIDparams", "single");
            t.setVariable("multiPIDparams", "multi\" selected=\"selected");

            t.setVariable("recipesdisabled", "");
            t.setVariable("atuneRecipeenabled\" value=\"", "\" value=\"");
            t.setVariable("PIDOverwriteenabled\" value=\"", "\" value=\"");
        }

        entrycount = m_pRecipeList->count();
        t.loop("recipe", entrycount);

        for(i=0;i<entrycount;i++)
        {
            strStep = QString("recipe%1.").arg(i);
            t.setVariable(strStep + "num", QString("%1").arg(i));
            t.setVariable(strStep + "name", (*m_pRecipeList)[i]->GetName());
        }

        entrycount = m_pMySettings->GetPIDParamCount();
        t.setVariable("entrycnt", QString("%1").arg(entrycount));
        t.loop("pidstep", entrycount);

        for(i=0;i<entrycount;i++)
        {
            pParams = m_pMySettings->GetPIDParam(i);
            strStep = QString("pidstep%1.").arg(i);
            if(pParams != NULL)
            {
                t.setVariable(strStep + "steptemp", QString("%1").arg(pParams->GetTemp()));
                t.setVariable(strStep + "stepkp",  QString("%1").arg(pParams->GetKp(), 0, 'f', 8));
                t.setVariable(strStep + "stepki",  QString("%1").arg(pParams->GetKi(), 0, 'f', 8));
                t.setVariable(strStep + "stepkd",  QString("%1").arg(pParams->GetKd(), 0, 'f', 8));
            }
            else
            {
                t.setVariable(strStep + "steptemp", "");
                t.setVariable(strStep + "stepkp",  "");
                t.setVariable(strStep + "stepki",  "");
                t.setVariable(strStep + "stepkd",  "");
            }
            t.setVariable(strStep + "error", "");
            t.setVariable(strStep + "steptempname", strStep + "steptempname");
            t.setVariable(strStep + "stepkpname", strStep + "stepkpname");
            t.setVariable(strStep + "stepkiname", strStep + "stepkiname");
            t.setVariable(strStep + "stepkdname", strStep + "stepkdname");
        }
        response.write(t.toLatin1(),true);
    }
    else if(button == FB_NEW_PARAM)
    {
        serviceSettingsResp(request, response);
    }
    else if(button == FB_ATUNE)
    {
        serviceRecipes(request, response);
    }
    else if(button == FB_ATUNE_SINGLE)
    {
        strStep = request.getParameter("pidtunetemp");
        if(strStep.length() != 0)
        {
            temp = strStep.toInt(&valid);
            if(valid)
            {
                m_aktRecipe = "";
                emit StopRecipe();
                emit SetAktRecipeText("");

                emit ATuneSingle(temp);
                serviceStatus(request, response);
            }
        }
    }
    else if(button == FB_SAVE_SETTINGS)
    {
        entry = CheckSettings(request, false);
        if(entry == -1)
        {
            CheckSettings(request, true);
            QFile tfile(WEBUI_TEMPLATE_DIR + "settingssaved.html");
            Template t(tfile, QTextCodec::codecForName("ISO 8859-1"));
            ReplaceGlobalTags(t);
            t.setVariable("Settings Saved", tr("Settings Saved"));
            t.setVariable("Settings have been successfully saved!!", tr("Settings have been successfully saved!!"));
            response.write(t.toLatin1(),true);
        }
        else
        {
            serviceSettingsResp(request, response, entry);
        }
    }
    else if(button == FB_CANCEL_SETTINGS)
    {
        serviceStatus(request, response);
    }
}

void QWebUi::SetAktRecipe(QRecipe* pRec)
{
    if(pRec != NULL)
        m_aktRecipe = pRec->GetName();
    else
        m_aktRecipe = "";
}

void QWebUi::SetAktTemp(double temp)
{
    char degree[] = " C";
    char val[20];

    degree[0] = 0xB0;

    sprintf(val,"%.2f %s", temp, degree);
    m_aktTemp = val;
}

void QWebUi::SetSetTemp(double temp)
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

    m_setTemp = val;
}

void QWebUi::SetAktPower(int power)
{
    m_aktPower = QString("%1 %").arg(power);
}

void QWebUi::SetAktAction(QString action)
{
    m_aktAction = action;
}

void QWebUi::SetRemainTime(QString t)
{
    m_remTime = t;
}
