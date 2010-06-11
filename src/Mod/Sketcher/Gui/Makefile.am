SUBDIRS=Resources

lib_LTLIBRARIES=libSketcherGui.la SketcherGui.la

BUILT_SOURCES=\
		moc_TaskSketcherConstrains.cpp \
		moc_TaskSketcherCreateCommands.cpp \
		moc_TaskSketcherGeneral.cpp \
		ui_TaskSketcherConstrains.h \
		ui_TaskSketcherGeneral.h

libSketcherGui_la_UI=\
		TaskSketcherConstrains.ui \
		TaskSketcherGeneral.ui

libSketcherGui_la_SOURCES=\
		AppSketcherGuiPy.cpp \
		Command.cpp \
		CommandCreateGeo.cpp \
		CommandConstraints.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		TaskSketcherConstrains.cpp \
		TaskSketcherConstrains.h \
		TaskSketcherCreateCommands.cpp \
		TaskSketcherCreateCommands.h \
		TaskSketcherGeneral.cpp \
		TaskSketcherGeneral.h \
		ViewProviderSketch.cpp \
		ViewProviderSketch.h \
		DrawSketchHandler.cpp \
		DrawSketchHandler.h \
		ViewProviderSketchSF.cpp \
		ViewProviderSketchSF.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libSketcherGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui \
		-L../../Part/App -L../../Part/Gui -L../App \
		$(QT_LIBS) $(GL_LIBS) $(all_libraries) \
		$(sim_ac_coin_ldflags) $(sim_ac_coin_libs) \
		$(sim_ac_soqt_ldflags) $(sim_ac_soqt_libs) \
		-version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libSketcherGui_la_CPPFLAGS = -DSketcherAppExport= -DSketcherGuiExport=

libSketcherGui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lPart \
		-lPartGui \
		-lSketcher

#--------------------------------------------------------------------------------------
# Loader of libSketcherGui

SketcherGui_la_SOURCES=\
		AppSketcherGui.cpp

# the library search path.
SketcherGui_la_LDFLAGS = $(libSketcherGui_la_LDFLAGS) -module -avoid-version
SketcherGui_la_CPPFLAGS = $(libSketcherGui_la_CPPFLAGS)

SketcherGui_la_LIBADD   = \
		$(libSketcherGui_la_LIBADD) \
		Resources/libResources.la \
		-lSketcherGui

SketcherGui_la_DEPENDENCIES = libSketcherGui.la

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
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes) $(QT_CXXFLAGS) -I$(OCC_INC) \
		-I$(sim_ac_coin_includedir) -I$(sim_ac_soqt_includedir)


libdir = $(prefix)/Mod/Sketcher

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		$(libSketcherGui_la_UI) \
		CMakeLists.txt

