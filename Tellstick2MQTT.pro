TEMPLATE = app
TARGET =

CONFIG += qt debug

DEPENDPATH  += . src
INCLUDEPATH += . src
LIBS += -ltelldus-core

target.path = /usr/local/bin/
INSTALLS += target

# Input
SOURCES += main.cpp

#SOURCES += WeekTimer.cpp
#HEADERS += WeekTimer.h

