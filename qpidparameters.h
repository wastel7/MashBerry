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
#ifndef QPIDPARAMETERS_H
#define QPIDPARAMETERS_H

#include <QObject>
#include <QXmlStreamWriter>
#include "tinyxml2.h"

using namespace tinyxml2;

class QPIDParameters : public QObject
{
    Q_OBJECT
public:
    explicit QPIDParameters(int temp, double kp, double ki, double kd, QObject *parent = 0);
    void serialize(QXmlStreamWriter &writer);
    void serialize(XMLNode *parent, XMLDocument* doc);

    void SetParams(int temp, double kp, double ki, double kd);

    int GetTemp();
    double GetKp();
    double GetKi();
    double GetKd();

signals:
    
public slots:
    
private:
    int m_temp;
    double m_kp;
    double m_ki;
    double m_kd;
};

#endif // QPIDPARAMETERS_H
