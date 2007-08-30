noinst_LTLIBRARIES=libResources.la

BUILT_SOURCES=\
		qrc_Drawing.cpp 

nodist_libResources_la_SOURCES=\
		qrc_Drawing.cpp 

EXTRA_DIST = \
		icons/actions/document-new.png \
		icons/actions/document-new.svg \
		icons/Page.svg \
		icons/Pages.svg \
		icons/View.svg \
		translations/Drawing_de.qm \
		translations/Drawing_de.ts \
		Drawing.qrc \
		UpdateResources.bat


# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	$(QT_MOC) $< -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	$(QT_MOC) $< -o $(@F)

# rules for Qt Resource Compiler:
qrc_%.cpp: %.qrc
	$(QT_RCC) -name $(*F) $< -o $(@F)


# set the include path found by configure
INCLUDES= $(all_includes) -I$(top_srcdir)/src -I$(top_builddir)/src -I$(srcdir)/.. $(QT_CXXFLAGS)

# set additional defines (for cygwin only)
AM_CPPFLAGS=-DFCGui

CLEANFILES = $(BUILT_SOURCES)
