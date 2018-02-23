#-------------------------------------------------
#
# Project created by QtCreator 2018-02-11T15:12:32
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Furnace_Controller
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    linechart.cpp

HEADERS  += mainwindow.h \
    linechart.h

FORMS    += mainwindow.ui
