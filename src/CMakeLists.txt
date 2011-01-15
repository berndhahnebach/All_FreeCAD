
add_subdirectory(Build)
add_subdirectory(3rdParty)
add_subdirectory(Base)
add_subdirectory(App)
add_subdirectory(Main)
add_subdirectory(Mod)
if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
	configure_file(Doc/Start_Page.html ${CMAKE_BINARY_DIR}/Doc/Start_Page.html COPYONLY)
endif(FREECAD_BUILD_GUI)

#if(FREECAD_MAINTAINERS_BUILD)
	add_subdirectory(Doc)
#endif(FREECAD_MAINTAINERS_BUILD)

if(FREECAD_MAINTAINERS_BUILD AND WIN32)
	#add_subdirectory(WindowsInstaller)
endif(FREECAD_MAINTAINERS_BUILD AND WIN32)
