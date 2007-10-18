
lib_LTLIBRARIES=libFreeCADApp.la

BUILT_SOURCES=\
		ComplexGeoDataPy.cpp \
		DocumentObjectGroupPy.cpp \
		DocumentObjectPy.cpp \
		DocumentPy.cpp \
		FeaturePy.cpp \
		FeaturePythonPy.cpp \
		PropertyContainerPy.cpp \
		InitScript.h \
		TestScript.h

libFreeCADApp_la_BUILT=\
		ComplexGeoDataPy.h \
		DocumentObjectGroupPy.h \
		DocumentObjectPy.h \
		DocumentPy.h \
		FeaturePy.h \
		FeaturePythonPy.h \
		PropertyContainerPy.h

libFreeCADApp_la_SOURCES=\
		Application.cpp \
		Application.h \
		ApplicationPy.cpp \
		ColorModel.cpp \
		ColorModel.h \
		ComplexGeoData.cpp \
		ComplexGeoData.h \
		ComplexGeoDataPyImp.cpp \
		Document.cpp \
		Document.h \
		DocumentObject.cpp \
		DocumentObject.h \
		DocumentObjectGroup.cpp \
		DocumentObjectGroup.h \
		DocumentObjectGroupPyImp.cpp \
		DocumentObjectPyImp.cpp \
		DocumentPyImp.cpp \
		Doxygen.cpp \
		Feature.cpp \
		Feature.h \
		FeaturePyImp.cpp \
		FeaturePython.cpp \
		FeaturePython.h \
		FeaturePythonPyImp.cpp \
		FeatureTest.cpp \
		FeatureTest.h \
		GeoFeature.cpp \
		GeoFeature.h \
		InventorObject.cpp \
		InventorObject.h \
		Material.cpp \
		Material.h \
		MaterialPy.cpp \
		MaterialPy.h \
		MatrixPy.cpp \
		MatrixPy.h \
		Placement.cpp \
		Placement.h \
		PreCompiled.cpp \
		PreCompiled.h \
		Property.cpp \
		Property.h \
		PropertyGeo.cpp \
		PropertyGeo.h \
		PropertyContainer.cpp \
		PropertyContainer.h \
		PropertyContainerPyImp.cpp \
		PropertyLinks.cpp \
		PropertyLinks.h \
		PropertyStandard.cpp \
		PropertyStandard.h \
		Transactions.cpp \
		Transactions.h \
		VectorPy.cpp \
		VectorPy.h

%Script.h: FreeCAD%.py
	$(PYTHON) $(top_srcdir)/src/Tools/PythonToCPP.py $< $@

%.cpp: %.xml
	$(PYTHON) $(top_srcdir)/src/Tools/generate.py --outputPath $(@D) $<

# the library search path.
libFreeCADApp_la_LDFLAGS = -L../Base $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libFreeCADApp_la_LIBADD   = \
		-lFreeCADBase \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lboost_filesystem \
		-lboost_program_options \
		-lboost_signals

# set the include path found by configure
INCLUDES= $(all_includes) -I$(top_srcdir)/src -I$(top_builddir)/src

# set additional defines (for cygwin only)
AM_CPPFLAGS=-DFCApp

CLEANFILES = $(BUILT_SOURCES) $(libFreeCADApp_la_BUILT)

EXTRA_DIST = \
		ComplexGeoDataPy.xml \
		DocumentObjectGroupPy.xml \
		DocumentObjectPy.xml \
		DocumentPy.xml \
		FeaturePy.xml \
		FeaturePythonPy.xml \
		PropertyContainerPy.xml \
		FreeCADDoc.vcproj \
		FreeCADInit.py \
		FreeCADTest.py



