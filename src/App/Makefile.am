
lib_LTLIBRARIES=libFreeCADApp.la

BUILT_SOURCES=\
		InitScript.h \
		TestScript.h \
		Plugin.h \
		Standard.h

libFreeCADApp_la_SOURCES=\
		Application.cpp \
		Application.h \
		ApplicationPy.cpp \
		Attribute.cpp \
		Attribute.h \
		DataWithProperty.cpp \
		DataWithProperty.h \
		DocTypeAttr.cpp \
		DocTypeAttr.h \
		Document.cpp \
		Document.h \
		DocumentPy.cpp \
		DocumentPy.h \
		Doxygen.cpp \
		Feature.cpp \
		Feature.h \
		FeatureAttr.cpp \
		FeatureAttr.h \
		FeatureDataProvider.cpp \
		FeatureDataProvider.h \
		FeatureDataProviderPy.cpp \
		FeatureDataProviderPy.h \
		FeaturePy.cpp \
		FeaturePy.h \
		Function.cpp \
		Function.h \
		Label.cpp \
		Label.h \
		Material.cpp \
		Material.h \
		MaterialPy.cpp \
		MaterialPy.h \
		PreCompiled.cpp \
		PreCompiled.h \
		Property.cpp \
		Property.h \
		PropertyAttr.cpp \
		PropertyAttr.h

%Script.h: FreeCAD%.py
	$(PYTHON) $(top_srcdir)/Tools/PythonToCPP.py $(<F) $@

Standard.h: Standard
	$(PYTHON) $(top_srcdir)/Tools/PythonToCPP.py $(<F) $@

Plugin.h: Plugin
	$(PYTHON) $(top_srcdir)/Tools/PythonToCPP.py $(<F) $@

# the library search path.
libFreeCADApp_la_LDFLAGS = -L../Base $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libFreeCADApp_la_LIBADD   = \
		-lTKernel \
		-lTKMath \
		-lTKService \
		-lTKGeomAlgo \
		-lTKGeomBase \
		-lTKG2d \
		-lTKG3d \
		-lTKBRep \
		-lTKTopAlgo \
		-lTKCAF \
		-lTKCDF \
		-lFreeCADBase

# set the include path found by configure
INCLUDES= $(all_includes)

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		DllMain.cpp \
		Libs.cpp \
		FreeCADDoc.dsp \
		FreeCADDoc.vcproj \
		FreeCADInit.py \
		FreeCADTest.py \
		Plugin \
		Standard



