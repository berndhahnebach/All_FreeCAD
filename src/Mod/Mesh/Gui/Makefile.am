
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
		ViewProviderTransform.cpp \
		ViewProviderTransform.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libMeshGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
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
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I../../../ -I../App

# We need this softlink for Python to load
install-data-local:
		cd $(prefix) && \
		rm -f MeshGui.so && \
		$(LN_S) lib/libMeshGui.so MeshGui.so
