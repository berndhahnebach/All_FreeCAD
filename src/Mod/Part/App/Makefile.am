
lib_LTLIBRARIES=libPart.la

libPart_la_SOURCES=\
		AppPart.cpp \
		FeaturePartBox.cpp \
		FeaturePartBox.h \
		FeaturePartCut.cpp \
		FeaturePartCut.h \
		FeaturePartImportIges.cpp \
		FeaturePartImportIges.h \
		FeaturePartImportStep.cpp \
		FeaturePartImportStep.h \
		PartFeature.cpp \
		PartFeature.h

# the library search path.
libPart_la_LDFLAGS = -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libPart_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp \
		-lTKService \
		-lTKSTEP \
		-lTKIGES \
		-lTKShHealing \
		-lTKXSBase

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../
