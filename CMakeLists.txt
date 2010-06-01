project(FreeCAD_trunk)
set(FREECAD_VERSION "0.9")

set(PACKAGE_NAME "FreeCAD")
set(PACKAGE_VERSION_MAJOR "0")
set(PACKAGE_VERSION_MINOR "9")
set(PACKAGE_VERSION_PATCH "2237")
set(PACKAGE_VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# include local  modules

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cMake")


if(COMMAND cmake_policy)
		 cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

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
		 set(PLATFORM_MK mkdir)
else(WIN32)
		 set(PLATFORM_CP cp)
		 set(PLATFORM_MK mkdir -p)
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
# == Win32 is default behaviour use the LibPack copied in Source tree ============
if(WIN32)
		 OPTION(FREECAD_LIBPACK_USE "Use the LibPack to Build FreeCAD (only Win32 so far)." ON)
		 set(FREECAD_LIBPACK_DIR ${CMAKE_SOURCE_DIR} CACHE PATH  "Directory of the FreeCAD LibPack")
else(WIN32)
		 OPTION(FREECAD_LIBPACK_USE "Use the LibPack to Build FreeCAD (only Win32 so far)." OFF)
		 set(FREECAD_LIBPACK_DIR ""  CACHE PATH  "Directory of the FreeCAD LibPack")
endif(WIN32)

# ================================================================================
# == All the options for the build process    ============

OPTION(FREECAD_BUILD_GUI "Build FreeCAD Gui. Otherwise you have only the command line and the Python import module." ON)
OPTION(FREECAD_MAINTAINERS_BUILD "Build FreeCAD for Maintainers, with Docu and 3rd party libs. On Windows the Installer is build." OFF)
OPTION(FREECAD_BUILD_CAM "Build the FreeCAD CAM module and the needed libs, be aware, unfinished code!" OFF)
OPTION(FREECAD_BUILD_FEM "Build the FreeCAD FEM module, be aware, unfinished code!" OFF)
OPTION(FREECAD_BUILD_ASSEMBLY "Build the FreeCAD Assembly module and the needed libs, be aware, unfinished code!" OFF)



# ================================================================================

if(FREECAD_LIBPACK_USE)
	# checking for a unique file in LibPack location to make sure the right version of the LibPack is there
	find_file(FREECAD_LIBPACK_CHECKFILE6X boost_program_options-vc80-mt-gd.lib ${FREECAD_LIBPACK_DIR}/lib )
	find_file(FREECAD_LIBPACK_CHECKFILE7X boost_program_options-vc90-mt-gd-1_39.lib ${FREECAD_LIBPACK_DIR}/lib )
	find_file(FREECAD_LIBPACK_CHECKCUSTOM boost_program_options-vc90-mt-gd-1_41.lib ${FREECAD_LIBPACK_DIR}/lib )
	IF(FREECAD_LIBPACK_CHECKFILE6X)
		include(cMake/UseLibPack6x.cmake)
		set(FREECAD_LIBPACK6 FOUND CACHE STRING "Displays if the libpack has been found")
		set(FREECAD_LIBPACK7 NOTFOUND CACHE STRING "Displays if the libpack has been found")
		MARK_AS_ADVANCED(FORCE FREECAD_LIBPACK7)
	ELSEIF(FREECAD_LIBPACK_CHECKFILE7X)
		include(cMake/UseLibPack7x.cmake)
		set(FREECAD_LIBPACK6 NOTFOUND CACHE STRING "Displays if the libpack has been found")
		MARK_AS_ADVANCED(FORCE FREECAD_LIBPACK6)
		set(FREECAD_LIBPACK7 FOUND CACHE STRING "Displays if the libpack has been found")	
	ELSEIF(FREECAD_LIBPACK_CHECKCUSTOM)
		include(cMake/UseLibPackCustom.cmake)
		set(FREECAD_LIBPACKX FOUND CACHE STRING "Displays if the libpack has been found")	
	ELSE(FREECAD_LIBPACK_CHECKFILE6X)
		MARK_AS_ADVANCED(FORCE FREECAD_LIBPACK_CHECKFILE6X FREECAD_LIBPACK_CHECKFILE7X)
		message(SEND_ERROR  "Could not find neither LibPack 6.x nor 7.x in specified location:" ${FREECAD_LIBPACK_DIR})
	ENDIF(FREECAD_LIBPACK_CHECKFILE6X)
	
else(FREECAD_LIBPACK_USE)

MARK_AS_ADVANCED(FORCE FREECAD_LIBPACK_CHECKFILE6X FREECAD_LIBPACK_CHECKFILE7X)



# ================================================================================
# == for other OSes search the packages ==========================================


# -------------------------------- Python --------------------------------


	find_package(PythonLibs REQUIRED)
	find_package(PythonInterp REQUIRED)

	IF(NOT PYTHONLIBS_FOUND)
		MESSAGE("Python not found, install python!")
	ENDIF(NOT PYTHONLIBS_FOUND)



# -------------------------------- Boost --------------------------------

	SET( _boost_TEST_VERSIONS ${Boost_ADDITIONAL_VERSIONS} "1.39.0" "1.38.0" "1.37.0"
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

	# not needed at the moment
	#find_package(OpenCV REQUIRED)

# -------------------------------- Swig ----------------------------------

	find_package(SWIG)

	IF(NOT SWIG_FOUND)
		MESSAGE("SWIG was not found, build no SWIG binding for pivy")
	ENDIF(NOT SWIG_FOUND)
	
	if(FREECAD_BUILD_GUI)	
# -------------------------------- OpenGL --------------------------------

	find_package(OpenGL)

# -------------------------------- Eigen --------------------------------

	find_package(Eigen2)

# -------------------------------- ODE ----------------------------------

	find_package(ODE)

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
	

endif(FREECAD_LIBPACK_USE)

if(FREECAD_MAINTAINERS_BUILD AND WIN32)
	find_package(Wix REQUIRED)
endif(FREECAD_MAINTAINERS_BUILD AND WIN32)

# copy build convenient files for M$
if(WIN32)
configure_file(BuildAll.bat ${CMAKE_BINARY_DIR}/BuildAll.bat COPYONLY)
configure_file(BuildAllNice.bat ${CMAKE_BINARY_DIR}/BuildAllNice.bat COPYONLY)
endif(WIN32)


include(cMake/FreeCadMacros.cmake)


# ================================================================================
# == Global Compiler and Linker Settings =========================================

include_directories(${CMAKE_BINARY_DIR}/src
					${CMAKE_SOURCE_DIR}/src)

# check for 64-bit platform
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
	MESSAGE(STATUS "Platform is 64-bit, set -D_OCC64")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_OCC64")
	add_definitions(-D_OCC64 )
ELSE(CMAKE_SIZEOF_VOID_P EQUAL 8)
	MESSAGE(STATUS "Platform is 32-bit")
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)



IF(MSVC)
	# set default compiler settings
	#SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GF /GY")
	SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFC_DEBUG")
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
ELSE(MSVC)
	SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFC_DEBUG")
	MESSAGE(STATUS "DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
ENDIF(MSVC)

add_subdirectory(src)

# ================================================================================
# == Packaging ===================================================================
if(FREECAD_MAINTAINERS_BUILD AND NOT WIN32)
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An extensible Open Source CAx program")
	set(CPACK_PACKAGE_VENDOR "FreeCAD development team")
	if(WIN32)
		set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.Win32")
	else(WIN32)
		set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.Linux")
	endif(WIN32)
	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/copying.lib")
	set(CPACK_PACKAGE_VERSION_MAJOR ${PACKAGE_VERSION_MAJOR})
	set(CPACK_PACKAGE_VERSION_MINOR ${PACKAGE_VERSION_MINOR})
	set(CPACK_PACKAGE_VERSION_PATCH ${PACKAGE_VERSION_PATCH})
	set(CPACK_SOURCE_PACKAGE_FILE_NAME ${PACKAGE_NAME}-${PACKAGE_VERSION})
	if(WIN32)
		set(CPACK_GENERATOR "ZIP")
		set(CPACK_SOURCE_GENERATOR "ZIP")
	else(WIN32)
		set(CPACK_GENERATOR "TGZ")
		set(CPACK_SOURCE_GENERATOR "TGZ")
	endif(WIN32)
	file(GLOB DOT ".*")
	file(GLOB TILD "*~")
	set(CPACK_SOURCE_IGNORE_FILES
	  "${DOT}"
	  "${TILD}"
	  ".a$"
	  ".o$"
	  ".so$"
	  "/.deps/"
	  "/.svn/"
	  "/CMakeFiles/"
	  "/CVS/"
	  "/autom4te.cache/"
	  "/build/"
	  "/debian/"
	  "/debug/"
	  "/docs/"
	  "/qt-build/"
	  "AdditionalInfo.txt$"
	  "CMakeCache.txt$"
	  "Makefile$"
	  "\\\\.sh$"
	  "_CPack_"
	  "config.h$"
	  "config.log$"
	  "config.status$"
	  "Doxyfile"
	  "html$"
	  "stamp-h1$"
	  ".swp$"
	  "tar.bz2"
	  "tar.gz"
	  "~$"
	)
	set(CPACK_SOURCE_STRIP_FILES "")
	include(CPack)

	#ADD_CUSTOM_TARGET(DIST make package_source)

endif(FREECAD_MAINTAINERS_BUILD AND NOT WIN32)
