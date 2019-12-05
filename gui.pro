#-------------------------------------------------
#
# Project created by QtCreator 2019-04-01T14:32:21
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gui
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    cJSON.c \
    q03ude.cpp

HEADERS  += widget.h \
    cJSON.h \
    q03ude.h

FORMS    += widget.ui
