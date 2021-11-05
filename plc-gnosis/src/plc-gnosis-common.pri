BIN_DIRECTORY = $$PWD/../bin


QJSON_SRC_DIR = ../../lib/qjson/src

INCLUDEPATH += ../../lib/qjson/src

windows:{
    DEFINES += QJSON_MAKEDLL
}

macx:{
    CONFIG -= app_bundle
}

HEADERS +=  $$QJSON_SRC_DIR/json_parser.hh \
            $$QJSON_SRC_DIR/json_scanner.h \
            $$QJSON_SRC_DIR/location.hh \
            $$QJSON_SRC_DIR/parser_p.h \
            $$QJSON_SRC_DIR/position.hh \
            $$QJSON_SRC_DIR/qjson_debug.h \
            $$QJSON_SRC_DIR/stack.hh \
            $$QJSON_SRC_DIR/parser.h \
            $$QJSON_SRC_DIR/qobjecthelper.h \
            $$QJSON_SRC_DIR/serializer.h \
            $$QJSON_SRC_DIR/qjson_export.h

SOURCES +=  $$QJSON_SRC_DIR/qobjecthelper.cpp \
            $$QJSON_SRC_DIR/parser.cpp \
            $$QJSON_SRC_DIR/serializer.cpp \
            $$QJSON_SRC_DIR/json_parser.cc \
            $$QJSON_SRC_DIR/json_scanner.cpp

include(PLCTopologyModel/plctopologymodel.pri)
