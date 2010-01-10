
SET(Test_SRCS
	Init.py
	Base.py
	Document.py
	Menu.py
	TestApp.py
	TestGui.py
	UnicodeTests.py
	UnitTests.py
	Workbench.py
	unittestgui.py
	InitGui.py
)
SOURCE_GROUP("" FILES ${Test_SRCS})

ADD_CUSTOM_TARGET(Test ALL
	SOURCES ${Test_SRCS}
)

COPY_IF_DIFFERENT( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/Mod/Test ${Test_SRCS} Test "TestScripts")

fc_copy_sources("Mod/Test" "Test" ${Test_SRCS})

install(
		FILES
				${Test_SRCS}
		DESTINATION
		 		Mod/Test)

if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
endif(FREECAD_BUILD_GUI)
