TEMPLATE = app

# try use global config
exists(../../../config.pri) {
    ROOT_DIR=../../..
    include($$ROOT_DIR/config.pri)
} else {
    message(Using local config for the $$TARGET)
    ROOT_DIR=..
    include($$ROOT_DIR/cvs-config.pri)
}

QT += qml quick widgets

include(../core/core.pri)

SOURCES += main.cpp \
    adcensus.cpp

HEADERS += \
    adcensus.h

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
