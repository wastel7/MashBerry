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
#include "qgpio.h"

QGpio::QGpio(QObject *parent) :
    QObject(parent)
{
    m_pValueFile = NULL;
}

bool QGpio::Open(int gpioNumber, bool dirOut)
{
    QFile xport("/sys/class/gpio/export");
    QString strDir = QString("/sys/class/gpio/gpio%1/").arg(gpioNumber);

    if(xport.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        xport.write(QString("%1").arg(gpioNumber).toAscii());
        xport.close();
        QFile direction(strDir + "direction");
        if(direction.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            if(dirOut)
            {
                direction.write("out");
            }
            else
            {
                direction.write("in");
            }
            direction.close();
            m_pValueFile = new QFile(strDir + "value", this);
            if(m_pValueFile->open(QIODevice::ReadWrite | QIODevice::Text))
            {
                return true;
            }
            delete m_pValueFile;
            m_pValueFile = NULL;
        }
    }
    return false;
}

void QGpio::Set(int value)
{
    if(m_pValueFile != NULL)
    {
        m_pValueFile->seek(0);
        if(value)
            m_pValueFile->write("1");
        else
            m_pValueFile->write("0");
        m_pValueFile->flush();
    }
}

int QGpio::Get()
{
    char ch;
    int ret = -1;

    if(m_pValueFile != NULL)
    {
        m_pValueFile->seek(0);
        if(m_pValueFile->getChar(&ch))
        {
            ret = ch - '0';
        }
    }
    return ret;
}
