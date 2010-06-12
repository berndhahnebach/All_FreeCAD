noinst_LTLIBRARIES=libResources.la

BUILT_SOURCES=\
		qrc_PartDesign.cpp 

nodist_libResources_la_SOURCES=\
		qrc_PartDesign.cpp 

EXTRA_DIST = \
		translations/PartDesign_de.qm \
		translations/PartDesign_de.ts \
		icons/PartDesign_Pad.svg \
		PartDesign.qrc


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
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src -I$(srcdir)/.. \
		$(all_includes) $(QT_CXXFLAGS) -I$(OCC_INC)

CLEANFILES = $(BUILT_SOURCES)
