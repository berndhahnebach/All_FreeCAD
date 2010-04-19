noinst_LTLIBRARIES=libResources.la

BUILT_SOURCES=\
		qrc_Drawing.cpp 

nodist_libResources_la_SOURCES=\
		qrc_Drawing.cpp 

EXTRA_DIST = \
		icons/actions/document-new.png \
		icons/actions/document-new.svg \
		icons/actions/drawing-landscape-A0.svg \
		icons/actions/drawing-landscape-A1.svg \
		icons/actions/drawing-landscape-A2.svg \
		icons/actions/drawing-landscape.svg \
		icons/actions/drawing-view.svg \
		icons/actions/drawing-landscape-A3.svg \
		icons/actions/drawing-landscape-A4.svg \
		icons/actions/drawing-landscape-new.svg \
		icons/actions/drawing-portrait-A4.svg \
		icons/Page.svg \
		icons/Pages.svg \
		icons/View.svg \
		translations/Drawing_de.qm \
		translations/Drawing_fr.qm \
		translations/Drawing_it.qm \
		translations/Drawing_se.qm \
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
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src -I$(srcdir)/.. $(QT_CXXFLAGS) $(all_includes)

CLEANFILES = $(BUILT_SOURCES)
