bin_PROGRAMS=FreeCAD FreeCADCmd
pkglib_LTLIBRARIES=libFreeCADCmdPy.la

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
libFreeCADCmdPy_la_SOURCES= \
	MainPy.cpp

# the library search path.
libFreeCADCmdPy_la_LDFLAGS = -L../Base -L../App -L../Gui $(all_libraries)

libFreeCADCmdPy_la_LIBADD=\
	-lFreeCADBase \
	-lFreeCADApp \
	-lFreeCADGui
