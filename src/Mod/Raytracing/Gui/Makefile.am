if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libRaytracingGui.la RaytracingGui.la

BUILT_SOURCES=\
		ui_DlgSettingsRay.h \
		moc_DlgSettingsRayImp.cpp \
		FreeCADpov.h \
		qrc_Raytracing.cpp 

libRaytracingGui_la_SOURCES=\
		Command.cpp \
		DlgSettingsRayImp.cpp \
		DlgSettingsRayImp.h \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libRaytracingGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../../Part/App -L../App $(QT_LIBS) $(sim_ac_coin_ldflags) $(sim_ac_coin_libs) $(sim_ac_soqt_ldflags) $(sim_ac_soqt_libs) -L$(OCC_LIB) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libRaytracingGui_la_CPPFLAGS = -DAppPartExport= -DAppRaytracingGuiExport= -DFeatureRayExportPov=

libRaytracingGui_la_LIBADD   = \
		-lxerces-c \
		-l@PYTHON_LIB@ \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lTKernel \
		-lTKG2d \
		-lTKG3d \
		-lTKMath \
		-lTKBool \
		-lTKBRep \
		-lTKGeomAlgo \
		-lTKGeomBase \
		-lPart \
		-lRaytracing

#--------------------------------------------------------------------------------------
# Loader of libRaytracingGui

RaytracingGui_la_SOURCES=\
		AppRaytracingGui.cpp

# the library search path.
RaytracingGui_la_LDFLAGS = $(libRaytracingGui_la_LDFLAGS) -module -avoid-version
RaytracingGui_la_CPPFLAGS = $(libRaytracingGui_la_CPPFLAGS)

RaytracingGui_la_LIBADD   = \
		$(libRaytracingGui_la_LIBADD) \
		-lRaytracingGui

#--------------------------------------------------------------------------------------

# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	$(QT_MOC) $(srcdir)/$< -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	$(QT_MOC) $(srcdir)/$< -o $(@F)

# rules for Qt User Interface Compiler:
# (it adds path to #include automatically, so cd before calling it)
ui_%.h: %.ui
	$(QT_UIC) $(srcdir)/$< -o $(@F)

# rules for Qt Resource Compiler:
# (it adds path to #include automatically, so cd before calling it)
qrc_%.cpp: %.qrc
	$(QT_RCC) -name $(*F) $(srcdir)/$< -o $(@F)

FreeCADpov.h: FreeCADpov
	$(PYTHON) $(top_srcdir)/src/Tools/PythonToCPP.py $(srcdir)/$< $@

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I$(OCC_INC) -I$(top_srcdir)/src -I$(top_builddir)/src

endif

libdir = $(prefix)/Mod/Raytracing

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		DlgSettingsRay.ui \
		FreeCADpov \
		Raytracing.qrc \
		Raytracing_de.qm \
		Raytracing_de.ts \
		AppRaytracingGui.vcproj
