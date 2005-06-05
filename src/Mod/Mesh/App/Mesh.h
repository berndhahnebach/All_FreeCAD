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

#include "core/Vector3D.h"

namespace Mesh
{

class MeshKernel;

/** Base class of all PropertyBags
 */
class AppMeshExport PropertyBag
{
public:
	/// Constructor
  PropertyBag(void){};
  virtual ~PropertyBag(void){};
  virtual const char* GetType(void)=0;
};



/** Property bag of the Mesh data structure
 *  with objects derived from this class the mesh
 *  data structur is enriched with aditional data.
 *  The linking point is the Point or face Index.
 *  PropertyBags get registered at the MeshWithProperty class
 *  and have a name and a type. Its posible to have more then 
 *  one PropertyBag of the same type.
 */
class AppMeshExport MeshPropertyNormal: public PropertyBag
{
public:
	/// Constructor
	MeshPropertyNormal(void)
    :PropertyBag() {}

  virtual const char* GetType(void) {return "Normal";}

  std::vector<Vector3D> _Normales;

};

/** Base class of all PropertyBag data structures
 *  This class handles the registration and acces of 
 *  PropertyBags
 */
class AppMeshExport DataWithPropertyBag
{
public:
  /// Adds a Property Bag to the container
  void Add(PropertyBag* New, const char* Name);
  /// deletes a named ProperyBag
  void Remove(const char* Name);
  /// delets all PropertyBags of a special type
  void RemoveType(const char* TypeName);
  /// Get a PropertyBag with a special name
  PropertyBag* Get(const char* Name);
  /// Get a ProperyBag of a special type, or NULL if non of this type is insertd
  PropertyBag* GetFirstOfType(const char* TypeName);
  /// Get a list of PorpertyBags of a special type
  std::list<PropertyBag*> GetAllOfType(const char* TypeName);
  /// get a list of all registered types
  std::set<std::string> GetAllTypes(void);

private:
  std::map<std::string,PropertyBag*> _Properties;

};


/** Mesh with property bags
 */
 class AppMeshExport MeshWithProperty: public DataWithPropertyBag
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

