QT += charts qml quick widgets

CONFIG += c++11

HEADERS += \
    Model.hpp \
    TransitionModel.hpp \
    Transition.hpp

SOURCES += main.cpp \
    Model.cpp \
    TransitionModel.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

