
lib_LTLIBRARIES=lib_TEMPLATE_.la

lib_TEMPLATE__la_SOURCES=\
		App_TEMPLATE_.cpp \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
lib_TEMPLATE__la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
lib_TEMPLATE__la_CPPFLAGS = -D_TEMPLATE_AppExport=

lib_TEMPLATE__la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

libdir = $(prefix)/Mod/_TEMPLATE_

# We need this softlink for Python to load
install-data-local:
		cd $(DESTDIR)$(libdir) && \
		rm -f _TEMPLATE_.so && \
		$(LN_S) lib_TEMPLATE_.so _TEMPLATE_.so

EXTRA_DIST = \
		App_TEMPLATE_.dsp \
		Libs.cpp
