# This Makefile was created at RedHat Linux 7.3.
# Kernel 2.4.18-4.
# g++ reports version 2.96.
# Qt is 2.3.1.
# qextmdi 2.0.2.
# OpenCascade 4.0. 
# Python 2.2
#
# It should suffice to following paths:
QTDIR=/usr/lib/qt-2.3.1
QTMDIDIR=$(HOME)/software/qextmdi
OCCDIR=$(HOME)/OCC
PYINC=/usr/include/python2.2
PYLIBS=-L/usr/lib/python2.2/config -lpython2.2 
PYTHON=python2

CC=gcc
CXX=g++
CFLAGS=$(CPPFLAGS) -g -Wall -I$(PYINC)
CXXFLAGS=$(CPPFLAGS) -g -Wall -I$(OCCINC) -I$(PYINC) -I$(QTINC) -I$(QTMDIINC)
# -DNO_KDE is needed by qextmdi, unless you compiled it aginast KDE 2
# stlport is for VC++ compatibility
# _MAX_PATH is used in Base/Interpreter.cpp
# AppExport, GuiExport, DocExport are #defined in Base/Export.h
#   but this file is included not everywhere where it would be appropriate;
#   this shortcut is only temporary
CPPFLAGS=-Dstlport=std -D_MAX_PATH=1024 \
         -DAppExport=  -DGuiExport=  -DDocExport= -DNO_KDE2
LDFLAGS= #-Wl,--trace -Wl,--verbose          
         
SYSLIBS=-lm -lpthread -lutil

MOC=$(QTDIR)/bin/moc
UIC=$(QTDIR)/bin/uic

QTINC=$(QTDIR)/include
QTLIBS=-L$(QTDIR)/lib -lqt

QTMDIINC=$(QTMDIDIR)/include
QTMDILIBS=-L$(QTMDIDIR)/lib -lqextmdi

OCCINC=$(OCCDIR)/inc

# Many of them are superfluous, remove them if you know which ones...
OCCLIBS=-L$(OCCDIR)/Linux/lib \
-lFWOSPlugin \
-lmscmd \
-lPAppStdPlugin \
-lPTKernel \
-lTKBool \
-lTKBRep \
-lTKCAF \
-lTKCDF \
-lTKCDLFront \
-lTKCPPClient \
-lTKCPPExt \
-lTKCPPIntExt \
-lTKCPPJini \
-lTKCSFDBSchema \
-lTKDCAF \
-lTKDrawDEB \
-lTKDraw \
-lTKernel \
-lTKFeat \
-lTKFillet \
-lTKGeomAlgo \
-lTKGeomBase \
-lTKG2d \
-lTKG3d \
-lTKHLR \
-lTKIDLFront \
-lTKIGES \
-lTKIGESStd \
-lTKjcas \
-lTKMath \
-lTKOffset \
-lTKOpenGl \
-lTKPAppStd \
-lTKPCAF \
-lTKPrim \
-lTKPShape \
-lTKService \
-lTKShapeSchema \
-lTKShHealing \
-lTKShHealingStd \
-lTKStdSchema \
-lTKSTEP \
-lTKSTEPStd \
-lTKSTL \
-lTKTCPPExt \
-lTKTopAlgo \
-lTKViewerTest \
-lTKVRML \
-lTKV2d \
-lTKV3d \
-lTKWOK \
-lTKWOKTcl \
-lTKXCAFSchema \
-lTKXCAF \
-lTKXDEDRAW \
-lTKXDEIGES \
-lTKXDESTEP \
-lTKXSBase \
-lTKXSDRAW \
-lXCAFPlugin


OBJECTS= \
App/Application.o App/Attribute.o App/Document.o \
\
Base/Console.o Base/Exception.o Base/Interpreter.o \
Base/Parameter.o Base/PyExport.o \
Base/PyTools.o \
\
Gui/Command.o Gui/CommandStd.o  \
Gui/DlgDocTemplates.o Gui/DlgMacros.o \
Gui/DlgDocTemplatesImp.o Gui/DlgUndoRedo.o \
Gui/GuiConsole.o Gui/MouseModel.o Gui/Themes.o Gui/Tree.o \
Gui/View.o Gui/View3D.o Gui/Application.o Gui/Document.o \
Gui/DlgDocTemplates_moc.o Gui/DlgMacros_moc.o \
\
Main/Main.o Main/DlgSplasher.o Main/DlgSplasher_moc.o

# only DlgDocTemplates_moc.cpp, DlgMacros_moc.cpp and DlgSplasher
# are compiled directly; the other ones are
# #included into their according cpp file
MOCFILES= \
Gui/Application_moc.cpp Gui/Command_moc.cpp Gui/DlgDocTemplates_moc.cpp \
Gui/DlgDocTemplatesImp_moc.cpp Gui/DlgMacros_moc.cpp Gui/DlgUndoRedo_moc.cpp \
Gui/Document_moc.cpp Gui/Tree_moc.cpp Gui/View_moc.cpp Gui/View3D_moc.cpp \
Main/DlgSplasher_moc.cpp

# use of DlgSplasher is commented out in the source,
# nonetheless we compile it too (for future compatibility ;) )
UICHEADERS= Gui/DlgMacros.h Gui/DlgDocTemplates.h Main/DlgSplasher.h
UICFILES= Gui/DlgMacros.cpp Gui/DlgDocTemplates.cpp Main/DlgSplasher.cpp


# Currently not used!
## These headers contain python scripts in const char[]
#PYSCRIPTHEADERS=Main/InitScript.h Main/TestScript.h Main/TestEnvScript.h


EXECUTABLE=./freecad

# rules for .c.o, .cpp.o are defined by make.

# rule for including python scripts in Main as header files
%Script.h: FreeCAD%.py
	cd $(@D) && $(PYTHON) ../Tools/PythonToCPP.py $(<F) $(@F)
        


# rules for Qt User Interface Compiler:
# (it unfortunately adds path to #include automatically,
# so we need to cd before calling it)
%.h: %.ui
	cd $(@D) && $(UIC) $(<F) -o $(@F)
%.cpp: %.h %.ui 
	cd $(@D) && $(UIC) -o $(@F) -impl $(*F).h $(*F).ui

# rule for Qt MetaObject Compiler:
%_moc.cpp: %.h
	cd $(@D) && $(MOC) $(<F) -o $(@F)
        
# Application and Document are *.cxx (why?...)
%.o: %.cxx
	$(CXX) -c $< -o $@ $(CPPFLAGS) $(CXXFLAGS)

all: $(PYSCRIPTHEADERS) $(UICHEADERS) $(UICFILES) $(MOCFILES) $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(SYSLIBS) $(OCCLIBS) $(PYLIBS) $(QTMDILIBS) $(QTLIBS) -o $(EXECUTABLE)

clean:
	rm -Rf core $(OBJECTS) $(MOCFILES) $(UICFILES) $(UICHEADERS) $(PYSCRIPTHEADERS) $(EXECUTABLE)
