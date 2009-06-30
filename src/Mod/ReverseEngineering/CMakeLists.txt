add_subdirectory(App)
add_subdirectory(Gui)

copy_to_local_output_paths(Init.py)
copy_to_local_output_paths(InitGui.py)

install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 DESTINATION
		 		 Mod/ReverseEngineering)
