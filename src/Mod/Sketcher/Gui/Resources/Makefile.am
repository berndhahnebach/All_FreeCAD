noinst_LTLIBRARIES=libResources.la

BUILT_SOURCES=\
		qrc_Sketcher.cpp 

nodist_libResources_la_SOURCES=\
		qrc_Sketcher.cpp 

EXTRA_DIST = \
		icons/Constraint_PointOnObject.svg \
		icons/Constraint_TangentToStart.svg \
		icons/Constraint_Length.svg \
		icons/Constraint_Concentric.svg \
		icons/Constraint_TangentToEnd.svg \
		icons/Constraint_PointOnMidPoint.svg \
		icons/Sketcher_ConstrainDistance.svg \
		icons/Sketcher_ConstrainParallel.svg \
		icons/Constraint_PointOnStart.svg \
		icons/Constraint_Vertical.svg \
		icons/Constraint_PointOnEnd.svg \
		icons/Constraint_PointToObject.svg \
		icons/Constraint_InternalAngle.svg \
		icons/Constraint_Radius.svg \
		icons/Constraint_Horizontal.svg \
		icons/Constraint_ExternalAngle.svg \
		icons/Constraint_Parallel.svg \
		icons/Constraint_Perpendicular.svg \
		icons/Constraint_PointOnPoint.svg \
		icons/Constraint_Tangent.svg \
		icons/Constraint_EqualLength.svg \
		icons/Sketcher_ConstrainCoincident.svg \
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
		translations/Sketcher_es.qm \
		translations/Sketcher_fr.qm \
		translations/Sketcher_it.qm \
		translations/Sketcher_se.qm \
		translations/Sketcher_de.ts \
		translations/Sketcher_es.ts \
		translations/Sketcher_fr.ts \
		translations/Sketcher_it.ts \
		translations/Sketcher_se.ts \
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
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src -I$(srcdir)/.. \
		$(all_includes) $(QT_CXXFLAGS) -I$(OCC_INC)

CLEANFILES = $(BUILT_SOURCES)
