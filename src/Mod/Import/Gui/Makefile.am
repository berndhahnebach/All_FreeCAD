
pkglib_LTLIBRARIES=libImportGui.la

# BUILT_SOURCES=\
#
libImportGui_la_SOURCES=\
	AppImportGui.cpp \
	Command.cpp

# the library search path.
libImportGui_la_LDFLAGS = -L../App $(all_libraries)

libImportGui_la_LIBADD   = \
		-lFreeCADGui \
		-lImport

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
