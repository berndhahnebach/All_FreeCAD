
lib_LTLIBRARIES=libMesh.la

libMesh_la_SOURCES=\
		AppMesh.cpp


# the library search path.
libMesh_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libMesh_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

# We need this softlink for Python to load
install-data-local:
		cd $(prefix) && \
		rm -f Mesh.so && \
		$(LN_S) lib/libMesh.so Mesh.so
