all: build

include ../Makefile.cfg

MODULES = View3D.o   Tree.o
SOURCES = View3D.cpp Tree.cpp


build: $(MODULES)

View3D.cpp:View3D.moc
View3D.moc:View3D.h

Tree.cpp:Tree.moc
Tree.moc:Tree.h


include depends


