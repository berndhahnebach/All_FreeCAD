
lib_LTLIBRARIES=libPoints.la

libPoints_la_SOURCES=\
		AppPoints.cpp \
		AppPointsPy.cpp \
		FeaturePointsImportAscii.cpp \
		FeaturePointsImportAscii.h \
		Points.cpp \
		Points.h \
		PointsPy.cpp \
		PointsPy.h \
		PointsAlgos.cpp \
		PointsAlgos.h \
		PointsFeature.cpp \
		PointsFeature.h \
		PointsFeaturePy.cpp \
		PointsFeaturePy.h \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libPoints_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPoints_la_CPPFLAGS = -DPointsAppExport=

libPoints_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

libdir = $(prefix)/Mod/Points

# We need this softlink for Python to load
install-data-local:
		cd $(libdir) && \
		rm -f Points.so && \
		$(LN_S) libPoints.so Points.so

EXTRA_DIST = \
		AppPoints.dsp \
		AppPoints.vcproj \
		Libs.cpp
