
fc_copy_to_mod_path("Test" 
	Init.py
	Base.py
	Document.py
	Menu.py
	TestApp.py
	TestGui.py
	UnicodeTests.py
	Workbench.py
	unittestgui.py
	Gui/qtunittest.py
	)

if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
	fc_copy_to_mod_path("Test" InitGui.py)
endif(FREECAD_BUILD_GUI)


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
		 		 Gui/qtunittest.py
		 DESTINATION
		 		 Mod/Test)
