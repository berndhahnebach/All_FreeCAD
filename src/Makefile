# This Makefile was created at Debian 3.0.0 Woody
# Kernel 2.4.18-4
# g++ reports version 2.95.4
# Qt is 2.3.1
# qextmdi 2.0.2
# OpenCascade 4.0
# Python 2.1
# Xerces-c 2.1.0-3
#
# It should suffice to following paths:
QTMDIDIR=$(HOME)/software/qextmdi
OCCDIR=$(HOME)/software/OCC
PYINC=/usr/include/python2.1
PYLIBS=-L/usr/lib/python2.1/config -lpython2.1
PYTHON=python

MOC=moc
UIC=uic
QTINC=/usr/include/qt #look for qt.h
# for some reason, my libqt-mt.so
# does not know QGLWwidget class, libqt-mt.a does however.
QTLIBS=-L/usr/lib -lqt-mt-st #ugly hack to include static version of qt

QTMDIINC=$(QTMDIDIR)/include
QTMDILIBS=-L$(QTMDIDIR)/lib -lqextmdi
XERCESCLIBS=-L/usr/local/lib -lxerces-c

OCCINC=$(OCCDIR)/inc

CC=gcc
CXX=g++
CPPFLAGS= -D_MAX_PATH=1024 -DGuiExport= -DAppExport=  -DDocExport= \
  -D_FC_GUI_ENABLED_
# -DNO_KDE2 already in Config.h
LDFLAGS= #-Wl,--verbose,--trace
# CPPFLAGS removed
CFLAGS=-pedantic  -w -I$(PYINC)
CXXFLAGS=-pedantic  -w -I$(OCCINC) -I$(QTINC) -I$(QTMDIINC) -I$(PYINC) 

# -DNO_KDE is needed by qextmdi, unless you compiled it aginast KDE 2
# stlport is for VC++ compatibility
# _MAX_PATH is used in Base/Interpreter.cpp
# AppExport, GuiExport, DocExport are #defined in Base/Export.h
#   but this file is included not everywhere where it would be appropriate;
#   this shortcut is only temporary

SYSLIBS=-lm -lpthread -lutil -lGL

# Many of them are superfluous, remove them if you know which ones...
OCCLIBS=-L$(OCCDIR)/Linux/lib -L. \
-lPAppStdPlugin \
-lPTKernel \
-lTKBool \
-lTKBRep \
-lTKCAF \
-lTKCDF \
-lTKernel \
-lTKFeat \
-lTKFillet \
-lTKGeomAlgo \
-lTKGeomBase \
-lTKG2d \
-lTKG3d \
-lTKHLR \
-lTKIGES \
-lTKIGESStd \
-lTKMath \
-lTKOffset \
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
-lTKTopAlgo \
-lTKVRML \
-lTKV2d \
-lTKV3d \
-lTKXCAF \
-lTKXCAFSchema \
-lTKXDEIGES \
-lTKXDESTEP \
-lTKXSBase

#$ -lTKOpenGl \
# -lTKjcas \
#-lTKTCPPExt \
#-lTKIDLFront \
#-lTKCSFDBSchema \
#-lTKCPPExt \
#-lTKCPPIntExt \
#-lTKCPPJini \
#-lTKCPPClient \
#-lTKCDLFront \
#-lTKWOK \
#-lTKWOKTcl \
#-lTKXSDRAW
#-lTKViewerTest \
#-lXCAFPlugin
#-lTKXDEDRAW \
#-lTKDrawDEB \
#-lFWOSPlugin \
#-lmscmd \
#-lTKDCAF \
#-lTKDraw \

OBJECTS= \
App/Application.o \
App/Attribute.o \
App/Document.o \
\
Base/Console.o \
Base/Exception.o \
Base/Handle.o \
Base/Interpreter.o \
Base/Observer.o\
Base/Parameter.o \
Base/PyExport.o \
Base/PyTools.o \
\
Gui/Application.o \
Gui/ButtonGroup.o \
Gui/CommandLine.o \
Gui/Command.o \
Gui/CommandStd.o  \
Gui/CommandView.o \
Gui/DlgCustomizeImpl.o \
Gui/DlgDocTemplatesImp.o \
Gui/DlgMacroExecuteImp.o \
Gui/DlgMacroRecordImp.o \
Gui/DlgMacros.o \
Gui/DlgParameterImp.o \
Gui/DlgPreferencesImp.o \
Gui/DlgUndoRedo.o \
Gui/Document.o \
Gui/GuiConsole.o \
Gui/HtmlView.o \
Gui/moc_DlgMacros.o \
Gui/MouseModel.o \
Gui/Splashscreen.o \
Gui/Themes.o \
Gui/Tree.o \
Gui/View.o \
Gui/View3D.o \
Gui/Window.o Gui/Widgets.o \
\
Main/DlgSplasher.o \
Main/Main.o \
Main/moc_DlgSplasher.o



