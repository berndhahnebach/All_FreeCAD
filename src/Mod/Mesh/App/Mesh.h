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


#ifndef _Mesh_h_
#define _Mesh_h_

#ifndef _PreComp_
# include <vector>
#endif



namespace Mesh
{

class MeshKernel;

/** Property bag of the Mesh data structure
 *  with objects derived from this class the mesh
 *  data structure is enriched with additional data.
 *  The linking point is the Point or face Index.
 */
class MeshPropertyBag
{
public:
	/// Constructor
	MeshPropertyBag(void);
};


/** Mesh with property bags
 */
class AppMeshExport MeshWithProperty
{
public:
	/// Constructor
	MeshWithProperty(void){}

  /// Gain access to the topological mesh data structure
  MeshKernel *GetKernel(void){return _Mesh;}

private:
  MeshKernel *_Mesh;
};

} // namespace Mesh

#endif // _Mesh_h_

