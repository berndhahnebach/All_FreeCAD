
lib_LTLIBRARIES=libImport.la

libImport_la_SOURCES=\
		AppImport.cpp


# the library search path.
libImport_la_LDFLAGS = $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libImport_la_LIBADD   = \
		-lTKIGES \
		-lTKSTEP \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../
