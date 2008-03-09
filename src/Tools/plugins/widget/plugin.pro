
TEMPLATE = lib
CONFIG += designer plugin
TARGET = FreeCAD_widgets
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += customwidgets.h wizard.h plugin.h
SOURCES += customwidgets.cpp wizard.cpp plugin.cpp
