if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libPart.la Part.la

libPart_la_SOURCES=\
		AppPartPy.cpp \
		FeaturePartBox.cpp \
		FeaturePartBox.h \
		FeaturePartCurveNet.cpp \
		FeaturePartCurveNet.h \
		FeaturePartCut.cpp \
		FeaturePartCut.h \
		FeaturePartImportBrep.cpp \
		FeaturePartImportBrep.h \
		FeaturePartImportIges.cpp \
		FeaturePartImportIges.h \
		FeaturePartImportStep.cpp \
		FeaturePartImportStep.h \
		FeaturePartLine.cpp \
		FeaturePartLine.h \
		FeaturePartPolygon.cpp \
		FeaturePartPolygon.h \
		PartAlgos.cpp \
		PartAlgos.h \
		PartFeature.cpp \
		PartFeature.h \
		PartFeaturePy.cpp \
		PartFeaturePy.h \
		PreCompiled.cpp \
		PreCompiled.h \
		TopologyPy.cpp \
		TopologyPy.h

# the library search path.
libPart_la_LDFLAGS = -L../../../Base -L../../../App -L$(OCC_LIB) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPart_la_CPPFLAGS = -DAppPartExport=

libPart_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp \
		-lTKService \
		-lTKSTEP \
		-lTKIGES \
		-lTKShHealing \
		-lTKXSBase

#--------------------------------------------------------------------------------------
# Loader of libPart

Part_la_SOURCES=\
		AppPart.cpp

# the library search path.
Part_la_LDFLAGS = $(libPart_la_LDFLAGS) -module -avoid-version
Part_la_CPPFLAGS = $(libPart_la_CPPFLAGS)

Part_la_LIBADD   = \
		-lPart

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) -I$(OCC_INC) -I../../../

endif

libdir = $(prefix)/Mod/Part

EXTRA_DIST = \
		AppPart.dsp \
		AppPart.vcproj \
		Libs.cpp
