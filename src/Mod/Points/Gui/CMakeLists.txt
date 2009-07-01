include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${CMAKE_CURRENT_BINARY_DIR}/../Points
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${SO_QT_INCLUDE_DIR}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(PointsGui_LIBS
		 		 Points
		 		 FreeCADGui)
else(WIN32)
		 set(PointsGui_LIBS
		 		 Points
		 		 FreeCADGui)
endif(WIN32)

set(PointsGui_MOC_HDRS
		 DlgPointsReadImp.h
)
fc_wrap_cpp(PointsGui_MOC_SRCS ${PointsGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${PointsGui_MOC_SRCS})

set(Dialog_UIC_SRCS
		 DlgPointsRead.ui
)
qt4_wrap_ui(Dialogs_UIC_HDRS ${Dialog_UIC_SRCS})
SET(Dialogs_SRCS
		 ${Dialogs_UIC_HDRS}
		 DlgPointsRead.ui
		 DlgPointsReadImp.cpp
		 DlgPointsReadImp.h
)
SOURCE_GROUP("Dialogs" FILES ${Dialogs_SRCS})

fc_add_resources(Resource_SRCS Points.qrc)
SET(Resource_SRCS
#		 ${Resource_SRCS}
		 Points.qrc
)
SOURCE_GROUP("Resource" FILES ${Resource_SRCS})

SET(PointsGui_SRCS
		 ${Dialogs_SRCS}
		 ${Resource_SRCS}
		 AppPointsGui.cpp
		 Command.cpp
		 PreCompiled.cpp
		 PreCompiled.h
		 ViewProvider.cpp
		 ViewProvider.h
		 Workbench.cpp
		 Workbench.h
)

add_library(PointsGui SHARED ${PointsGui_SRCS})

target_link_libraries(PointsGui ${PointsGui_LIBS})

if(WIN32)
		 set_target_properties(PointsGui PROPERTIES SUFFIX ".pyd")
		 set_target_properties(PointsGui PROPERTIES DEBUG_OUTPUT_NAME "PointsGui_d")
else(WIN32)
		 set_target_properties(PointsGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS PointsGui DESTINATION lib)
