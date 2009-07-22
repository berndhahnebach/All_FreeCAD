set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(App)
if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
	file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/InitGui.py" NATIVE_SOURCE)
	file(TO_NATIVE_PATH "../../../Mod/Mesh/" NATIVE_DEST)
	execute_process( COMMAND ${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_DEST} )
endif(FREECAD_BUILD_GUI)

file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Init.py" NATIVE_SOURCE)
file(TO_NATIVE_PATH "../../../Mod/Mesh/" NATIVE_DEST)
execute_process( COMMAND ${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_DEST} )

file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/BuildRegularGeoms.py" NATIVE_SOURCE)
file(TO_NATIVE_PATH "../../../Mod/Mesh/" NATIVE_DEST)
execute_process( COMMAND ${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_DEST} )
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/MeshTestsApp.py" NATIVE_SOURCE)
file(TO_NATIVE_PATH "../../../Mod/Mesh/" NATIVE_DEST)
execute_process( COMMAND ${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_DEST} )


install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 		 BuildRegularGeoms.py
				 App/MeshTestsApp.py
		 DESTINATION
		 		 Mod/Mesh)
