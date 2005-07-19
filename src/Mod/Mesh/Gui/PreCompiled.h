#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include "../../../FCConfig.h"

#ifdef _PreComp_

/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif

// Importing of App classes
#ifdef FC_OS_WIN32
# define AppMeshExport __declspec(dllimport)
# define GuiMeshExport __declspec(dllexport)
#else // for Linux
# define AppMeshExport
# define GuiMeshExport
#endif

// standard
#include <stdio.h>
#include <assert.h>

// STL
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

// Xerces
#include <xercesc/util/XercesDefs.hpp>

#include <Python.h>
#ifdef FC_OS_WIN32
# include <windows.h>
#endif

// OCC
#include <AIS_InteractiveContext.hxx>
#include <TDF_Label.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <V3d_OrthographicView.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

// Qt Toolkit
#include <qaction.h>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qmainwindow.h>
#include <qscrollview.h>
#include <qthread.h>
#include <qworkspace.h>

// Open Inventor
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoSwitch.h>

#endif  //_PreComp_

#endif // __PRECOMPILED_GUI__ 
