noinst_LTLIBRARIES=libResources.la

BUILT_SOURCES=\
		qrc_ReverseEngineering.cpp 

nodist_libResources_la_SOURCES=\
		qrc_ReverseEngineering.cpp 

EXTRA_DIST = \
		icons/actions/FitSurface.svg \
		translations/ReverseEngineering_de.qm \
		translations/ReverseEngineering_de.ts \
		ReverseEngineering.qrc \
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
AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src -I$(srcdir)/.. $(all_includes) $(QT_CXXFLAGS)

CLEANFILES = $(BUILT_SOURCES)
