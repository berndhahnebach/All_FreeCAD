IF(NOT QT4_FOUND)
	#add_subdirectory(Cam)
	#add_subdirectory(Drawing)
	#add_subdirectory(Complete)
	add_subdirectory(Part)
	#add_subdirectory(PartDesign)
	#add_subdirectory(Raytracing)
	#add_subdirectory(Sketcher)
	#add_subdirectory(Import)
ENDIF(NOT QT4_FOUND)
#add_subdirectory(Mesh)
#add_subdirectory(Points)
#add_subdirectory(TemplatePyMod)
#add_subdirectory(Test)
add_subdirectory(Image)



#EXTRA_DIST = \
#		 		 __init__.py \
#		 		 Sketcher/Init.py \
#		 		 Sketcher/InitGui.py
