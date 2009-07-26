bin_PROGRAMS=FreeCAD FreeCADCmd
lib_LTLIBRARIES=FreeCAD.la FreeCADGui.la

#--------------------------------------------------------------------------------------
FreeCAD_SOURCES= \
	MainGui.cpp

# the library search path.
FreeCAD_LDFLAGS = -L../Base -L../App -L../Gui $(QT_LIBS) $(all_libraries)

FreeCAD_LDADD=\
	-l@PYTHON_LIB@ \
	-lFreeCADBase \
	-lFreeCADApp \
	-lFreeCADGui

#--------------------------------------------------------------------------------------
FreeCADCmd_SOURCES= \
	MainCmd.cpp

# the library search path.
FreeCADCmd_LDFLAGS = -L../Base -L../App $(all_libraries)

FreeCADCmd_LDADD=\
	-l@PYTHON_LIB@ \
	-lFreeCADBase \
	-lFreeCADApp

#--------------------------------------------------------------------------------------
FreeCAD_la_SOURCES= \
	MainPy.cpp

# the library search path.
FreeCAD_la_LDFLAGS = -L../Base -L../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@ -module -avoid-version

FreeCAD_la_LIBADD=\
	-ldl \
	-l@PYTHON_LIB@ \
	-lFreeCADBase \
	-lFreeCADApp
#--------------------------------------------------------------------------------------
FreeCADGui_la_SOURCES= \
	FreeCADGuiPy.cpp

# the library search path.
FreeCADGui_la_LDFLAGS = -L../Base -L../App -L../Gui $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@ -module -avoid-version

FreeCAD_la_LIBADD=\
	-ldl \
	-l@PYTHON_LIB@ \
	-lFreeCADBase \
	-lFreeCADApp \
	-lFreeCADGui

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) -I$(top_srcdir)/src $(QT_CXXFLAGS)

EXTRA_DIST = \
		res/FreeCAD.exe.manifest \
		res/FreeCADD.exe.manifest \
		freecad.rc \
		CMakeLists.txt \
		icon.ico
