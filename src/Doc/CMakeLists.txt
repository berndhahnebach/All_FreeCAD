
#find_package(Doxygen REQUIRED)
find_package(Doxygen)

IF (DOXYGEN_DOT_EXECUTABLE)
 SET(HAVE_DOT YES)
ELSE (DOXYGEN_DOT_EXECUTABLE)
 SET(HAVE_DOT NO)
ENDIF (DOXYGEN_DOT_EXECUTABLE)

SET(DOXYGEN_LANGUAGE "English" CACHE STRING "Language used by doxygen")
MARK_AS_ADVANCED(DOXYGEN_LANGUAGE)

SET(DOXYGEN_SOURCE_DIR
   ${CMAKE_SOURCE_DIR}/src/Base
   ${CMAKE_SOURCE_DIR}/src/App
   ${CMAKE_SOURCE_DIR}/src/Gui
   ${CMAKE_SOURCE_DIR}/src/Tools/plugins/widget
#   ${CMAKE_SOURCE_DIR}/src/Mod
   )

SET(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/Doc/SourceDocu)
STRING(REGEX REPLACE ";" " " DOXYGEN_INPUT_LIST "${DOXYGEN_SOURCE_DIR}")

if( FREECAD_MAINTAINERS_BUILD )
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BuildDocDoxy.cfg.in  ${CMAKE_CURRENT_BINARY_DIR}/Doxygen.config @ONLY)
#SET(HTML_TARGET ${CMAKE_BINARY_DIR}/Doc/refman-html/index.html)

   ADD_CUSTOM_TARGET(SourceDocu ALL
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxygen.config 
            COMMAND ${QT_HELPCOMPILER_EXECUTABLE} "\"${CMAKE_BINARY_DIR}/Doc/SourceDocu/html/index.qhp\""
            #COMMAND ${PLATFORM_MK} "\"${CMAKE_BINARY_DIR}/Doc/SourceDocu\""
            #COMMAND ${PLATFORM_CP} "\"${CMAKE_BINARY_DIR}/Doc/SourceDocu/html/index.qch\"" "\"${CMAKE_BINARY_DIR}/Doc/\""
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/Doc/SourceDocu/html/index.qch ${CMAKE_BINARY_DIR}/Doc/FreeCADSource.qch
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/BuildDocDoxy.cfg
   )

   ADD_CUSTOM_TARGET(UserDocu ALL
            ${PYTHON_EXECUTABLE} 
            ${CMAKE_SOURCE_DIR}/src/Tools/wiki2qhelp.py 
         -c ${QT_HELPCOMPILER_EXECUTABLE}   
         -g ${QT_COLLECTIOMGENERATOR_EXECUTABLE}   
         -o ${CMAKE_BINARY_DIR}/Doc   
   )

else( FREECAD_MAINTAINERS_BUILD )
   # Doxygen configuration file is generated from BuildDevDoc.cfg.in template,
   # using variables DOXYGEN_LANGUAGE, DOXYGEN_SOURCE_DIR, DOXYGEN_OUTPUT_DIR
   # defined above
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BuildDevDoc.cfg.in
                  ${CMAKE_CURRENT_BINARY_DIR}/DevDoc.config @ONLY)

   # This creates an independant target. Type make DevDoc to generate
   # source code documentation
   ADD_CUSTOM_TARGET(DevDoc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/DevDoc.config 
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   )
endif( FREECAD_MAINTAINERS_BUILD )

#ADD_CUSTOM_TARGET(SourceDocu)

#ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_BINARY_DIR}/Doc/refman-html/index.html
	#COMMAND ${DOXYGEN}
	#ARGS BuildDocDoxy.cfg
	#MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/src/Base/Doxygen.cpp
#)

#ADD_CUSTOM_COMMAND(TARGET target  PRE_BUILD
#	COMMAND ${DOXYGEN}
#	ARGS BuildDocDoxy.cfg
#)

