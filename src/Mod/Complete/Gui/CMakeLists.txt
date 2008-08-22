include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(CompleteGui_LIBS
		 		 Complete
		 		 FreeCADGui)
else(WIN32)
		 set(CompleteGui_LIBS
#libCompleteGui_la_LIBADD   = \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lxerces-c \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lComplete
)
endif(WIN32)

set(CompleteGui_MOC_HDRS
		 CompleteView.h
)
fc_wrap_cpp(CompleteGui_MOC_SRCS ${CompleteGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${CompleteGui_MOC_SRCS})

fc_add_resources(CompleteGui_SRCS Resources/Complete.qrc)

SET(CompleteGui_SRCS
		 ${CompleteGui_SRCS}
		 AppCompleteGui.cpp
		 AppCompleteGuiPy.cpp
		 Command.cpp
		 Resources/Complete.qrc
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(CompleteGui SHARED ${CompleteGui_SRCS})

target_link_libraries(CompleteGui ${CompleteGui_LIBS})

set_target_properties(CompleteGui PROPERTIES SUFFIX ".pyd")

install(TARGETS CompleteGui DESTINATION lib)
