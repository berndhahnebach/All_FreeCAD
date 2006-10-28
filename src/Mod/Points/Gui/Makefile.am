
lib_LTLIBRARIES=libPointsGui.la PointsGui.la

BUILT_SOURCES=\
		DlgPointsRead.cpp \
		DlgPointsRead.h \
		moc_DlgPointsRead.cpp \
		moc_DlgPointsReadImp.cpp \
		Points_de.h

libPointsGui_la_SOURCES=\
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
libPointsGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(sim_ac_coin_libs) $(sim_ac_soqt_libs) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPointsGui_la_CPPFLAGS = -DPointsAppExport= -DPointsGuiExport=

libPointsGui_la_LIBADD   = \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lPoints

#--------------------------------------------------------------------------------------
# Loader of libPointsGui

PointsGui_la_SOURCES=\
		AppPointsGui.cpp

# the library search path.
PointsGui_la_LDFLAGS = $(libPointsGui_la_LDFLAGS) -module -avoid-version
PointsGui_la_CPPFLAGS = $(libPointsGui_la_CPPFLAGS)

PointsGui_la_LIBADD   = \
		$(libPointsGui_la_LIBADD) \
		-lPointsGui

#--------------------------------------------------------------------------------------

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

# rule for translation files using python script :
%.h: %.ts
	$(PYTHON) ../../../Tools/qembed.py $(<F)  $(@F) $(*F)

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I../../../ -I../App

libdir = $(prefix)/Mod/Points

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		DlgPointsRead.ui \
		AppPointsGui.dsp \
		AppPointsGui.vcproj \
		Points_de.ts \
		Libs.cpp
