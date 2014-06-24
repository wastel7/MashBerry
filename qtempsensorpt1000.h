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
#ifndef QTEMPSENSORPT1000_H
#define QTEMPSENSORPT1000_H

#include <QObject>
#include <QMutex>
#include <QFile>
#include "qtempsensor.h"

class QTempSensorPT1000 : public QTempSensor
{
    Q_OBJECT
public:
    explicit QTempSensorPT1000(QObject *parent = 0);
    virtual int Open();
    virtual double ReadTemp();
    virtual int GetSampleTime();

signals:
    
public slots:

protected:
    void run();
    double ReadTempI2C();

private:
    int m_fd;
    bool m_bEndThread;
    QMutex m_CtrlMutex;
    double m_tempAvg;
};

#endif // QTEMPSENSOR_H
