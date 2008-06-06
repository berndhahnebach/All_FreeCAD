add_definitions(-DFCAppImage -DFC_DEBUG)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${OPENCV_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(Image_LIBS
		 		 ${OPENCV_LIBRARIES}
		 		 FreeCADApp)
else(WIN32)
		 set(Image_LIBS
#		 		 -l@PYTHON_LIB@ \
#		 		 -lxerces-c \
#		 		 -lcv \
#		 		 -lcxcore \
#		 		 -lhighgui \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp
)
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

set_target_properties(Image PROPERTIES SUFFIX ".pyd")

install(TARGETS Image DESTINATION lib)
