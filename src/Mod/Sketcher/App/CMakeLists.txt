add_definitions(-DFCAppSketcher -DFC_DEBUG)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(Sketcher_LIBS
		 		 Part
		 		 TKHLR
		 		 FreeCADApp)
else(WIN32)
#libSketcher_la_LIBADD   = \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lboost_regex \
#		 		 -lxerces-c \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lPart \
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
#		 		 -lTKHLR
endif(WIN32)

SET(Features_SRCS
		 FeaturePage.cpp
		 FeaturePage.h
		 FeatureView.cpp
		 FeatureView.h
		 FeatureViewPart.cpp
		 FeatureViewPart.h
		 PageGroup.cpp
		 PageGroup.h
)
SOURCE_GROUP("Features" FILES ${Features_SRCS})

SET(Sketcher_SRCS
		 ${Features_SRCS}
		 AppSketcher.cpp
		 AppSketcherPy.cpp
		 PreCompiled.cpp
		 PreCompiled.h
)

add_library(Sketcher SHARED ${Sketcher_SRCS})

target_link_libraries(Sketcher ${Sketcher_LIBS})

set_target_properties(Sketcher PROPERTIES SUFFIX ".pyd")

install(TARGETS Sketcher DESTINATION lib)
