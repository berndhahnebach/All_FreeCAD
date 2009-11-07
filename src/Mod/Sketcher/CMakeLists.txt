
add_subdirectory(App)
fc_copy_to_mod_path("Sketcher" Init.py)

if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
	fc_copy_to_mod_path("Sketcher" InitGui.py)
endif(FREECAD_BUILD_GUI)

configure_file(Templates/Sketch.skf ${CMAKE_BINARY_DIR}/Mod/Sketcher/Templates/Sketch.skf COPYONLY)

install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 DESTINATION
		 		 Mod/Sketcher)
