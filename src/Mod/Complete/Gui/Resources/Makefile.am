noinst_LTLIBRARIES=libResources.la

BUILT_SOURCES=\
		qrc_Complete.cpp 

nodist_libResources_la_SOURCES=\
		qrc_Complete.cpp 

EXTRA_DIST = \
		translations/Complete_de.qm \
		translations/Complete_fr.qm \
		translations/Complete_it.qm \
		translations/Complete_se.qm \
		translations/Complete_de.ts \
		translations/Complete_fr.ts \
		translations/Complete_it.ts \
		translations/Complete_se.ts \
		Complete.qrc \
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
