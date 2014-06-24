#-------------------------------------------------
#
# Project created by QtCreator 2012-12-12T11:25:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MashBerry
TEMPLATE = app

TRANSLATIONS = gui_de.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    PID/PID_v1.cpp \
    PID/PID_AutoTune_v0.cpp \
    qpidcontroller.cpp \
    qrecipe.cpp \
    qrecipeentry.cpp \
    qpidparameters.cpp \
    tinyxml2.cpp \
    qrecipemanager.cpp \
    QtWebApp/lib/bfHttpServer/src/staticfilecontroller.cpp \
    QtWebApp/lib/bfHttpServer/src/httpsessionstore.cpp \
    QtWebApp/lib/bfHttpServer/src/httpsession.cpp \
    QtWebApp/lib/bfHttpServer/src/httpresponse.cpp \
    QtWebApp/lib/bfHttpServer/src/httprequesthandler.cpp \
    QtWebApp/lib/bfHttpServer/src/httprequest.cpp \
    QtWebApp/lib/bfHttpServer/src/httplistener.cpp \
    QtWebApp/lib/bfHttpServer/src/httpcookie.cpp \
    QtWebApp/lib/bfHttpServer/src/httpconnectionhandlerpool.cpp \
    QtWebApp/lib/bfHttpServer/src/httpconnectionhandler.cpp \
    QtWebApp/lib/bfTemplateEngine/src/templateloader.cpp \
    QtWebApp/lib/bfTemplateEngine/src/templatecache.cpp \
    QtWebApp/lib/bfTemplateEngine/src/template.cpp \
    qwebui.cpp \
    qmysettings.cpp \
    qtempsensorpt1000.cpp \
    qtempsensords18b20.cpp \
    qgpio.cpp

HEADERS  += mainwindow.h \
    PID/PID_v1.h \
    PID/PID_AutoTune_v0.h \
    qpidcontroller.h \
    qrecipe.h \
    qrecipeentry.h \
    qpidparameters.h \
    qtempsensor.h \
    tinyxml2.h \
    qrecipemanager.h \
    QtWebApp/lib/bfHttpServer/src/staticfilecontroller.h \
    QtWebApp/lib/bfHttpServer/src/httpsessionstore.h \
    QtWebApp/lib/bfHttpServer/src/httpsession.h \
    QtWebApp/lib/bfHttpServer/src/httpresponse.h \
    QtWebApp/lib/bfHttpServer/src/httprequesthandler.h \
    QtWebApp/lib/bfHttpServer/src/httprequest.h \
    QtWebApp/lib/bfHttpServer/src/httplistener.h \
    QtWebApp/lib/bfHttpServer/src/httpcookie.h \
    QtWebApp/lib/bfHttpServer/src/httpconnectionhandlerpool.h \
    QtWebApp/lib/bfHttpServer/src/httpconnectionhandler.h \
    QtWebApp/lib/bfTemplateEngine/src/templateloader.h \
    QtWebApp/lib/bfTemplateEngine/src/templatecache.h \
    QtWebApp/lib/bfTemplateEngine/src/template.h \
    qwebui.h \
    qmysettings.h \
    qtempsensorpt1000.h \
    qtempsensords18b20.h \
    qgpio.h

FORMS    += mainwindow.ui

target.path += /usr/bin
INSTALLS += target
!win32:LIBS += -llirc_client

OTHER_FILES +=
