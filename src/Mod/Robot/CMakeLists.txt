
add_subdirectory(App)
fc_copy_to_mod_path("Robot" Init.py)

if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
	fc_copy_to_mod_path("Robot" InitGui.py)
endif(FREECAD_BUILD_GUI)

fc_copy_to_path("Mod/Robot/Lib/Kuka" 
	Lib/Kuka/kr500_1.wrl
)


install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 DESTINATION
		 		 Mod/Robot)
