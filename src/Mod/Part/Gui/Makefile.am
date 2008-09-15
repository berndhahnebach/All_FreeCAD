if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libPartGui.la PartGui.la

BUILT_SOURCES=\
		ui_DlgBooleanOperation.h \
		ui_DlgPartBox.h \
		ui_DlgPartCylinder.h \
		ui_DlgPartImportIges.h \
		ui_DlgPartImportStep.h \
		ui_DlgPrimitives.h \
		ui_DlgSettings3DViewPart.h \
		moc_DlgBooleanOperation.cpp \
		moc_DlgPartBoxImp.cpp \
		moc_DlgPartCylinderImp.cpp \
		moc_DlgPartImportIgesImp.cpp \
		moc_DlgPartImportStepImp.cpp \
		moc_DlgSettings3DViewPartImp.cpp \
		qrc_Part.cpp 

libPartGui_la_SOURCES=\
		Command.cpp \
		CommandSimple.cpp \
		DlgBooleanOperation.cpp \
		DlgBooleanOperation.h \
		DlgPartBoxImp.cpp \
		DlgPartBoxImp.h \
		DlgPartCylinderImp.cpp \
		DlgPartCylinderImp.h \
		DlgPartImportIgesImp.cpp \
		DlgPartImportIgesImp.h \
		DlgPartImportStepImp.cpp \
		DlgPartImportStepImp.h \
		DlgPrimitives.cpp \
		DlgPrimitives.h \
		DlgSettings3DViewPartImp.cpp \
		DlgSettings3DViewPartImp.h \
		PreCompiled.cpp \
		PreCompiled.h \
		SoFCShapeObject.cpp \
		ViewProvider.cpp \
		ViewProviderBox.cpp \
		ViewProviderCurveNet.cpp \
		ViewProviderImport.cpp \
		ViewProviderExtrusion.cpp \
		Workbench.cpp

include_HEADERS=\
		SoFCShapeObject.h \
		ViewProvider.h \
		ViewProviderBox.h \
		ViewProviderCurveNet.h \
		ViewProviderImport.h \
		ViewProviderExtrusion.h \
		Workbench.h

# the library search path.
libPartGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(sim_ac_coin_ldflags) $(sim_ac_coin_libs) $(sim_ac_soqt_ldflags) $(sim_ac_soqt_libs) -L$(OCC_LIB) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPartGui_la_CPPFLAGS = -DAppPartExport= -DAppPartGuiExport=

if OCC_VERSION6
libPartGui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-lxerces-c \
		-l@PYTHON_LIB@ \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lTKernel \
		-lTKG2d \
		-lTKG3d \
		-lTKMath \
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
	$(QT_MOC) $< -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	$(QT_MOC) $< -o $(@F)

# rules for Qt User Interface Compiler:
ui_%.h: %.ui
	$(QT_UIC) $< -o $(@F)

# rules for Qt Resource Compiler:
qrc_%.cpp: %.qrc
	$(QT_RCC) -name $(*F) $< -o $(@F)

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I$(OCC_INC) -I$(top_srcdir)/src -I$(top_builddir)/src

endif

includedir = @includedir@/Mod/Part/Gui
libdir = $(prefix)/Mod/Part

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		Icons/Part_Box.svg \
		Icons/Part_Cylinder.svg \
		Icons/Part_Extrude.svg \
		Icons/PartFeature.xpm \
		Icons/PartFeatureImport.xpm \
		Icons/preferences-part_design.svg \
		DlgBooleanOperation.ui \
		DlgPartBox.ui \
		DlgPartCylinder.ui \
		DlgPartImportIges.ui \
		DlgPartImportStep.ui \
		DlgPrimitives.ui \
		DlgSettings3DViewPart.ui \
		Part.qrc \
		Part_de.qm \
		Part_de.ts \
		Part.qrc \
		AppPartGui.vcproj
