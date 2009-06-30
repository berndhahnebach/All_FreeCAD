copy_to_local_output_paths(Init.py)
copy_to_local_output_paths(InitGui.py)
copy_to_local_output_paths(Commands.py)
copy_to_local_output_paths(PythonQt.py)
copy_to_local_output_paths(Tests.py)

install(
		 FILES
		 		 Init.py
		 		 InitGui.py
		 		 Commands.py
				 FeaturePython.py
		 		 PythonQt.py
		 		 Tests.py
		 DESTINATION
		 		 Mod/TemplatePyMod)
