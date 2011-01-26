include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${COIN_INCLUDE_DIR}
    ${QT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
    ${XERCESC_INCLUDE_DIR}
)

set(InspectionGui_LIBS
    Inspection
    FreeCADGui
)

qt4_add_resources(Inspection_QRC_SRCS Resources/Inspection.qrc)

SET(InspectionGui_SRCS
    ${Inspection_QRC_SRCS}
    AppInspectionGui.cpp
    AppInspectionGuiPy.cpp
    Command.cpp
    PreCompiled.cpp
    PreCompiled.h
    Workbench.cpp
    Workbench.h
)

add_library(InspectionGui SHARED ${InspectionGui_SRCS})
target_link_libraries(InspectionGui ${InspectionGui_LIBS})
fc_copy_script("Mod/Inspection" "InspectionGui" InitGui.py)

if(MSVC)
    set_target_properties(InspectionGui PROPERTIES SUFFIX ".pyd")
    set_target_properties(InspectionGui PROPERTIES DEBUG_OUTPUT_NAME "InspectionGui_d")
    set_target_properties(InspectionGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Inspection)
    set_target_properties(InspectionGui PROPERTIES PREFIX "../")
elseif(MINGW)
    set_target_properties(InspectionGui PROPERTIES SUFFIX ".pyd")
    set_target_properties(InspectionGui PROPERTIES DEBUG_OUTPUT_NAME "InspectionGui_d")
    set_target_properties(InspectionGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Inspection)
    set_target_properties(InspectionGui PROPERTIES PREFIX "")
else(MSVC)
    set_target_properties(InspectionGui PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Inspection)
    set_target_properties(InspectionGui PROPERTIES PREFIX "")
endif(MSVC)

install(TARGETS InspectionGui DESTINATION lib)
