
lib_LTLIBRARIES=libFreeCADApp.la

BUILT_SOURCES=\
		InitScript.h \
		TestScript.h

libFreeCADApp_la_SOURCES=\
		Application.cpp \
		Application.h \
		ApplicationPy.cpp \
		ColorModel.cpp \
		ColorModel.h \
		DataWithProperty.cpp \
		DataWithProperty.h \
		Document.cpp \
		Document.h \
		DocumentObject.cpp \
		DocumentObject.h \
		DocumentPy.cpp \
		DocumentPy.h \
		Doxygen.cpp \
		Feature.cpp \
		Feature.h \
		FeaturePy.cpp \
		FeaturePy.h \
		FeatureTest.cpp \
		FeatureTest.h \
		Material.cpp \
		Material.h \
		MaterialPy.cpp \
		MaterialPy.h \
		MatrixPy.cpp \
		MatrixPy.h \
		PreCompiled.cpp \
		PreCompiled.h \
		Property.cpp \
		Property.h \
		PropertyGeo.cpp \
		PropertyGeo.h \
		PropertyContainer.cpp \
		PropertyContainer.h \
		PropertyLinks.cpp \
		PropertyLinks.h \
		PropertyStandard.cpp \
		PropertyStandard.h \
		VectorPy.cpp \
		VectorPy.h

%Script.h: FreeCAD%.py
	$(PYTHON) $(top_srcdir)/Tools/PythonToCPP.py $(<F) $@

# the library search path.
libFreeCADApp_la_LDFLAGS = -L../Base $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libFreeCADApp_la_LIBADD   = \
		-lFreeCADBase -l@PYTHON_LIB@ -lxerces-c

# set the include path found by configure
INCLUDES= $(all_includes)

# set additional defines (for cygwin only)
AM_CPPFLAGS=-DFCApp

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		DllMain.cpp \
		Libs.cpp \
		FreeCADDoc.dsp \
		FreeCADDoc.vcproj \
		FreeCADInit.py \
		FreeCADTest.py



