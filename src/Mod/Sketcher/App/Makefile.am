if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libSketcher.la Sketcher.la

libSketcher_la_SOURCES=\
                AppSketcherPy.cpp \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libSketcher_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Mod/Part/App $(all_libraries) -L$(OCC_LIB) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libSketcher_la_CPPFLAGS = -DSketcherAppExport=

libSketcher_la_LIBADD   = \
		@BOOST_REGEX_LIB@ @BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
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
# Loader of libSketcher

Sketcher_la_SOURCES=\
		AppSketcher.cpp

# the library search path.
Sketcher_la_LDFLAGS = $(libSketcher_la_LDFLAGS) -module -avoid-version
Sketcher_la_CPPFLAGS = $(libSketcher_la_CPPFLAGS)

Sketcher_la_LIBADD   = \
		$(libSketcher_la_LIBADD) \
		-lSketcher

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) -I$(OCC_INC) -I$(top_srcdir)/src -I$(top_builddir)/src

endif

libdir = $(prefix)/Mod/Sketcher

EXTRA_DIST = \
		AppSketcher.vcproj
