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


#ifndef ROBOT_WAYPOINT_H
#define ROBOT_WAYPOINT_H

#include "kdl/chain.hpp"
#include "kdl/jntarray.hpp"

#include <Base/Persistence.h>
#include <Base/Placement.h>

namespace Robot
{


/** The representation of a waypoint in a trajectory
 */
class AppRobotExport Waypoint : public Base::Persistence
{
    TYPESYSTEM_HEADER();

public:
   enum WaypointType {
        UNDEF,
        PTP,
        LIN,
        CIRC,
        WAIT };

    Waypoint();
    /// full constructor 
    Waypoint(const char* name, 
             WaypointType type=Waypoint::PTP, 
             const Base::Placement &endPos=Base::Placement(), 
             float velocity=100.0, 
             bool cont=false,
             unsigned int tool=0, 
             unsigned int base = 0);

    ~Waypoint();

	// from base class
    virtual unsigned int getMemSize (void) const;
	virtual void Save (Base::Writer &/*writer*/) const;
    virtual void Restore(Base::XMLReader &/*reader*/);

     
    std::string Name;
    WaypointType Type;
    float Velocity;
    bool Cont;
    unsigned int Tool,Base;
    Base::Placement EndPos;

};

} //namespace Part


#endif // ROBOT_WAYPOINT_H
