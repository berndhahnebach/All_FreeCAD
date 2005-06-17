/** \file Libs.cpp
 *  \brief Include all needed libs on Windows
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here all the libs get includet by a #pragma dirctive.
 *  Unfortunatly there is nothin comperable on UNIX, so there
 *  you have to use compiler -l staments, which are somwere deep
 *  in the Makefile.
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


// === Including of libs: ============================================================================
#include "../../../FCConfig.h"
#ifdef FC_OS_WIN32
#	pragma comment(lib,"TKernel.lib")
#	pragma comment(lib,"TKMath.lib")
#	pragma comment(lib,"TKService.lib")
#	pragma comment(lib,"TKGeomAlgo.lib")
#	pragma comment(lib,"TKGeomBase.lib")
#	pragma comment(lib,"TKG2d.lib")
#	pragma comment(lib,"TKG3d.lib")
#	pragma comment(lib,"TKBRep.lib")
#	pragma comment(lib,"TKTopAlgo.lib")
#	pragma comment(lib,"TKPrim.lib")
	// OpenCasCade Std IO
//#	pragma comment(lib,"TKIGESStd.lib")
//#	pragma comment(lib,"TKShHealingStd.lib")
//#	pragma comment(lib,"TKSTEPStd.lib")
	// OpenCascade View
#	pragma comment(lib,"TKV2d.lib")
#	pragma comment(lib,"TKV3d.lib")
	// Application Framwork OCAF
#	pragma comment(lib,"TKCAF.lib")
#	pragma comment(lib,"TKCDF.lib")
	// QT extension
#	pragma comment(lib,QTLIBNAME)

#else
//#	error "Dont compile this file on UNIX!"
#endif

