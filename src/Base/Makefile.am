
pkglib_LTLIBRARIES=libFreeCADBase.la

libFreeCADBase_la_SOURCES=\
Console.cpp        Exception.h       Interpreter.cpp  ParameterPy.cpp\
Console.h          Factory.cpp       Interpreter.h    PreCompiled.cpp\
Factory.h          PreCompiled.h     EnvMacros.h\
Documentation.cpp  Observer.cpp      PyExport.cpp\
Documentation.h    Observer.h        PyExport.h\
Doxygen.cpp        Handle.cpp        Parameter.cpp    PyTools.c\
Exception.cpp      Handle.h          Parameter.h      PyTools.h\
FileTemplate.cpp   FileTemplate.h    Process.cpp      Process.h

libFreeCADBase_la_LDADD   =  -lpython2.3 -lxerces-c

#libFreeCADBase_a_LIBADD   = -L/usr/local/lib -lpython2.3 -L/usr/local/lib -lxerces-c

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
#libFreeCADBase_a_LDFLAGS = $(all_libraries)
#libFreeCADBase_a_LIBADD = $(all_libraries)

