project(FreeCAD_trunk)

cmake_minimum_required(VERSION 2.4.6 FATAL_ERROR)

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

# -------------------------------- Python --------------------------------

# sets ${PYTHON_INCLUDE_PATH}, ${PYTHON_DEBUG_LIBRARIES} and ${PYTHON_LIBRARIES}

find_package(PythonLibs REQUIRED)
find_package(PythonInterp REQUIRED)

#link_directories("C:\\Program Files\\Python 2.5\\libs")

# -------------------------------- Qt --------------------------------

# sets ${QT_LIBRARIES}

find_package(Qt4 REQUIRED)

set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTSVG TRUE)
set(QT_USE_QTUITOOLS TRUE)

include(${QT_USE_FILE})

# -------------------------------- Platform specific config --------------------------------

if(WIN32)
		 include(ConfigWin.cmake)
endif(WIN32)

# -------------------------------- Verify that everything was found --------------------------------

if(NOT ATLAS_FOUND)
		 message(FATAL_ERROR "atlas not found")
endif(NOT ATLAS_FOUND)

if(NOT Boost_FOUND)
		 message(FATAL_ERROR "boost not found")
endif(NOT Boost_FOUND)

if(NOT COIN_FOUND)
		 message(FATAL_ERROR "coin not found")
endif(NOT COIN_FOUND)

if(NOT OCC_FOUND)
		 message(FATAL_ERROR "Open CASCADE not found")
endif(NOT OCC_FOUND)

if(NOT OPENCV_FOUND)
		 message(FATAL_ERROR "opencv not found")
endif(NOT OPENCV_FOUND)

if(NOT SO_QT_FOUND)
		 message(FATAL_ERROR "SoQt not found")
endif(NOT SO_QT_FOUND)

if(NOT XERCES_FOUND)
		 message(FATAL_ERROR "xerces not found")
endif(NOT XERCES_FOUND)

if(NOT ZLIB_FOUND)
		 message(FATAL_ERROR "zlib not found")
endif(NOT ZLIB_FOUND)


# -------------------------------- Macros --------------------------------


# It would be a bit cleaner to generate these files in ${CMAKE_CURRENT_BINARY_DIR}

macro(generate_from_xml BASE_NAME)
		 file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR}/src/Tools/generate.py TOOL_PATH)
		 file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.xml SOURCE_PATH)
		 add_custom_command(
		 		 OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.h ${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME}.cpp
		 		 COMMAND ${PYTHON_EXECUTABLE} ${TOOL_PATH} ${SOURCE_PATH}
		 		 MAIN_DEPENDENCY ${BASE_NAME}.xml
		 		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
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

# This is a special version of the built in macro qt4_wrap_cpp
# It is required since moc'ed files are now included instead of being added to projects directly
# It adds a reverse dependency to solve this
# This has the unfortunate side effect that some files are always rebuilt
# There is probably a cleaner solution than this

include(AddFileDependencies)

macro(fc_wrap_cpp outfiles )
		 # get include dirs
		 QT4_GET_MOC_INC_DIRS(moc_includes)
		 QT4_EXTRACT_OPTIONS(moc_files moc_options ${ARGN})

		 foreach(it ${moc_files})
		 		 get_filename_component(it ${it} ABSOLUTE)
		 		 QT4_MAKE_OUTPUT_FILE(${it} moc_ cpp outfile)
		 		 QT4_CREATE_MOC_COMMAND(${it} ${outfile} "${moc_includes}" "${moc_options}")
		 		 set(${outfiles} ${${outfiles}} ${outfile})
		 		 add_file_dependencies(${it} ${outfile})
		 endforeach(it)
endmacro(fc_wrap_cpp)

# This is a special version of the built in macro qt4_add_resources that generates .cpp files

macro(fc_add_resources outfiles )
    QT4_EXTRACT_OPTIONS(rcc_files rcc_options ${ARGN})

    foreach (it ${rcc_files})
      get_filename_component(outfilename ${it} NAME_WE)
      get_filename_component(infile ${it} ABSOLUTE)
      get_filename_component(rc_path ${infile} PATH)
      set(outfile ${CMAKE_CURRENT_BINARY_DIR}/qrc_${outfilename}.cpp)
      #  parse file for dependencies 
      #  all files are absolute paths or relative to the location of the qrc file
      file(READ "${infile}" _RC_FILE_CONTENTS)
      string(REGEX MATCHALL "<file[^<]+" _RC_FILES "${_RC_FILE_CONTENTS}")
      set(_RC_DEPENDS)
      foreach(_RC_FILE ${_RC_FILES})
        string(REGEX REPLACE "^<file[^>]*>" "" _RC_FILE "${_RC_FILE}")
        string(REGEX MATCH "^/|([A-Za-z]:/)" _ABS_PATH_INDICATOR "${_RC_FILE}")
        if(NOT _ABS_PATH_INDICATOR)
          set(_RC_FILE "${rc_path}/${_RC_FILE}")
        endif(NOT _ABS_PATH_INDICATOR)
        set(_RC_DEPENDS ${_RC_DEPENDS} "${_RC_FILE}")
      endforeach(_RC_FILE)
      add_custom_command(OUTPUT ${outfile}
        COMMAND ${QT_RCC_EXECUTABLE}
        ARGS ${rcc_options} -name ${outfilename} -o ${outfile} ${infile}
        MAIN_DEPENDENCY ${infile}
        DEPENDS ${_RC_DEPENDS})
      set(${outfiles} ${${outfiles}} ${outfile})
    endforeach (it)
endmacro(fc_add_resources)

add_subdirectory(src)
