if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libDrawing.la Drawing.la

libDrawing_la_SOURCES=\
                AppDrawingPy.cpp \
		FeaturePage.cpp \
		FeaturePage.h \
		FeatureView.cpp \
		FeatureView.h \
		FeatureViewPart.cpp \
		FeatureViewPart.h \
		PageGroup.cpp \
		PageGroup.h \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libDrawing_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Mod/Part/App $(all_libraries) -L$(OCC_LIB) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libDrawing_la_CPPFLAGS = -DDrawingAppExport=

libDrawing_la_LIBADD   = \
		-l@PYTHON_LIB@ \
		-lboost_regex \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lPart \
		-lTKernel \
		-lTKG2d \
		-lTKG3d \
		-lTKMath \
		-lTKSTEP \
		-lTKIGES \
		-lTKSTL \
		-lTKShHealing \
		-lTKXSBase \
		-lTKBool \
		-lTKBO \
		-lTKBRep \
		-lTKTopAlgo \
		-lTKGeomAlgo \
		-lTKGeomBase \
		-lTKOffset \
		-lTKPrim \
		-lTKHLR

#--------------------------------------------------------------------------------------
# Loader of libDrawing

Drawing_la_SOURCES=\
		AppDrawing.cpp

# the library search path.
Drawing_la_LDFLAGS = $(libDrawing_la_LDFLAGS) -module -avoid-version
Drawing_la_CPPFLAGS = $(libDrawing_la_CPPFLAGS)

Drawing_la_LIBADD   = \
		$(libDrawing_la_LIBADD) \
		-lDrawing

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) -I$(OCC_INC) -I$(top_srcdir)/src -I$(top_builddir)/src

endif

libdir = $(prefix)/Mod/Drawing

EXTRA_DIST = \
		AppDrawing.vcproj
