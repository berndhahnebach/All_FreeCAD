if(WIN32)
	add_definitions(-DWNT -DFCAppCam -DHAVE_ACOSH -DHAVE_ATANH -DHAVE_ASINH)
else(WIN32)
	add_definitions(-DFCAppCam)
endif(WIN32)
	
include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_SOURCE_DIR}/src/3rdParty
		 ${CMAKE_SOURCE_DIR}/src/3rdParty/ANN/include
		 #${CMAKE_SOURCE_DIR}/src/3rdParty/OCCAdaptMesh/Include
		 ${Boost_INCLUDE_DIRS}
		 ${QT_INCLUDE_DIR}
		 ${OCC_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(Cam_LIBS
		 		 Mesh
		 		 Part
				 ${QT_DEBUG_LIBRARIES}
				 ${QT_LIBRARIES}
		 		 ANN
		 		 #${ATLAS_LIBRARIES}
				 importlib_atlas.lib 
				 importlib_umfpackamd.lib
				 )
else(WIN32)
		 set(Cam_LIBS
#libCam_la_LIBADD		 = \
#		 		 -lxerces-c \
#		 		 -lboost_filesystem \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lOCCAdaptMesh \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lPart \
#		 		 -lMesh \
#		 		 -lTKernel \
#		 		 -lTKG2d \
#		 		 -lTKG3d \
#		 		 -lTKMath \
#		 		 -lTKSTEP \
#		 		 -lTKIGES \
#		 		 -lTKSTL \
#		 		 -lTKShHealing \
#		 		 -lTKXSBase \
#		 		 -lTKBool \
#		 		 -lTKBO \
#		 		 -lTKBRep \
#		 		 -lTKTopAlgo \
#		 		 -lTKGeomAlgo \
#		 		 -lTKGeomBase \
#		 		 -lTKOffset \
#		 		 -lTKPrim \
#		 		 -lTKMesh \
#		 		 -lblas \
#		 		 -lumfpack \
#		 		 -lamd
)
endif(WIN32)

SET(Cam_SRCS
		 AppCamPy.cpp
		 deviation.cpp
		 deviation.h
		 Approx.cpp
		 Approx.h
		 mergedata.h
		 mergedata.cpp
		 best_fit.cpp
		 best_fit.h
		 BRepAdaptor_CompCurve2.cxx
		 BRepAdaptor_CompCurve2.h
		 BRepUtils.h
		 BRepUtils.cpp
		 ChangeDyna.cpp
		 ChangeDyna.h
		 ConvertDyna.cpp
		 ConvertDyna.h
		 cutting_tools.cpp
		 cutting_tools.h
		 edgesort.cpp
		 edgesort.h
		 path_simulate.cpp
		 path_simulate.h
		 PreCompiled.cpp
		 PreCompiled.h
		 routine.cpp
		 routine.h
		 stuff.h
		 SpringbackCorrection.cpp
		 SpringbackCorrection.h
		 UniGridApprox.cpp
		 UniGridApprox.h
		 WireExplorer.cxx
		 WireExplorer.h
		 AppCam.cpp
)

add_library(Cam SHARED ${Cam_SRCS})

target_link_libraries(Cam ${Cam_LIBS})

if(WIN32)
		 set_target_properties(Cam PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Cam PROPERTIES DEBUG_OUTPUT_NAME "Cam_d")
		 set_target_properties(Cam PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Cam)
		 set_target_properties(Cam PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(Cam PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Cam DESTINATION lib)
