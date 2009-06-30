project(FreeCAD_trunk)
set(FREECAD_VERSION "0.7")

cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# include local  modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(COMMAND cmake_policy)
		 cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(COMPILE_DEFINITIONS_DEBUG "-DFC_DEBUG=1")

if(CMAKE_CFG_INTDIR STREQUAL .)
		 # No Debug/Release output paths
		 set(DEBUG_MAIN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/src/Main)
		 set(RELEASE_MAIN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/src/Main)
else(CMAKE_CFG_INTDIR STREQUAL .)
		 set(DEBUG_MAIN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/src/Main/Debug)
		 set(RELEASE_MAIN_OUTPUT_PATH ${CMAKE_BINARY_DIR}/src/Main/Release)
endif(CMAKE_CFG_INTDIR STREQUAL .)

if(WIN32)
		 set(PLATFORM_CP xcopy /Y /S)
else(WIN32)
		 set(PLATFORM_CP cp)
endif(WIN32)


# ================================================================================


if(CMAKE_COMPILER_IS_GNUCC)
	include(cmake/ConfigureChecks.cmake)
	configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
	add_definitions(-DHAVE_CONFIG_H)
	add_definitions(-Wno-write-strings)
	add_definitions(-Wno-deprecated)
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
endif(CMAKE_COMPILER_IS_GNUCC)


# ================================================================================
# == Win32 is default behaviour use the LibPack copied in Source tree ============
if(WIN32)

# --------------------------------------------------------------------------------
# General includes 
 
	link_directories(${CMAKE_SOURCE_DIR}/lib)
	include_directories(${CMAKE_SOURCE_DIR}/include)

# Python
	set(PYTHON_DEBUG_LIBRARY python25_d.lib)
	set(PYTHON_LIBRARY python25.lib)
	set(PYTHON_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include/python)
	set(PYTHON_EXECUTABLE   ${CMAKE_SOURCE_DIR}/bin/python.exe)
	set(PYTHONLIBS_FOUND TRUE) 
	
# XercesC
	set(XERCESC_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/xercesc)
	set(XERCESC_LIBRARIES       xerces-c_2.lib)
	set(XERCESC_DEBUG_LIBRARIES xerces-c_2D.lib)
	set(XERCESC_FOUND TRUE) 
	
# Boost
	set(Boost_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/boost)
	set(Boost_LIBRARIES 
		boost_filesystem-vc80-mt.lib 
		boost_graph-vc80-mt.lib 
		boost_program_options-vc80-mt.lib
	)
	set(Boost_DEBUG_LIBRARIES 
		boost_filesystem-vc80-mt-gd.lib 
		boost_graph-vc80-mt-gd.lib 
		boost_program_options-vc80-mt-gd.lib
	)
	set(Boost_FOUND TRUE) 

# Zlib
	set(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/zlib)
	set(ZLIB_LIBRARIES  zlib1.lib)
	set(ZLIB_FOUND TRUE) 
	
# Coin3D
	set(COIN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/coin)
	set(COIN3D_LIBRARY_DEBUG  coin2d.lib)
	set(COIN3D_LIBRARY_RELEASE  coin2.lib)
	set(COIN3D_FOUND TRUE) 


