
pkglib_LTLIBRARIES=libRaytracingGui.la

BUILT_SOURCES=\
		DlgSettingsRay.cpp \
		DlgSettingsRay.h \
		moc_DlgSettingsRay.cpp \
		moc_DlgSettingsRayImp.cpp

libRaytracingGui_la_SOURCES=\
		AppRaytracingGui.cpp \
		Command.cpp \
		DlgSettingsRayImp.cpp \
		DlgSettingsRayImp.h

# the library search path.
libRaytracingGui_la_LDFLAGS = -L../App $(all_libraries)

libRaytracingGui_la_LIBADD   = \
		-lFreeCADGui \
		-lRaytracing

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
