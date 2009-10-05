if(MSVC)
		 add_definitions(-DFCAppSketcher -DHAVE_ACOSH -DHAVE_ASINH -DHAVE_ATANH)
else(MSVC)
		 add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_SOURCE_DIR}
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${CMAKE_CURRENT_SOURCE_DIR}/..
		 ${CMAKE_CURRENT_BINARY_DIR}/..
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(Sketcher_LIBS
		 		 Part
		 		 TKHLR
		 		 FreeCADApp)
else(WIN32)
		 set(Sketcher_LIBS
		 		 Part
		 		 TKHLR
		 		 FreeCADApp)
endif(WIN32)

generate_from_xml(SketchObjectPy)

SET(Features_SRCS
		 SketchObject.cpp
		 SketchObject.h
		 SketchObjectPy.xml
		 SketchObjectPyImp.cpp
)
SOURCE_GROUP("Features" FILES ${Features_SRCS})

SET(Sketchflat_SRCS
		 sketchflat/assume.cpp
		 sketchflat/constraint.cpp
		 sketchflat/curve.cpp
		 sketchflat/derive.cpp
		 sketchflat/derived.h
		 sketchflat/draw.cpp
		 sketchflat/draw_constraint.cpp
		 sketchflat/draw_sketch.cpp
		 sketchflat/export.cpp
		 sketchflat/expr.cpp
		 sketchflat/expr.h
		 sketchflat/FCsimple.cpp
		 sketchflat/FCtext.cpp
		 sketchflat/FCutil.cpp
		 sketchflat/FCwindow.cpp
		 sketchflat/layer.cpp
		 sketchflat/loadsave.cpp
		 sketchflat/measure.cpp
		 sketchflat/newton.cpp
		 sketchflat/polygon.cpp
		 sketchflat/sketch.cpp
		 sketchflat/sketch.h
		 sketchflat/sketchflat.cpp
		 sketchflat/sketchflat.h
		 sketchflat/solve.cpp
		 sketchflat/ttf.cpp
		 sketchflat/ui.h
		 sketchflat/undoredo.cpp
		 sketchflat/util.cpp
		 sketchflat/COPYING.txt
		 sketchflat/README.txt
)
SOURCE_GROUP("Sketchflat" FILES ${Sketchflat_SRCS})

SET(SketchSolver_SRCS
		 sketchsolver/Derivatives.cpp
		 sketchsolver/solve.cpp
		 sketchsolver/solve.h
		 sketchsolver/SolverPointers.cpp
		 sketchsolver/timer.h
)
SOURCE_GROUP("Sketchsolver" FILES ${SketchSolver_SRCS})

SET(Sketcher_SRCS
		 ${Features_SRCS}
		 ${Sketchflat_SRCS}
		 ${SketchSolver_SRCS}
		 AppSketcher.cpp
		 AppSketcherPy.cpp
		 PreCompiled.cpp
		 PreCompiled.h
		 SketchFlatInterface.cpp
		 SketchFlatInterface.h
)

add_library(Sketcher SHARED ${Sketcher_SRCS})

target_link_libraries(Sketcher ${Sketcher_LIBS})

if(WIN32)
		 set_target_properties(Sketcher PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Sketcher PROPERTIES DEBUG_OUTPUT_NAME "Sketcher_d")
		 set_target_properties(Sketcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Sketcher)
		 set_target_properties(Sketcher PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(Sketcher PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Sketcher)
		 set_target_properties(Sketcher PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Sketcher DESTINATION lib)
