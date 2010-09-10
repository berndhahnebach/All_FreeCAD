include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${COIN3D_INCLUDE_DIR}
    ${QT_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${SOQT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
    ${XERCESC_INCLUDE_DIR}
)
link_directories(${OCC_LIBRARY_DIR})

set(MeshPartGui_LIBS
    MeshPart
    FreeCADGui
)

qt4_add_resources(MeshPartGui_QRC_SRCS Resources/MeshPart.qrc)

SET(MeshPartGui_SRCS
    ${MeshPartGui_QRC_SRCS}
    AppMeshPartGui.cpp
    AppMeshPartGuiPy.cpp
    Command.cpp
    Resources/MeshPart.qrc
    PreCompiled.cpp
    PreCompiled.h
    Workbench.cpp
    Workbench.h
)

add_library(MeshPartGui SHARED ${MeshPartGui_SRCS})
target_link_libraries(MeshPartGui ${MeshPartGui_LIBS})
fc_copy_script("Mod/MeshPart" "MeshPartGui" InitGui.py)

if(MSVC)
    set_target_properties(MeshPartGui PROPERTIES SUFFIX ".pyd")
    set_target_properties(MeshPartGui PROPERTIES DEBUG_OUTPUT_NAME "MeshPartGui_d")
    set_target_properties(MeshPartGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/MeshPart)
    set_target_properties(MeshPartGui PROPERTIES PREFIX "../")
elseif(MINGW)
    set_target_properties(MeshPartGui PROPERTIES SUFFIX ".pyd")
    set_target_properties(MeshPartGui PROPERTIES DEBUG_OUTPUT_NAME "MeshPartGui_d")
    set_target_properties(MeshPartGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/MeshPart)
    set_target_properties(MeshPartGui PROPERTIES PREFIX "")
else(MSVC)
    set_target_properties(MeshPartGui PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/MeshPart)
    set_target_properties(MeshPartGui PROPERTIES PREFIX "")
endif(MSVC)

install(TARGETS MeshPartGui DESTINATION lib)
