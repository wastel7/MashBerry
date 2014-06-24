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
#include "qrecipeentry.h"

QRecipeEntry::QRecipeEntry(QString name, double temp, int time, QPIDParameters *pParams, QObject *parent) :
    QObject(parent)
{
    m_name = name;
    m_temp = temp;
    m_time = time;
    m_pPIDParams = pParams;
}

QString QRecipeEntry::GetName()
{
    return m_name;
}

double QRecipeEntry::GetTemp()
{
    return m_temp;
}

int QRecipeEntry::GetTime()
{
    return m_time;
}

QPIDParameters *QRecipeEntry::GetPidParams()
{
    return m_pPIDParams;
}

void QRecipeEntry::SetPidParams(QPIDParameters *pParams)
{
    m_pPIDParams = pParams;
}

void QRecipeEntry::SetName(QString name)
{
    m_name = name;
}

void QRecipeEntry::SetTemp(double temp)
{
    m_temp = temp;
}

void QRecipeEntry::SetTime(int time)
{
    m_time = time;
}

void QRecipeEntry::serialize(QXmlStreamWriter &writer)
{
    writer.writeStartElement("Step");
    writer.writeTextElement("Name", m_name);
    writer.writeTextElement("Temp", QString("%1").arg(m_temp));
    writer.writeTextElement("Time", QString("%1").arg(m_time));
/*    if(m_pPIDParams)
        m_pPIDParams->serialize(writer);
*/    writer.writeEndElement();
}

void QRecipeEntry::serialize(XMLNode *parent, XMLDocument* doc)
{
    XMLElement *elem;
    parent = parent->InsertEndChild(doc->NewElement("Step"));
    if(parent)
    {
        elem = doc->NewElement("Name");
        elem->InsertFirstChild(doc->NewText(m_name.toAscii()));
        parent->InsertEndChild(elem);

        elem = doc->NewElement("Temp");
        elem->InsertFirstChild(doc->NewText(QString("%1").arg(m_temp).toAscii()));
        parent->InsertEndChild(elem);

        elem = doc->NewElement("Time");
        elem->InsertFirstChild(doc->NewText(QString("%1").arg(m_time).toAscii()));
        parent->InsertEndChild(elem);
/*        if(m_pPIDParams)
            m_pPIDParams->serialize(parent, doc);
*/    }
}
