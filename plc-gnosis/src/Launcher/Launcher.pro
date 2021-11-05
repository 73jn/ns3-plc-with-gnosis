TEMPLATE = app

BIN_DIRECTORY = ../../bin

include(../plc-ns3-common.pri)

#Setup destination for binaries and temp files
DESTDIR = $$BIN_DIRECTORY/app

#Configuration specific settings
CONFIG(debug, release|debug) {
    TARGET = launcher
    OBJECTS_DIR =   $$BIN_DIRECTORY/tmp/obj/debug
    MOC_DIR =       $$BIN_DIRECTORY/tmp/moc/debug
}
else {
    TARGET = launcher
    OBJECTS_DIR =   $$BIN_DIRECTORY/tmp/obj/release
    MOC_DIR =       $$BIN_DIRECTORY/tmp/moc/release
}

SOURCES += \
    launchermain.cpp
