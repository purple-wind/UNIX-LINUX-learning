TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
INCLUDEPATH+=/usr/include/x86_64-linux-gnu/
SOURCES += \
    pipe.cpp \
    main.cpp \
    fifo.cpp

HEADERS += \
    pipe.h \
    fifo.h

