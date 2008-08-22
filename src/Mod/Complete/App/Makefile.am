if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libComplete.la Complete.la

libComplete_la_SOURCES=\
                AppCompletePy.cpp \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libComplete_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Mod/Part/App $(all_libraries) -L$(OCC_LIB) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libComplete_la_CPPFLAGS = -DCompleteAppExport=

libComplete_la_LIBADD   = \
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
# Loader of libComplete

Complete_la_SOURCES=\
		AppComplete.cpp

# the library search path.
Complete_la_LDFLAGS = $(libComplete_la_LDFLAGS) -module -avoid-version
Complete_la_CPPFLAGS = $(libComplete_la_CPPFLAGS)

Complete_la_LIBADD   = \
		$(libComplete_la_LIBADD) \
		-lComplete

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) -I$(OCC_INC) -I$(top_srcdir)/src -I$(top_builddir)/src

endif

libdir = $(prefix)/Mod/Complete

EXTRA_DIST = \
		AppComplete.vcproj
