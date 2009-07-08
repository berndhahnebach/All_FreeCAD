include_directories(
#		 ${CMAKE_SOURCE_DIR}/src
#		 ${CMAKE_CURRENT_BINARY_DIR}
#		 ${Boost_INCLUDE_DIRS}
#		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${QT_INCLUDE_DIR}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(ImportGui_LIBS
		 		 Impor
		 		 FreeCADGui)
else(WIN32)
		 set(ImportGui_LIBS
#libImportGui_la_LIBADD   = \
#		 		 -lTKPrim \
#		 		 -lTKV2d \
#		 		 -lTKV3d \
#		 		 -lFreeCADGui \
#		 		 -lPartGui \
#		 		 -lImport
)
endif(WIN32)

#fc_add_resources(ImportGui_SRCS Import.qrc)

SET(ImportGui_SRCS
#		 ${ImportGui_SRCS}
		 AppImportGui.cpp
		 Command.cpp
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(ImportGui SHARED ${ImportGui_SRCS})

target_link_libraries(ImportGui ${ImportGui_LIBS})

set_target_properties(ImportGui PROPERTIES SUFFIX ".pyd")

install(TARGETS ImportGui DESTINATION lib)