# QT
	set(QT_INCLUDE_DIR 
	    ${CMAKE_SOURCE_DIR}/include/QT
	    ${CMAKE_SOURCE_DIR}/include/QT/QtCore
	    ${CMAKE_SOURCE_DIR}/include/QT/QtGui
	    ${CMAKE_SOURCE_DIR}/include/QT/QtDesigner
	    ${CMAKE_SOURCE_DIR}/include/QT/QtSvg
	    ${CMAKE_SOURCE_DIR}/include/QT/QtNetwork
	    ${CMAKE_SOURCE_DIR}/include/QT/QtSql
	    ${CMAKE_SOURCE_DIR}/include/QT/QtTest
	    ${CMAKE_SOURCE_DIR}/include/QT/QtUiTools
	    ${CMAKE_SOURCE_DIR}/include/QT/QtXml
	    ${CMAKE_SOURCE_DIR}/include/QT/QtOpenGl
		)
		
	set(QT_LIBRARIES 
	    QtCore4.lib
	    QtGui4.lib
	    QtDesigner4.lib
	    QtSvg4.lib
	    QtNetwork4.lib
	    QtSql4.lib
	    QtTest4.lib
	    QtUiTools4.lib
	    QtXml4.lib
	    QtOpenGl4.lib
	)
	set(QT_DEBUG_LIBRARIES 
	    QtCored4.lib
	    QtGuid4.lib
	    QtDesignerd4.lib
	    QtSvgd4.lib
	    QtNetworkd4.lib
	    QtSqld4.lib
	    QtTestd4.lib
	    QtUiToolsd4.lib
	    QtXmld4.lib
	    QtOpenGld4.lib
	)
	MACRO (QT4_EXTRACT_OPTIONS _qt4_files _qt4_options)
		SET(${_qt4_files})
		SET(${_qt4_options})
		#SET(_QT4_DOING_OPTIONS FALSE)
		#FOREACH(_currentArg ${ARGN})
		#  IF ("${_currentArg}" STREQUAL "OPTIONS")
		#	SET(_QT4_DOING_OPTIONS TRUE)
		#  ELSE ("${_currentArg}" STREQUAL "OPTIONS")
		#	IF(_QT4_DOING_OPTIONS) 
		#	  LIST(APPEND ${_qt4_options} "${_currentArg}")
		#	ELSE(_QT4_DOING_OPTIONS)
		#	  LIST(APPEND ${_qt4_files} "${_currentArg}")
		#	ENDIF(_QT4_DOING_OPTIONS)
		#  ENDIF ("${_currentArg}" STREQUAL "OPTIONS")
		#ENDFOREACH(_currentArg)  
	ENDMACRO (QT4_EXTRACT_OPTIONS)

	MACRO (QT4_WRAP_CPP outfiles )

		FOREACH (it ${moc_files})
		  GET_FILENAME_COMPONENT(it ${it} ABSOLUTE)
		  #QT4_MAKE_OUTPUT_FILE(${it} moc_ cxx outfile)
		  #QT4_CREATE_MOC_COMMAND(${it} ${outfile} "${moc_flags}" "${moc_options}")
		  SET(${outfiles} ${${outfiles}} ${outfile})
		ENDFOREACH(it)
	ENDMACRO (QT4_WRAP_CPP)
	
	
	# This is a special version of the built in macro qt4_wrap_cpp
	# It is required since moc'ed files are now included instead of being added to projects directly
	# It adds a reverse dependency to solve this
	# This has the unfortunate side effect that some files are always rebuilt
	# There is probably a cleaner solution than this

	include(AddFileDependencies)

	macro(fc_wrap_cpp outfiles )

			 foreach(it ${moc_files})
					 get_filename_component(it ${it} ABSOLUTE)
					 #QT4_MAKE_OUTPUT_FILE(${it} moc_ cpp outfile)
					 #QT4_CREATE_MOC_COMMAND(${it} ${outfile} "${moc_includes}" "${moc_options}")
					 set(${outfiles} ${${outfiles}} ${outfile})
					 add_file_dependencies(${it} ${outfile})
			 endforeach(it)
	endmacro(fc_wrap_cpp)
	
	
	MACRO (QT4_ADD_RESOURCES outfiles )
		QT4_EXTRACT_OPTIONS(rcc_files rcc_options ${ARGN})
		SET(ARGN)
		FOREACH (it ${rcc_files})
		  GET_FILENAME_COMPONENT(outfilename ${it} NAME_WE)
		  GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
		  GET_FILENAME_COMPONENT(rc_path ${infile} PATH)
		  SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/qrc_${outfilename}.cxx)
		  #  parse file for dependencies 
		  #  all files are absolute paths or relative to the location of the qrc file
		  FILE(READ "${infile}" _RC_FILE_CONTENTS)
		  STRING(REGEX MATCHALL "<file[^<]+" _RC_FILES "${_RC_FILE_CONTENTS}")
		  SET(_RC_DEPENDS)
		  FOREACH(_RC_FILE ${_RC_FILES})
			STRING(REGEX REPLACE "^<file[^>]*>" "" _RC_FILE "${_RC_FILE}")
			STRING(REGEX MATCH "^/|([A-Za-z]:/)" _ABS_PATH_INDICATOR "${_RC_FILE}")
			IF(NOT _ABS_PATH_INDICATOR)
			  SET(_RC_FILE "${rc_path}/${_RC_FILE}")
			ENDIF(NOT _ABS_PATH_INDICATOR)
			SET(_RC_DEPENDS ${_RC_DEPENDS} "${_RC_FILE}")
		  ENDFOREACH(_RC_FILE)
		  ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
			COMMAND ${QT_RCC_EXECUTABLE}
			ARGS ${rcc_options} -name ${outfilename} -o ${outfile} ${infile}
			MAIN_DEPENDENCY ${infile}
			DEPENDS ${_RC_DEPENDS})
		  SET(${outfiles} ${${outfiles}} ${outfile})
		ENDFOREACH (it)
	ENDMACRO (QT4_ADD_RESOURCES)

	MACRO (QT4_WRAP_UI outfiles )
		QT4_EXTRACT_OPTIONS(ui_files ui_options ${ARGN})
		SET(ARGN)
		FOREACH (it ${ui_files})
		  GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)
		  GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
		  SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/ui_${outfile}.h)
		  ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
			COMMAND ${QT_UIC_EXECUTABLE}
			ARGS ${ui_options} -o ${outfile} ${infile}
			MAIN_DEPENDENCY ${infile})
		  SET(${outfiles} ${${outfiles}} ${outfile})
		ENDFOREACH (it)
	ENDMACRO (QT4_WRAP_UI)


	set(QT4_FOUND TRUE) 

