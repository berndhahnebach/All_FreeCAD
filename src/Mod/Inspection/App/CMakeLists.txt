
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
    ${ZLIB_INCLUDE_DIR}
    ${XERCES_INCLUDE_DIR}
)

set(Inspection_LIBS
    FreeCADApp
)

SET(Inspection_SRCS
    AppInspection.cpp
    AppInspectionPy.cpp
    PreCompiled.cpp
    PreCompiled.h
)

add_library(Inspection SHARED ${Inspection_SRCS})
target_link_libraries(Inspection ${Inspection_LIBS})
fc_copy_script("Mod/Inspection" "Inspection" Init.py)

if(MSVC)
    set_target_properties(Inspection PROPERTIES SUFFIX ".pyd")
    set_target_properties(Inspection PROPERTIES DEBUG_OUTPUT_NAME "Inspection_d")
    set_target_properties(Inspection PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Inspection)
    set_target_properties(Inspection PROPERTIES PREFIX "../")
elseif(MINGW)
    set_target_properties(Inspection PROPERTIES SUFFIX ".pyd")
    set_target_properties(Inspection PROPERTIES DEBUG_OUTPUT_NAME "Inspection_d")
    set_target_properties(Inspection PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Inspection)
    set_target_properties(Inspection PROPERTIES PREFIX "")
else(MSVC)
    set_target_properties(Inspection PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Inspection)
    set_target_properties(Inspection PROPERTIES PREFIX "")
endif(MSVC)

install(TARGETS Inspection DESTINATION lib)
