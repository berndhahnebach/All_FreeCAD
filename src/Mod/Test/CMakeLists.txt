set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
	file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/InitGui.py" NATIVE_SOURCE)
	file(TO_NATIVE_PATH "../../../Mod/Test/" NATIVE_DEST)
	execute_process( COMMAND ${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_DEST} )
endif(FREECAD_BUILD_GUI)

file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Init.py" NATIVE_SOURCE)
file(TO_NATIVE_PATH "../../../Mod/Test/" NATIVE_DEST)
execute_process( COMMAND ${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_DEST} )

copy_to_local_output_paths(Base.py)
copy_to_local_output_paths(Document.py)
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
		 		 Gui/qtunittest.py
		 DESTINATION
		 		 Mod/Test)
