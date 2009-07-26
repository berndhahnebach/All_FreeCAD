
add_subdirectory(App)
fc_copy_to_mod_path("ReverseEngineering" Init.py)

if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
	fc_copy_to_mod_path("ReverseEngineering" InitGui.py)
endif(FREECAD_BUILD_GUI)

install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 DESTINATION
		 		 Mod/ReverseEngineering)
