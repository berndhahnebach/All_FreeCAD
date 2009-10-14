
lib_LTLIBRARIES=libImageGui.la ImageGui.la

BUILT_SOURCES=\
		moc_GLImageBox.cpp \
		moc_ImageView.cpp \
		qrc_Image.cpp 

libImageGui_la_SOURCES=\
		AppImageGuiPy.cpp \
		Command.cpp \
		GLImageBox.cpp \
		ImageView.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		XpmImages.h

includedir = @includedir@/Mod/Image/Gui

include_HEADERS=\
		GLImageBox.h \
		ImageView.h \
		Workbench.h

# the library search path.
libImageGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(GL_LIBS) \
		$(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libImageGui_la_CPPFLAGS = -DImageAppExport= -DImageGuiExport=

#		$(opencv_LIBS) \
libImageGui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
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

ImageGui_la_DEPENDENCIES = libImageGui.la

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
# $(opencv_CFLAGS)
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes) $(QT_CXXFLAGS)

libdir = $(prefix)/Mod/Image

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		CMakeLists.txt \
		Image.qrc \
		Image_de.qm \
		Image_de.ts \
		image-import.svg

