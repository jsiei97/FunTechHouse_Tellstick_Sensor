TEMPLATE = app
TARGET =

CONFIG += qt debug

DEPENDPATH  += . src
INCLUDEPATH += . src

LIBS += -ltelldus-core
LIBS += -lmosquittopp -lmosquitto

target.path = /usr/local/bin/
INSTALLS += target

# Input
SOURCES += main.cpp

SOURCES += MosqConnect.cpp
HEADERS += MosqConnect.h

SOURCES += HumiditySensor.cpp
HEADERS += HumiditySensor.h

