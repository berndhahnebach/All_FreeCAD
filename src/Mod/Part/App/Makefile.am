if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libPart.la

libPart_la_SOURCES=\
		AppPart.cpp \
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

# set the include path found by configure
INCLUDES= $(all_includes) -I$(OCC_INC) -I../../../

endif

libdir = $(prefix)/Mod/Part

# We need this softlink for Python to load
install-data-local:
		cd $(DESTDIR)$(libdir) && \
		rm -f Part.so && \
		$(LN_S) libPart.so Part.so

EXTRA_DIST = \
		AppPart.dsp \
		AppPart.vcproj \
		Libs.cpp
