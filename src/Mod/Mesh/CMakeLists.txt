set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(App)
add_subdirectory(Gui)

copy_to_local_output_paths(Init.py)
copy_to_local_output_paths(InitGui.py)
copy_to_local_output_paths(BuildRegularGeoms.py)
copy_to_local_output_paths(App/MeshTestsApp.py)

install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 		 BuildRegularGeoms.py
				 App/MeshTestsApp.py
		 DESTINATION
		 		 Mod/Mesh)
