
pkglib_LTLIBRARIES=libRaytracing.la

libRaytracing_la_SOURCES=\
		AppRaytracing.cpp \
		FeatureRayExportPov.cpp \
		FeatureRayExportPov.h \
		RayFeature.cpp \
		RayFeature.h


# the library search path.
libRaytracing_la_LDFLAGS = -L../../../App -L../../Part $(all_libraries)

libRaytracing_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp \
		-lPart

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../
