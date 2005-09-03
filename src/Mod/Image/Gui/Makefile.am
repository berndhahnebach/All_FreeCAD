
lib_LTLIBRARIES=libImageGui.la

BUILT_SOURCES=\
		moc_GLImageBox.cpp \
		moc_ImageView.cpp

libImageGui_la_SOURCES=\
		AppImageGui.cpp \
		Command.cpp \
		GLImageBox.cpp \
		GLImageBox.h \
		ImageView.cpp \
		ImageView.h \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libImageGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libImageGui_la_CPPFLAGS = -DImageAppExport= -DImageGuiExport= 

libImageGui_la_LIBADD   = \
		-lFreeCADGui \
		-lImage

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
		rm -f ImageGui.so && \
		$(LN_S) lib/libImageGui.so ImageGui.so

CLEANFILES = $(BUILT_SOURCES)
