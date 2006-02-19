if OCC_LIBRARY_FILES
if WM3_LIBRARY_FILES
if GTS_LIBRARY_FILES

lib_LTLIBRARIES=libMeshGui.la

# BUILT_SOURCES=\
#
libMeshGui_la_SOURCES=\
		AppMeshGui.cpp \
		Command.cpp \
		Doxygen.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		ViewProvider.cpp \
		ViewProvider.h \
		ViewProviderCurvature.cpp \
		ViewProviderCurvature.h \
		ViewProviderTransform.cpp \
		ViewProviderTransform.h \
		ViewProviderTransformDemolding.cpp \
		ViewProviderTransformDemolding.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libMeshGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App -L$(OCC_LIB) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libMeshGui_la_CPPFLAGS = -DAppMeshExport= -DGuiMeshExport=

libMeshGui_la_LIBADD   = \
		-lFreeCADGui \
		-lMesh

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
INCLUDES= $(all_includes) $(QT_CXXFLAGS) $(WM3_CFLAGS) $(GTS_CFLAGS) -I$(OCC_INC) -I../../../ -I../App

endif
endif
endif

libdir = $(prefix)/Mod/Mesh

# We need this softlink for Python to load
install-data-local:
		cd $(DESTDIR)$(libdir) && \
		rm -f MeshGui.so && \
		$(LN_S) libMeshGui.so MeshGui.so

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		AppMeshGui.dsp \
		AppMeshGui.vcproj \
		Libs.cpp \
		images.h
