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
#ifndef QGPIO_H
#define QGPIO_H

#include <QObject>
#include <QFile>

class QGpio : public QObject
{
    Q_OBJECT
public:
    explicit QGpio(QObject *parent = 0);
    bool Open(int gpioNumber, bool dirOut);
    void Set(int value);
    int Get();

signals:
    
public slots:

private:
    QFile *m_pValueFile;
};

#endif // QGPIO_H
