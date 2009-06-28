add_subdirectory(Gui)

copy_to_local_output_paths(Base.py)
copy_to_local_output_paths(Document.py)
copy_to_local_output_paths(Init.py)
copy_to_local_output_paths(InitGui.py)
copy_to_local_output_paths(Menu.py)
copy_to_local_output_paths(TestApp.py)
copy_to_local_output_paths(TestGui.py)
copy_to_local_output_paths(UnicodeTests.py)
copy_to_local_output_paths(Workbench.py)
copy_to_local_output_paths(unittestgui.py)
copy_to_local_output_paths(Gui/qtunittest.py)

install(
		 FILES
		 		 Base.py
		 		 Document.py
		 		 Init.py
		 		 InitGui.py
		 		 Menu.py
		 		 TestApp.py
		 		 TestGui.py
		 		 UnicodeTests.py
		 		 Workbench.py
		 DESTINATION
		 		 Mod/Test)
