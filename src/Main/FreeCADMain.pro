TEMPLATE = app
DEFINES += _FC_GUI_ENABLED_
unix {
  OBJECTS_DIR = .obj
  DEFINES += HAVE_CONFIG_H BaseExport= AppExport= GuiExport= 
}
HEADERS = PreCompiled.h
SOURCES = Main.cpp PreCompiled.cpp

CONFIG = warn_off qt thread

unix {
LIBS = -L/usr/local/lib  -L/opt/OCC/Cas5.1/ros/Linux/lib -L../../bin\
		-lqui\
		-lxerces-c \
		-lpython2.3 \
		-lTKernel \
		-lTKMath \
		-lTKService \
		-lTKGeomAlgo \
		-lTKGeomBase \
		-lTKG2d \
		-lTKG3d \
		-lTKBRep \
		-lTKTopAlgo \
		-lTKPrim \
		-lTKV2d \
		-lTKV3d \
		-lTKCAF \
		-lTKCDF \
		-lCoin \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui
}

unix {
INCLUDEPATH += 	/usr/local/include/python2.3 \
				/opt/OCC/Cas5.1/ros/inc ../.
}

unix {
	system(python ../Tools/PythonToCPP.py FreeCADInit.py InitScript.h)
	!exists( InitScript.h ){
		error("Couldn't create file InitScript.h")
	}
	message ("Created TestScript.h")
	system(python ../Tools/PythonToCPP.py FreeCADTest.py TestScript.h)
	!exists( TestScript.h ){
		error("Couldn't create file TestScript.h")
	}
	message ("Created TestScript.h")
	system(python ../Tools/PythonToCPP.py FreeCADGuiInit.py GuiInitScript.h)
	!exists( GuiInitScript.h ){
		error("Couldn't create file GuiInitScript.h")
	}
	message ("Created GuiInitScript.h")
}

DESTDIR = ../../bin
TARGET   = FreeCAD
