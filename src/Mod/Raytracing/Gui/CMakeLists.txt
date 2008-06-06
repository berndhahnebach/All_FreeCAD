include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_BINARY_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${SO_QT_INCLUDE_DIR}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(RaytracingGui_LIBS
		 		 Raytracing
		 		 FreeCADGui)
else(WIN32)
		 set(RaytracingGui_LIBS
#libRaytracingGui_la_LIBADD   = \
#		 		 -lxerces-c \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lTKernel \
#		 		 -lTKG2d \
#		 		 -lTKG3d \
#		 		 -lTKMath \
#		 		 -lTKBool \
#		 		 -lTKBRep \
#		 		 -lTKGeomAlgo \
#		 		 -lTKGeomBase \
#		 		 -lPart \
#		 		 -lRaytracing
)
endif(WIN32)

macro(generate_from_py2 BASE_NAME OUTPUT_FILE)
		 file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR}/src/Tools/PythonToCPP.py TOOL_PATH)
		 file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME} SOURCE_PATH)
		 add_custom_command(
		 		 OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILE}
		 		 COMMAND ${PYTHON_EXECUTABLE} ${TOOL_PATH} ${SOURCE_PATH} ${OUTPUT_FILE}
		 		 MAIN_DEPENDENCY ${BASE_NAME}
		 		 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		 		 COMMENT Building files out of ${BASE_NAME}.py)
endmacro(generate_from_py2)

generate_from_py2(FreeCADpov FreeCADpov.h)

set(RaytracingGui_MOC_HDRS
		 DlgSettingsRayImp.h
)
fc_wrap_cpp(RaytracingGui_MOC_SRCS ${RaytracingGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${RaytracingGui_MOC_SRCS})

fc_add_resources(Resource_SRCS Raytracing.qrc)
SET(Resource_SRCS
#		 ${Resource_SRCS}
		 Raytracing.qrc
)
SOURCE_GROUP("Resource" FILES ${Resource_SRCS})

set(RaytracingGui_UIC_SRCS
		 DlgSettingsRay.ui
		 ../../../Gui/DlgPreferences.ui
)
qt4_wrap_ui(RaytracingGui_UIC_HDRS ${RaytracingGui_UIC_SRCS})

SET(RaytracingGui_SRCS
		 ${Resource_SRCS}
		 ${RaytracingGui_UIC_HDRS}
		 AppRaytracingGui.cpp
		 Command.cpp
		 DlgSettingsRay.ui
		 DlgSettingsRayImp.cpp
		 DlgSettingsRayImp.h
		 FreeCADpov
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(RaytracingGui SHARED ${RaytracingGui_SRCS})

target_link_libraries(RaytracingGui ${RaytracingGui_LIBS})

set_target_properties(RaytracingGui PROPERTIES SUFFIX ".pyd")

install(TARGETS RaytracingGui DESTINATION lib)
