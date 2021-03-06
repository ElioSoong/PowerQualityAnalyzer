# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------
QMAKE_CXXFLAGS += -g -ggdb
QMAKESPEC = arm-linaro-linux-gnueabihf-g++-lib
TEMPLATE = lib
TARGET = CoreLibrary
QT += core sql network uitools widgets gui location positioning

INCLUDEPATH +=  . \
    ./src \
    ./src/common \
    ./src/gui \
    ./src/pqa \
    ./src/remote \
    ./src/storage \
    ./tests \
    ./tests/common \
    ./tests/gui \
    ./tests/pqa \
    ./ui \
    ./src/remote/libscpi/inc/scpi \
    ./src/remote/libscpi/inc \
    ./tests/remote \
    ../ \
    ../FieldLibrary

LIBS += $$PWD/lib/libscpi.a

#LIBS += -L $$OUT_PWD/FieldLibrary/lib/-lFieldLibrary
#LIBS += -L $$OUT_PWD/JsonLoader/lib/JsonLoader/-lJsonLoader

# DEPENDPATH += .
MOC_DIR += ./moc
OBJECTS_DIR += ./debug
DESTDIR = $$PWD/../desdir
include(CoreLibrary.pri)
