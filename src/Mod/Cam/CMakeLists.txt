set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

aadd_subdirectory(App)
fc_copy_to_mod_path("Cam" Init.py)

if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
	fc_copy_to_mod_path("Cam" InitGui.py)
endif(FREECAD_BUILD_GUI)

install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 DESTINATION
		 		 Mod/Cam)
