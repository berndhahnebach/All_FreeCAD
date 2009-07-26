
add_subdirectory(App)
fc_copy_to_mod_path("Drawing" Init.py)

if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
	fc_copy_to_mod_path("Drawing" InitGui.py)
endif(FREECAD_BUILD_GUI)

configure_file(Templates/A3_Landscape.svg ${CMAKE_BINARY_DIR}/Mod/Drawing/Templates/A3_Landscape.svg COPYONLY)

install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 DESTINATION
		 		 Mod/Drawing)
