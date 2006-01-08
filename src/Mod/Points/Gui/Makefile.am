
lib_LTLIBRARIES=libPointsGui.la

BUILT_SOURCES=\
		DlgPointsRead.cpp \
		DlgPointsRead.h \
		moc_DlgPointsRead.cpp \
		moc_DlgPointsReadImp.cpp

libPointsGui_la_SOURCES=\
		AppPointsGui.cpp \
		Command.cpp \
		DlgPointsReadImp.cpp \
		DlgPointsReadImp.h \
		PreCompiled.cpp \
		PreCompiled.h \
		ViewProvider.cpp \
		ViewProvider.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libPointsGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPointsGui_la_CPPFLAGS = -DPointsAppExport= -DPointsGuiExport=

libPointsGui_la_LIBADD   = \
		-lFreeCADGui \
		-lPoints

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

libdir = $(prefix)/Mod/Points

# We need this softlink for Python to load
install-data-local:
		cd $(DESTDIR)$(libdir) && \
		rm -f PointsGui.so && \
		$(LN_S) libPointsGui.so PointsGui.so

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		DlgPointsRead.ui \
		AppPointsGui.dsp \
		AppPointsGui.vcproj \
		Libs.cpp
