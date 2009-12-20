SUBDIRS=Resources

lib_LTLIBRARIES=libFemGui.la FemGui.la

#BUILT_SOURCES=\
#		moc_TaskFem6Axis.cpp \
#		moc_TaskTrajectory.cpp \
#		ui_TaskFem6Axis.h \
#		ui_TaskTrajectory.h

#libFemGui_la_UI=\
#		TaskFem6Axis.ui \
#		TaskTrajectory.ui

libFemGui_la_SOURCES=\
		AppFemGuiPy.cpp \
		Command.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		ViewProviderFemMeshObject.cpp \
		ViewProviderFemMeshObject.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libFemGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App \
		$(QT_LIBS) $(GL_LIBS) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libFemGui_la_CPPFLAGS = -DFemAppExport= -DFemGuiExport=

libFemGui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lFem

#--------------------------------------------------------------------------------------
# Loader of libFemGui

FemGui_la_SOURCES=\
		AppFemGui.cpp

# the library search path.
FemGui_la_LDFLAGS = $(libFemGui_la_LDFLAGS) -module -avoid-version
FemGui_la_CPPFLAGS = $(libFemGui_la_CPPFLAGS)

FemGui_la_LIBADD   = \
		$(libFemGui_la_LIBADD) \
		Resources/libResources.la \
		-lFemGui

FemGui_la_DEPENDENCIES = libFemGui.la

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
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(QT_CXXFLAGS) $(all_includes) -I/usr/include/eigen2


libdir = $(prefix)/Mod/Fem

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		$(libFemGui_la_UI) \
		CMakeLists.txt

