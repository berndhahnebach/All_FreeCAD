
lib_LTLIBRARIES=libPoints.la Points.la

libPoints_la_SOURCES=\
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
		PointsGrid.cpp \
		PointsGrid.h \
		PropertyPointKernel.cpp \
		PropertyPointKernel.h \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libPoints_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPoints_la_CPPFLAGS = -DPointsAppExport=

libPoints_la_LIBADD   = \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp

#--------------------------------------------------------------------------------------
# Loader of libPoints

Points_la_SOURCES=\
		AppPoints.cpp

# the library search path.
Points_la_LDFLAGS = $(libPoints_la_LDFLAGS) -module -avoid-version
Points_la_CPPFLAGS = $(libPoints_la_CPPFLAGS)

Points_la_LIBADD   = \
		$(libPoints_la_LIBADD) \
		-lPoints

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

libdir = $(prefix)/Mod/Points

EXTRA_DIST = \
		AppPoints.vcproj \
		Libs.cpp
