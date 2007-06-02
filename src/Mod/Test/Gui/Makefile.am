
lib_LTLIBRARIES=libQtUnitGui.la QtUnitGui.la

BUILT_SOURCES=\
		ui_UnitTest.h \
		moc_UnitTestImp.cpp \
		qrc_Test.cpp

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
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I$(top_srcdir)/src -I$(top_builddir)/src

libdir = $(prefix)/Mod/Test

CLEANFILES = $(BUILT_SOURCES)

# Change data dir from default ($(prefix)/share) to $(prefix)
datadir = $(prefix)/Mod/Test
data_DATA = \
		qtunittest.py

EXTRA_DIST = \
		$(data_DATA) \
		AppTestGui.vcproj \
		UnitTest.ui \
		Test.qrc \
		Test_de.qm \
		Test_de.ts \
		qtunittest.py
