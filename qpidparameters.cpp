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
#include "qpidparameters.h"

QPIDParameters::QPIDParameters(int temp, double kp, double ki, double kd, QObject *parent) :
    QObject(parent)
{
    m_temp = temp;
    m_kp = kp;
    m_ki = ki;
    m_kd = kd;
}

void QPIDParameters::SetParams(int temp, double kp, double ki, double kd)
{
    m_temp = temp;
    m_kp = kp;
    m_ki = ki;
    m_kd = kd;
}

double QPIDParameters::GetKp()
{
    return m_kp;
}

double QPIDParameters::GetKi()
{
    return m_ki;
}

double QPIDParameters::GetKd()
{
    return m_kd;
}

int QPIDParameters::GetTemp()
{
    return m_temp;
}

void QPIDParameters::serialize(QXmlStreamWriter &writer)
{
    writer.writeStartElement("PIDParams");
    writer.writeTextElement("Temp", QString("%1").arg(m_temp));
    writer.writeTextElement("Kp", QString("%1").arg(m_kp));
    writer.writeTextElement("Ki", QString("%1").arg(m_ki));
    writer.writeTextElement("Kd", QString("%1").arg(m_kd));
    writer.writeEndElement();
}

void QPIDParameters::serialize(XMLNode *parent, XMLDocument* doc)
{
    XMLElement *elem;
    parent = parent->InsertEndChild(doc->NewElement("PIDParams"));
    if(parent)
    {
        elem = doc->NewElement("Temp");
        elem->InsertFirstChild(doc->NewText(QString("%1").arg(m_temp).toAscii()));
        parent->InsertEndChild(elem);

        elem = doc->NewElement("Kp");
        elem->InsertFirstChild(doc->NewText(QString("%1").arg(m_kp).toAscii()));
        parent->InsertEndChild(elem);

        elem = doc->NewElement("Ki");
        elem->InsertFirstChild(doc->NewText(QString("%1").arg(m_ki).toAscii()));
        parent->InsertEndChild(elem);

        elem = doc->NewElement("Kd");
        elem->InsertFirstChild(doc->NewText(QString("%1").arg(m_kd).toAscii()));
        parent->InsertEndChild(elem);
    }
}
