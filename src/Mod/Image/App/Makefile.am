
lib_LTLIBRARIES=libImage.la

libImage_la_SOURCES=\
		AppImage.cpp \
		ImageBase.cpp \
		ImageBase.h \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libImage_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libImage_la_CPPFLAGS = -DImageAppExport=

libImage_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

# We need this softlink for Python to load
install-data-local:
		cd $(prefix) && \
		rm -f Image.so && \
		$(LN_S) lib/libImage.so Image.so
