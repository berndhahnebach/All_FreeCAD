
pkglib_LTLIBRARIES=libMesh.la

libMesh_la_SOURCES=\
	AppMesh.cpp


# the library search path.
libMesh_la_LDFLAGS = -L../../../App $(all_libraries)

libMesh_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../
