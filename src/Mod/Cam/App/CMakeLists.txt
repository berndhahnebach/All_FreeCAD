add_definitions(-DFCAppCam)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_SOURCE_DIR}/src/3rdParty
		 ${CMAKE_SOURCE_DIR}/src/3rdParty/ANN/include
		 ${CMAKE_SOURCE_DIR}/src/3rdParty/OCCAdaptMesh/Include
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(Cam_LIBS
		 		 Mesh
		 		 Part
		 		 ANN
		 		 ${ATLAS_LIBRARIES})
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
#		 Approx.cpp
#		 Approx.h
		 best_fit.cpp
		 best_fit.h
		 BRepAdaptor_CompCurve2.cxx
		 BRepAdaptor_CompCurve2.h
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

set_target_properties(Cam PROPERTIES SUFFIX ".pyd")

install(TARGETS Cam DESTINATION lib)
