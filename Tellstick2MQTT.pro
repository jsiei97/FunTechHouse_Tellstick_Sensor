CONFIG += qt debug
TEMPLATE = app
TARGET =

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

SOURCES += UnixTime.cpp
HEADERS += UnixTime.h

