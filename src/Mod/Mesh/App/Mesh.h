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
# include <list>
# include <set>
# include <string>
# include <map>
#endif

#include "Core/Vector3D.h"
#include "Core/Matrix.h"

namespace Mesh
{

class MeshKernel;

/** Base class of all PropertyBags
 *  with objects derived from this class the mesh
 *  data structur is enriched with aditional data.
 *  The linking point is the Point or face Index.
 *  PropertyBags get registered at the MeshWithProperty class
 *  and have a name and a type. Its posible to have more then 
 *  one PropertyBag of the same type.
 */
class AppMeshExport PropertyBag
{
public:
	/// Constructor
  PropertyBag(void){}
  virtual ~PropertyBag(void){};
  /** returns the type of the property bag
    * is to reimplemented by inhereting classes to
    * allow besides the name a type checking of the bag.
    * So its posible to have more property bags with the 
    * same type and different names in the data structure.
    */
  virtual const char* GetType(void)=0;
  /** Get called when the mesh size is changing
    * this means dependend of the type of the property 
    * to get invalid. Invalid properties can not get used
    * until they are recalculated. Its also a good idea to 
    * clear a included container to save RAM.
    */
  virtual void resizeFaces(void){}
  /** Get called when the amount of the points changing
   * same as resizeFaces only for the points
   */
  virtual void resizePoints(void){}

  /** transform was called for the mesh
   *  this is the message that the topo data structure is transformed
   *  if the properties are geometric they need to transformed too!
   */
  virtual void transform(const Matrix4D &rclMat){}

  /** Set the property bag valid
    * this has to be done after build up or recalculation of the 
    * property bag. The data in the property bag only gets used when 
    * the bag is valid!
    */
  void setValid(void)
    {_bValid=true;}
  /** Set the property bag invalid
    * this is e.g. done when the size of the faces or points has chhanged 
    * and the data in the property bag are wrong!
    */
  void setInvalid(void)
    {_bValid=false;}
  /** checks if the bag is valid
    * invalid bags shut not used for any purpos! Until 
    * they got recalculated.
    */
  bool isValid(void)
    {return _bValid;}

private:
  bool _bValid;
};



/** Vertex noamal property bag
 *  This property bag holds normal vectors of the mesh points
 */
class AppMeshExport MeshPropertyNormal: public PropertyBag
{
public:
	/** Constructor
    * can initialized with the size of the points in the mesh to fit well.
    */
	MeshPropertyNormal(int size=0);

  /// returns the type
  virtual const char* GetType(void) {return "Normal";}

  /// clears the property
  virtual void resizePoints(void);

  virtual void transform(const Matrix4D &rclMat);

  /// vector of the Normals
  std::vector<Vector3D> Normales;

};

/** Vertex color property bag
 *  This property bag holds normal vectors of the mesh points
 */
class AppMeshExport MeshPropertyColor: public PropertyBag
{
public:
	/// Constructor
	MeshPropertyColor(void)
    :PropertyBag() {}

  virtual const char* GetType(void) {return "VertexColor";}

  /// struct to save the color as float r,g,b
  struct fColor
  {
    float r,g,b;
  };

  /// clears the property
  virtual void resizePoints(void)
  {
    setInvalid();
    Color.clear();
  }

  /// color vector
  std::vector<fColor> Color;

};

/**
 * The MeshPropertyCurvature class holds curvature information for each element of a mesh object.
 * @author Werner Mayer
 */
class AppMeshExport MeshPropertyCurvature: public PropertyBag
{
public:
  /** Helper class. */
  struct fCurvature
  {
    float fMaxCurvature, fMinCurvature;
    Vector3D cMaxCurvDir, cMinCurvDir;
  };

  enum Mode { 
    MaxCurvature,  /**< Maximum curvature */ 
    MinCurvature,  /**< Minimum curvature */
    AvgCurvature,  /**< Average curvature */
    GaussCurvature /**< Gaussian curvature */
  };

public:
	MeshPropertyCurvature(int size=0);
  virtual ~MeshPropertyCurvature();

  const char* GetType() {return "Curvature";}

  void resizePoints();
  void resizeFaces();

  std::vector<float> getCurvature( Mode tMode) const;
  void setValue(unsigned long pos, const fCurvature& val)
  { Curvature[pos] = val; }

private:
  std::vector<fCurvature> Curvature;
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
  /// Get a list of PorpertyBags of a special type
  std::list<std::string> GetAllNamesOfType(const char* TypeName);
  /// get a list of all registered types
  std::set<std::string> GetAllTypes(void);
  /// delete all properties
  void clear(void);
  /// transform all properties
  void transform(const Matrix4D &rclMat);

  void operator= ( const DataWithPropertyBag& New);


private:
  std::map<std::string,PropertyBag*> _Properties;
};


/** Mesh with property bags
 */
class AppMeshExport MeshWithProperty: public DataWithPropertyBag
{
public:
	/// Constructor
	MeshWithProperty();
	/// Copy Constructor
	MeshWithProperty(const MeshWithProperty&);

 	/// destructor
	~MeshWithProperty(void);

  /// assignment operator
  void operator= ( const MeshWithProperty& );

  /// clears the mesh (and all its properties)
  void clear(void);

  /// transform the mesh and all properties
  void transform(const Matrix4D &rclMat);

  /// Gain access to the topological mesh data structure
  MeshKernel *getKernel(void){return _Mesh;}

private:
  MeshKernel *_Mesh;
};

} // namespace Mesh

#endif // _Mesh_h_

