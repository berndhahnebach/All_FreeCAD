
lib_LTLIBRARIES=lib_TEMPLATE_.la

lib_TEMPLATE__la_SOURCES=\
	App_TEMPLATE_.cpp


# the library search path.
lib_TEMPLATE__la_LDFLAGS = $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

lib_TEMPLATE__la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../
