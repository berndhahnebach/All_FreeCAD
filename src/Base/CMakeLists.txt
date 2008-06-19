add_definitions(-DFCBase)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${Boost_INCLUDE_DIRS}
	${PYTHON_INCLUDE_PATH}
	${XERCES_INCLUDE_DIR})

if(WIN32)
	set(FreeCADBase_LIBS
		${Boost_LIBRARIES}
		debug ${PYTHON_DEBUG_LIBRARIES}
		optimized ${PYTHON_LIBRARIES}
		${XERCES_LIBRARIES}
		${ZLIB_LIBRARIES})
else(WIN32)
	set(FreeCADBase_LIBS
#libFreeCADBase_la_LIBADD   = \
#		-l@PYTHON_LIB@ \
#		-lboost_filesystem \
#		-lxerces-c \
#		-lz
)
endif(WIN32)

generate_from_xml(BaseClassPy)
generate_from_xml(BoundBoxPy)
generate_from_xml(PersistancePy)
generate_from_xml(VectorPy)

SET(PyCXX_SRCS
	PyCXX/Config.hxx
	PyCXX/cxx_extensions.cxx
	PyCXX/cxxextensions.c
	PyCXX/cxxsupport.cxx
	PyCXX/Exception.hxx
	PyCXX/Extensions.hxx
	PyCXX/IndirectPythonInterface.cxx
	PyCXX/IndirectPythonInterface.hxx
	PyCXX/Objects.hxx
)
SOURCE_GROUP("PyCXX" FILES ${PyCXX_SRCS})

SET(zipios_SRCS
	zipios/backbuffer.h
	zipios/basicentry.cpp
	zipios/basicentry.h
	zipios/collcoll.cpp
	zipios/collcoll.h
	zipios/deflateoutputstreambuf.cpp
	zipios/deflateoutputstreambuf.h
	zipios/dircoll.cpp
	zipios/dircoll.h
	zipios/directory.cpp
	zipios/directory.h
	zipios/fcoll.cpp
	zipios/fcoll.h
	zipios/fcollexceptions.cpp
	zipios/fcollexceptions.h
	zipios/fileentry.cpp
	zipios/fileentry.h
	zipios/filepath.cpp
	zipios/filepath.h
	zipios/filterinputstreambuf.cpp
	zipios/filterinputstreambuf.h
	zipios/filteroutputstreambuf.cpp
	zipios/filteroutputstreambuf.h
	zipios/gzipoutputstream.cpp
	zipios/gzipoutputstream.h
	zipios/gzipoutputstreambuf.cpp
	zipios/gzipoutputstreambuf.h
	zipios/inflateinputstreambuf.cpp
	zipios/inflateinputstreambuf.h
	zipios/meta-iostreams.h
	zipios/outputstringstream.h
	zipios/simplesmartptr.h
	zipios/virtualseeker.h
	zipios/zipfile.cpp
	zipios/zipfile.h
	zipios/ziphead.cpp
	zipios/ziphead.h
	zipios/zipheadio.cpp
	zipios/zipheadio.h
	zipios/zipinputstream.cpp
	zipios/zipinputstream.h
	zipios/zipinputstreambuf.cpp
	zipios/zipinputstreambuf.h
	zipios/zipios_common.h
	zipios/zipios-config.h
	zipios/zipios_defs.h
	zipios/zipoutputstreambuf.cpp
	zipios/zipoutputstreambuf.h
	zipios/zipoutputstream.cpp
	zipios/zipoutputstream.h
)
SOURCE_GROUP("zipios" FILES ${zipios_SRCS})

SET(FreeCADBase_SRCS
	${PyCXX_SRCS}
	${zipios_SRCS}
	BaseClass.cpp
	BaseClass.h
	BaseClassPy.xml
	BaseClassPyImp.cpp
	BoundBox.h
	BoundBoxPy.xml
	BoundBoxPyImp.cpp
	Builder3D.cpp
	Builder3D.h
	Console.cpp
	Console.h
	Doxygen.cpp
	Exception.cpp
	Exception.h
	Factory.cpp
	Factory.h
	FileInfo.cpp
	FileInfo.h
	FileTemplate.cpp
	FileTemplate.h
	gzstream.cpp
	gzstream.h
	Handle.cpp
	Handle.h
	InputSource.cpp
	InputSource.h
	Interpreter.cpp
	Interpreter.h
	Matrix.cpp
	Matrix.h
	MemDebug.cpp
	MemDebug.h
	Observer.h
	Parameter.cpp
	Parameter.h
	ParameterPy.cpp
	Persistance.cpp
	Persistance.h
	PersistancePy.xml
	PersistancePyImp.cpp
	Placement.cpp
	Placement.h
	PreCompiled.cpp
	PreCompiled.h
	PyExport.cpp
	PyExport.h
	PyObjectBase.cpp
	PyObjectBase.h
	PyTools.c
	PyTools.h
	Reader.cpp
	Reader.h
	Rotation.cpp
	Rotation.h
	Sequencer.cpp
	Sequencer.h
	Stream.cpp
	Stream.h
	Swap.cpp
	Swap.h
	swigpyrun_1.3.25.cpp
	swigpyrun_1.3.25.h
	swigpyrun_1.3.33.cpp
	swigpyrun_1.3.33.h
	TimeInfo.cpp
	TimeInfo.h
	Tools2D.cpp
	Tools2D.h
	Type.cpp
	Type.h
	Vector3D.cpp
	Vector3D.h
	VectorPy.xml
	VectorPyImp.cpp
	ViewProj.h
	Writer.cpp
	Writer.h
	XMLTools.cpp
	XMLTools.h
)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../Main)

add_library(FreeCADBase SHARED ${FreeCADBase_SRCS})

target_link_libraries(FreeCADBase ${FreeCADBase_LIBS})

install(TARGETS FreeCADBase DESTINATION lib)

