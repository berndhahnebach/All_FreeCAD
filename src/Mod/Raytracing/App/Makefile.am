
pkglib_LTLIBRARIES=libRaytracing.la

libRaytracing_la_SOURCES=\
	AppRaytracing.cpp


# the library search path.
libRaytracing_la_LDFLAGS = -L../../../App $(all_libraries)

libRaytracing_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../
