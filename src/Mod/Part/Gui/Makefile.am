if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libPartGui.la PartGui.la

BUILT_SOURCES=\
		ui_DlgPartBox.h \
		ui_DlgPartImportIges.h \
		ui_DlgPartImportStep.h \
		ui_DlgSettings3DViewPart.h \
		moc_DlgPartBoxImp.cpp \
		moc_DlgPartImportIgesImp.cpp \
		moc_DlgPartImportStepImp.cpp \
		moc_DlgSettings3DViewPartImp.cpp \
		qrc_Part.cpp 

libPartGui_la_SOURCES=\
		Command.cpp \
		DlgPartBoxImp.cpp \
		DlgPartBoxImp.h \
		DlgPartImportIgesImp.cpp \
		DlgPartImportIgesImp.h \
		DlgPartImportStepImp.cpp \
		DlgPartImportStepImp.h \
		DlgSettings3DViewPartImp.cpp \
		DlgSettings3DViewPartImp.h \
		PreCompiled.cpp \
		PreCompiled.h \
		ViewProvider.cpp \
		ViewProvider.h \
		ViewProviderBox.cpp \
		ViewProviderBox.h \
		ViewProviderCurveNet.cpp \
		ViewProviderCurveNet.h \
		ViewProviderImport.cpp \
		ViewProviderImport.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libPartGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(sim_ac_coin_ldflags) $(sim_ac_coin_libs) $(sim_ac_soqt_ldflags) $(sim_ac_soqt_libs) -L$(OCC_LIB) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPartGui_la_CPPFLAGS = -DAppPartExport= -DAppPartGuiExport=

if OCC_VERSION6
libPartGui_la_LIBADD   = \
		-lxerces-c \
		-l@PYTHON_LIB@ \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lTKernel \
		-lTKG2d \
		-lTKG3d \
		-lTKMath \
		-lTKService \
		-lTKSTEP \
		-lTKIGES \
		-lTKShHealing \
		-lTKXSBase \
		-lTKBool \
		-lTKBO \
		-lTKBRep \
		-lTKTopAlgo \
		-lTKGeomAlgo \
		-lTKGeomBase \
		-lTKPrim \
		-lTKMesh \
		-lPart 
else
libPartGui_la_LIBADD   = \
		-lxerces-c \
		-l@PYTHON_LIB@ \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lTKernel \
		-lTKG2d \
		-lTKG3d \
		-lTKMath \
		-lTKService \
		-lTKSTEP \
		-lTKIGES \
		-lTKShHealing \
		-lTKXSBase \
		-lTKBool \
		-lTKBO \
		-lTKBRep \
		-lTKTopAlgo \
		-lTKGeomAlgo \
		-lTKGeomBase \
		-lTKPrim \
		-lPart 
endif

#--------------------------------------------------------------------------------------
# Loader of libPartGui

PartGui_la_SOURCES=\
		AppPartGui.cpp

# the library search path.
PartGui_la_LDFLAGS = $(libPartGui_la_LDFLAGS) -module -avoid-version
PartGui_la_CPPFLAGS = $(libPartGui_la_CPPFLAGS)

PartGui_la_LIBADD   = \
		$(libPartGui_la_LIBADD) \
		-lPartGui

#--------------------------------------------------------------------------------------

# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	cd $(@D) && $(QT_MOC) $(<F) -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	cd $(@D) && $(QT_MOC) $(<F) -o $(@F)

# rules for Qt User Interface Compiler:
# (it adds path to #include automatically, so cd before calling it)
ui_%.h: %.ui
	cd $(@D) && $(QT_UIC) $(<F) -o $(@F)

# rules for Qt Resource Compiler:
# (it adds path to #include automatically, so cd before calling it)
qrc_%.cpp: %.qrc
	cd $(@D) && $(QT_RCC) -name $(*F) $(<F) -o $(@F)

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I$(OCC_INC) -I../../../ -I../App

endif

libdir = $(prefix)/Mod/Part

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		Icons/PartFeature.xpm \
		Icons/PartFeatureImport.xpm \
		DlgPartBox.ui \
		DlgPartImportIges.ui \
		DlgPartImportStep.ui \
		DlgSettings3DViewPart.ui \
		Part_de.qm \
		Part_de.ts \
		Part.qrc \
		AppPartGui.vcproj
