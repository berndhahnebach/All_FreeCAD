add_definitions(-DSOQT_DLL -DCOIN_DLL)

include_directories(
		../include
		${COIN_INCLUDE_DIR}
		${PYTHON_INCLUDE_PATH}
		${SOQT_INCLUDE_DIR}
		${QT_INCLUDE_DIR}
		)

########################### CoinPy ###########################

if(MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif(MSVC)

set(CoinPy_SRCS
		__init__.py
		coin.py
		coin_wrap.cpp
		coin_header_includes.h)

if(WIN32)
		set(CoinPy_LIBS
				debug MSVCRTD.LIB
				debug MSVCPRTD.LIB
				optimized MSVCRT.LIB
				optimized MSVCPRT.LIB
				Rpcrt4.lib
				debug ${COIN3D_LIBRARY_DEBUG}
				optimized ${COIN3D_LIBRARY_RELEASE}
				debug ${PYTHON_DEBUG_LIBRARY}
				optimized ${PYTHON_LIBRARY})
else(WIN32)
		set(CoinPy_LIBS
				${COIN3D_LIBRARY}
				${PYTHON_LIBRARY})
endif(WIN32)

add_library(coin SHARED ${CoinPy_SRCS})

target_link_libraries(coin ${CoinPy_LIBS})

if(MSVC)
	set_target_properties(coin PROPERTIES SUFFIX ".pyd")
	set_target_properties(coin PROPERTIES DEBUG_OUTPUT_NAME "_coin_d")
	set_target_properties(coin PROPERTIES OUTPUT_NAME "_coin")
	set_target_properties(coin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/pivy)
	set_target_properties(coin PROPERTIES PREFIX "../")
else(MSVC)
	set_target_properties(coin PROPERTIES OUTPUT_NAME "_coin")
	set_target_properties(coin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/pivy)
	set_target_properties(coin PROPERTIES PREFIX "")
endif(MSVC)

fc_copy_to_path("bin/pivy" 
	__init__.py
	coin.py
	sogui.py)

install(FILES __init__.py coin.py sogui.py DESTINATION bin/pivy)

########################### SoQtPy ###########################

set(SoQtPy_SRCS
		__init__.py
		sogui.py
		soqt.py
		soqt_wrap.cpp)

if(WIN32)
		set(SoQtPy_LIBS
				debug MSVCRTD.LIB
				debug MSVCPRTD.LIB
				optimized MSVCRT.LIB
				optimized MSVCPRT.LIB
				Rpcrt4.lib
				debug ${SOQT_LIBRARY_DEBUG}
				optimized ${SOQT_LIBRARY_RELEASE}
				debug ${COIN3D_LIBRARY_DEBUG}
				optimized ${COIN3D_LIBRARY_RELEASE}
				debug ${PYTHON_DEBUG_LIBRARY}
				optimized ${PYTHON_LIBRARY})
else(WIN32)
		set(CoinPy_LIBS
				${SOQT_LIBRARIES}
				${COIN3D_LIBRARY}
				${PYTHON_LIBRARY})
endif(WIN32)

add_library(soqt SHARED ${SoQtPy_SRCS})

target_link_libraries(soqt ${SoQtPy_LIBS})

if(MSVC)
	set_target_properties(soqt PROPERTIES SUFFIX ".pyd")
	set_target_properties(soqt PROPERTIES DEBUG_OUTPUT_NAME "_soqt_d")
	set_target_properties(soqt PROPERTIES OUTPUT_NAME "_soqt")
	set_target_properties(soqt PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/pivy/gui)
	set_target_properties(soqt PROPERTIES PREFIX "../")
else(MSVC)
	set_target_properties(soqt PROPERTIES OUTPUT_NAME "_soqt")
	set_target_properties(soqt PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/pivy/gui)
	set_target_properties(soqt PROPERTIES PREFIX "")
endif(MSVC)

fc_copy_to_path("bin/pivy/gui" 
	gui/__init__.py
	soqt.py)

install(FILES __init__.py soqt.py DESTINATION bin/pivy/gui)
