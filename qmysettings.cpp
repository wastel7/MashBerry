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

#include "qmysettings.h"
#include <QProcess>

QMySettings::QMySettings(QObject *parent) :
    QObject(parent)
{
    LoadSettings();
}

int QMySettings::GetPIDParamCount()
{
    return m_PIDParams.count();
}

QPIDParameters *QMySettings::GetPIDParam(int idx)
{
    if(idx < m_PIDParams.count())
        return m_PIDParams[idx];
    return NULL;
}

int QMySettings::diff(int a, int b)
{
    return qAbs(a-b);
}

QPIDParameters *QMySettings::GetNearestPIDParam(int temp)
{
    int i,cnt;
    QPIDParameters *pParams;
    QPIDParameters *pNearestParams = NULL;

    if(m_singlePIDparam)
    {
        if(m_PIDParams.count() > 0)
            return m_PIDParams[0];
        else
            return NULL;
    }

    cnt = m_PIDParams.count();
    for(i = 0; i<cnt; i++)
    {
        if(pNearestParams != NULL)
        {
            pParams = m_PIDParams[i];
            if(diff(pParams->GetTemp(), temp) < diff(pNearestParams->GetTemp(), temp))
            {
                pNearestParams = pParams;
            }
        }
        else
            pNearestParams = m_PIDParams[i];
    }
    return pNearestParams;
}

bool QMySettings::PIDTempExists(int temp)
{
    int i,cnt;

    cnt = m_PIDParams.count();
    for(i = 0; i<cnt; i++)
    {
        if(m_PIDParams[i]->GetTemp() == temp)
            return true;
    }
    return false;
}

void QMySettings::AddPIDParam(int temp, double kp, double ki, double kd, bool saveSettings)
{
    int i,cnt;
    bool found = false;
    QPIDParameters *pParams;

    if(m_singlePIDparam)
    {
        if(m_PIDParams.count() > 0)
        {
            m_PIDParams[0]->SetParams(temp, kp, ki, kd);
            found = true;
        }
    }
    else
    {
        cnt = m_PIDParams.count();
        for(i = 0; i<cnt; i++)
        {
            pParams = m_PIDParams[i];
            if(pParams->GetTemp() == temp)
            {
                pParams->SetParams(temp, kp, ki, kd);
                found = true;
            }
        }

    }
    if(!found)
    {
        m_PIDParams.append(new QPIDParameters(temp, kp, ki, kd, this));
    }
    if(saveSettings)
        SaveSettings();
}

void QMySettings::SetPIDParam(int idx, int temp, double kp, double ki, double kd, bool saveSettings)
{
    if(m_singlePIDparam)
    {
        if(m_PIDParams.count() > 0)
            m_PIDParams[0]->SetParams(temp, kp, ki, kd);
        else
            AddPIDParam(temp, kp ,ki , kd, saveSettings);
    }
    else
    {
        if(idx < m_PIDParams.count())
        {
            m_PIDParams[idx]->SetParams(temp, kp, ki, kd);
        }
        else
        {
            AddPIDParam(temp, kp ,ki , kd, saveSettings);
        }
    }
    if(saveSettings)
        SaveSettings();
}

void QMySettings::RemovePIDParam(int idx, bool saveSettings)
{
    if(idx < m_PIDParams.count())
    {
        m_PIDParams.removeAt(idx);
        if(saveSettings)
            SaveSettings();
    }
}

void QMySettings::SetSensorName(QString name, bool saveSettings)
{
    m_Sensor = name;
    if(saveSettings)
        SaveSettings();
}

QString QMySettings::GetSensorName()
{
    return m_Sensor;
}

QString QMySettings::GetLanguage()
{
    return m_lang;
}

void QMySettings::SetLanguage(QString lang, bool saveSettings)
{
    m_lang = lang;
    if(saveSettings)
        SaveSettings();
}

void QMySettings::SetSinglePIDparam(bool single, bool saveSettings)
{
    m_singlePIDparam = single;
    if(saveSettings)
        SaveSettings();
}

bool QMySettings::IsSinglePIDparam()
{
    return m_singlePIDparam;
}

void QMySettings::LoadSettings()
{
    XMLElement *akt;
    XMLElement *set;
    QPIDParameters *pParams = NULL;
    XMLDocument* doc = new XMLDocument();

    if(doc->LoadFile(SETTINGS_FILE) != XML_ERROR_FILE_NOT_FOUND)
    {
        set = doc->FirstChildElement("Settings");
        if(set != NULL)
        {
            akt = set->FirstChildElement("PID");
            if(akt != NULL)
                akt = akt->FirstChildElement("PIDParams");
            while(akt)
            {
                pParams = GetPidParams(akt);
                if(pParams != NULL)
                {
                    m_PIDParams.append(pParams);
                }
                akt = akt->NextSiblingElement("PIDParams");
            }

            akt = set->FirstChildElement("Sensor");
            if(akt != NULL)
            {
                m_Sensor = akt->GetText();
            }
            else
                m_Sensor = "PT1000";

            akt = set->FirstChildElement("Language");
            if(akt != NULL)
            {
                m_lang = akt->GetText();
            }
            else
                m_lang = "de";

            akt = set->FirstChildElement("SinglePIDparam");
            if(akt != NULL)
            {
                m_singlePIDparam = atoi(akt->GetText());
            }
            else
                m_singlePIDparam = true;
        }
    }
    delete doc;
}

QPIDParameters *QMySettings::GetPidParams(XMLElement *parent)
{
    XMLElement *elem;
    int temp;
    double kp, ki, kd;
    QPIDParameters *ret = NULL;

    elem = parent->FirstChildElement("Temp");
    if(elem != NULL)
    {
        temp = atoi(elem->GetText());
        elem = parent->FirstChildElement("Kp");
        if(elem != NULL)
        {
            kp = atof(elem->GetText());
            elem = parent->FirstChildElement("Ki");
            if(elem != NULL)
            {
                ki = atof(elem->GetText());
                elem = parent->FirstChildElement("Kd");
                if(elem != NULL)
                {
                    kd = atof(elem->GetText());
                    ret = new QPIDParameters(temp, kp, ki, kd, this);
                }
            }
        }
    }
    return ret;
}

void QMySettings::SaveSettings()
{
    int i;
    XMLElement *elem;
    XMLDocument* doc = new XMLDocument();

    XMLNode* element = doc->InsertEndChild( doc->NewElement( "Settings" ) );

    elem = doc->NewElement("Sensor");
    elem->InsertFirstChild(doc->NewText(m_Sensor.toAscii()));
    element->InsertEndChild(elem);

    elem = doc->NewElement("Language");
    elem->InsertFirstChild(doc->NewText(m_lang.toAscii()));
    element->InsertEndChild(elem);

    elem = doc->NewElement("SinglePIDparam");
    elem->InsertFirstChild(doc->NewText(QString("%1").arg(m_singlePIDparam).toAscii()));
    element->InsertEndChild(elem);

    element = element->InsertEndChild( doc->NewElement( "PID" ) );
    for(i=0;i<m_PIDParams.count();i++)
        m_PIDParams[i]->serialize(element, doc);
    doc->SaveFile(SETTINGS_FILE);
#ifndef Q_OS_WIN
    QProcess::execute("/bin/sync");
#endif
    delete doc;
}

