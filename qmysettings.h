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
#ifndef QMYSETTINGS_H
#define QMYSETTINGS_H

#include <QObject>
#include <QList>
#include <QString>
#include "qpidparameters.h"

#ifndef Q_OS_WIN
#define WEBUI_ROOT_DIR QString("/home/pi/.mashberry/templates/")
#define WEBUI_TEMPLATE_DIR QString("/home/pi/.mashberry/templates/html/")
#define SETTINGS_DIR "/home/pi/.mashberry/"
#define SETTINGS_FILE "/home/pi/.mashberry/settings.xml"
#define RECIPE_FILE "/home/pi/.mashberry/recipe.xml"
#define CSV_FILE "/home/pi/.mashberry/templog.csv"
#else
#define WEBUI_ROOT_DIR QString("templates/")
#define WEBUI_TEMPLATE_DIR QString("templates/html/")
#define SETTINGS_DIR ""
#define SETTINGS_FILE "settings.xml"
#define RECIPE_FILE "recipe.xml"
#define CSV_FILE "templog.csv"
#endif

class QMySettings : public QObject
{
    Q_OBJECT
public:
    explicit QMySettings(QObject *parent = 0);
    int GetPIDParamCount();
    QPIDParameters *GetPIDParam(int idx);
    QPIDParameters *GetNearestPIDParam(int temp);
    bool PIDTempExists(int temp);
    QString GetLanguage();
    QString GetSensorName();
    bool IsSinglePIDparam();

signals:
    
public slots:
    void AddPIDParam(int temp, double kp, double ki, double kd, bool saveSettings);
    void SetPIDParam(int idx, int temp, double kp, double ki, double kd, bool saveSettings);
    void RemovePIDParam(int idx, bool saveSettings);
    void SetSensorName(QString name, bool saveSettings);
    void SetLanguage(QString lang, bool saveSettings);
    void SetSinglePIDparam(bool single, bool saveSettings);
    void SaveSettings();

protected:
    void LoadSettings();
    QPIDParameters *GetPidParams(XMLElement *parent);
    int diff(int a, int b);

private:
    QList<QPIDParameters*> m_PIDParams;
    QString m_Sensor;
    QString m_lang;
    bool m_singlePIDparam;
};

#endif // QMYSETTINGS_H
