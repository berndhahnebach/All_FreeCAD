# cMake file for the FreeCAD build
# (c) 2007 Jürgen Riegel LGPL

PROJECT(FREECAD)

# Don't build anything unless the version of CMake is high enough.
# The matching ELSE/ENDIF should be the last lines in the file.
IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 1.9)


#-----------------------------------------------------------------------------
# FreeCAD version number.  An even minor number corresponds to releases.
SET(FREECAD_MAJOR_VERSION 0)
SET(FREECAD_MINOR_VERSION 7)
SET(FREECAD_BUILD_VERSION 911)
SET(FREECAD_VERSION
    "${FREECAD_VERSION_MAJOR_VERSION}.${FREECAD_VERSION_MINOR_VERSION}.${FREECAD_VERSION_BUILD_VERSION}")

# check for needet libraries
#FIND_PACKAGE(QT4)
FIND_PACKAGE(PythonLibs)
FIND_PACKAGE(PythonInterp)
FIND_PACKAGE(Boost)

	
	


# Recurse into the src subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.
SUBDIRS(src)


ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 1.9)