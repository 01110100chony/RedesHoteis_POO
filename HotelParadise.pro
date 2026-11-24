QT += core gui widgets

CONFIG += c++17 utf8_source
TARGET = HotelParadise
TEMPLATE = app

INCLUDEPATH += $$PWD

HEADERS += \
    model/Pessoa.h \
    model/Atendente.h \
    model/Cliente.h \
    model/TiposHotel.h \
    model/EstrategiaDesconto.h \
    model/Reserva.h \
    core/GerenciadorReservas.h \
    core/PersistenciaArquivo.h

SOURCES += \
    main.cpp \
    model/Pessoa.cpp \
    model/Atendente.cpp \
    model/Cliente.cpp \
    model/TiposHotel.cpp \
    model/EstrategiaDesconto.cpp \
    model/Reserva.cpp \
    core/GerenciadorReservas.cpp \
    core/PersistenciaArquivo.cpp
