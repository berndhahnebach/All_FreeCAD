project(FreeCAD_trunk)
set(FREECAD_VERSION "0.9")

cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# include local  modules
if (NOT WIN32)
		 set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cMake")
endif(NOT WIN32)

if(COMMAND cmake_policy)
		 cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(COMPILE_DEFINITIONS_DEBUG "-DFC_DEBUG=1")

#if(CMAKE_CFG_INTDIR STREQUAL .)
		 # No Debug/Release output paths
		 set(DEBUG_MAIN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
		 set(RELEASE_MAIN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
#else(CMAKE_CFG_INTDIR STREQUAL .)
#		 set(DEBUG_MAIN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/src/Main/Debug)
#		 set(RELEASE_MAIN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/src/Main/Release)
#endif(CMAKE_CFG_INTDIR STREQUAL .)

if(WIN32)
		 set(PLATFORM_CP xcopy /Y /S)
else(WIN32)
		 set(PLATFORM_CP cp)
endif(WIN32)


# ================================================================================


if(CMAKE_COMPILER_IS_GNUCC)
	include(cMake/ConfigureChecks.cmake)
	configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
	add_definitions(-DHAVE_CONFIG_H)
	add_definitions(-Wno-write-strings)
	add_definitions(-Wno-deprecated)
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
endif(CMAKE_COMPILER_IS_GNUCC)


# ================================================================================


if(WIN32)
		 SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
else(WIN32)
		 SET(CMAKE_INSTALL_PREFIX "/usr/lib/freecad")
endif(WIN32)

# ================================================================================

if(WIN32)
		 OPTION(FREECAD_USE_LIBPACK "Use the LibPack." ON)
else(WIN32)
		 OPTION(FREECAD_USE_LIBPACK "Use the LibPack." OFF)
endif(WIN32)

OPTION(FREECAD_BUILD_GUI "Build the FreeCAD Gui." ON)
OPTION(FREECAD_MAINTAINERS_BUILD "Build FreeCAD for Maintainers, with Docu and 3rd party libs." OFF)



# ================================================================================
# == Win32 is default behaviour use the LibPack copied in Source tree ============
if(FREECAD_USE_LIBPACK)

	include(cMake/UseLibPack6x.cmake)	
	
else(FREECAD_USE_LIBPACK)

# ================================================================================
# == for other OSes search the packages ==========================================


# -------------------------------- Python --------------------------------


	find_package(PythonLibs REQUIRED)
	find_package(PythonInterp REQUIRED)

	IF(NOT PYTHONLIBS_FOUND)
		MESSAGE("Python not found, install python!")
	ENDIF(NOT PYTHONLIBS_FOUND)



# -------------------------------- Boost --------------------------------

	SET( _boost_TEST_VERSIONS ${Boost_ADDITIONAL_VERSIONS} "1.38.0" "1.37.0"
	    "1.36.1" "1.36.0" "1.35.1" "1.35.0" "1.35" "1.34.1" "1.34.0" "1.34"
	    "1.33.1" "1.33.0" "1.33" )

	find_package(Boost COMPONENTS filesystem program_options regex signals)

# -------------------------------- XercesC --------------------------------

	find_package(XercesC REQUIRED)

# -------------------------------- ZLIB --------------------------------

	find_package(ZLIB REQUIRED)

# -------------------------------- OpenCasCade --------------------------------

	find_package(OpenCasCade)

	IF(NOT OCC_FOUND)
		MESSAGE("OpenCasCade was not found, build no CAD modules!")
	ENDIF(NOT OCC_FOUND)

# -------------------------------- OpenCV --------------------------------

	find_package(OpenCV REQUIRED)

	
	if(FREECAD_BUILD_GUI)	
# -------------------------------- OpenGL --------------------------------

		find_package(OpenGL)

# -------------------------------- Qt --------------------------------

# sets ${QT_LIBRARIES}

		SET(QT_MIN_VERSION 4.1.4)
		set(QT_USE_QTNETWORK TRUE)
		set(QT_USE_QTOPENGL TRUE)
		set(QT_USE_QTSVG TRUE)
		set(QT_USE_QTUITOOLS TRUE)
		set(QT_USE_QTXML TRUE)
		set(QT_USE_QTWEBKIT TRUE)
		find_package(Qt4)

		include(${QT_USE_FILE})

		IF(NOT QT4_FOUND)
			MESSAGE("Library qt-4.3 or above is not available, install QT or FreeCAD Gui version will not be built")
		ENDIF(NOT QT4_FOUND)

		IF(NOT QT_QTWEBKIT_FOUND)
			MESSAGE("QT Webkit not found, will not build browser integration!")
		ENDIF(NOT QT_QTWEBKIT_FOUND)


		# This is a special version of the built in macro qt4_wrap_cpp
		# It is required since moc'ed files are now included instead of being added to projects directly
		# It adds a reverse dependency to solve this
		# This has the unfortunate side effect that some files are always rebuilt
		# There is probably a cleaner solution than this
		include(AddFileDependencies)
		macro(fc_wrap_cpp outfiles )
				 # get include dirs
				 QT4_GET_MOC_FLAGS(moc_includes)
				 QT4_EXTRACT_OPTIONS(moc_files moc_options ${ARGN})

				 foreach(it ${moc_files})
						 get_filename_component(it ${it} ABSOLUTE)
						 QT4_MAKE_OUTPUT_FILE(${it} moc_ cpp outfile)
						 QT4_CREATE_MOC_COMMAND(${it} ${outfile} "${moc_includes}" "${moc_options}")
						 set(${outfiles} ${${outfiles}} ${outfile})
						 add_file_dependencies(${it} ${outfile})
				 endforeach(it)
		endmacro(fc_wrap_cpp)

# -------------------------------- Coin3D --------------------------------

		find_package(Coin3D REQUIRED)
		find_package(SoQt REQUIRED)
		
	endif(FREECAD_BUILD_GUI)

endif(FREECAD_USE_LIBPACK)


include(cMake/FreeCadMacros.cmake)

# ================================================================================
# == Global Compiler and Linker Settings =========================================

include_directories(${CMAKE_BINARY_DIR}/src
					${CMAKE_SOURCE_DIR}/src)

# check for 64-bit platform
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
	MESSAGE(STATUS "Platform is 64-bit, set -D_OCC64")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_OCC64")
ELSE(CMAKE_SIZEOF_VOID_P EQUAL 8)
	MESSAGE(STATUS "Platform is 32-bit")
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)


IF(MSVC)
	# set default compiler settings
	#SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GF /GY")
	# set default libs
	SET (CMAKE_C_STANDARD_LIBRARIES "kernel32.lib user32.lib gdi32.lib winspool.lib SHFolder.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib ")
	set (CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES}")
	# set linker flag /nodefaultlib
	set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB")
	SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB")

	# Mark 32 bit executables large address aware so they can use > 2GB address space
	# NOTE: This setting only has an effect on machines with at least 3GB of RAM, although it sets the linker option it doesn't set the the linker switch 'Enable Large Addresses'
	IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LARGEADDRESSAWARE")
		SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
	ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 4)
ENDIF(MSVC)

add_subdirectory(src)

