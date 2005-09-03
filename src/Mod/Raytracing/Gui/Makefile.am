
lib_LTLIBRARIES=libRaytracingGui.la

BUILT_SOURCES=\
		DlgSettingsRay.cpp \
		DlgSettingsRay.h \
		moc_DlgSettingsRay.cpp \
		moc_DlgSettingsRayImp.cpp \
		FreeCADpov.h \
		Raytracing_de.h 

libRaytracingGui_la_SOURCES=\
		AppRaytracingGui.cpp \
		Command.cpp \
		DlgSettingsRayImp.cpp \
		DlgSettingsRayImp.h \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libRaytracingGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libRaytracingGui_la_CPPFLAGS = -DAppPartExport= -DFeatureRayExportPov=

libRaytracingGui_la_LIBADD   = \
		-lFreeCADGui \
		-lRaytracing

# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	cd $(@D) && $(QT_MOC) $(<F) -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	cd $(@D) && $(QT_MOC) $(<F) -o $(@F)

# rules for Qt User Interface Compiler:
# (it adds path to #include automatically, so cd before calling it)
%.h: %.ui
	cd $(@D) && $(QT_UIC) $(<F) -o $(@F)
%.cpp: %.h %.ui
	cd $(@D) && $(QT_UIC) -o $(@F) -impl $(*F).h $(*F).ui

# rule for translation files using python script :
%.h: %.ts
	$(PYTHON) ../../../Tools/tstocpp.py $(<F)  $(@F) $(*F)

FreeCADpov.h: FreeCADpov
	$(PYTHON) ../../../Tools/PythonToCPP.py $(<F) $@

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I../../../ -I../App

# We need this softlink for Python to load
install-data-local:
		cd $(prefix) && \
		rm -f RaytracingGui.so && \
		$(LN_S) lib/libRaytracingGui.so RaytracingGui.so

CLEANFILES = $(BUILT_SOURCES)
