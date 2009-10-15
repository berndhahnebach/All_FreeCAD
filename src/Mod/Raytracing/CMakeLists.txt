
add_subdirectory(App)
fc_copy_to_mod_path("Raytracing" Init.py)

if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
	fc_copy_to_mod_path("Raytracing" InitGui.py)
endif(FREECAD_BUILD_GUI)

configure_file(Templates/ProjectStd.pov ${CMAKE_BINARY_DIR}/Mod/Raytracing/Templates/ProjectStd.pov COPYONLY)

install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 DESTINATION
		 		 Mod/Raytracing)
