
lib_LTLIBRARIES=libPoints.la

libPoints_la_SOURCES=\
		AppPoints.cpp \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libPoints_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPoints_la_CPPFLAGS = -DPointsAppExport=

libPoints_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

# We need this softlink for Python to load
install-data-local:
		cd $(prefix) && \
		rm -f Points.so && \
		$(LN_S) lib/libPoints.so Points.so
