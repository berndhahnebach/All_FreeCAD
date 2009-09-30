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


#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include <Base/Writer.h>

#include "kdl/chain.hpp"
#include "kdl/chainfksolver.hpp"
#include "kdl/chainfksolverpos_recursive.hpp"
#include "kdl/frames_io.hpp"
#include "kdl/chainiksolver.hpp"
#include "kdl/chainiksolvervel_pinv.hpp"
#include "kdl/chainjnttojacsolver.hpp"
#include "kdl/chainiksolverpos_nr.hpp"

#include "Waypoint.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
    #define M_PI    3.14159265358979323846 /* pi */
#endif

#ifndef M_PI_2
    #define M_PI_2  1.57079632679489661923 /* pi/2 */
#endif

using namespace Robot;
using namespace Base;
using namespace KDL;


TYPESYSTEM_SOURCE(Robot::Waypoint , Base::Persistence);

Waypoint::Waypoint(  const char* name, 
                     const Base::Placement &endPos, 
                     WaypointType type, 
                     float velocity, 
                     bool cont,
                     unsigned int tool, 
                     unsigned int base)

:Name(name),Type(type),EndPos(endPos),Velocity(velocity),Cont(cont),Tool(tool),Base(base)
{
}

Waypoint::Waypoint()
:Velocity(0),Cont(false),Tool(0),Base(0)
{
}

Waypoint::~Waypoint()
{
}

unsigned int Waypoint::getMemSize (void) const
{
	return 0;
}

void Waypoint::Save (Writer &/*writer*/) const
{
}

void Waypoint::Restore(XMLReader &/*reader*/)
{
}

