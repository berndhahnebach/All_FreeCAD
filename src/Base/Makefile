all: build

include ../Makefile.cfg

CXXSOURCES = \
Console.cpp \
Exception.cpp \
Factory.cpp \
Handle.cpp \
Interpreter.cpp \
Observer.cpp\
Parameter.cpp \
PyExport.cpp 

MODULES = $(patsubst %.cpp,%.o,$(CXXSOURCES)) $(patsubst %.c,%.o,$(CSOURCES))

default: build

build: $(MODULES)


clean:
	rm -rf $(MODULES)



#include depends

