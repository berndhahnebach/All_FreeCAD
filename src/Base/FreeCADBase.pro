TEMPLATE = lib
unix {
  OBJECTS_DIR = .obj
  DEFINES += HAVE_CONFIG_H BaseExport=
}
HEADERS = 	Console.h \
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
		PyTools.h

SOURCES = 	Console.cpp \
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
		PyTools.c

CONFIG = warn_off 

unix {
LIBS = -L/usr/local/lib -lpython2.3 -lxerces-c
}

unix {
INCLUDEPATH = /usr/local/include/python2.3 \
			 /usr/local/include
}

DESTDIR = ../../bin
