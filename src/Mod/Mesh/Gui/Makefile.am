if OCC_LIBRARY_FILES
if WM3_LIBRARY_FILES
if GTS_LIBRARY_FILES

lib_LTLIBRARIES=libMeshGui.la MeshGui.la

BUILT_SOURCES=\
		ui_DlgEvaluateMesh.h \
		ui_DlgRegularSolid.h \
		moc_DlgEvaluateMeshImp.cpp \
		moc_DlgRegularSolidImp.cpp \
		qrc_Mesh.cpp 

libMeshGui_la_SOURCES=\
		Command.cpp \
		DlgEvaluateMeshImp.cpp \
		DlgEvaluateMeshImp.h \
		DlgRegularSolidImp.cpp \
		DlgRegularSolidImp.h \
		Doxygen.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		PropertyEditorMesh.cpp \
		PropertyEditorMesh.h \
		SoFCMeshFaceSet.cpp \
		SoFCMeshFaceSet.h \
		SoFCMeshNode.cpp \
		SoFCMeshNode.h \
		SoFCMeshVertex.cpp \
		SoFCMeshVertex.h \
		ViewProvider.cpp \
		ViewProvider.h \
		ViewProviderMeshNode.cpp \
		ViewProviderMeshNode.h \
		ViewProviderMeshFaceSet.cpp \
		ViewProviderMeshFaceSet.h \
		ViewProviderCurvature.cpp \
		ViewProviderCurvature.h \
		ViewProviderDefects.cpp \
		ViewProviderDefects.h \
		ViewProviderTransform.cpp \
		ViewProviderTransform.h \
		ViewProviderTransformDemolding.cpp \
		ViewProviderTransformDemolding.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libMeshGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(sim_ac_coin_ldflags) $(sim_ac_coin_libs) $(sim_ac_soqt_ldflags) $(sim_ac_soqt_libs) -L$(OCC_LIB) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libMeshGui_la_CPPFLAGS = -DAppMeshExport= -DGuiMeshExport=

libMeshGui_la_LIBADD   = \
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

#--------------------------------------------------------------------------------------

# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	$(QT_MOC) $< -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	$(QT_MOC) $< -o $(@F)

# rules for Qt User Interface Compiler:
# (it adds path to #include automatically, so cd before calling it)
ui_%.h: %.ui
	$(QT_UIC) $< -o $(@F)

# rules for Qt Resource Compiler:
# (it adds path to #include automatically, so cd before calling it)
qrc_%.cpp: %.qrc
	$(QT_RCC) -name $(*F) $< -o $(@F)

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) $(WM3_CFLAGS) $(GTS_CFLAGS) -I$(OCC_INC) -I$(top_srcdir)/src -I$(top_builddir)/src

endif
endif
endif

libdir = $(prefix)/Mod/Mesh

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		AppMeshGui.vcproj \
		DlgEvaluateMesh.ui \
		DlgRegularSolid.ui \
		Mesh.qrc \
		Mesh_de.qm \
		Mesh_de.ts \
		images.h
