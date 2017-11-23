QT += charts qml quick widgets

CONFIG += c++11

HEADERS += \
    Model.hpp \
    Transition.hpp \
    TransitionList.hpp \
    functions.hpp

SOURCES += main.cpp \
    Model.cpp \
    TransitionList.cpp \
    Transition.cpp \
    functions.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

