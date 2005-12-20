/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


// === Including of libs: ============================================================================
#include "../FCConfig.h"
#ifdef FC_OS_WIN32
# pragma comment(lib,"user32.lib")

# pragma comment(lib,"TKernel.lib")
# pragma comment(lib,"TKMath.lib")
# pragma comment(lib,"TKService.lib")
# pragma comment(lib,"TKGeomAlgo.lib")
# pragma comment(lib,"TKGeomBase.lib")
# pragma comment(lib,"TKG2d.lib")
# pragma comment(lib,"TKG3d.lib")
# pragma comment(lib,"TKBRep.lib")
# pragma comment(lib,"TKTopAlgo.lib")
# pragma comment(lib,"TKPrim.lib")
  // OpenCascade View
# pragma comment(lib,"TKV2d.lib")
# pragma comment(lib,"TKV3d.lib")
  // Application Framwork OCAF
# pragma comment(lib,"TKCAF.lib")
# pragma comment(lib,"TKCDF.lib")

# ifdef FC_USE_OCAFBROWSER
#   pragma comment(lib,"DFBrowser.lib")
# endif

  // Qt extension
# pragma comment(lib,QTLIBNAME)
# pragma comment(lib,"qui.lib")
# pragma warning( disable : 4098 )

# pragma comment(lib,INVENTORLIBNAME)
# pragma comment(lib,SOQTLIBNAME)
# pragma comment(lib,"OpenGL32.lib")

#else
//# error "Dont compile this file on UNIX!"
#endif

