SOURCES  += plugin.cpp customwidgets.cpp
HEADERS  += plugin.h customwidgets.h
TARGET    = FreeCAD_widgets

target.path=$$plugins.path

INSTALLS    += target
TEMPLATE     = lib
CONFIG      += qt warn_on release plugin thread
INCLUDEPATH += $$QT_SOURCE_TREE/tools/designer/interfaces
DBFILE       = plugin.db
LANGUAGE     = C++
