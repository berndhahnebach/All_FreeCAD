bin_PROGRAMS=freecad
freecad_SOURCES= \
	Main.cpp \
	PreCompiled.cpp


BUILT_SOURCES=\
	InitScript.h \
	TestScript.h \
	GuiInitScript.h

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

# the library search path.
freecad_LDFLAGS = -L../Base -L../App -L../Gui $(all_libraries)

LDADD=\
	-lFreeCADBase \
	-lFreeCADApp \
	-lFreeCADGui \
	-lpython@fc_py_ver@ \
	-lxerces-c \
	-lqt-mt \
	-lqui \
	-lCoin \
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
