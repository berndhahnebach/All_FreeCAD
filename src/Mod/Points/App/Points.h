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


namespace Points
{

/** A single Point
 *  This is a single Point of the point datastructure. Note:
 *  No additional data will be attached to this basic point. 
 *  All the aditional data go by PointsPropertyBacks.
 *  @see PointsPropertyBack
 *  @see Points
 */
class PointsAppExport Point
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


/** Property back of the Points datastructure
 *  with objects derived from this class the points
 *  data structur is enriched with aditional data.
 *  The linking point is the Point Index. 
 */
class PointsPropertyBack
{
public:
	/// Constructor
	PointsPropertyBack(void);

};


/** Points with property backs
 */
class PointsAppExport Points
{
public:
	/// Constructor
	Points(void);

private:
  std::vector<Point> _Points;

};



} // namespace Points
#endif 
