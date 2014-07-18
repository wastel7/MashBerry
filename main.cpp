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
#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QNetworkInterface>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap(":/MashBerryLogo.PNG");
    QSplashScreen splash(pixmap);
    splash.show();
    //splash.showFullScreen();

    QFont splashFont;
    splashFont.setFamily("Arial");
    splashFont.setBold(true);
    splashFont.setPixelSize(11);

    splash.setFont(splashFont);

    QList<QHostAddress> ips = QNetworkInterface::allAddresses();

    QString ipstr("");
    for (int i = 0; i < ips.size(); ++i)
    {
        if(ips[i].protocol() == QAbstractSocket::IPv4Protocol && ips[i] != QHostAddress::LocalHost)
        {
            ipstr += ips[i].toString();
            ipstr += "  ";
        }
    }

    splash.showMessage("MashBerry version V1.2\n\nIP-address:\n"+ipstr, Qt::AlignBottom|Qt::AlignHCenter);
    a.processEvents();

    MainWindow w;

    QTimer::singleShot(10000, &splash, SLOT(close()));
    QTimer::singleShot(10000, &w, SLOT(show()));
//    splash.finish(&w);
//    w.show();
    
    return a.exec();
}
