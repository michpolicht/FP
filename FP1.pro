QT += charts qml quick widgets

CONFIG += c++11

HEADERS += \
    Model.hpp \
    T1.hpp \
    T1ListModel.hpp

SOURCES += main.cpp \
    Model.cpp \
    T1ListModel.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

