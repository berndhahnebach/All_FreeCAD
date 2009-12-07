
SET(Test_SRCS
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
	InitGui.py
)
SOURCE_GROUP("" FILES ${Test_SRCS})

ADD_CUSTOM_TARGET(Test ALL
	SOURCES ${Test_SRCS}
)

fc_copy_to_mod_path("Test" Init.py)
fc_copy_to_mod_path("Test" Base.py)
fc_copy_to_mod_path("Test" Document.py)
fc_copy_to_mod_path("Test" Menu.py)
fc_copy_to_mod_path("Test" TestApp.py)
fc_copy_to_mod_path("Test" TestGui.py)
fc_copy_to_mod_path("Test" UnicodeTests.py)
fc_copy_to_mod_path("Test" Workbench.py)
fc_copy_to_mod_path("Test" InitGui.py)
fc_copy_to_mod_path("Test" Gui/qtunittest.py)


install(
		FILES
				${Test_SRCS}
		DESTINATION
		 		Mod/Test)

if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
endif(FREECAD_BUILD_GUI)
