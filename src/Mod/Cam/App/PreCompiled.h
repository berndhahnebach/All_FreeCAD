#ifndef __PRECOMPILED__
#define __PRECOMPILED__

#include "../../../FCConfig.h"


// Exporting of App classes
#ifdef FC_OS_WIN32
    # define AppCamExport   __declspec(dllexport)
    # define CamExport      __declspec(dllexport)
    # define AppPartExport  __declspec(dllimport)
    # define PartExport     __declspec(dllimport)
    # define AppMeshExport  __declspec(dllimport)
    # define MeshExport     __declspec(dllimport)
#else // for Linux
    # define AppCamExport
    # define CamExport
    # define AppPartExport
    # define PartExport
    # define AppMeshExport
    # define MeshExport
#endif


#ifdef _PreComp_

    /// here get the warnings of to long specifieres disabled (needed for VC6)
    #ifdef _MSC_VER
        # pragma warning( disable : 4251 )
        # pragma warning( disable : 4275 )
        # pragma warning( disable : 4503 )
        # pragma warning( disable : 4786 )  // specifier longer then 255 chars
        # pragma warning( disable : 4101 )
    #endif

    // STL
    #include <vector>
    #include <list>
    #include <map>
    #include <string>
    #include <set>
    #include <algorithm>
    #include <stack>
    #include <queue>
    #include <bitset>

    #include <Python.h>

    #ifdef FC_OS_WIN32
        #include <windows.h>
    #endif

    // Xerces
    #include <xercesc/util/XercesDefs.hpp>

    // OpenCasCade Base
    #include <Standard_Failure.hxx>

    // OpenCascade View
    #include <Mod/Part/App/OpenCascadeAll.h>

#endif //_PreComp_

#endif //__PRECOMPILED__


