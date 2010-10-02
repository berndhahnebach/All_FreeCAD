if(MSVC)
    add_definitions(-DFCAppMeshPart -DHAVE_ACOSH -DHAVE_ASINH -DHAVE_ATANH)
else(MSVC)
    add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)

# At the moment we have SMESH only for MSVC on Win32
if (MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    add_definitions(-DHAVE_SMESH)
endif(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 4)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
    ${XERCESC_INCLUDE_DIR}
    ${SMESH_INCLUDE_DIR}
)
link_directories(${OCC_LIBRARY_DIR})

if(MSVC)
set(MeshPart_LIBS
    Part
    Mesh
    ${SMESH_LIBRARIES}
)
else(MSVC)
set(MeshPart_LIBS
    Part
    Mesh
)
endif(MSVC)

SET(MeshPart_SRCS
    AppMeshPart.cpp
    AppMeshPartPy.cpp
    CurveProjector.cpp
    CurveProjector.h
    MeshAlgos.cpp
    MeshAlgos.h
    Mesher.cpp
    Mesher.h
    PreCompiled.cpp
    PreCompiled.h
)

add_library(MeshPart SHARED ${MeshPart_SRCS})
target_link_libraries(MeshPart ${MeshPart_LIBS})
fc_copy_script("Mod/MeshPart" "MeshPart" Init.py)

if(MSVC)
    set_target_properties(MeshPart PROPERTIES SUFFIX ".pyd")
    set_target_properties(MeshPart PROPERTIES DEBUG_OUTPUT_NAME "MeshPart_d")
    set_target_properties(MeshPart PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/MeshPart)
    set_target_properties(MeshPart PROPERTIES PREFIX "../")
elseif(MINGW)
    set_target_properties(MeshPart PROPERTIES SUFFIX ".pyd")
    set_target_properties(MeshPart PROPERTIES DEBUG_OUTPUT_NAME "MeshPart_d")
    set_target_properties(MeshPart PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/MeshPart)
    set_target_properties(MeshPart PROPERTIES PREFIX "")
else(MSVC)
    set_target_properties(MeshPart PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/MeshPart)
    set_target_properties(MeshPart PROPERTIES PREFIX "")
endif(MSVC)

install(TARGETS MeshPart DESTINATION lib)
