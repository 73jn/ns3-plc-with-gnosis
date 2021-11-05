#Setup required Qt modules
QT += gui

#Build template
TEMPLATE = subdirs

BIN_DIRECTORY = $$PWD/../bin

SUBDIRS +=  PLCTopologyLoader \
            PLCGui \
            PLCSimulator \
            Launcher

CONFIG += ordered

NS3_DIR = .
