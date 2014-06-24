QT = core network
TARGET = QtWebApp
TEMPLATE = app
CONFIG += CONSOLE

CONFIG(debug, debug|release) {
  DEFINES += SUPERVERBOSE
}

HEADERS = \
    static.h \
    startup.h \
    requestmapper.h \
    controller/dumpcontroller.h \
    controller/templatecontroller.h \
    controller/formcontroller.h \
    controller/fileuploadcontroller.h \
    controller/sessioncontroller.h

SOURCES = main.cpp \
    static.cpp \
    startup.cpp \
    requestmapper.cpp \
    controller/dumpcontroller.cpp \
    controller/templatecontroller.cpp \
    controller/formcontroller.cpp \
    controller/fileuploadcontroller.cpp \
    controller/sessioncontroller.cpp

OTHER_FILES += \
    ../etc/QtWebApp.ini \
    ../etc/demo.tpl \
    ../etc/demo-de.tpl \
    ../doc/releasenotes.txt \
    ../etc/docroot/Schmetterling klein.png \
    ../etc/docroot/index.html \
    ../etc/docroot/Schmetterling klein.png \
    ../Doxyfile \
    ../etc/templates/demo-de.tpl \
    ../etc/templates/demo.tpl \
    ../doc/releasenotes.txt \
    ../doc/license.txt \
    ../doc/example-response-normal.txt \
    ../doc/example-response-chunked.txt \
    ../doc/example-request-get.txt \
    ../doc/example-request-form-post.txt \
    ../doc/example-request-form-get.txt \
    ../doc/example-request-file-upload.txt \
    ../doc/readme.txt

include(../lib/qtservice-2.6_1/src/qtservice.pri)
include(../lib/bfLogging/src/bfLogging.pri)
include(../lib/bfHttpServer/src/bfHttpServer.pri)
include(../lib/bfTemplateEngine/src/bfTemplateEngine.pri)
