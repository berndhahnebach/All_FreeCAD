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
QTMDIDIR=$(HOME)/software/qextmdi.g++2.9
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

CSOURCES=Base/PyTools.c

CXXSOURCES= \
App/Application.cpp \
App/Attribute.cpp \
App/Document.cpp \
\
Base/Console.cpp \
Base/Exception.cpp \
Base/Factory.cpp \
Base/Handle.cpp \
Base/Interpreter.cpp \
Base/Observer.cpp\
Base/Parameter.cpp \
Base/PyExport.cpp \
\
Gui/Application.cpp \
Gui/ButtonGroup.cpp \
Gui/CommandLine.cpp \
Gui/Command.cpp \
Gui/CommandStd.cpp  \
Gui/CommandView.cpp \
Gui/DlgCustomizeImp.cpp \
Gui/DlgDocTemplatesImp.cpp \
Gui/DlgMacroExecuteImp.cpp \
Gui/DlgMacroRecordImp.cpp \
Gui/DlgMacros.cpp \
Gui/DlgParameterImp.cpp \
Gui/DlgPreferencesImp.cpp \
Gui/DlgSettingsImp.cpp \
Gui/DlgUndoRedo.cpp \
Gui/Document.cpp \
Gui/GuiConsole.cpp \
Gui/HtmlView.cpp \
Gui/moc_DlgMacros.cpp \
Gui/MouseModel.cpp \
Gui/PrefWidgets.cpp \
Gui/Splashscreen.cpp \
Gui/Themes.cpp \
Gui/Tree.cpp \
Gui/View.cpp \
Gui/View3D.cpp \
Gui/Window.cpp \
Gui/WidgetFactory.cpp \
Gui/Widgets.cpp \
\
Main/DlgSplasher.cpp \
Main/Main.cpp \
Main/moc_DlgSplasher.cpp

OBJECTS=$(patsubst %.cpp,%.o,$(CXXSOURCES)) $(patsubst %.c,%.o,$(CSOURCES))


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
Gui/moc_DlgCustomizeImp.cpp \
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
Gui/moc_DlgSettings.cpp \
Gui/moc_DlgSettingsImp.cpp \
Gui/moc_DlgUndoRedo.cpp \
Gui/moc_Document.cpp \
Gui/moc_HtmlView.cpp \
Gui/moc_PrefWidgets.cpp \
Gui/moc_Splashscreen.cpp \
Gui/moc_Tree.cpp \
Gui/moc_View.cpp\
Gui/moc_View3D.cpp \
Gui/moc_WidgetFactory.cpp \
Gui/moc_Widgets.cpp \
Gui/moc_Window.cpp \
Main/moc_DlgSplasher.cpp

UICHEADERS=\
Gui/DlgCustomize.h\
Gui/DlgDocTemplates.h \
Gui/DlgMacroExecute.h\
Gui/DlgMacroRecord.h \
Gui/DlgMacros.h \
Gui/DlgParameter.h \
Gui/DlgPreferences.h \
Gui/DlgSettings.h \
Main/DlgSplasher.h

UICFILES=\
Gui/DlgCustomize.cpp\
Gui/DlgDocTemplates.cpp \
Gui/DlgMacroExecute.cpp \
Gui/DlgMacroRecord.cpp \
Gui/DlgMacros.cpp \
Gui/DlgParameter.cpp \
Gui/DlgPreferences.cpp \
Gui/DlgSettings.cpp \
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

dep:
	$(CXX) -MM -H $(CXXSOURCES) > Makefile.dep


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
moc_%.cpp: %.h
	cd $(@D) && $(MOC) $(<F) -o $(@F)
        
# Application and Document are *.cxx (why?...)
%.o: %.cxx
	$(CXX) -c $< -o $@ $(CPPFLAGS) $(CXXFLAGS)

all: $(PYSCRIPTHEADERS) $(UICHEADERS) $(UICFILES) $(MOCFILES) $(CXXSOURCES) $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(SYSLIBS) $(OCCLIBS) $(PYLIBS) $(QTMDILIBS) $(XERCESCLIBS) $(QTLIBS)  -o $(EXECUTABLE)

clean:
	rm -Rf core $(OBJECTS) $(MOCFILES) $(UICFILES) $(UICHEADERS) $(PYSCRIPTHEADERS) $(EXECUTABLE)
	echo > Makefile.dep

include Makefile.dep