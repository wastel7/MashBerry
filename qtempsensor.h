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
#ifndef QTEMPSENSOR_H
#define QTEMPSENSOR_H

#include <QThread>
#include <QFile>

class QTempSensor : public QThread
{
    Q_OBJECT
public:
    explicit QTempSensor(QObject *parent = 0) : QThread(parent) {};
    virtual int Open() = 0;
    virtual double ReadTemp() = 0;
    virtual int GetSampleTime() = 0;

signals:
    
public slots:

};

#endif // QTEMPSENSOR_H
