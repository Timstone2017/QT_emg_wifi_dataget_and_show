#-------------------------------------------------
#
# Project created by QtCreator 2018-08-26T19:08:48
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = yeswifi
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    workthread.cpp

HEADERS  += mainwindow.h \
    workthread.h \
    cglobal.h

FORMS    += mainwindow.ui
