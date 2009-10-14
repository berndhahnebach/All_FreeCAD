
lib_LTLIBRARIES=lib_TEMPLATE_Gui.la _TEMPLATE_Gui.la

lib_TEMPLATE_Gui_la_SOURCES=\
		App_TEMPLATE_GuiPy.cpp \
		Command.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
lib_TEMPLATE_Gui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(all_libraries) \
		-version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

lib_TEMPLATE_Gui_la_CPPFLAGS = -D_TEMPLATE_AppExport= -D_TEMPLATE_GuiExport=

lib_TEMPLATE_Gui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-l_TEMPLATE_

#--------------------------------------------------------------------------------------
# Loader of lib_TEMPLATE_Gui

_TEMPLATE_Gui_la_SOURCES=\
		App_TEMPLATE_Gui.cpp

# the library search path.
_TEMPLATE_Gui_la_LDFLAGS = $(lib_TEMPLATE_Gui_la_LDFLAGS) -module -avoid-version
_TEMPLATE_Gui_la_CPPFLAGS = $(lib_TEMPLATE_Gui_la_CPPFLAGS)

_TEMPLATE_Gui_la_LIBADD   = \
		$(lib_TEMPLATE_Gui_la_LIBADD) \
		-l_TEMPLATE_Gui

_TEMPLATE_Gui_la_DEPENDENCIES = lib_TEMPLATE_Gui.la

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
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(QT_CXXFLAGS) $(all_includes)

libdir = $(prefix)/Mod/_TEMPLATE_

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		CMakeLists.txt
