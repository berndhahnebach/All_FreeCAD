
lib_LTLIBRARIES=lib_TEMPLATE_.la _TEMPLATE_.la

lib_TEMPLATE__la_SOURCES=\
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
lib_TEMPLATE__la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
lib_TEMPLATE__la_CPPFLAGS = -D_TEMPLATE_AppExport=

lib_TEMPLATE__la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp

#--------------------------------------------------------------------------------------
# Loader of lib_TEMPLATE_

_TEMPLATE__la_SOURCES=\
		App_TEMPLATE_.cpp

# the library search path.
_TEMPLATE__la_LDFLAGS = $(lib_TEMPLATE__la_LDFLAGS) -module -avoid-version
_TEMPLATE__la_CPPFLAGS = $(lib_TEMPLATE__la_CPPFLAGS)

_TEMPLATE__la_LIBADD   = \
		-l_TEMPLATE_

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

libdir = $(prefix)/Mod/_TEMPLATE_

EXTRA_DIST = \
		App_TEMPLATE_.dsp \
		App_TEMPLATE_.vcproj
