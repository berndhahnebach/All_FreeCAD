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

#include <vector>
#include <list>
#include <set>
#include <string>
#include <map>

#include <Base/Matrix.h>
#include <Base/Vector3D.h>

#include <App/PropertyStandard.h>
#include <App/PropertyGeo.h>

#include "Core/MeshKernel.h"


namespace Mesh
{
class AppMeshExport MeshObject
{
public:
  MeshObject(MeshCore::MeshKernel *Kernel, const Base::Matrix4D &Mtrx )
    : _pcKernel(Kernel),_Mtrx(Mtrx){}

  /** Returns an iterator object to go over all facets. */
  MeshCore::MeshFacetIterator FacetIterator() const;
  /** Returns an iterator object to go over all points. */
  MeshCore::MeshPointIterator PointIterator() const;

  /** Applies an additional transformation to the current transformation. */
  void ApplyTransform( const Base::Matrix4D& rclTrf );
  /** Override the current transformation with the new one. */
  void SetTransform( const Base::Matrix4D& rclTrf );

protected:
  MeshCore::MeshKernel* _pcKernel;
  Base::Matrix4D _Mtrx;
};


/** The normals property class.
 * Note: We need an own class for that to distinguish from the base vector list.
 */
class AppMeshExport PropertyNormalList : public App::PropertyVectorList
{
  TYPESYSTEM_HEADER();

public:
  PropertyNormalList()
  {
  }
  virtual ~PropertyNormalList()
  {
  }
  void transform(const Base::Matrix4D &rclMat);
};

/** Curvature information. */
struct AppMeshExport CurvatureInfo
{
  float fMaxCurvature, fMinCurvature;
  Base::Vector3f cMaxCurvDir, cMinCurvDir;
};

/** The Curvature property class.
 */
class AppMeshExport PropertyCurvatureList: public App::PropertyLists
{
  TYPESYSTEM_HEADER();

public:
  enum { 
    MeanCurvature  = 0,  /**< Mean curvature */
    GaussCurvature = 1,  /**< Gaussian curvature */
    MaxCurvature   = 2,  /**< Maximum curvature */ 
    MinCurvature   = 3,  /**< Minimum curvature */
    AbsCurvature   = 4   /**< Absolute curvature */
  };

public:
  PropertyCurvatureList();
  ~PropertyCurvatureList();

  void setSize(int newSize){_lValueList.resize(newSize);}   
  int getSize(void) const {return _lValueList.size();}   
  std::vector<float> getCurvature( int tMode) const;
  void setValue(const CurvatureInfo&);
  
  /// index operator
  const CurvatureInfo& operator[] (const int idx) const {return _lValueList.operator[] (idx);} 
  void  set1Value (const int idx, const CurvatureInfo& value){_lValueList.operator[] (idx) = value;}
  const std::vector<CurvatureInfo> &getValues(void) const{return _lValueList;}
  void transform(const Base::Matrix4D &rclMat);

  void Save (Base::Writer &writer) const;
  void Restore(Base::XMLReader &reader);
  
  void SaveDocFile (Base::Writer &writer) const;
  void RestoreDocFile(Base::Reader &reader);

  App::Property *Copy(void) const;
  void Paste(const App::Property &from);

  virtual unsigned int getMemSize (void) const{return _lValueList.size() * sizeof(CurvatureInfo);}

private:
  std::vector<CurvatureInfo> _lValueList;
};

/** The meshkernl property
 */
class AppMeshExport PropertyMeshKernel : public App::Property
{
  TYPESYSTEM_HEADER();

public:
	PropertyMeshKernel();
	~PropertyMeshKernel();

  /** @name Getter/setter */
  //@{
  /// COPIES the mesh into the Property
  void setValue( const MeshCore::MeshKernel& m);
  /// REPLACES the mesh in the Property
  void setValue( MeshCore::MeshKernel* m);
  /// get the Kernel (only const possible!)
  const MeshCore::MeshKernel &getValue(void) const;
  virtual unsigned int getMemSize (void) const;

  //@}

  /** @name Modify */
  //@{
  void applyTransformation( const Base::Matrix4D& rclTrf );
  void deletePointIndices ( const std::vector<unsigned long>& );
  void deleteFacetIndices ( const std::vector<unsigned long>& );
  void clear();
  //@}

  /** @name Python interface */
  //@{
  PyObject* getPyObject(void);
  void setPyObject(PyObject *value);
  //@}

  const char* getEditorName(void) const { return "MeshGui::PropertyEditorMesh"; }

  /** @name Save/restore */
  //@{
  void Save (Base::Writer &writer) const;
  void Restore(Base::XMLReader &reader);

  void SaveDocFile (Base::Writer &writer) const;
  void RestoreDocFile(Base::Reader &reader);

  App::Property *Copy(void) const;
  void Paste(const App::Property &from);
  //@}

private:
  MeshCore::MeshKernel *_pcMesh;
};

} // namespace Mesh

#endif // _Mesh_h_

