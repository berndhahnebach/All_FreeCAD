/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2005     *
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


#ifndef _PovTools_h_
#define _PovTools_h_


class PovTools
{
  /// writes the given camera position as povray defines in a file
  static void writeCamera(const char*   FileName, 
                     const gp_Vec& CamPos,
                     const gp_Vec& CamDir,
                     const gp_Vec& LookAt,
                     const gp_Vec& Up);

  /// writes the given camera positions as povray defines in a file
  static void writeCameraVec(const char*                FileName, 
                        const std::vector<gp_Vec>& CamPos,
                        const std::vector<gp_Vec>& CamDir,
                        const std::vector<gp_Vec>& LookAt,
                        const std::vector<gp_Vec>& Up);


  /// write a given shape as povray file to disk
  static void writeShape(const char *FileName,
                         const char *PartName,
                         const TopoDS_Shape& Shape,
                         float fMeshDeviation=0.1);


  static void transferToArray(const TopoDS_Face& aFace,gp_Vec** vertices,gp_Vec** vertexnormals, long** cons,int &nbNodesInFace,int &nbTriInFace );


};




#endif // _PovTools_h_