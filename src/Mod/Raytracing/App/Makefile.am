
lib_LTLIBRARIES=libRaytracing.la

libRaytracing_la_SOURCES=\
		AppRaytracing.cpp \
		FeatureRayExportPov.cpp \
		FeatureRayExportPov.h \
		RayFeature.cpp \
		RayFeature.h


# the library search path.
libRaytracing_la_LDFLAGS = -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libRaytracing_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp \
		-lPart

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../
