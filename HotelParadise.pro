QT += core gui widgets

CONFIG += c++17 utf8_source
TARGET = HotelParadise
TEMPLATE = app

INCLUDEPATH += $$PWD

HEADERS += \
    model/Pessoa.h \
    model/Atendente.h \
    model/Cliente.h

SOURCES += \
    main.cpp \
    model/Pessoa.cpp \
    model/Atendente.cpp \
    model/Cliente.cpp
