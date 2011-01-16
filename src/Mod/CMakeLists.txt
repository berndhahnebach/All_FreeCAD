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
add_subdirectory(Machining_Distortion)

add_subdirectory(ReverseEngineering)
add_subdirectory(MeshPart)
add_subdirectory(Draft)

add_subdirectory(Robot)
add_subdirectory(Web)
add_subdirectory(Start)
add_subdirectory(Idf)
add_subdirectory(Import)

if(FREECAD_BUILD_ASSEMBLY)
	add_subdirectory(Assembly)
endif(FREECAD_BUILD_ASSEMBLY)
if(FREECAD_BUILD_CAM)
	add_subdirectory(Cam)
endif(FREECAD_BUILD_CAM)
if(FREECAD_BUILD_FEM)
	add_subdirectory(Fem)
endif(FREECAD_BUILD_FEM)
if(FREECAD_BUILD_SANDBOX)
	add_subdirectory(Sandbox)
endif(FREECAD_BUILD_SANDBOX)

