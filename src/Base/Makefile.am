
lib_LTLIBRARIES=libFreeCADBase.la

libFreeCADBase_la_SOURCES=\
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
		Observer.cpp \
		Observer.h \
		Parameter.cpp \
		Parameter.h \
		ParameterPy.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		PyExport.cpp \
		PyExport.h \
		PyExportImp.cpp \
		PyExportImp.h \
		PyTools.c \
		PyTools.h \
		Sequencer.cpp \
		Sequencer.h

libFreeCADBase_la_LIBADD   =  -lpython@fc_py_ver@ -lxerces-c

# set the include path found by configure
INCLUDES= $(all_includes)

# the library version number
libFreeCADBase_la_LDFLAGS = -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
