noinst_LTLIBRARIES=libIcons.la

BUILT_SOURCES=\
		qrc_resource.cpp

nodist_libIcons_la_SOURCES=\
		qrc_resource.cpp

EXTRA_DIST = \
		ClassBrowser/const_member.png \
		ClassBrowser/member.png \
		ClassBrowser/method.png \
		ClassBrowser/property.png \
		ClassBrowser/type_class.png \
		ClassBrowser/type_enum.png \
		ClassBrowser/type_module.png \
		BmpFactoryIcons.cpp \
		background.xpm \
		bulb.xpm \
		button_down.xpm \
		button_left.xpm \
		button_right.xpm \
		button_up.xpm \
		Document.xpm \
		FCIcon.xpm \
		Feature.xpm \
		SplashScreen.xpm \
		freecadsplash.png \
		images.cpp \
		ParamDlgImgs.h \
		macro-execute.svg \
		macro-record.svg \
		macro-stop.svg \
		preferences-display.svg \
		preferences-general.svg \
		Std_ViewScreenShot.svg \
		utilities-terminal.svg \
		document-new.svg \
		document-open.svg \
		document-save.svg \
		document-save-as.svg \
		document-print.svg \
		document-properties.svg \
		system-log-out.svg \
		edit_Cancel.svg \
		edit_OK.svg \
		edit-copy.svg \
		edit-cut.svg \
		edit-delete.svg \
		edit-paste.svg \
		edit-select-all.svg \
		edit-redo.svg \
		edit-undo.svg \
		preferences-system.svg \
		window-new.svg \
		camera-photo.svg \
		applications-accessories.svg \
		accessories-text-editor.svg \
		help-browser.svg \
		view-refresh.svg \
		view-fullscreen.svg \
		view-axometric.svg \
		view-isometric.svg \
		view-perspective.svg \
		view-bottom.svg \
		view-front.svg \
		view-left.svg \
		view-rear.svg \
		view-right.svg \
		view-top.svg \
		view-zoom-all.svg \
		view-zoom-border.svg \
		view-zoom-fit.svg \
		view-zoom-in.svg \
		view-zoom-out.svg \
		view-zoom-selection.svg \
		Tree_Annotation.svg \
		Tree_Dimension.svg \
		Tree_Python.svg \
		resource.qrc

data_DATA = FCIcon.xpm

# rules for Qt Resource Compiler:
qrc_%.cpp: %.qrc
	$(QT_RCC) -name $(*F) $< -o $(@F)


# set the include path found by configure
AM_CPPFLAGS = $(all_includes) -I$(top_srcdir)/src -I$(top_builddir)/src $(QT_CXXFLAGS)

CLEANFILES = $(BUILT_SOURCES)
