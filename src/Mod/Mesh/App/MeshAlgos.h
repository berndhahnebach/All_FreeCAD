/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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


#ifndef _MeshAlgos_h_
#define _MeshAlgos_h_

#ifndef _PreComp_
#endif



namespace Mesh
{

class MeshWithProperty;

/** The mesh algorithems container class
 */
class AppMeshExport MeshAlgos
{
public:
  /** Load a Mesh
   */
  static MeshWithProperty* Load(const char *FileName);

  /** write a Mesh as binery STL
   */
  static void writeBin(MeshWithProperty* Mesh,const char *FileName);

  /** write a Mesh as ASCII STL
   */
  static void writeAscii(MeshWithProperty* Mesh,const char *FileName);

  /** Calculate per Vertex normales and adds the Normal property bag
  */
  static void CalcVertexNormales(MeshWithProperty* Mesh);

};



} // namespace Mesh
#endif 
