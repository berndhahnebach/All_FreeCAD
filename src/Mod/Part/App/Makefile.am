
lib_LTLIBRARIES=libPart.la

libPart_la_SOURCES=\
		AppPart.cpp \
		AppPartPy.cpp \
		FeaturePartBox.cpp \
		FeaturePartBox.h \
		FeaturePartCut.cpp \
		FeaturePartCut.h \
		FeaturePartImportIges.cpp \
		FeaturePartImportIges.h \
		FeaturePartImportStep.cpp \
		FeaturePartImportStep.h \
		PartFeature.cpp \
		PartFeature.h \
		PartFeaturePy.cpp \
		PartFeaturePy.h \
		PreCompiled.cpp \
		PreCompiled.h

# the library search path.
libPart_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

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

# We need this softlink for Python to load
install-data-local:
		cd $(prefix) && \
		rm -f Part.so && \
		$(LN_S) lib/libPart.so Part.so
