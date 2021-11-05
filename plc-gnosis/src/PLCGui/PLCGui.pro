include(../plc-gnosis-common.pri)
include(../plc-ns3-common.pri)

#Setup required Qt modules
QT += gui

#Build template
TEMPLATE = app

#Add platform specific library paths
#Add platform specific library paths
win32: INCLUDEPATH += $$quote(../../lib)
win32: LIBS += -L$$quote($$BIN_DIRECTORY/app)

#Source files
HEADERS +=  src/MainWindow/mainwindow.h \
            src/DiagramSheet/diagramsheet.h \
            src/DiagramEditor/diagrameditor.h \
            src/NodeConfiguration/nodeconfiguration.h \
            src/NodeConfiguration/plcdatainputwidget.h \
            src/PLCGraphicsNodeItem/plcgraphicsnodeitem.h \
            src/PLCGraphicsEdgeItem/plcgraphicsedgeitem.h \
            src/EdgeConfiguration/edgeconfiguration.h \
            src/PLCSpectrumConfiguration/plcspectrumconfiguration.h \
            src/NodeConfiguration/netdeviceeditor.h \
            src/NodeConfiguration/noisesourceeditor.h \
            src/ZoomSlider/zoomslider.h \

SOURCES +=  src/main.cpp \
            src/MainWindow/mainwindow.cpp \
            src/DiagramSheet/diagramsheet.cpp \
            src/DiagramEditor/diagrameditor.cpp \
            src/NodeConfiguration/nodeconfiguration.cpp \
            src/NodeConfiguration/plcdatainputwidget.cpp \
            src/PLCGraphicsNodeItem/plcgraphicsnodeitem.cpp \
            src/PLCGraphicsEdgeItem/plcgraphicsedgeitem.cpp \
            src/EdgeConfiguration/edgeconfiguration.cpp \
            src/PLCSpectrumConfiguration/plcspectrumconfiguration.cpp \
            src/ZoomSlider/zoomslider.cpp \
            src/NodeConfiguration/netdeviceeditor.cpp \
            src/NodeConfiguration/noisesourceeditor.cpp \

RESOURCES += \
    res/resources.qrc


#Setup destination for binaries and temp files
DESTDIR =      $$BIN_DIRECTORY/app/

#Configuration specific settings
CONFIG(debug, release|debug) {
    TARGET = ns-3gui-debug
    OBJECTS_DIR =   $$BIN_DIRECTORY/tmp/obj/debug
    MOC_DIR =       $$BIN_DIRECTORY/tmp/moc/debug
}
else { 
    TARGET = ns-3gui
    OBJECTS_DIR =   $$BIN_DIRECTORY/tmp/obj/release
    MOC_DIR =       $$BIN_DIRECTORY/tmp/moc/release
}


#Notes:
    #License information for:
        #svg handling: http://qt-project.org/doc/qt-4.8/qtsvg.html
        #qt library: ???
        #qtjson: ???
        #ns-3 stuff??
