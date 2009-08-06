
add_subdirectory(3rdParty)
add_subdirectory(Base)
add_subdirectory(App)
add_subdirectory(Main)
add_subdirectory(Mod)
if(FREECAD_BUILD_GUI)
	add_subdirectory(Gui)
endif(FREECAD_BUILD_GUI)

if(FREECAD_BUILD_DOXYGEN_DOCU)
	add_subdirectory(Doc)
endif(FREECAD_BUILD_DOXYGEN_DOCU)

if(FREECAD_BUILD_INSTALLER)
	add_subdirectory(WindowsInstaller)
endif(FREECAD_BUILD_INSTALLER)
