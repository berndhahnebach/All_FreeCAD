
lib_LTLIBRARIES=libPartGui.la PartGui.la

BUILT_SOURCES=\
		ui_DlgBooleanOperation.h \
		ui_DlgExtrusion.h \
		ui_DlgFilletEdges.h \
		ui_DlgRevolution.h \
		ui_DlgPartBox.h \
		ui_DlgPartCylinder.h \
		ui_DlgPartImportIges.h \
		ui_DlgPartImportStep.h \
		ui_DlgPrimitives.h \
		ui_DlgSettings3DViewPart.h \
		ui_DlgSettingsGeneral.h \
		moc_DlgBooleanOperation.cpp \
		moc_DlgExtrusion.cpp \
		moc_DlgFilletEdges.cpp \
		moc_DlgRevolution.cpp \
		moc_DlgPartBoxImp.cpp \
		moc_DlgPartCylinderImp.cpp \
		moc_DlgPartImportIgesImp.cpp \
		moc_DlgPartImportStepImp.cpp \
		moc_DlgPrimitives.cpp \
		moc_DlgSettings3DViewPartImp.cpp \
		moc_DlgSettingsGeneral.cpp \
		qrc_Part.cpp 

libPartGui_la_SOURCES=\
		Command.cpp \
		CommandSimple.cpp \
		CommandParametric.cpp \
		DlgBooleanOperation.cpp \
		DlgBooleanOperation.h \
		DlgExtrusion.cpp \
		DlgExtrusion.h \
		DlgFilletEdges.cpp \
		DlgFilletEdges.h \
		DlgRevolution.cpp \
		DlgRevolution.h \
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
		DlgSettingsGeneral.cpp \
		DlgSettingsGeneral.h \
		PreCompiled.cpp \
		PreCompiled.h \
		SoFCShapeObject.cpp \
		ViewProvider.cpp \
		ViewProviderBox.cpp \
		ViewProviderCurveNet.cpp \
		ViewProviderImport.cpp \
		ViewProviderExtrusion.cpp \
		ViewProvider2DObject.cpp \
		ViewProviderPython.cpp \
		Workbench.cpp

include_HEADERS=\
		SoFCShapeObject.h \
		ViewProvider.h \
		ViewProviderBox.h \
		ViewProviderCurveNet.h \
		ViewProviderImport.h \
		ViewProviderExtrusion.h \
		ViewProvider2DObject.h \
		ViewProviderPython.h \
		Workbench.h

# the library search path.
libPartGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) \
		$(sim_ac_coin_ldflags) $(sim_ac_coin_libs) $(sim_ac_soqt_ldflags) $(sim_ac_soqt_libs) \
		-L$(OCC_LIB) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPartGui_la_CPPFLAGS = -DAppPartExport= -DAppPartGuiExport=

libPartGui_la_LIBADD   = \
		@BOOST_SIGNALS_LIB@ \
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

PartGui_la_DEPENDENCIES = libPartGui.la

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
qrc_%.cpp: Resources/%.qrc
	$(QT_RCC) -name $(*F) $< -o $(@F)

# set the include path found by configure
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes) $(QT_CXXFLAGS) -I$(OCC_INC) \
		-I$(sim_ac_coin_includedir) -I$(sim_ac_soqt_includedir)


includedir = @includedir@/Mod/Part/Gui
libdir = $(prefix)/Mod/Part

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		Resources/icons/Part_Cylinder.svg \
		Resources/icons/Part_Box.svg \
		Resources/icons/Part_Torus.svg \
		Resources/icons/Part_Cone.svg \
		Resources/icons/Part_Sphere.svg \
		Resources/icons/Part_Booleans.svg \
		Resources/icons/Part_Cut.svg \
		Resources/icons/Part_Common.svg \
		Resources/icons/Part_Fuse.svg \
		Resources/icons/Part_Section.svg \
		Resources/icons/Part_Extrude.svg \
		Resources/icons/Part_Fillet.svg \
		Resources/icons/Part_Revolve.svg \
		Resources/icons/Part_Import.svg \
		Resources/icons/Tree_Part.svg \
		Resources/icons/preferences-part_design.svg \
		Resources/icons/PartFeature.xpm \
		Resources/icons/PartFeatureImport.xpm \
		DlgBooleanOperation.ui \
		DlgExtrusion.ui \
		DlgFilletEdges.ui \
		DlgRevolution.ui \
		DlgPartBox.ui \
		DlgPartCylinder.ui \
		DlgPartImportIges.ui \
		DlgPartImportStep.ui \
		DlgPrimitives.ui \
		DlgSettings3DViewPart.ui \
		DlgSettingsGeneral.ui \
		Resources/Part.qrc \
		Resources/translations/Part_de.qm \
		Resources/translations/Part_fr.qm \
		Resources/translations/Part_it.qm \
		Resources/translations/Part_se.qm \
		CMakeLists.txt
