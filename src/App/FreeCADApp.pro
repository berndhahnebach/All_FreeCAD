TEMPLATE = lib
unix {
  OBJECTS_DIR = .obj
  DEFINES += HAVE_CONFIG_H BaseExport= AppExport= 
}
HEADERS = 	Application.h \
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
		Topology.h

SOURCES = 	Application.cpp \
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

CONFIG = warn_off 

unix {
LIBS = -L/usr/local/lib  -L/opt/OCC/Cas5.1/ros/Linux/lib -L../../bin\
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
}

unix {
INCLUDEPATH = 	/usr/local/include/python2.3 \
				/opt/OCC/Cas5.1/ros/inc
}

DESTDIR = ../../bin
