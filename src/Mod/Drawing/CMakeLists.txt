add_subdirectory(App)
add_subdirectory(Gui)

copy_to_local_output_paths(Init.py)
copy_to_local_output_paths(InitGui.py)

configure_file(Templates/A3_Landscape.svg ${CMAKE_CURRENT_BINARY_DIR}/Templates/A3_Landscape.svg COPYONLY)

install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 DESTINATION
		 		 Mod/Drawing)
