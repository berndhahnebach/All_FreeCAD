SUBDIRS=Resources

lib_LTLIBRARIES=libPartDesignGui.la PartDesignGui.la

BUILT_SOURCES=\
		moc_TaskPadParameters.cpp \
		ui_TaskPadParameters.h

libPartDesignGui_la_UI=\
		TaskPadParameters.ui

libPartDesignGui_la_SOURCES=\
		AppPartDesignGuiPy.cpp \
		Command.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		TaskPadParameters.cpp \
		TaskPadParameters.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libPartDesignGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui \
	-L../../Part/App -L../App $(QT_LIBS) $(GL_LIBS) $(all_libraries) \
	-version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libPartDesignGui_la_CPPFLAGS = -DTEMPLATEDIR=\"$(datarootdir)/Templates\" -DPartDesignAppExport= -DPartDesignGuiExport=

libPartDesignGui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lPart \
		-lPartDesign

#--------------------------------------------------------------------------------------
# Loader of libPartDesignGui

PartDesignGui_la_SOURCES=\
		AppPartDesignGui.cpp

# the library search path.
PartDesignGui_la_LDFLAGS = $(libPartDesignGui_la_LDFLAGS) -module -avoid-version
PartDesignGui_la_CPPFLAGS = $(libPartDesignGui_la_CPPFLAGS)

PartDesignGui_la_LIBADD   = \
		$(libPartDesignGui_la_LIBADD) \
		Resources/libResources.la \
		-lPartDesignGui

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
INCLUDES=-I$(top_srcdir)/src -I$(top_builddir)/src -I$(srcdir)/.. \
		$(all_includes) $(QT_CXXFLAGS) -I$(OCC_INC) \
		-I$(sim_ac_coin_includedir) -I$(sim_ac_soqt_includedir)


libdir = $(prefix)/Mod/PartDesign

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		$(libPartDesignGui_la_UI) \
		CMakeLists.txt

