
lib_LTLIBRARIES=libPoints.la Points.la

BUILT_SOURCES=\
		PointsPy.cpp

libPoints_la_BUILT=\
		PointsPy.h \

libPoints_la_SOURCES=\
		AppPointsPy.cpp \
		FeaturePointsImportAscii.cpp \
		Points.cpp \
		PointsPyImp.cpp \
		PointsAlgos.cpp \
		PointsFeature.cpp \
		PointsGrid.cpp \
		Properties.cpp \
		PropertyPointKernel.cpp \
		PreCompiled.cpp \
		PreCompiled.h

includedir = @includedir@/Mod/Points/App

include_HEADERS=\
		FeaturePointsImportAscii.h \
		Points.h \
		PointsAlgos.h \
		PointsFeature.h \
		PointsGrid.h \
		Properties.h \
		PropertyPointKernel.h


# the library search path.
libPoints_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPoints_la_CPPFLAGS = -DPointsAppExport=

libPoints_la_LIBADD   = \
		@BOOST_REGEX_LIB@ @BOOST_SYSTEM_LIB@ \
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

Points_la_DEPENDENCIES = libPoints.la

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) -I$(top_srcdir)/src -I$(top_builddir)/src

libdir = $(prefix)/Mod/Points

EXTRA_DIST = \
		PointsPy.xml \
		AppPoints.vcproj
