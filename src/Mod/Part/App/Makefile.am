
pkglib_LTLIBRARIES=libPart.la

libPart_la_SOURCES=\
		AppPart.cpp \
		FeaturePartBox.cpp \
		FeaturePartBox.h \
		PartDocType.cpp \
		PartDocType.h

# the library search path.
libPart_la_LDFLAGS = -L../../../App $(all_libraries)

libPart_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../
