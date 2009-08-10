add_subdirectory(Points)
add_subdirectory(Complete)
add_subdirectory(Test)
#add_subdirectory(TemplatePyMod)

add_subdirectory(Image)

add_subdirectory(Mesh)

add_subdirectory(Part)
add_subdirectory(PartDesign)
add_subdirectory(Raytracing)
add_subdirectory(Drawing)
add_subdirectory(Sketcher)

add_subdirectory(ReverseEngineering)
add_subdirectory(MeshPart)
add_subdirectory(Draft)
add_subdirectory(Assembly)
if(FREECAD_BUILD_CAM)
	add_subdirectory(Cam)
endif(FREECAD_BUILD_CAM)

if(FREECAD_BUILD_ROBOT)
	add_subdirectory(Robot)
endif(FREECAD_BUILD_ROBOT)
