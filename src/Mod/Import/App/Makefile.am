
pkglib_LTLIBRARIES=libImport.la

libImport_la_SOURCES=\
	AppImport.cpp


# the library search path.
libImport_la_LDFLAGS = -L../../../App $(all_libraries)

libImport_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../
