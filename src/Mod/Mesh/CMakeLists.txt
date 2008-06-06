add_subdirectory(App .)
add_subdirectory(Gui .)

copy_to_local_output_paths(Init.py)
copy_to_local_output_paths(InitGui.py)
copy_to_local_output_paths(BuildRegularGeoms.py)
copy_to_local_output_paths(App/MeshTestsApp.py)

install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 		 BuildRegularGeoms.py
		 DESTINATION
		 		 Mod/Mesh)
