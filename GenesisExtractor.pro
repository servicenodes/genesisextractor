#-------------------------------------------------
#
# Project created by QtCreator 2016-01-10T15:10:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GenesisExtractor
TEMPLATE = app

DEPENDPATH += src
SOURCES += src/main.cpp\
        src/genesisextractor.cpp

HEADERS  += src/genesisextractor.h

FORMS    += src/genesisextractor.ui


win32:RC_FILE = src/genesisextractor.rc

win32:DESTDIR = ./bin
