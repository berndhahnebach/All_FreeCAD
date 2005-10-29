bin_PROGRAMS=FreeCAD FreeCADCmd
lib_LTLIBRARIES=libFreeCADPy.la

#--------------------------------------------------------------------------------------
FreeCAD_SOURCES= \
	MainGui.cpp

# the library search path.
FreeCAD_LDFLAGS = -L../Base -L../App -L../Gui $(all_libraries)

FreeCAD_LDADD=\
	-lFreeCADBase \
	-lFreeCADApp \
	-lFreeCADGui

#--------------------------------------------------------------------------------------
FreeCADCmd_SOURCES= \
	MainCmd.cpp

# the library search path.
FreeCADCmd_LDFLAGS = -L../Base -L../App $(all_libraries)

FreeCADCmd_LDADD=\
	-lFreeCADBase \
	-lFreeCADApp

#--------------------------------------------------------------------------------------
libFreeCADPy_la_SOURCES= \
	MainPy.cpp

# the library search path.
libFreeCADPy_la_LDFLAGS = -L../Base -L../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libFreeCADPy_la_LIBADD=\
	-lFreeCADBase \
	-lFreeCADApp

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS)