# only DlgDocTemplates_moc.cpp, DlgMacros_moc.cpp and DlgSplasher
# are compiled directly; the other ones are
# #included into their according cpp file
MOCFILES= \
Gui/moc_Application.cpp \
Gui/moc_ButtonGroup.cpp \
Gui/moc_ButtonGroup.cpp \
Gui/moc_Command.cpp \
Gui/moc_CommandLine.cpp \
Gui/moc_DlgCustomize.cpp \
Gui/moc_DlgCustomizeImpl.cpp \
Gui/moc_DlgDocTemplates.cpp\
Gui/moc_DlgDocTemplatesImp.cpp \
Gui/moc_DlgMacroExecute.cpp \
Gui/moc_DlgMacroExecuteImp.cpp \
Gui/moc_DlgMacroRecord.cpp \
Gui/moc_DlgMacroRecordImp.cpp \
Gui/moc_DlgMacros.cpp \
Gui/moc_DlgParameter.cpp \
Gui/moc_DlgParameterImp.cpp \
Gui/moc_DlgPreferences.cpp \
Gui/moc_DlgPreferencesImp.cpp \
Gui/moc_DlgUndoRedo.cpp \
Gui/moc_Document.cpp \
Gui/moc_HtmlView.cpp \
Gui/moc_Splashscreen.cpp \
Gui/moc_Tree.cpp \
Gui/moc_View.cpp\
Gui/moc_View3D.cpp \
Gui/moc_Widgets.cpp \
Gui/moc_Window.cpp \
Main/moc_DlgSplasher.cpp


UICHEADERS=\
Gui/DlgCustomize.h\
Gui/DlgMacroExecute.h\
Gui/DlgMacroRecord.h \
Gui/DlgParameter.h \
Gui/DlgPreferences.h \
Main/DlgSplasher.h
#Gui/DlgMacros.h Gui/DlgDocTemplates.h

UICFILES=\
Gui/DlgCustomize.cpp\
Gui/DlgDocTemplates.cpp \
Gui/DlgMacroExecute.cpp \
Gui/DlgMacroRecord.cpp \
Gui/DlgMacros.cpp \
Gui/DlgParameter.cpp \
Gui/DlgPreferences.cpp \
Main/DlgSplasher.cpp




# These headers contain python scripts in const char[]
PYSCRIPTHEADERS=\
Main/GuiInitScript.h \
Main/InitScript.h \
Main/InstallScript.h \
Main/StartupScript.h \
Main/TestEnvScript.h \
Main/TestScript.h


EXECUTABLE=./freecad

default: all

# rules for .c.o, .cpp.o are defined by make.

# rule for including python scripts in Main as header files
%Script.h: FreeCAD%.py
	cd $(@D) && $(PYTHON) ../Tools/PythonToCPP.py $(<F) $(@F)
        
# rules for Qt User Interface Compiler:
# (it adds path to #include automatically, so cd before calling it)
%.h: %.ui
	cd $(@D) && $(UIC) $(<F) -o $(@F)
%.cpp: %.h %.ui 
	cd $(@D) && $(UIC) -o $(@F) -impl $(*F).h $(*F).ui

# rule for Qt MetaObject Compiler:
#%_moc.cpp: %.h
#	cd $(@D) && $(MOC) $(<F) -o $(@F)
moc_%.cpp: %.h
	cd $(@D) && $(MOC) $(<F) -o $(@F)
        
# Application and Document are *.cxx (why?...)
%.o: %.cxx
	$(CXX) -c $< -o $@ $(CPPFLAGS) $(CXXFLAGS)

all: $(PYSCRIPTHEADERS) $(UICHEADERS) $(UICFILES) $(MOCFILES) $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(SYSLIBS) $(OCCLIBS) $(PYLIBS) $(QTMDILIBS) $(XERCESCLIBS) $(QTLIBS)  -o $(EXECUTABLE)

clean:
	rm -Rf core $(OBJECTS) $(MOCFILES) $(UICFILES) $(UICHEADERS) $(PYSCRIPTHEADERS) $(EXECUTABLE)