# SoQt
	set(SOQT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/soqt)
	set(SOQT_LIBRARY_RELEASE  soqt1.lib)
	set(SOQT_LIBRARY_DEBUG  soqt1d.lib)
	set(SOQT_FOUND TRUE) 

	
else(WIN32)

# ================================================================================
# == for other OSes search the packages ==========================================

# -------------------------------- Python --------------------------------


	find_package(PythonLibs REQUIRED)
	find_package(PythonInterp REQUIRED)

	IF(NOT PYTHONLIBS_FOUND)
		MESSAGE("Python not found, install python!")
	ENDIF(NOT PYTHONLIBS_FOUND)


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


endif(WIN32)



# ================================================================================
# == Macros, mostly for special targets ==========================================


macro(copy_to_local_output_paths SOURCE_PATHS)
		 if(CMAKE_CFG_INTDIR STREQUAL .)
		 		 # No Debug/Release output paths
		 		 set(DEBUG_LOCAL_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
		 		 set(RELEASE_LOCAL_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
		 else(CMAKE_CFG_INTDIR STREQUAL .)
		 		 set(DEBUG_LOCAL_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/Debug)
		 		 set(RELEASE_LOCAL_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/Release)
		 endif(CMAKE_CFG_INTDIR STREQUAL .)
		 file(TO_NATIVE_PATH ${SOURCE_PATHS} NATIVE_SOURCE)
		 file(TO_NATIVE_PATH ${DEBUG_LOCAL_OUTPUT_PATH}/ NATIVE_DEBUG_DESTINATION)
		 file(TO_NATIVE_PATH ${RELEASE_LOCAL_OUTPUT_PATH}/ NATIVE_RELESE_DESTINATION)
		 message(STATUS "${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_DEBUG_DESTINATION}")
		 execute_process(
		 		 COMMAND ${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_DEBUG_DESTINATION}
		 		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		 if(NOT ${DEBUG_LOCAL_OUTPUT_PATH} STREQUAL ${RELEASE_LOCAL_OUTPUT_PATH})
		 		 message(STATUS "${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_RELESE_DESTINATION}")
		 		 execute_process(
		 		 		 COMMAND ${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_RELESE_DESTINATION}
		 		 		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		 endif(NOT ${DEBUG_LOCAL_OUTPUT_PATH} STREQUAL ${RELEASE_LOCAL_OUTPUT_PATH})
endmacro(copy_to_local_output_paths)

macro(copy_to_main_output_paths SOURCE_PATHS)
		 file(TO_NATIVE_PATH ${SOURCE_PATHS} NATIVE_SOURCE)
		 file(TO_NATIVE_PATH ${DEBUG_MAIN_OUTPUT_PATH}/ NATIVE_DEBUG_DESTINATION)
		 file(TO_NATIVE_PATH ${RELEASE_MAIN_OUTPUT_PATH}/ NATIVE_RELESE_DESTINATION)
		 message(STATUS "${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_DEBUG_DESTINATION}")
		 execute_process(
		 		 COMMAND ${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_DEBUG_DESTINATION}
		 		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		 if(NOT ${DEBUG_MAIN_OUTPUT_PATH} STREQUAL ${RELEASE_MAIN_OUTPUT_PATH})
		 		 message(STATUS "${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_RELESE_DESTINATION}")
		 		 execute_process(
		 		 		 COMMAND ${PLATFORM_CP} ${NATIVE_SOURCE} ${NATIVE_RELESE_DESTINATION}
		 		 		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
		 endif(NOT ${DEBUG_MAIN_OUTPUT_PATH} STREQUAL ${RELEASE_MAIN_OUTPUT_PATH})
endmacro(copy_to_main_output_paths)

# It would be a bit cleaner to generate these files in ${CMAKE_CURRENT_BINARY_DIR}

macro(generate_from_xml BASE_NAME)
		 file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR}/src/Tools/generate.py TOOL_PATH)
		 file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.xml SOURCE_PATH)
		 add_custom_command(
		 		 OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.h" "${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.cpp"
		 		 COMMAND "${PYTHON_EXECUTABLE}" "${TOOL_PATH}" "${SOURCE_PATH}"
		 		 MAIN_DEPENDENCY "${BASE_NAME}.xml"
		 		 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		 		 COMMENT Building ${BASE_NAME}.h/.cpp out of ${BASE_NAME}.xml)
endmacro(generate_from_xml)

macro(generate_from_py BASE_NAME OUTPUT_FILE)
		 file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR}/src/Tools/PythonToCPP.py TOOL_PATH)
		 file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.py SOURCE_PATH)
		 add_custom_command(
		 		 OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILE}
		 		 COMMAND ${PYTHON_EXECUTABLE} ${TOOL_PATH} ${SOURCE_PATH} ${OUTPUT_FILE}
		 		 MAIN_DEPENDENCY ${BASE_NAME}.py
		 		 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		 		 COMMENT Building files out of ${BASE_NAME}.py)
