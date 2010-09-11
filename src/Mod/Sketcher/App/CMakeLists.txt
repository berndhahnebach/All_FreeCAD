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
    ${XERCESC_INCLUDE_DIR}
)
link_directories(${OCC_LIBRARY_DIR})

set(Sketcher_LIBS
    Part
    FreeCADApp
)

generate_from_xml(SketchObjectSFPy)
generate_from_xml(SketchObjectPy)
generate_from_xml(ConstraintPy)
generate_from_xml(SketchPy)


SET(Properties_SRCS
    PropertyConstraintList.cpp
    PropertyConstraintList.h
)
SOURCE_GROUP("Properties" FILES ${Properties_SRCS})


SET(Features_SRCS
    SketchObjectSF.cpp
    SketchObjectSF.h
    SketchObject.cpp
    SketchObject.h
)
SOURCE_GROUP("Features" FILES ${Features_SRCS})

SET(Datatypes_SRCS
    Constraint.cpp
    Constraint.h
    Sketch.cpp
    Sketch.h
)
SOURCE_GROUP("Datatypes" FILES ${Datatypes_SRCS})

SET(Python_SRCS
    SketchObjectSFPy.xml
    SketchObjectSFPyImp.cpp
    SketchObjectPy.xml
    SketchObjectPyImp.cpp
    ConstraintPyImp.cpp
    ConstraintPy.xml
    SketchPy.xml
    SketchPyImp.cpp
)
SOURCE_GROUP("Python" FILES ${Python_SRCS})

SET(Sketchflat_SRCS
    sketchflat/assume.cpp
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
    sketchflat/sketch_constraint.cpp
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
    #sketchsolve_cp/Derivatives.cpp
    sketchsolve_cp/solve.cpp
    sketchsolve_cp/solve.h
    #sketchsolve_cp/SolverPointers.cpp
    sketchsolve_cp/timer.h
    sketchsolve_cp/errorfuncs.cpp
    sketchsolve_cp/solveimpl.cpp
)
SOURCE_GROUP("Sketchsolve" FILES ${SketchSolver_SRCS})

SET(SketchModule_SRCS
    AppSketcher.cpp
    AppSketcherPy.cpp
    PreCompiled.cpp
    PreCompiled.h
    SketchFlatInterface.cpp
    SketchFlatInterface.h
)
SOURCE_GROUP("Module" FILES ${SketchModule_SRCS})

SET(Sketcher_SRCS
    ${Features_SRCS}
    ${Sketchflat_SRCS}
    ${SketchSolver_SRCS}
    ${SketchModule_SRCS}
    ${Python_SRCS}
    ${Properties_SRCS}
    ${Datatypes_SRCS}
)

SET(Sketcher_Scripts
    Init.py
    SketcherExample.py
    Templates/Sketch.skf
)

add_library(Sketcher SHARED ${Sketcher_SRCS})
target_link_libraries(Sketcher ${Sketcher_LIBS})
fc_copy_script("Mod/Sketcher" "Sketcher" ${Sketcher_Scripts})
fc_copy_sources("Mod/Sketcher" "Sketcher" SketcherTestsApp.py)

if(MSVC)
    set_target_properties(Sketcher PROPERTIES SUFFIX ".pyd")
    set_target_properties(Sketcher PROPERTIES DEBUG_OUTPUT_NAME "Sketcher_d")
    set_target_properties(Sketcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Sketcher)
    set_target_properties(Sketcher PROPERTIES PREFIX "../")
elseif(MINGW)
    set_target_properties(Sketcher PROPERTIES SUFFIX ".pyd")
    set_target_properties(Sketcher PROPERTIES DEBUG_OUTPUT_NAME "Sketcher_d")
    set_target_properties(Sketcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Sketcher)
    set_target_properties(Sketcher PROPERTIES PREFIX "")
else(MSVC)
    set_target_properties(Sketcher PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Sketcher)
    set_target_properties(Sketcher PROPERTIES PREFIX "")
endif(MSVC)

install(TARGETS Sketcher DESTINATION lib)
