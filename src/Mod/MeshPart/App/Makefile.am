
lib_LTLIBRARIES=libMeshPart.la MeshPart.la

libMeshPart_la_SOURCES=\
                AppMeshPartPy.cpp \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libMeshPart_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Mod/Part/App $(all_libraries) -L$(OCC_LIB) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libMeshPart_la_CPPFLAGS = -DMeshPartAppExport=

libMeshPart_la_LIBADD   = \
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
# Loader of libMeshPart

MeshPart_la_SOURCES=\
		AppMeshPart.cpp

# the library search path.
MeshPart_la_LDFLAGS = $(libMeshPart_la_LDFLAGS) -module -avoid-version
MeshPart_la_CPPFLAGS = $(libMeshPart_la_CPPFLAGS)

MeshPart_la_LIBADD   = \
		$(libMeshPart_la_LIBADD) \
		-lMeshPart

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes) -I$(OCC_INC)


libdir = $(prefix)/Mod/MeshPart

EXTRA_DIST = \
		AppMeshPart.vcproj
