TEMPLATE = lib
CONFIG += dll plugin

LINK_NS3 = true

include(../plc-gnosis-common.pri)
include(../plc-ns3-common.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += PLCTOPOLOGYMODEL_MAKEDLL

HEADERS += \
    plctopologyloader.h

SOURCES += \
    plctopologyloader.cpp

INCLUDEPATH +=  $$NS3_DIR
LIBS += -L$$NS3_DIR $$NS3_LIBRARIES

#Setup destination for binaries and temp files
DESTDIR = $$BIN_DIRECTORY/app

#Configuration specific settings
CONFIG(debug, release|debug) {
    TARGET = plctopologyloader
    OBJECTS_DIR =   $$BIN_DIRECTORY/tmp/obj/debug
    MOC_DIR =       $$BIN_DIRECTORY/tmp/moc/debug
}
else {
    TARGET = plctopologyloader
    OBJECTS_DIR =   $$BIN_DIRECTORY/tmp/obj/release
    MOC_DIR =       $$BIN_DIRECTORY/tmp/moc/release
}
