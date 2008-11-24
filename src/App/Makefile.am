
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
		Annotation.cpp \
		Annotation.h \
		Application.cpp \
		ApplicationPy.cpp \
		ColorModel.cpp \
		ComplexGeoData.cpp \
		ComplexGeoDataPyImp.cpp \
		Document.cpp \
		DocumentObject.cpp \
		DocumentObjectFileIncluded.cpp \
		DocumentObjectGroup.cpp \
		DocumentObjectGroupPyImp.cpp \
		DocumentObjectPyImp.cpp \
		DocumentObserver.cpp \
		DocumentPyImp.cpp \
		Doxygen.cpp \
		Feature.cpp \
		FeaturePyImp.cpp \
		FeaturePython.cpp \
		FeaturePythonPyImp.cpp \
		FeatureTest.cpp \
		GeoFeature.cpp \
		InventorObject.cpp \
		Material.cpp \
		MaterialPy.cpp \
		MeasureDistance.cpp \
		Placement.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		Property.cpp \
		PropertyFile.cpp \
		PropertyGeo.cpp \
		PropertyContainer.cpp \
		PropertyContainerPyImp.cpp \
		PropertyLinks.cpp \
		PropertyStandard.cpp \
		Transactions.cpp

includedir = @includedir@/App

nodist_include_HEADERS=\
		$(libFreeCADApp_la_BUILT)

include_HEADERS=\
		Application.h \
		ColorModel.h \
		ComplexGeoData.h \
		Document.h \
		DocumentObject.h \
		DocumentObjectFileIncluded.h \
		DocumentObjectGroup.h \
		DocumentObserver.h \
		Feature.h \
		FeaturePython.h \
		FeatureTest.h \
		GeoFeature.h \
		InventorObject.h \
		Material.h \
		MaterialPy.h \
		MeasureDistance.h \
		Placement.h \
		Property.h \
		PropertyFile.h \
		PropertyGeo.h \
		PropertyContainer.h \
		PropertyLinks.h \
		PropertyStandard.h \
		Transactions.h

%Script.h: FreeCAD%.py
	$(PYTHON) $(top_srcdir)/src/Tools/PythonToCPP.py $< $@

%.cpp: %.xml
	$(PYTHON) $(top_srcdir)/src/Tools/generate.py --outputPath $(@D) $<

# the library search path.
libFreeCADApp_la_LDFLAGS = -L../Base $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libFreeCADApp_la_LIBADD   = \
		@BOOST_FILESYSTEM_LIB@ @BOOST_PROGOPTIONS_LIB@ \
		@BOOST_SIGNALS_LIB@ @BOOST_SYSTEM_LIB@ \
		-lFreeCADBase \
		-l@PYTHON_LIB@ \
		-lxerces-c

# set the include path found by configure
INCLUDES= -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes)

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



