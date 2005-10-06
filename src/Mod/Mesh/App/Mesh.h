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

#include <Base/Matrix.h>
#include <Base/Vector3D.h>
using Base::Vector3D;
using Base::Matrix4D;

#include <App/DataWithProperty.h>


namespace MeshCore
{
class MeshKernel;
};

using MeshCore::MeshKernel;

namespace Mesh
{

/** Vertex noamal property bag
 *  This property bag holds normal vectors of the mesh points
 */
class AppMeshExport MeshPropertyNormal: public App::PropertyBag
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
class AppMeshExport MeshPropertyColor: public App::PropertyBag
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
class AppMeshExport MeshPropertyCurvature: public App::PropertyBag
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



/** Mesh with property bags
 */
class AppMeshExport MeshWithProperty: public App::DataWithPropertyBag
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
  const MeshKernel *getKernel(void) const {return _Mesh;}

private:
  MeshKernel *_Mesh;
};

} // namespace Mesh

#endif // _Mesh_h_

