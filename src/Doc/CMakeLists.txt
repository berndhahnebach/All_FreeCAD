find_package(Doxygen)
if( DOXYGEN_FOUND)

    IF (DOXYGEN_DOT_EXECUTABLE)
        SET(HAVE_DOT YES)
    ELSE (DOXYGEN_DOT_EXECUTABLE)
        SET(HAVE_DOT NO)
    ENDIF (DOXYGEN_DOT_EXECUTABLE)

    SET(DOXYGEN_LANGUAGE "English" CACHE STRING "Language used by doxygen")
    MARK_AS_ADVANCED(DOXYGEN_LANGUAGE)

    if (NOT FREECAD_BUILD_GUI)
        message("Note: Gui is not built. Documentation may lack some parts.")
    endif (NOT FREECAD_BUILD_GUI)

    SET(DOXYGEN_SOURCE_DIR
        ${CMAKE_SOURCE_DIR}/src/Base
        ${CMAKE_SOURCE_DIR}/src/App
        ${CMAKE_SOURCE_DIR}/src/Main
        ${CMAKE_SOURCE_DIR}/src/Gui
    )

    SET(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/Doc/SourceDocu)

    STRING(REGEX REPLACE ";" " " DOXYGEN_INPUT_LIST "${DOXYGEN_SOURCE_DIR}")

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BuildDevDoc.cfg.in  
                   ${CMAKE_CURRENT_BINARY_DIR}/BuildDevDoc.cfg @ONLY)

    if( FREECAD_MAINTAINERS_BUILD )
        ADD_CUSTOM_TARGET(SourceDocu ALL
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/BuildDevDoc.cfg 
            COMMAND ${QT_HELPCOMPILER_EXECUTABLE} "\"${CMAKE_BINARY_DIR}/Doc/SourceDocu/html/index.qhp\""
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/Doc/SourceDocu/html/index.qch ${CMAKE_BINARY_DIR}/Doc/FreeCADSource.qch
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/BuildDevDoc.cfg
        )

        ADD_CUSTOM_TARGET(UserDocu ALL
            ${PYTHON_EXECUTABLE} 
            ${CMAKE_SOURCE_DIR}/src/Tools/wiki2qhelp.py 
         -c ${QT_HELPCOMPILER_EXECUTABLE}   
         -g ${QT_COLLECTIOMGENERATOR_EXECUTABLE}   
         -o ${CMAKE_BINARY_DIR}/Doc   
        )

    else( FREECAD_MAINTAINERS_BUILD )
        ADD_CUSTOM_TARGET(DevDoc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/BuildDevDoc.cfg 
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

    endif( FREECAD_MAINTAINERS_BUILD )

endif( DOXYGEN_FOUND )

