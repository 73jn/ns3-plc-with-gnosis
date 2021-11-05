#Setup required Qt modules
QT += gui

#Build template
TEMPLATE = subdirs

BIN_DIRECTORY = $$PWD/../bin

SUBDIRS +=  PLCGui \

CONFIG += ordered
