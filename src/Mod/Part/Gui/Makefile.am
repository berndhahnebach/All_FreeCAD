
pkglib_LTLIBRARIES=libPartGui.la

BUILT_SOURCES=\
	DlgPartBox.cpp \
	DlgPartBox.h \
	moc_DlgPartBox.cpp \
	moc_DlgPartBoxImp.cpp

libPartGui_la_SOURCES=\
		AppPartGui.cpp \
		Command.cpp \
		DlgPartBoxImp.cpp \
		DlgPartBoxImp.h

# the library search path.
libPartGui_la_LDFLAGS = -L../App $(all_libraries)

libPartGui_la_LIBADD   = \
		-lFreeCADGui \
		-lPart

# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	cd $(@D) && $(MOC) $(<F) -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	cd $(@D) && $(MOC) $(<F) -o $(@F)

# rules for Qt User Interface Compiler:
# (it adds path to #include automatically, so cd before calling it)
%.h: %.ui
	cd $(@D) && $(UIC) $(<F) -o $(@F)
%.cpp: %.h %.ui
	cd $(@D) && $(UIC) -o $(@F) -impl $(*F).h $(*F).ui

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../ -I../App
