
lib_LTLIBRARIES=lib_TEMPLATE_.la

lib_TEMPLATE__la_SOURCES=\
	App_TEMPLATE_.cpp


# the library search path.
lib_TEMPLATE__la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

lib_TEMPLATE__la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

# We need this softlink for Python to load
install-data-local:
		cd $(prefix) && \
		rm -f _TEMPLATE_.so && \
		$(LN_S) lib/lib_TEMPLATE_.so _TEMPLATE_.so
