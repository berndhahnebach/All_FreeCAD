SUBDIRS=Resources

lib_LTLIBRARIES=libRobotGui.la RobotGui.la

BUILT_SOURCES=\
		moc_RobotView.cpp

libRobotGui_la_SOURCES=\
		AppRobotGuiPy.cpp \
		Command.cpp \
		RobotView.cpp \
		RobotView.h \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libRobotGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(GL_LIBS) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libRobotGui_la_CPPFLAGS = -DTEMPLATEDIR=\"$(datarootdir)/Templates\" -DRobotAppExport= -DRobotGuiExport=

libRobotGui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lRobot

#--------------------------------------------------------------------------------------
# Loader of libRobotGui

RobotGui_la_SOURCES=\
		AppRobotGui.cpp

# the library search path.
RobotGui_la_LDFLAGS = $(libRobotGui_la_LDFLAGS) -module -avoid-version
RobotGui_la_CPPFLAGS = $(libRobotGui_la_CPPFLAGS)

RobotGui_la_LIBADD   = \
		$(libRobotGui_la_LIBADD) \
		Resources/libResources.la \
		-lRobotGui

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
INCLUDES= -I$(top_srcdir)/src -I$(top_builddir)/src $(QT_CXXFLAGS) $(all_includes)


libdir = $(prefix)/Mod/Robot

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		CMakeLists.txt

