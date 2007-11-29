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
		images.cpp \
		Icons.h \
		ParamDlgImgs.h \
		preferences-display.svg \
		preferences-general.svg \
		resource.qrc

data_DATA = FCIcon.xpm

# rules for Qt Resource Compiler:
qrc_%.cpp: %.qrc
	$(QT_RCC) -name $(*F) $< -o $(@F)


# set the include path found by configure
INCLUDES= $(all_includes) -I$(top_srcdir)/src -I$(top_builddir)/src $(QT_CXXFLAGS)

CLEANFILES = $(BUILT_SOURCES)
