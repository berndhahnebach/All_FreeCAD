if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libCamGui.la CamGui.la

BUILT_SOURCES=

libCamGui_la_SOURCES=\
		Command.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

# the library search path.
libCamGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App $(QT_LIBS) $(sim_ac_coin_ldflags) $(sim_ac_coin_libs) $(sim_ac_soqt_ldflags) $(sim_ac_soqt_libs) -L$(OCC_LIB) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libCamGui_la_CPPFLAGS = -DAppCamExport= -DAppCamGuiExport=

libCamGui_la_LIBADD   = \
		-lxerces-c \
		-l@PYTHON_LIB@ \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lTKernel \
		-lCam 

#--------------------------------------------------------------------------------------
# Loader of libCamGui

CamGui_la_SOURCES=\
		AppCamGui.cpp

# the library search path.
CamGui_la_LDFLAGS = $(libCamGui_la_LDFLAGS) -module -avoid-version
CamGui_la_CPPFLAGS = $(libCamGui_la_CPPFLAGS)

CamGui_la_LIBADD   = \
		$(libCamGui_la_LIBADD) \
		-lCamGui

#--------------------------------------------------------------------------------------

# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	$(QT_MOC) $< -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	$(QT_MOC) $< -o $(@F)

# rules for Qt User Interface Compiler:
ui_%.h: %.ui
	$(QT_UIC) $< -o $(@F)

# rules for Qt Resource Compiler:
qrc_%.cpp: %.qrc
	$(QT_RCC) -name $(*F) $< -o $(@F)

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I$(OCC_INC) -I$(top_srcdir)/src -I$(top_builddir)/src

endif

libdir = $(prefix)/Mod/Cam

CLEANFILES = $(BUILT_SOURCES)

#EXTRA_DIST = \
#		AppCamGui.vcproj
