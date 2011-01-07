noinst_LTLIBRARIES=libResources.la

BUILT_SOURCES=\
		qrc_Robot.cpp 

nodist_libResources_la_SOURCES=\
		qrc_Robot.cpp 

EXTRA_DIST = \
		icons/Robot_CreateRobot.svg \
		icons/Robot_CreateTrajectory.svg \
		icons/Robot_Edge2Trac.svg \
		icons/Robot_Export.svg \
		icons/Robot_InsertWaypoint.svg \
		icons/Robot_InsertWaypointPre.svg \
		icons/Robot_Simulate.svg \
		icons/Robot_TrajectoryCompound.svg \
		icons/Robot_TrajectoryDressUp.svg \
		translations/Robot_de.qm \
		translations/Robot_de.ts \
		Robot.qrc \
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
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src -I$(srcdir)/.. $(all_includes) $(QT_CXXFLAGS)

CLEANFILES = $(BUILT_SOURCES)

