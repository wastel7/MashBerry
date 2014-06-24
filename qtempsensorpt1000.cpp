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
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#include <QTime>
#include <QtDebug>

#include "qtempsensorpt1000.h"

#ifndef Q_OS_WIN
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#endif

#define SENSOR_I2C_ADDR 0x78

QTempSensorPT1000::QTempSensorPT1000(QObject *parent) :
    QTempSensor(parent)
{
    m_bEndThread = false;
    m_tempAvg = 20;
    start();
}

int QTempSensorPT1000::Open()
{
#ifndef Q_OS_WIN
    m_fd = open("/dev/i2c-0", O_RDWR);
    if (m_fd < 0)
    {
        printf("couldn't open /dev/i2c-0\n");
        return 1;
    }

    if (ioctl(m_fd, I2C_SLAVE, SENSOR_I2C_ADDR) < 0)
    {
        printf("couldn't set i2c slave addr\n");
        close(m_fd);
        return 1;
    }
#endif
    return 0;
}

void QTempSensorPT1000::run()
{
    double temp;
    double tempall = 0;
    int count = 0;
    QTime tm = QTime::currentTime();

    tm.start();
    while(!m_bEndThread)
    {
//        m_CtrlMutex.lock();
        temp = ReadTempI2C();
        tempall += temp;
        count++;

        if(count == 50)
        {
            m_tempAvg = tempall / count;
            tempall = 0;
            count = 0;
qDebug() << "cycle time:" <<  tm.elapsed() <<  "temp:" << m_tempAvg << endl;
            tm.start();
        }
//        m_CtrlMutex.unlock();

        msleep(100);
    }
}

double QTempSensorPT1000::ReadTempI2C()
{
#ifndef Q_OS_WIN
    double temp = 0;
    unsigned int adval;
    unsigned char buf[2];

    if (read(m_fd, buf, 2) < 1)
    {
        printf("i2c-read failed %u\n",errno);
    }
    else
    {
        adval = (buf[1]&0xF8) | buf[0]<<8;
        temp = (((double)adval)/128) - 32.0;
//        printf("temp:%f   val:%X", temp,val);
    }
    return temp;
#else
    return 20.0;
#endif
}

double QTempSensorPT1000::ReadTemp()
{
//    m_CtrlMutex.lock();
    return m_tempAvg;
//    m_CtrlMutex.unlock();
}

int QTempSensorPT1000::GetSampleTime()
{
    return 10;//5;
}
