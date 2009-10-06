noinst_LTLIBRARIES=libResources.la

BUILT_SOURCES=\
		qrc_Sketcher.cpp 

nodist_libResources_la_SOURCES=\
		qrc_Sketcher.cpp 

EXTRA_DIST = \
		icons/Sketcher_ConstrainHorizontal.svg \
		icons/Sketcher_ConstrainLock.svg \
		icons/Sketcher_ConstrainVertical.svg \
		icons/Sketcher_CreateArc.png \
		icons/Sketcher_CreateCircle.png \
		icons/Sketcher_CreateLine.png \
		icons/Sketcher_CreatePoint.svg \
		icons/Sketcher_CreatePolyline.png \
		icons/Sketcher_CreateRectangle.png \
		icons/Sketcher_CreateText.png \
		icons/Sketcher_DraftLine.svg \
		icons/Sketcher_LeaveSketch.svg \
		icons/Sketcher_NewSketch.svg \
		translations/Sketcher_de.qm \
		translations/Sketcher_de.ts \
		Sketcher.qrc


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
INCLUDES= -I$(top_srcdir)/src -I$(top_builddir)/src -I$(srcdir)/.. $(all_includes) $(QT_CXXFLAGS) -I$(OCC_INC)

# set additional defines (for cygwin only)
AM_CPPFLAGS=-DFCGui

CLEANFILES = $(BUILT_SOURCES)
