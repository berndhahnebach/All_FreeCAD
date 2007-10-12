/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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




#ifndef _AppComplexGeoData_h_
#define _AppComplexGeoData_h_

#include <Base/Placement.h>
#include <Base/Persistance.h>
#include <Base/Handle.h>
#include <Base/Matrix.h>



namespace Data
{


/** Placement Object
 *  Handles the repositioning of data. Also can do grouping
 */
class AppExport ComplexGeoData: public Base::Persistance, public Base::Handled
{

  TYPESYSTEM_HEADER();
 
public:

	/// Constructor
	ComplexGeoData(void);
  ComplexGeoData(const Base::Matrix4D &mtrx):_Mtrx(mtrx){}
  virtual ~ComplexGeoData();


  /** Applies an additional transformation to the current transformation. */
  void applyTransform( const Base::Matrix4D& rclTrf );
  /** Override the current transformation with the new one. */
  void setTransform( const Base::Matrix4D& rclTrf );

  const Base::Matrix4D& getMatrix(void) const {return _Mtrx;}

protected:
  Base::Matrix4D _Mtrx;


};




} //namespace App



#endif
