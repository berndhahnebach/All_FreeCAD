
lib_LTLIBRARIES=libQtUnitGui.la QtUnitGui.la

BUILT_SOURCES=\
		UnitTest.cpp \
		UnitTest.h \
		moc_UnitTest.cpp

libQtUnitGui_la_SOURCES=\
		PreCompiled.cpp \
		PreCompiled.h \
		UnitTestImp.cpp \
		UnitTestImp.h \
		UnitTestPy.cpp \
		UnitTestPy.h

# the library search path.
libQtUnitGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui $(QT_LIBS) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libQtUnitGui_la_CPPFLAGS = -DQtUnitAppExport= -DQtUnitGuiExport=

libQtUnitGui_la_LIBADD   = \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui

#--------------------------------------------------------------------------------------
# Loader of libQtUnitGui

QtUnitGui_la_SOURCES=\
		AppTestGui.cpp

# the library search path.
QtUnitGui_la_LDFLAGS = $(libQtUnitGui_la_LDFLAGS) -module -avoid-version
QtUnitGui_la_CPPFLAGS = $(libQtUnitGui_la_CPPFLAGS)

QtUnitGui_la_LIBADD   = \
		$(libQtUnitGui_la_LIBADD) \
		-lQtUnitGui

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

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I../../../ -I../App

libdir = $(prefix)/Mod/Test

CLEANFILES = $(BUILT_SOURCES)

# Change data dir from default ($(prefix)/share) to $(prefix)
datadir = $(prefix)/Mod/Test
data_DATA = \
		qtunittest.py

EXTRA_DIST = \
		$(data_DATA) \
		AppTestGui.dsp \
		AppTestGui.vcproj \
		UnitTest.ui \
		qtunittest.py
