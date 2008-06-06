add_definitions(-DFCAppPoints -DFC_DEBUG)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${Boost_INCLUDE_DIRS}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(Points_LIBS
		 		 FreeCADApp)
else(WIN32)
#libPoints_la_LIBADD   = \
#		 -l@PYTHON_LIB@ \
#		 -lxerces-c \
#		 -lboost_regex \
#		 -lFreeCADBase \
#		 -lFreeCADApp
endif(WIN32)

SET(Points_SRCS
		 AppPoints.cpp
		 AppPointsPy.cpp
		 FeaturePointsImportAscii.cpp
		 FeaturePointsImportAscii.h
		 Points.cpp
		 Points.h
		 PointsAlgos.cpp
		 PointsAlgos.h
		 PointsFeature.cpp
		 PointsFeature.h
		 PointsFeaturePy.cpp
		 PointsFeaturePy.h
		 PointsGrid.cpp
		 PointsGrid.h
		 PointsPy.cpp
		 PointsPy.h
		 PreCompiled.cpp
		 PreCompiled.h
		 PropertyPointKernel.cpp
		 PropertyPointKernel.h
)

add_library(Points SHARED ${Points_SRCS})

target_link_libraries(Points ${Points_LIBS})

set_target_properties(Points PROPERTIES SUFFIX ".pyd")

install(TARGETS Points DESTINATION lib)
