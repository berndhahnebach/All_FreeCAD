if(WIN32)
		 add_definitions(-DFCAppImage -DFC_DEBUG)
endif(WIN32)

include_directories(
		 ${OPENCV_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
 		 ${ZLIB_INCLUDE_DIR}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(Image_LIBS
		 		 ${OPENCV_LIBRARIES}
		 		 FreeCADApp)
else(WIN32)
		 set(Image_LIBS
		 		 ${OPENCV_LIBRARIES}
		 		 FreeCADApp)
endif(WIN32)

set(Image_SRCS
		 CaptureClass.cpp
		 CaptureClass.h
		 ImageBase.cpp
		 ImageBase.h
		 PreCompiled.cpp
		 PreCompiled.h
		 AppImage.cpp
)

add_library(Image SHARED ${Image_SRCS})

target_link_libraries(Image ${Image_LIBS})

if(WIN32)
		 set_target_properties(Image PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Image PROPERTIES DEBUG_OUTPUT_NAME "Image_d")
		 set_target_properties(Image PROPERTIES PREFIX "../../../../Mod/Image/")
else(WIN32)
		 set_target_properties(Image PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Image DESTINATION lib)
