all: build

include ../../Makefile.cfg

INCLUDE = -I$(QTDIR)/include 

MODULES = 
build: $(MODULES)


 
DlgBlock.cpp:moc_DlgBlock.cpp
moc_DlgBlock.cpp:DlgBlock.h
	$(MOC) DlgBlock.h -o moc_DlgBlock.cpp

