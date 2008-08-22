SUBDIRS=Resources

if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libSketcherGui.la SketcherGui.la

BUILT_SOURCES=\
		moc_SketcherView.cpp

libSketcherGui_la_SOURCES=\
		AppSketcherGuiPy.cpp \
		Command.cpp \
		SketcherView.cpp \
		SketcherView.h \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libSketcherGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(GL_LIBS) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libSketcherGui_la_CPPFLAGS = -DTEMPLATEDIR=\"$(datarootdir)/Templates\" -DSketcherAppExport= -DSketcherGuiExport=

libSketcherGui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
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

libdir = $(prefix)/Mod/Sketcher

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		AppSketcherGui.vcproj 

