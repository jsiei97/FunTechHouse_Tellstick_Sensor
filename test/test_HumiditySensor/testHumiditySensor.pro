CONFIG += qtestlib debug
TEMPLATE = app
TARGET =
DEFINES += private=public

# Test code
DEPENDPATH += .
INCLUDEPATH += .
SOURCES += testHumiditySensor.cpp

# Code to test
DEPENDPATH  += ../../src/
INCLUDEPATH += ../../src/
SOURCES += HumiditySensor.cpp

