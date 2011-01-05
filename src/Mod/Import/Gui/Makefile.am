
lib_LTLIBRARIES=libImportGui.la ImportGui.la

# BUILT_SOURCES=\
#
libImportGui_la_SOURCES=\
		AppImportGuiPy.cpp \
		Command.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libImportGui_la_LDFLAGS = \
		-L../../../Base \
		-L../../../App  \
		-L../../../Gui  \
		-L../../Part/App \
		-L../../Part/Gui \
		$(all_libraries) $(QT4_CORE_LIBS) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libImportGui_la_CPPFLAGS = -DAppPartExport= -DAppPartGuiExport=

libImportGui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lTKernel \
		-lTKXCAF \
		-lTKLCAF \
		-lTKXDESTEP \
		-lTKXDEIGES \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lPart \
		-lPartGui

#--------------------------------------------------------------------------------------
# Loader of libImportGui

ImportGui_la_SOURCES=\
		AppImportGui.cpp

# the library search path.
ImportGui_la_LDFLAGS = $(libImportGui_la_LDFLAGS) -module -avoid-version
ImportGui_la_CPPFLAGS = $(libImportGui_la_CPPFLAGS)

ImportGui_la_LIBADD   = \
		$(libImportGui_la_LIBADD) \
		-lImportGui

ImportGui_la_DEPENDENCIES = libImportGui.la

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
qrc_%.cpp: Resources/%.qrc
	$(QT_RCC) -name $(*F) $< -o $(@F)

# set the include path found by configure
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes) $(QT_CXXFLAGS) -I$(OCC_INC)

libdir = $(prefix)/Mod/Import

#CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		CMakeLists.txt
