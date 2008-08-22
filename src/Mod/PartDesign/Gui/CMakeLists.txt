include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(PartDesignGui_LIBS
		 		 PartDesign
		 		 FreeCADGui)
else(WIN32)
		 set(PartDesignGui_LIBS
#libPartDesignGui_la_LIBADD   = \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lxerces-c \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lPartDesign
)
endif(WIN32)

set(PartDesignGui_MOC_HDRS
		 PartDesignView.h
)
fc_wrap_cpp(PartDesignGui_MOC_SRCS ${PartDesignGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${PartDesignGui_MOC_SRCS})

fc_add_resources(PartDesignGui_SRCS Resources/PartDesign.qrc)

SET(PartDesignGui_SRCS
		 ${PartDesignGui_SRCS}
		 AppPartDesignGui.cpp
		 AppPartDesignGuiPy.cpp
		 Command.cpp
		 Resources/PartDesign.qrc
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(PartDesignGui SHARED ${PartDesignGui_SRCS})

target_link_libraries(PartDesignGui ${PartDesignGui_LIBS})

set_target_properties(PartDesignGui PROPERTIES SUFFIX ".pyd")

install(TARGETS PartDesignGui DESTINATION lib)
