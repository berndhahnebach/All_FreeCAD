/***************************************************************************
 *   Copyright (c) 2006 Juergen Riegel                                     *
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


#ifndef BASE_PLACEMENT_H
#define BASE_PLACEMENT_H

#include "Vector3D.h"
#include "Matrix.h"



namespace Base {


/**
 * The Matrix4D class.
 */
class BaseExport Placement
{
public:
  /// default constructor
  Placement(void){};
  /// Destruction
  virtual ~Placement () {};

  const Vector3<double> &getPos(void) const {return _Pos;}
  const double *getRotation(void) const {return _q;}

  Base::Matrix4D getAsMatrix(void);


  Vector3<double> _Pos;
  double _q[4];

};

} // namespace Base


#endif // BASE_PLACEMENT_H 


