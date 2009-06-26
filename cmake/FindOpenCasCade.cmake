# Try to find OCC
# Once done this will define
#
# OCC_FOUND        - system has OCC - Open Inventor
# OCC_INCLUDE_DIR  - where the Inventor include directory can be found
# OCC_LIBRARY      - Link this to use OCC
#
 

IF (WIN32)
  IF (CYGWIN)

    FIND_PATH(OCC_INCLUDE_DIR Inventor/So.h
      /usr/include
      /usr/local/include
    )

    FIND_LIBRARY(OCC_LIBRARY Coin
      /usr/lib
      /usr/local/lib
    )

  ELSE (CYGWIN)

    FIND_PATH(OCC_INCLUDE_DIR Inventor/So.h
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SIM\\OCC\\2;Installation Path]/include"
    )

    FIND_LIBRARY(OCC_LIBRARY_DEBUG coin2d
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SIM\\OCC\\2;Installation Path]/lib"
    )

    FIND_LIBRARY(OCC_LIBRARY_RELEASE coin2
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SIM\\OCC\\2;Installation Path]/lib"
    )

    IF (OCC_LIBRARY_DEBUG AND OCC_LIBRARY_RELEASE)
      SET(OCC_LIBRARY optimized ${OCC_LIBRARY_RELEASE}
                         debug ${OCC_LIBRARY_DEBUG})
    ELSE (OCC_LIBRARY_DEBUG AND OCC_LIBRARY_RELEASE)
      IF (OCC_LIBRARY_DEBUG)
        SET (OCC_LIBRARY ${OCC_LIBRARY_DEBUG})
      ENDIF (OCC_LIBRARY_DEBUG)
      IF (OCC_LIBRARY_RELEASE)
        SET (OCC_LIBRARY ${OCC_LIBRARY_RELEASE})
      ENDIF (OCC_LIBRARY_RELEASE)
    ENDIF (OCC_LIBRARY_DEBUG AND OCC_LIBRARY_RELEASE)

    IF (OCC_LIBRARY)
      ADD_DEFINITIONS ( -DCOIN_NOT_DLL )
    #ELSE (OCC_LIBRARY)
    #  SET (OCC_LIBRARY coin2d CACHE STRING "OCC Library (Debug) - Open Inventor API")
    ENDIF (OCC_LIBRARY)

  ENDIF (CYGWIN)

ELSE (WIN32)
  IF(APPLE)
    FIND_PATH(OCC_INCLUDE_DIR Inventor/So.h
     /Library/Frameworks/Inventor.framework/Headers 
     /usr/local/include
     /usr/include
    )
    FIND_LIBRARY(OCC_LIBRARY Coin
      /Library/Frameworks/Inventor.framework/Libraries
      /usr/lib
      /usr/local/lib
    )   
    SET(OCC_LIBRARY "-framework OCC" CACHE STRING "OCC library for OSX")
   ELSE(APPLE)

  FIND_PATH(OCC_INCLUDE_DIR Inventor/So.h
    /usr/include
    /usr/local/include
  )

  FIND_LIBRARY(OCC_LIBRARY Coin
    /usr/lib
    /usr/local/lib
  )   
  ENDIF(APPLE)

ENDIF (WIN32)


SET( OCC_FOUND "NO" )
IF(OCC_LIBRARY)
  SET( OCC_FOUND "YES" )
ENDIF(OCC_LIBRARY)

