
lib_LTLIBRARIES=lib_TEMPLATE_Gui.la

# BUILT_SOURCES=\
#
lib_TEMPLATE_Gui_la_SOURCES=\
		App_TEMPLATE_Gui.cpp \
		Command.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
lib_TEMPLATE_Gui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
lib_TEMPLATE_Gui_la_CPPFLAGS = -D_TEMPLATE_AppExport= -D_TEMPLATE_GuiExport=

lib_TEMPLATE_Gui_la_LIBADD   = \
		-lFreeCADGui \
		-l_TEMPLATE_

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

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I../../../ -I../App

libdir = $(prefix)/Mod/_TEMPLATE_

# We need this softlink for Python to load
install-data-local:
		cd $(DESTDIR)$(libdir) && \
		rm -f _TEMPLATE_Gui.so && \
		$(LN_S) lib_TEMPLATE_Gui.so _TEMPLATE_Gui.so

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		App_TEMPLATE_Gui.dsp \
		Libs.cpp
