
lib_LTLIBRARIES=libFreeCADBase.la

libFreeCADBase_la_SOURCES=\
		Base.cpp \
		Base.h \
		BoundBox.cpp \
		BoundBox.h \
		Console.cpp \
		Console.h \
		Documentation.cpp \
		Documentation.h \
		Doxygen.cpp \
		EnvMacros.h \
		Exception.cpp \
		Exception.h \
		Factory.cpp \
		Factory.h \
		FileInfo.cpp \
		FileInfo.h \
		FileTemplate.cpp \
		FileTemplate.h \
		Handle.cpp \
		Handle.h \
		Interpreter.cpp \
		Interpreter.h \
		Matrix.cpp \
		Matrix.h \
		Observer.cpp \
		Observer.h \
		Parameter.cpp \
		Parameter.h \
		ParameterPy.cpp \
		Persistance.cpp \
		Persistance.h \
		PreCompiled.cpp \
		PreCompiled.h \
		PyExport.cpp \
		PyExport.h \
		PyExportImp.cpp \
		PyExportImp.h \
		PyTools.c \
		PyTools.h \
		Reader.cpp \
		Reader.h \
		Sequencer.cpp \
		Sequencer.h \
		Stream.cpp \
		Stream.h \
		Swap.cpp \
		Swap.h \
		Time.cpp \
		Time.h \
		Type.cpp \
		Type.h \
		Tools2D.cpp \
		Tools2D.h \
		Vector3D.cpp \
		Vector3D.h \
		ViewProj.h \
		XMLTools.h 

libFreeCADBase_la_LIBADD   =  -lpython@fc_py_ver@ -lxerces-c

# set the include path found by configure
INCLUDES= $(all_includes)

# the library version number
libFreeCADBase_la_LDFLAGS = -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

EXTRA_DIST = \
		DllMain.cpp \
		FreeCADBase.dsp \
		FreeCADBase.vcproj \
		Libs.cpp

