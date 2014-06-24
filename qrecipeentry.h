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
#ifndef QRECIPEENTRY_H
#define QRECIPEENTRY_H

#include <QObject>
#include <QString>
#include "qpidparameters.h"

class QRecipeEntry : public QObject
{
    Q_OBJECT
public:
    explicit QRecipeEntry(QString name, double temp, int time, QPIDParameters *pParams, QObject *parent = 0);
    void serialize(QXmlStreamWriter &writer);
    void serialize(XMLNode *parent, XMLDocument* doc);

    QString GetName();
    double GetTemp();
    int GetTime();
    QPIDParameters *GetPidParams();
    void SetPidParams(QPIDParameters *pParams);
    void SetName(QString name);
    void SetTemp(double temp);
    void SetTime(int time);

signals:
    
public slots:

private:
    double m_temp;
    int m_time;
    QString m_name;
    QPIDParameters *m_pPIDParams;
};

#endif // QRECIPEENTRY_H
