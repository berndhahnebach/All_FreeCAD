set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(App)
fc_copy_to_mod_path("Part" Init.py)

if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
	fc_copy_to_mod_path("Part" InitGui.py)
endif(FREECAD_BUILD_GUI)


install(
		 FILES
		 		 Init.py
		 		 InitGui.py

		 DESTINATION
		 		 Mod/Part)