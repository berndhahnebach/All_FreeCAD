noinst_LTLIBRARIES=libLanguage.la

BUILT_SOURCES=\
		moc_Translator.cpp \
		qrc_translation.cpp

nodist_libLanguage_la_SOURCES=\
		qrc_translation.cpp

libLanguage_la_SOURCES=\
		Translator.cpp \
		Translator.h

EXTRA_DIST = \
		translation.qrc \
		FreeCAD.po \
		FreeCAD_de.qm \
		FreeCAD_fr.qm \
		FreeCAD_it.qm \
		FreeCAD_se.qm \
		qt_de.qm \
		qt_fr.qm \
		qt_it.qm


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

