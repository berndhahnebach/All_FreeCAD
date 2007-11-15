# cMake file for the FreeCAD build
# (c) 2007 Jürgen Riegel LGPL

# force to c++ compiler and linker
SET(CMAKE_C_COMPILER ${CMAKE_CXX_COMPILER})
SET(LINKER_LANGUAGE CXX)

# Recurse into the src subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.
#SUBDIRS(PyCXX zipios)


# Create a library FreeCADBase which includes the source files.
# The extension is already found.  Any number of sources could be listed here.
ADD_LIBRARY(FreeCADBase SHARED 
 BaseClass
 BaseClassPy.xml
 BaseClassPyImp.cpp
 BoundBox
 Builder3D
 Console
 Exception
 Factory
 FileInfo
 FileTemplate
 Handle
 InputSource
 Interpreter
 Matrix
 MemDebug
 Observer
 Parameter
 ParameterPy
 Persistance
 PersistancePy.xml
 PersistancePyImp
 Placement
 PreCompiled
 PyExport
 PyObjectBase
 PyTools
 Reader
 Rotation
 Sequencer
 Stream
 Swap
 TimeInfo
 Tools2D
 Type
 Vector3D
 ViewProj
 Writer
 XMLTools
 gzstream
  
 )

