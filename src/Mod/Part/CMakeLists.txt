set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(App)
if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
endif(FREECAD_BUILD_GUI)

#copy_to_local_output_paths(Init.py)
#copy_to_local_output_paths(InitGui.py)
execute_process(
		 COMMAND ${PLATFORM_CP} "Init.py" "../../../Mod/Part/"
		)


install(
		 FILES
		 		 Init.py
		 		 InitGui.py

		 DESTINATION
		 		 Mod/Part)