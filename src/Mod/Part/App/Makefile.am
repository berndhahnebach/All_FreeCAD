if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libPart.la Part.la

BUILT_SOURCES=\
		TopoShapePy.cpp

libPart_la_BUILT=\
		TopoShapePy.h

libPart_la_SOURCES=\
		AppPartPy.cpp \
		CirclePy.cpp \
		CirclePy.h \
		FeaturePartBox.cpp \
		FeaturePartBox.h \
		FeaturePartCircle.cpp \
		FeaturePartCircle.h \
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
		FeaturePartLineSet.cpp \
		FeaturePartLineSet.h \
		FeaturePartPolygon.cpp \
		FeaturePartPolygon.h \
		LinePy.cpp \
		LinePy.h \
		PartFeature.cpp \
		PartFeature.h \
		PartFeaturePy.cpp \
		PartFeaturePy.h \
		PreCompiled.cpp \
		PreCompiled.h \
		PropertyCircle.cpp \
		PropertyCircle.h \
		PropertyLine.cpp \
		PropertyLine.h \
		TopoShape.cpp \
		TopoShape.h \
		TopoShapePyImp.cpp \
		TopologyPy.cpp \
		TopologyPy.h

# the library search path.
libPart_la_LDFLAGS = -L../../../Base -L../../../App -L/usr/X11R6/lib -L$(OCC_LIB) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPart_la_CPPFLAGS = -DAppPartExport=

libPart_la_LIBADD   = \
		-lxerces-c \
		-lboost_filesystem \
		-l@PYTHON_LIB@ \
		-lFreeCADBase \
		-lFreeCADApp \
		-lTKernel \
		-lTKG2d \
		-lTKG3d \
		-lTKMath \
		-lTKService \
		-lTKSTEP \
		-lTKIGES \
		-lTKSTL \
		-lTKShHealing \
		-lTKXSBase \
		-lTKBool \
		-lTKBO \
		-lTKBRep \
		-lTKTopAlgo \
		-lTKGeomAlgo \
		-lTKGeomBase \
		-lTKOffset \
		-lTKPrim

%.cpp: %.xml
	$(PYTHON) $(top_srcdir)/src/Tools/generate.py --outputPath $(@D) $<

#--------------------------------------------------------------------------------------
# Loader of libPart

Part_la_SOURCES=\
		AppPart.cpp

# the library search path.
Part_la_LDFLAGS = $(libPart_la_LDFLAGS) -module -avoid-version
Part_la_CPPFLAGS = $(libPart_la_CPPFLAGS)

Part_la_LIBADD   = \
		$(libPart_la_LIBADD) \
		-lPart

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) -I$(OCC_INC) -I$(top_srcdir)/src -I$(top_builddir)/src

endif

libdir = $(prefix)/Mod/Part

CLEANFILES = $(BUILT_SOURCES) $(libPart_la_BUILT)

EXTRA_DIST = \
		TopoShapePy.xml \
		AppPart.vcproj
