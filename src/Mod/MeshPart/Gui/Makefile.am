SUBDIRS=Resources

if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libMeshPartGui.la MeshPartGui.la

BUILT_SOURCES=\
		moc_MeshPartView.cpp

libMeshPartGui_la_SOURCES=\
		AppMeshPartGuiPy.cpp \
		Command.cpp \
		MeshPartView.cpp \
		MeshPartView.h \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libMeshPartGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(GL_LIBS) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libMeshPartGui_la_CPPFLAGS = -DTEMPLATEDIR=\"$(datarootdir)/Templates\" -DMeshPartAppExport= -DMeshPartGuiExport=

libMeshPartGui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lMeshPart

#--------------------------------------------------------------------------------------
# Loader of libMeshPartGui

MeshPartGui_la_SOURCES=\
		AppMeshPartGui.cpp

# the library search path.
MeshPartGui_la_LDFLAGS = $(libMeshPartGui_la_LDFLAGS) -module -avoid-version
MeshPartGui_la_CPPFLAGS = $(libMeshPartGui_la_CPPFLAGS)

MeshPartGui_la_LIBADD   = \
		$(libMeshPartGui_la_LIBADD) \
		Resources/libResources.la \
		-lMeshPartGui

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
INCLUDES= -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes) $(QT_CXXFLAGS)

endif

libdir = $(prefix)/Mod/MeshPart

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		AppMeshPartGui.vcproj 

