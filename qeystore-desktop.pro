#-------------------------------------------------
#
# Project created by QtCreator 2013-08-14T12:37:45
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qeystore-desktop
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    entry.cpp

HEADERS  += mainwindow.h \
    entry.h

FORMS    += mainwindow.ui \
    newEntry.ui
