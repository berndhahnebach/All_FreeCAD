
lib_LTLIBRARIES=lib_TEMPLATE_Gui.la _TEMPLATE_Gui.la

lib_TEMPLATE_Gui_la_SOURCES=\
		Command.cpp \
		PreCompiled.cpp \
		Workbench.cpp \
		Workbench.h

include_HEADERS = PreCompiled.h

if USE_PRECOMPILED_HEADERS
PreCompiled.h.gch: PreCompiled.h
	$(CXXCOMPILE) -xc++-header -o $@ $<

PRECOMPILED = PreCompiled.h.gch
else
PRECOMPILED =
endif

BUILT_SOURCES=\
		$(PRECOMPILED)

# the library search path.
lib_TEMPLATE_Gui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

if USE_PRECOMPILED_HEADERS
lib_TEMPLATE_Gui_la_CPPFLAGS = -D_TEMPLATE_AppExport= -D_TEMPLATE_GuiExport= -D_PreComp_=
else
lib_TEMPLATE_Gui_la_CPPFLAGS = -D_TEMPLATE_AppExport= -D_TEMPLATE_GuiExport=
endif

lib_TEMPLATE_Gui_la_LIBADD   = \
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

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I$(top_srcdir)/src -I$(srcdir)/../App

libdir = $(prefix)/Mod/_TEMPLATE_

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		App_TEMPLATE_Gui.vcproj
