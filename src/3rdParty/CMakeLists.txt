
if(FREECAD_BUILD_CAM)
	add_subdirectory(ANN)
	add_subdirectory(OCCAdaptMesh)
endif(FREECAD_BUILD_CAM)

if(FREECAD_BUILD_GUI AND WIN32)
	add_subdirectory(Pivy ../Main/pivy)
endif(FREECAD_BUILD_GUI AND WIN32)
