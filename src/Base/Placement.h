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
#include "Rotation.h"
#include "Matrix.h"



namespace Base {


/**
 * The Placement class.
 */
class BaseExport Placement
{
public:
    /// default constructor
    Placement(void);
    Placement(const Placement&);
    /// Destruction
    ~Placement () {};

    Base::Matrix4D toMatrix(void) const;
    void fromMatrix(const Base::Matrix4D& m);
    void invert();
    Placement inverse() const;

    bool operator == (const Placement&) const;
    bool operator != (const Placement&) const;

    Vector3<double> _pos;
    Base::Rotation  _rot;
};

} // namespace Base


#endif // BASE_PLACEMENT_H 


