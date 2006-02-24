if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libPartGui.la

BUILT_SOURCES=\
		DlgPartBox.cpp \
		DlgPartBox.h \
		DlgPartImportIges.cpp \
		DlgPartImportIges.h \
		DlgPartImportStep.cpp \
		DlgPartImportStep.h \
		DlgSettings3DViewPart.cpp \
		DlgSettings3DViewPart.h \
		moc_DlgPartBox.cpp \
		moc_DlgPartBoxImp.cpp \
		moc_DlgPartImportIges.cpp \
		moc_DlgPartImportIgesImp.cpp \
		moc_DlgPartImportStep.cpp \
		moc_DlgPartImportStepImp.cpp \
		moc_DlgSettings3DViewPart.cpp \
		moc_DlgSettings3DViewPartImp.cpp \
		Part_de.h

libPartGui_la_SOURCES=\
		AppPartGui.cpp \
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
libPartGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App -L$(OCC_LIB) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPartGui_la_CPPFLAGS = -DAppPartExport= -DAppPartGuiExport=

libPartGui_la_LIBADD   = \
		-lFreeCADGui \
		-lPart

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
	$(PYTHON) ../../../Tools/qembed.py $(<F)  $(@F) $(*F)

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I$(OCC_INC) -I../../../ -I../App

endif

libdir = $(prefix)/Mod/Part

# We need this softlink for Python to load
install-data-local:
		cd $(DESTDIR)$(libdir) && \
		rm -f PartGui.so && \
		$(LN_S) libPartGui.so PartGui.so

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		Icons/PartFeature.xpm \
		Icons/PartFeatureImport.xpm \
		DlgPartBox.ui \
		DlgPartImportIges.ui \
		DlgPartImportStep.ui \
		DlgSettings3DViewPart.ui \
		Part_de.ts \
		AppPartGui.dsp \
		AppPartGui.vcproj \
		Libs.cpp
