include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(AssemblyGui_LIBS
		 		 Assembly
		 		 FreeCADGui)
else(WIN32)
		 set(AssemblyGui_LIBS
#libAssemblyGui_la_LIBADD   = \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lxerces-c \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lAssembly
)
endif(WIN32)

set(AssemblyGui_MOC_HDRS
		 AssemblyView.h
)
fc_wrap_cpp(AssemblyGui_MOC_SRCS ${AssemblyGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${AssemblyGui_MOC_SRCS})

fc_add_resources(AssemblyGui_SRCS Resources/Assembly.qrc)

SET(AssemblyGui_SRCS
		 ${AssemblyGui_SRCS}
		 AppAssemblyGui.cpp
		 AppAssemblyGuiPy.cpp
		 Command.cpp
		 Resources/Assembly.qrc
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(AssemblyGui SHARED ${AssemblyGui_SRCS})

target_link_libraries(AssemblyGui ${AssemblyGui_LIBS})

set_target_properties(AssemblyGui PROPERTIES SUFFIX ".pyd")

install(TARGETS AssemblyGui DESTINATION lib)
