set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(App)
add_subdirectory(Gui)

#copy_to_local_output_paths(Init.py)
#copy_to_local_output_paths(InitGui.py)

execute_process(
		 COMMAND ${PLATFORM_CP} Init.py "../../../Mod/Points/Init.py"
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 DESTINATION
		 		 Mod/Points)
