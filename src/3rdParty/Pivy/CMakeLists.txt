add_definitions(-DSOQT_DLL -DCOIN_DLL)

include_directories(
		 ../include
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${SO_QT_INCLUDE_DIR})

########################### CoinPy ###########################

if(MSVC)
		 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif(MSVC)

set(CoinPy_SRCS
		 __init__.py
		 coin.py
		 coin_wrap.cpp
		 coin_header_includes.h
		 sogui.py)

add_library(coin SHARED ${CoinPy_SRCS})

target_link_libraries(coin
		debug ${COIN3D_LIBRARY_DEBUG}
		optimized ${COIN3D_LIBRARY_RELEASE}
		debug ${PYTHON_DEBUG_LIBRARY}
		optimized ${PYTHON_LIBRARY}
		debug MSVCRTD.LIB
        debug MSVCPRTD.LIB
		optimized MSVCRT.LIB
        optimized MSVCPRT.LIB
		Rpcrt4.lib
		)

#set_target_properties(coin PROPERTIES PREFIX "_")
#set_target_properties(coin PROPERTIES SUFFIX ".pyd")
set_target_properties(coin PROPERTIES SUFFIX ".pyd")
set_target_properties(coin PROPERTIES DEBUG_OUTPUT_NAME "coin_d")
set_target_properties(coin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(coin PROPERTIES PREFIX "../_")

copy_to_local_output_paths(__init__.py)
copy_to_local_output_paths(soqt.py)

install(FILES __init__.py soqt.py DESTINATION bin/pivy/gui)

########################### SoQtPy ###########################

set(SoQtPy_SRCS
		 __init__.py
		 soqt.py
		 soqt_wrap.cpp)

add_library(soqt SHARED ${SoQtPy_SRCS})

target_link_libraries(soqt
		debug ${COIN3D_LIBRARY_DEBUG}
		optimized ${COIN3D_LIBRARY_RELEASE}
		debug ${PYTHON_DEBUG_LIBRARY}
		optimized ${PYTHON_LIBRARY}
		debug ${SOQT_LIBRARY_DEBUG}
		optimized ${SOQT_LIBRARY_RELEASE}
		debug MSVCRTD.LIB
        debug MSVCPRTD.LIB
		optimized MSVCRT.LIB
        optimized MSVCPRT.LIB
		Rpcrt4.lib
		)

#set_target_properties(soqt PROPERTIES PREFIX "_")
#set_target_properties(soqt PROPERTIES SUFFIX ".pyd")
set_target_properties(coin PROPERTIES SUFFIX ".pyd")
set_target_properties(coin PROPERTIES DEBUG_OUTPUT_NAME "soqt_d")
set_target_properties(coin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(coin PROPERTIES PREFIX "../_")

copy_to_local_output_paths(__init__.py)
copy_to_local_output_paths(coin.py)
copy_to_local_output_paths(sogui.py)

install(FILES __init__.py coin.py sogui.py DESTINATION bin/pivy)
