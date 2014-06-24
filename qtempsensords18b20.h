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
#ifndef QTEMPSENSORDS18B20_H
#define QTEMPSENSORDS18B20_H

#include <QObject>
#include <QFile>
#include "qtempsensor.h"

class QTempSensorDS18B20 : public QTempSensor
{
    Q_OBJECT
public:
    explicit QTempSensorDS18B20(QObject *parent = 0);
    ~QTempSensorDS18B20();
    virtual int Open();
    virtual double ReadTemp();
    virtual int GetSampleTime();

signals:
    
public slots:

private:
    FILE *m_fd;
    char *m_fname;
    double m_lastTemp;
};

#endif // QTEMPSENSOR_H
