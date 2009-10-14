
lib_LTLIBRARIES=libPartDesign.la PartDesign.la

libPartDesign_la_SOURCES=\
		AppPartDesignPy.cpp \
		FeatureFillet.cpp \
		FeatureFillet.h \
		FeaturePad.cpp \
		FeaturePad.h \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libPartDesign_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Mod/Part/App \
		$(all_libraries) -L$(OCC_LIB) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPartDesign_la_CPPFLAGS = -DPartDesignAppExport=

libPartDesign_la_LIBADD   = \
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
		-lTKXSBase \
		-lTKBRep \
		-lTKGeomAlgo \
		-lTKGeomBase \
		-lTKOffset \
		-lTKPrim \
		-lTKFillet \
		-lTKShHealing \
		-lTKBool \
		-lTKBO \
		-lTKTopAlgo 

#--------------------------------------------------------------------------------------
# Loader of libPartDesign

PartDesign_la_SOURCES=\
		AppPartDesign.cpp

# the library search path.
PartDesign_la_LDFLAGS = $(libPartDesign_la_LDFLAGS) -module -avoid-version
PartDesign_la_CPPFLAGS = $(libPartDesign_la_CPPFLAGS)

PartDesign_la_LIBADD   = \
		$(libPartDesign_la_LIBADD) \
		-lPartDesign

#--------------------------------------------------------------------------------------

# set the include path found by configure
AM_CXXFLAGS = -I$(OCC_INC) -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes)


libdir = $(prefix)/Mod/PartDesign

EXTRA_DIST = \
		CMakeLists.txt
