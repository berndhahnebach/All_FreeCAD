if(MSVC)
    add_definitions(-DFCAppPartDesign -DHAVE_ACOSH -DHAVE_ASINH -DHAVE_ATANH)
else(MSVC)
    add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
    ${XERCESC_INCLUDE_DIR}
)
link_directories(${OCC_LIBRARY_DIR})

set(PartDesign_LIBS
    Part
    FreeCADApp
)

SET(Features_SRCS
    FeatureFillet.cpp
    FeatureFillet.h
    FeaturePad.cpp
    FeaturePad.h
    FeaturePocket.cpp
    FeaturePocket.h
)
SOURCE_GROUP("Features" FILES ${Features_SRCS})

SET(PartDesign_SRCS
    ${Features_SRCS}
    AppPartDesign.cpp
    AppPartDesignPy.cpp
    PreCompiled.cpp
    PreCompiled.h
)

SET(PartDesign_Scripts
    __init__.py
    Scripts/__init__.py
    Scripts/DistanceBolt.py
    Scripts/RadialCopy.py
)

add_library(PartDesign SHARED ${PartDesign_SRCS})
target_link_libraries(PartDesign ${PartDesign_LIBS})
fc_copy_script("Mod/PartDesign" "PartDesign" Init.py)
fc_copy_script("Mod/PartDesign" "PartDesign" ${PartDesign_Scripts})

if(MSVC)
    set_target_properties(PartDesign PROPERTIES SUFFIX ".pyd")
    set_target_properties(PartDesign PROPERTIES DEBUG_OUTPUT_NAME "PartDesign_d")
    set_target_properties(PartDesign PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/PartDesign)
    set_target_properties(PartDesign PROPERTIES PREFIX "../")
elseif(MINGW)
    set_target_properties(PartDesign PROPERTIES SUFFIX ".pyd")
    set_target_properties(PartDesign PROPERTIES DEBUG_OUTPUT_NAME "PartDesign_d")
    set_target_properties(PartDesign PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/PartDesign)
    set_target_properties(PartDesign PROPERTIES PREFIX "")
else(MSVC)
    set_target_properties(PartDesign PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/PartDesign)
    set_target_properties(PartDesign PROPERTIES PREFIX "")
endif(MSVC)

install(TARGETS PartDesign DESTINATION lib)
