
pkglib_LTLIBRARIES=libFreeCADApp.la

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
		Geometry2D.h \
		Geometry3D.h \
		Label.h \
		Parameter.h \
		PreCompiled.h \
		Properties.h \
		Topology.h \
		Application.cpp \
		Attribute.cpp \
		DocType.cpp \
		DocTypeAttr.cpp \
		Document.cpp \
		Feature.cpp \
		FeatureAttr.cpp \
		Function.cpp \
		Geometry2D.cpp \
		Geometry3D.cpp \
		Label.cpp \
		Parameter.cpp \
		PreCompiled.cpp \
		Properties.cpp \
		Topology.cpp

%Script.h: FreeCAD%.py
	$(PYTHON) ../Tools/PythonToCPP.py $(<F) $@

# the library search path.
libFreeCADApp_la_LDFLAGS = -L../Base $(all_libraries)

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
