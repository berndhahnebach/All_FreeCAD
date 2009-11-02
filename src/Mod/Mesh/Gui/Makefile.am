
lib_LTLIBRARIES=libMeshGui.la MeshGui.la

BUILT_SOURCES=\
		ui_DlgEvaluateMesh.h \
		ui_DlgRegularSolid.h \
		ui_DlgSettingsMeshView.h \
		moc_DlgEvaluateMeshImp.cpp \
		moc_DlgRegularSolidImp.cpp \
		moc_DlgSettingsMeshView.cpp \
		moc_PropertyEditorMesh.cpp \
		qrc_Mesh.cpp 

libMeshGui_la_SOURCES=\
		Command.cpp \
		DlgEvaluateMeshImp.cpp \
		DlgEvaluateMeshImp.h \
		DlgRegularSolidImp.cpp \
		DlgRegularSolidImp.h \
		DlgSettingsMeshView.cpp \
		DlgSettingsMeshView.h \
		Doxygen.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		PropertyEditorMesh.cpp \
		SoFCIndexedFaceSet.cpp \
		SoFCMeshObject.cpp \
		ViewProvider.cpp \
		ViewProviderMeshFaceSet.cpp \
		ViewProviderCurvature.cpp \
		ViewProviderDefects.cpp \
		ViewProviderTransform.cpp \
		ViewProviderTransformDemolding.cpp \
		Workbench.cpp

include_HEADERS=\
		PropertyEditorMesh.h \
		SoFCIndexedFaceSet.h \
		SoFCMeshObject.h \
		ViewProvider.h \
		ViewProviderMeshFaceSet.h \
		ViewProviderCurvature.h \
		ViewProviderDefects.h \
		ViewProviderTransform.h \
		ViewProviderTransformDemolding.h \
		Workbench.h

# the library search path.
libMeshGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) \
		$(sim_ac_coin_ldflags) $(sim_ac_coin_libs) $(sim_ac_soqt_ldflags) $(sim_ac_soqt_libs) \
		$(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libMeshGui_la_CPPFLAGS = -DMeshExport= -DMeshGuiExport=

libMeshGui_la_LIBADD   = \
		@BOOST_SIGNALS_LIB@ @BOOST_SYSTEM_LIB@ \
		@GL_LIBS@ \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lMesh

#--------------------------------------------------------------------------------------
# Loader of libMeshGui

MeshGui_la_SOURCES=\
		AppMeshGui.cpp

# the library search path.
MeshGui_la_LDFLAGS = $(libMeshGui_la_LDFLAGS) -module -avoid-version
MeshGui_la_CPPFLAGS = $(libMeshGui_la_CPPFLAGS)

MeshGui_la_LIBADD   = \
		$(libMeshGui_la_LIBADD) \
		-l@PYTHON_LIB@ \
		-lMeshGui

MeshGui_la_DEPENDENCIES = libMeshGui.la

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
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes) $(QT_CXXFLAGS) $(GTS_CFLAGS) \
		-I$(sim_ac_coin_includedir) -I$(sim_ac_soqt_includedir)


includedir = @includedir@/Mod/Mesh/Gui
libdir = $(prefix)/Mod/Mesh

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		Icons/mesh_cut.svg \
		Icons/mesh_boundary.svg \
		Icons/Tree_Mesh.svg \
		CMakeLists.txt \
		DlgEvaluateMesh.ui \
		DlgRegularSolid.ui \
		DlgSettingsMeshView.ui \
		Mesh.qrc \
		Mesh_de.qm \
		Mesh_it.qm \
		images.h
