all: build

include ../Makefile.cfg

CXXSOURCES = \
Application.cpp \
Attribute.cpp \
Document.cpp

MODULES = $(patsubst %.cpp,%.o,$(CXXSOURCES)) $(patsubst %.c,%.o,$(CSOURCES))

LINKAGE = -L../Base -lFreeCADBase.so
#LINKAGE = -L/home/riegel/FC-Libs/lib -lpython21

default: build

build: $(MODULES)
	$(CPP)  $(MODULES) $(LINKAGE) -o FreeCADApp.so

clean:
	rm -rf $(MODULES) Makefile.dep
	touch Makefile.dep

dep:
	$(CXX) -MM -H $(CXXFLAGS) $(CXXSOURCES) $(INCLUDE) > Makefile.dep



#include Makefile.dep


