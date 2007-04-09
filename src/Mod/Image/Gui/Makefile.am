
lib_LTLIBRARIES=libImageGui.la ImageGui.la

BUILT_SOURCES=\
		moc_GLImageBox.cpp \
		moc_ImageView.cpp \
		qrc_Image.cpp 

libImageGui_la_SOURCES=\
		AppImageGuiPy.cpp \
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
libImageGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libImageGui_la_CPPFLAGS = -DImageAppExport= -DImageGuiExport=

libImageGui_la_LIBADD   = \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lGLU \
		-lGL \
		-lImage

#--------------------------------------------------------------------------------------
# Loader of libImageGui

ImageGui_la_SOURCES=\
		AppImageGui.cpp

# the library search path.
ImageGui_la_LDFLAGS = $(libImageGui_la_LDFLAGS) -module -avoid-version
ImageGui_la_CPPFLAGS = $(libImageGui_la_CPPFLAGS)

ImageGui_la_LIBADD   = \
		$(libImageGui_la_LIBADD) \
		-lImageGui

#--------------------------------------------------------------------------------------

# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	$(QT_MOC) $(srcdir)/$< -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	$(QT_MOC) $(srcdir)/$< -o $(@F)

# rules for Qt User Interface Compiler:
# (it adds path to #include automatically, so cd before calling it)
ui_%.h: %.ui
	$(QT_UIC) $(srcdir)/$< -o $(@F)

# rules for Qt Resource Compiler:
# (it adds path to #include automatically, so cd before calling it)
qrc_%.cpp: %.qrc
	$(QT_RCC) -name $(*F) $(srcdir)/$< -o $(@F)

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I$(top_srcdir)/src -I$(top_builddir)/src

libdir = $(prefix)/Mod/Image

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		AppImageGui.vcproj \
		Image.qrc \
		Image_de.qm \
		Image_de.ts \
		XpmImages.h
