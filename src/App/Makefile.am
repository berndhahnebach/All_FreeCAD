
lib_LTLIBRARIES=libFreeCADApp.la

BUILT_SOURCES=\
	InitScript.h \
	TestScript.h

libFreeCADApp_la_SOURCES=\
		Application.h \
		Attribute.h \
		DocType.h \
		DocTypeAttr.h \
		Document.h \
		Feature.h \
		FeatureAttr.h \
		Function.h \
		Label.h \
		Parameter.h \
		PreCompiled.h \
		Property.h \
    PropertyAttr.h \
		Topology.h \
		Application.cpp \
		Attribute.cpp \
		DocType.cpp \
		DocTypeAttr.cpp \
		Document.cpp \
		Feature.cpp \
		FeatureAttr.cpp \
		Function.cpp \
		Label.cpp \
		PreCompiled.cpp \
		Property.cpp \
    PropertyAttr.cpp \
		Topology.cpp

%Script.h: FreeCAD%.py
	$(PYTHON) ../Tools/PythonToCPP.py $(<F) $@

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
