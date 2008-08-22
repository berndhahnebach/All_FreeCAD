include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(ReverseEngineeringGui_LIBS
		 		 ReverseEngineering
		 		 FreeCADGui)
else(WIN32)
		 set(ReverseEngineeringGui_LIBS
#libReverseEngineeringGui_la_LIBADD   = \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lxerces-c \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lReverseEngineering
)
endif(WIN32)

set(ReverseEngineeringGui_MOC_HDRS
		 ReverseEngineeringView.h
)
fc_wrap_cpp(ReverseEngineeringGui_MOC_SRCS ${ReverseEngineeringGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${ReverseEngineeringGui_MOC_SRCS})

fc_add_resources(ReverseEngineeringGui_SRCS Resources/ReverseEngineering.qrc)

SET(ReverseEngineeringGui_SRCS
		 ${ReverseEngineeringGui_SRCS}
		 AppReverseEngineeringGui.cpp
		 AppReverseEngineeringGuiPy.cpp
		 Command.cpp
		 Resources/ReverseEngineering.qrc
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(ReverseEngineeringGui SHARED ${ReverseEngineeringGui_SRCS})

target_link_libraries(ReverseEngineeringGui ${ReverseEngineeringGui_LIBS})

set_target_properties(ReverseEngineeringGui PROPERTIES SUFFIX ".pyd")

install(TARGETS ReverseEngineeringGui DESTINATION lib)
