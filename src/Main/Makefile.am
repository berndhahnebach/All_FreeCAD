bin_PROGRAMS=fc
fc_SOURCES= \
	Libs.cpp \
	Main.cpp


BUILT_SOURCES=\
	DlgSplasher.h \
	moc_DlgSplasher.cpp \
	\
	InitScript.h \
	TestScript.h \
	TestEnvScript.h \
	StartupScript.h \
	InstallScript.h

# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	cd $(@D) && $(MOC) $(<F) -o $(@F)

# rules for Qt User Interface Compiler:
# (it adds path to #include automatically, so cd before calling it)
%.h: %.ui
	cd $(@D) && $(UIC) $(<F) -o $(@F)
%.cpp: %.h %.ui 
	cd $(@D) && $(UIC) -o $(@F) -impl $(*F).h $(*F).ui

%Script.h: FreeCAD%.py
	../Tools/PythonToCPP.py $(<F) $@



#OCCLIBS=-L$(OCCDIR)/Linux/lib -L. 

LDADD=\
	../Base/libFreeCADBase.a \
	../App/libFreeCADApp.a \
	../Gui/libFreeCADGui.a \
	-lpython@fc_py_ver@ \
	-lxerces-c \
-lPAppStdPlugin \
-lPTKernel \
-lTKBO \
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
-lTKMath \
-lTKOffset \
-lTKPCAF \
-lTKPrim \
-lTKPShape \
-lTKService \
-lTKShapeSchema \
-lTKShHealing \
-lTKStdSchema \
-lTKSTEP \
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

# -lTKIGESStd
# -lTKPAppStd
# -lTKShHealingStd
# -lTKSTEPStd
