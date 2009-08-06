
lib_LTLIBRARIES=libRobot.la Robot.la

libRobot_la_SOURCES=\
                AppRobotPy.cpp \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libRobot_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Mod/Part/App $(all_libraries) -L$(OCC_LIB) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libRobot_la_CPPFLAGS = -DRobotAppExport=

libRobot_la_LIBADD   = \
		@BOOST_REGEX_LIB@ @BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lPart \
		-lTKernel \
		-lTKG2d \
		-lTKG3d \
		-lTKMath \
		-lTKSTEP \
		-lTKIGES \
		-lTKSTL \
		-lTKShHealing \
		-lTKXSBase \
		-lTKBool \
		-lTKBO \
		-lTKBRep \
		-lTKTopAlgo \
		-lTKGeomAlgo \
		-lTKGeomBase \
		-lTKOffset \
		-lTKPrim \
		-lTKHLR

#--------------------------------------------------------------------------------------
# Loader of libRobot

Robot_la_SOURCES=\
		AppRobot.cpp

# the library search path.
Robot_la_LDFLAGS = $(libRobot_la_LDFLAGS) -module -avoid-version
Robot_la_CPPFLAGS = $(libRobot_la_CPPFLAGS)

Robot_la_LIBADD   = \
		$(libRobot_la_LIBADD) \
		-lRobot

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= -I$(top_srcdir)/src -I$(top_builddir)/src -I$(OCC_INC) $(all_includes)


libdir = $(prefix)/Mod/Robot

EXTRA_DIST = \
		CMakeLists.txt
