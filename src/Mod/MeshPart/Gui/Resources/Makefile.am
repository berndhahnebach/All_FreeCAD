noinst_LTLIBRARIES=libResources.la

BUILT_SOURCES=\
		qrc_MeshPart.cpp 

nodist_libResources_la_SOURCES=\
		qrc_MeshPart.cpp 

EXTRA_DIST = \
		icons/actions/MeshFace.svg \
		translations/MeshPart_de.qm \
		translations/MeshPart_it.qm \
		translations/MeshPart_se.qm \
		translations/MeshPart_de.ts \
		translations/MeshPart_it.ts \
		translations/MeshPart_se.ts \
		MeshPart.qrc


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
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src -I$(srcdir)/.. $(all_includes) $(QT_CXXFLAGS)

CLEANFILES = $(BUILT_SOURCES)
