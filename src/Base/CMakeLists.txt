if(WIN32)
		 add_definitions(-DFCBase)
		 add_definitions(-DPYCXX_DLL)
		 add_definitions(-DBOOST_DYN_LINK)
endif(WIN32)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}/zipios
	${Boost_INCLUDE_DIRS}
	${PYTHON_INCLUDE_PATH}
	${XERCESC_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR})

if(WIN32)
	set(FreeCADBase_LIBS
 		#debug ${Boost_DEBUG_LIBRARIES}
 		#optimized ${Boost_LIBRARIES}
		debug ${PYTHON_DEBUG_LIBRARY}
		optimized ${PYTHON_LIBRARY}
		debug ${XERCESC_DEBUG_LIBRARIES}
		optimized ${XERCESC_LIBRARIES}
		${ZLIB_LIBRARIES}
		debug MSVCRTD.LIB
        debug MSVCPRTD.LIB
		optimized MSVCRT.LIB
        optimized MSVCPRT.LIB
		Rpcrt4.lib
		)
else(WIN32)
	set(FreeCADBase_LIBS
		${PYTHON_LIBRARY}
		${XERCESC_LIBRARIES}
		${Boost_LIBRARIES}
		${ZLIB_LIBRARIES}
)
endif(WIN32)

generate_from_xml(BaseClassPy)
generate_from_xml(BoundBoxPy)
generate_from_xml(PersistencePy)
generate_from_xml(VectorPy)
generate_from_xml(MatrixPy)
generate_from_xml(RotationPy)
generate_from_xml(PlacementPy)



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

SET(pycxx_SRCS
	../CXX/Config.hxx
	../CXX/cxxextensions.c
	../CXX/cxx_extensions.cxx
	../CXX/cxxsupport.cxx
	../CXX/Exception.hxx
	../CXX/Extensions.hxx
	../CXX/IndirectPythonInterface.cxx
	../CXX/IndirectPythonInterface.hxx
	../CXX/Objects.hxx
	../CXX/Version.hxx
	../CXX/WrapPython.h
)
SOURCE_GROUP("pycxx" FILES ${pycxx_SRCS})


SET(FreeCADBase_SRCS
	${zipios_SRCS}
	${pycxx_SRCS}
	Base64.cpp
	Base64.h
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
	fdstream.hpp
	gzstream.cpp
	gzstream.h
	GeometryPyCXX.cpp
	GeometryPyCXX.h
	Handle.cpp
	Handle.h
	InputSource.cpp
	InputSource.h
	Interpreter.cpp
	Interpreter.h
	Matrix.cpp
	Matrix.h
	MatrixPy.xml
	MatrixPyImp.cpp
	MemDebug.cpp
	MemDebug.h
	Observer.h
	Parameter.cpp
	Parameter.h
	ParameterPy.cpp
	Persistence.cpp
	Persistence.h
	PersistencePy.xml
	PersistencePyImp.cpp
	Placement.cpp
	Placement.h
	PlacementPy.xml
	PlacementPyImp.cpp
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
	RotationPy.xml
	RotationPyImp.cpp
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
	swigpyrun_1.3.36.cpp
	swigpyrun_1.3.36.h
	swigpyrun_1.3.38.cpp
	swigpyrun_1.3.38.h
	swigpyrun.cpp
	swigpyrun.inl
	TimeInfo.cpp
	TimeInfo.h
	Tools2D.cpp
	Tools2D.h
	Type.cpp
	Type.h
	Uuid.cpp
	Uuid.h
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

#set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../Main)

add_library(FreeCADBase SHARED ${FreeCADBase_SRCS})

target_link_libraries(FreeCADBase ${FreeCADBase_LIBS})

if(MSVC)
		set_target_properties(FreeCADBase PROPERTIES DEBUG_OUTPUT_NAME "FreeCADBaseD")
		set_target_properties(FreeCADBase PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin)
endif(MSVC)

if(WIN32)
	INSTALL(TARGETS FreeCADBase
	  RUNTIME DESTINATION bin
	  LIBRARY DESTINATION lib
	)
else(WIN32)
	INSTALL(TARGETS FreeCADBase
	  LIBRARY DESTINATION lib
	)
endif(WIN32)

