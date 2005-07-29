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


#include "PreCompiled.h"
#ifndef _PreComp_
#endif


#include "Points.h"

using namespace Points;

/*

Point::Point ()
{
    // For efficiency in construction of large arrays of vectors, the
    // default constructor does not initialize the vector.
}

Point::Point (float fX, float fY, float fZ)
{
    x = fX;
    y = fY;
    z = fZ;
}

Point::Point (float Coord[3])
{
    x = Coord[0];
    y = Coord[1];
    z = Coord[2];
}

Point::Point (const Point& rPt)
{
    x = rPt.x;
    y = rPt.y;
    z = rPt.z;
}

float& Point::operator[] (int i) const
{
    return (float&) *(&x + i);
}

Point::operator float* ()
{
    return &x;
}

Point& Point::operator= (const Point& rPt)
{
    x = rPt.x;
    y = rPt.y;
    z = rPt.z;
    return *this;
}

bool Point::operator== (const Point& rPt) const
{
    return ( x == rPt.x && y == rPt.y && z == rPt.z );
}

bool Point::operator!= (const Point& rPt) const
{
    return ( x != rPt.x || y != rPt.y || z != rPt.z );
}
*/