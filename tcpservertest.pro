#-------------------------------------------------
#
# Project created by QtCreator 2014-01-10T21:58:01
#
#-------------------------------------------------

QT       += core gui network
QT += multimedia
QT += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tcpservertest
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    socketlistmodel.cpp \

HEADERS  += dialog.h \
    socketlistmodel.h \
    ../../../aac/fdk-aac-0.1.4/include/fdk-aac/aacdecoder_lib.h \
    ../../../aac/fdk-aac-0.1.4/include/fdk-aac/aacenc_lib.h \
    ../../../aac/fdk-aac-0.1.4/include/fdk-aac/FDK_audio.h \
    ../../../aac/fdk-aac-0.1.4/include/fdk-aac/genericStds.h \
    ../../../aac/fdk-aac-0.1.4/include/fdk-aac/machine_type.h


unix:!macx: LIBS += -L$$PWD/../../../aac/fdk-aac-0.1.4/lib/ -lfdk-aac

INCLUDEPATH += $$PWD/../../../aac/fdk-aac-0.1.4/include/fdk-aac
DEPENDPATH += $$PWD/../../../aac/fdk-aac-0.1.4/include/fdk-aac
