
lib_LTLIBRARIES=libDrawingGui.la DrawingGui.la

BUILT_SOURCES=\
		moc_DrawingView.cpp \
		qrc_Drawing.cpp 

libDrawingGui_la_SOURCES=\
		AppDrawingGuiPy.cpp \
		Command.cpp \
		DrawingView.cpp \
		DrawingView.h \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libDrawingGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(GL_LIBS) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libDrawingGui_la_CPPFLAGS = -DDrawingAppExport= -DDrawingGuiExport=

libDrawingGui_la_LIBADD   = \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lDrawing

#--------------------------------------------------------------------------------------
# Loader of libDrawingGui

DrawingGui_la_SOURCES=\
		AppDrawingGui.cpp

# the library search path.
DrawingGui_la_LDFLAGS = $(libDrawingGui_la_LDFLAGS) -module -avoid-version
DrawingGui_la_CPPFLAGS = $(libDrawingGui_la_CPPFLAGS)

DrawingGui_la_LIBADD   = \
		$(libDrawingGui_la_LIBADD) \
		-lDrawingGui

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

libdir = $(prefix)/Mod/Drawing

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		AppDrawingGui.vcproj \
		Drawing.qrc \
		Drawing_de.qm \
		Drawing_de.ts \
		XpmDrawings.h
