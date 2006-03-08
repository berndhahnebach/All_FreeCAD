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


#ifndef _Points_h_
#define _Points_h_

#ifndef _PreComp_
# include <vector>
#endif

#include <Base/Vector3D.h>
#include <Base/Matrix.h>
#include <Base/Reader.h>
#include <Base/Writer.h>
using Base::Vector3D;
using Base::Matrix4D;
using Base::Matrix4D;

#include <App/DataWithProperty.h>
#include <App/PropertyStandard.h>
#include <App/PropertyGeo.h>

namespace Points
{

/** Greyvalue property.
 */
class PointsAppExport PropertyGreyValue : public App::PropertyFloat
{
  TYPESYSTEM_HEADER();

public:
  PropertyGreyValue(void)
  {
  }
  virtual ~PropertyGreyValue()
  {
  }
};

/**
 * Own class to distinguish from real float list
 */
class PointsAppExport PropertyGreyValueList : public App::PropertyFloatList
{
  TYPESYSTEM_HEADER();

public:
  PropertyGreyValueList()
  {
  }
  virtual ~PropertyGreyValueList()
  {
  }
};

/**
 * Own class to distinguish from real vector list
 */
class PointsAppExport PropertyNormalList : public App::PropertyVectorList
{
  TYPESYSTEM_HEADER();

public:
  PropertyNormalList()
  {
  }
  virtual ~PropertyNormalList()
  {
  }
  void transform(const Matrix4D &rclMat);
};

/** Curvature information. */
struct PointsAppExport CurvatureInfo
{
  float fMaxCurvature, fMinCurvature;
  Vector3D cMaxCurvDir, cMinCurvDir;
};

/** The Curvature property class.
 */
class PointsAppExport PropertyCurvatureList: public App::PropertyLists
{
  TYPESYSTEM_HEADER();

public:
	PropertyCurvatureList();
	~PropertyCurvatureList();

  void setSize(int newSize){_lValueList.resize(newSize);}   
  int getSize(void) const {return _lValueList.size();}   
	void setValue(const CurvatureInfo&);
  
  /// index operator
  const CurvatureInfo& operator[] (const int idx) const {return _lValueList.operator[] (idx);} 
  void  set1Value (const int idx, const CurvatureInfo& value){_lValueList.operator[] (idx) = value;}
  const std::vector<CurvatureInfo> &getValues(void) const{return _lValueList;}
  void transform(const Matrix4D &rclMat);

  void Save (Base::Writer &writer);
  void Restore(Base::XMLReader &reader);
  void SaveDocFile (Base::Writer &writer);
  void RestoreDocFile(Base::Reader &reader);

private:
  std::vector<CurvatureInfo> _lValueList;
};








typedef std::vector<Vector3D> PointKernel;


/** Vertex color property bag
 * This property bag holds normal vectors of the points.
 * @deprecated Use App::PropertyColorList
 */
class PointsAppExport PointsPropertyColor: public App::PropertyBag
{
public:
	/// Constructor
	PointsPropertyColor(void)
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

/** Vertex grey value property bag
 * This property bag holds grey values of the points.
 * @deprecated Use PropertyGreyValueList
 */
class PointsAppExport PointsPropertyGreyvalue: public App::PropertyBag
{
public:
	/// Constructor
	PointsPropertyGreyvalue(void)
    :PropertyBag() {}

  virtual const char* GetType(void) {return "VertexGreyvalue";}

  /// clears the property
  virtual void resizePoints(void)
  {
    setInvalid();
    aGreyvalue.clear();
  }

  /// greyvalue vector
  std::vector<float> aGreyvalue;

};

/** Vertex normal property bag
 * This property bag holds normals of the points.
 * @deprecated Use PropertyNormalList.
 */
class PointsAppExport PointsPropertyNormal : public App::PropertyBag
{
public:
	/// Constructor
	PointsPropertyNormal(void)
    :PropertyBag() {}

  virtual const char* GetType(void) {return "VertexNormal";}

  /// clears the property
  virtual void resizePoints(void)
  {
    setInvalid();
    aVertexNormal.clear();
  }

  virtual void transform(const Matrix4D &rclMat);

  /// normal vector
  std::vector<Base::Vector3D> aVertexNormal;

};

/**
 * The PointsPropertyCurvature class holds curvature information for each point.
 * @author Werner Mayer
 */
class PointsAppExport PointsPropertyCurvature: public App::PropertyBag
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
    MeanCurvature, /**< Mean curvature */
    GaussCurvature /**< Gaussian curvature */
  };

public:
	PointsPropertyCurvature(int size=0);
  virtual ~PointsPropertyCurvature();

  const char* GetType() {return "VertexCurvature";}

  void resizePoints();
  void transform(const Matrix4D &rclMat);

  std::vector<float> getCurvature( Mode tMode) const;
  /** Returns the principal curvature directions either for \c MaxCurvature or \c MinCurvature.
   * for \c MeanCurvature or \c GaussCurvature an empty list is returned.
   */
  std::vector<Vector3D> getCurvatureDir( Mode tMode) const;
  /** Returns the principal curvature directions either for \c MaxCurvature or \c MinCurvature.
   * for \c MeanCurvature or \c GaussCurvature an empty list is returned.
   */
  std::vector<Vector3D> getAbsCurvatureDir( Mode tMode) const;
  void setValue(unsigned long pos, const fCurvature& val)
  { Curvature[pos] = val; }

private:
  std::vector<fCurvature> Curvature;
};

/** A single Point
 *  This is a single Point of the point datastructure. Note:
 *  No additional data will be attached to this basic point. 
 *  All the aditional data go by PointsPropertyBacks.
 *  @see PointsPropertyBack
 *  @see Points
 */
/*
  class PointsAppExport Point: puplic Base::Vector3D
{
public:
    // construction
    Point ();
    Point (float fX, float fY, float fZ);
    Point (float Coord[3]);
    Point (const Point& rPt);

    // member access (allows V.x or V[0], V.y or V[1], V.z or V[2])
    float x, y, z;
    float& operator[] (int i) const;
    operator float* ();

    // assignment and comparison
    Point& operator= (const Point& rPt);
    bool operator== (const Point& rPt) const;
    bool operator!= (const Point& rPt) const;
};

*/

/** Points with property backs
 */
class PointsAppExport PointsWithProperty: public App::DataWithPropertyBag
{
public:
	/// Constructor
	PointsWithProperty(void){}

  /// transform the points and all their properties
  void transform(const Matrix4D &rclMat);

  PointKernel &getKernel(void){return _Points;}

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::XMLReader &reader);

private:
  PointKernel _Points;

};

} // namespace Points


#endif // _Points_h_ 
