
lib_LTLIBRARIES=libImport.la

libImport_la_SOURCES=\
		AppImport.cpp \
		AppImportPy.cpp \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libImport_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libImport_la_LIBADD   = \
		-lTKIGES \
		-lTKSTEP \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

# We need this softlink for Python to load
install-data-local:
		cd $(prefix) && \
		rm -f Import.so && \
		$(LN_S) lib/libImport.so Import.so
