
lib_LTLIBRARIES=libImportGui.la

# BUILT_SOURCES=\
#
libImportGui_la_SOURCES=\
		AppImportGui.cpp \
		Command.cpp \
		PreCompiled.cpp \
		PreCompiled.h

# the library search path.
libImportGui_la_LDFLAGS = \
		-L../../../Base \
		-L../../../App  \
		-L../../../Gui  \
		-L../../Part/Gui \
		-L../App \
		$(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libImportGui_la_CPPFLAGS = -DAppPartExport= -DAppPartGuiExport=

libImportGui_la_LIBADD   = \
		-lTKPrim \
		-lTKV2d \
		-lTKV3d \
		-lFreeCADGui \
		-lPartGui \
		-lImport

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
		rm -f ImportGui.so && \
		$(LN_S) lib/libImportGui.so ImportGui.so