endmacro(generate_from_py)


# generates the ui -> cpp h files
#macro(qt4_wrap_ui infiles )
#
#endmacro(qt4_wrap_ui)


# This is a special version of the built in macro qt4_add_resources that generates .cpp files
#
#macro(fc_add_resources outfiles )
#    #QT4_EXTRACT_OPTIONS(rcc_files rcc_options ${ARGN})
#	set(ARGN )
#    foreach (it ${rcc_files})
#      get_filename_component(outfilename ${it} NAME_WE)
#      get_filename_component(infile ${it} ABSOLUTE)
#      get_filename_component(rc_path ${infile} PATH)
#      set(outfile ${CMAKE_CURRENT_BINARY_DIR}/qrc_${outfilename}.cpp)
#      #  parse file for dependencies 
#      #  all files are absolute paths or relative to the location of the qrc file
#      file(READ "${infile}" _RC_FILE_CONTENTS)
#      string(REGEX MATCHALL "<file[^<]+" _RC_FILES "${_RC_FILE_CONTENTS}")
#      set(_RC_DEPENDS)
#      foreach(_RC_FILE ${_RC_FILES})
#        string(REGEX REPLACE "^<file[^>]*>" "" _RC_FILE "${_RC_FILE}")
#        string(REGEX MATCH "^/|([A-Za-z]:/)" _ABS_PATH_INDICATOR "${_RC_FILE}")
#        if(NOT _ABS_PATH_INDICATOR)
#          set(_RC_FILE "${rc_path}/${_RC_FILE}")
#        endif(NOT _ABS_PATH_INDICATOR)
#        set(_RC_DEPENDS ${_RC_DEPENDS} "${_RC_FILE}")
#      endforeach(_RC_FILE)
#      add_custom_command(OUTPUT ${outfile}
#        COMMAND ${QT_RCC_EXECUTABLE}
#        ARGS ${rcc_options} -name ${outfilename} -o ${outfile} ${infile}
#        MAIN_DEPENDENCY ${infile}
#        DEPENDS ${_RC_DEPENDS})
#      set(${outfiles} ${${outfiles}} ${outfile})
#    endforeach (it)
#endmacro(fc_add_resources)

  MACRO (fc_add_resources outfiles )
    QT4_EXTRACT_OPTIONS(rcc_files rcc_options ${ARGN})

    FOREACH (it ${rcc_files})
      GET_FILENAME_COMPONENT(outfilename ${it} NAME_WE)
      GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
      GET_FILENAME_COMPONENT(rc_path ${infile} PATH)
      SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/qrc_${outfilename}.cpp)
      #  parse file for dependencies 
      #  all files are absolute paths or relative to the location of the qrc file
      FILE(READ "${infile}" _RC_FILE_CONTENTS)
      STRING(REGEX MATCHALL "<file[^<]+" _RC_FILES "${_RC_FILE_CONTENTS}")
      SET(_RC_DEPENDS)
      FOREACH(_RC_FILE ${_RC_FILES})
        STRING(REGEX REPLACE "^<file[^>]*>" "" _RC_FILE "${_RC_FILE}")
        STRING(REGEX MATCH "^/|([A-Za-z]:/)" _ABS_PATH_INDICATOR "${_RC_FILE}")
        IF(NOT _ABS_PATH_INDICATOR)
          SET(_RC_FILE "${rc_path}/${_RC_FILE}")
        ENDIF(NOT _ABS_PATH_INDICATOR)
        SET(_RC_DEPENDS ${_RC_DEPENDS} "${_RC_FILE}")
      ENDFOREACH(_RC_FILE)
      ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
        COMMAND ${QT_RCC_EXECUTABLE}
        ARGS ${rcc_options} -name ${outfilename} -o ${outfile} ${infile}
        MAIN_DEPENDENCY ${infile}
        DEPENDS ${_RC_DEPENDS})
      SET(${outfiles} ${${outfiles}} ${outfile})
    ENDFOREACH (it)

  ENDMACRO (fc_add_resources)

add_subdirectory(src)

