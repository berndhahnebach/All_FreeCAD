
lib_LTLIBRARIES=libFreeCADBase.la

libFreeCADBase_la_SOURCES=\
		Console.h \
		Documentation.h \
		EnvMacros.h \
		Exception.h \
		Factory.h \
		FileTemplate.h \
		Handle.h \
		Interpreter.h \
		Observer.h \
		Parameter.h \
		PreCompiled.h \
		Process.h \
		PyExport.h \
		PyTools.h \
		Sequencer.h \
		Console.cpp \
		Documentation.cpp \
		Doxygen.cpp \
		Exception.cpp \
		Factory.cpp \
		FileTemplate.cpp \
		Handle.cpp \
		Interpreter.cpp \
		Observer.cpp \
		Parameter.cpp \
		ParameterPy.cpp \
		PreCompiled.cpp \
		Process.cpp \
		PyExport.cpp \
		PyTools.c \
		Sequencer.cpp

libFreeCADBase_la_LIBADD   =  -lpython@fc_py_ver@ -lxerces-c

# set the include path found by configure
INCLUDES= $(all_includes)

# the library version number
libFreeCADBase_la_LDFLAGS = -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
