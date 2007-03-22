
lib_LTLIBRARIES=libImage.la Image.la

libImage_la_SOURCES=\
		ImageBase.cpp \
		ImageBase.h \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libImage_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libImage_la_CPPFLAGS = -DImageAppExport=

libImage_la_LIBADD   = \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp

#--------------------------------------------------------------------------------------
# Loader of libImage

Image_la_SOURCES=\
		AppImage.cpp

# the library search path.
Image_la_LDFLAGS = $(libImage_la_LDFLAGS) -module -avoid-version
Image_la_CPPFLAGS = $(libImage_la_CPPFLAGS)

Image_la_LIBADD   = \
		$(libImage_la_LIBADD) \
		-lImage

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

libdir = $(prefix)/Mod/Image

EXTRA_DIST = \
		AppImage.vcproj \
		Libs.cpp
