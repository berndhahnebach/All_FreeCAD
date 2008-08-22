SUBDIRS=Resources

if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libPartDesignGui.la PartDesignGui.la

BUILT_SOURCES=\
		moc_PartDesignView.cpp

libPartDesignGui_la_SOURCES=\
		AppPartDesignGuiPy.cpp \
		Command.cpp \
		PartDesignView.cpp \
		PartDesignView.h \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libPartDesignGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(GL_LIBS) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPartDesignGui_la_CPPFLAGS = -DTEMPLATEDIR=\"$(datarootdir)/Templates\" -DPartDesignAppExport= -DPartDesignGuiExport=

libPartDesignGui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
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
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I$(top_srcdir)/src -I$(top_builddir)/src

endif

libdir = $(prefix)/Mod/PartDesign

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		AppPartDesignGui.vcproj 

