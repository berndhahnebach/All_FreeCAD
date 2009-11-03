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


#ifndef ROBOT_ROBOT6AXLE_H
#define ROBOT_ROBOT6AXLE_H

#include "kdl_cp/chain.hpp"
#include "kdl_cp/jntarray.hpp"

#include <Base/Persistence.h>
#include <Base/Placement.h>

namespace Robot
{


/** The representation for a 6-Axis industry grade robot
 */
class AppRobotExport Robot6Axis : public Base::Persistence
{
    TYPESYSTEM_HEADER();

public:
    Robot6Axis();
    ~Robot6Axis();

	// from base class
    virtual unsigned int getMemSize (void) const;
	virtual void Save (Base::Writer &/*writer*/) const;
    virtual void Restore(Base::XMLReader &/*reader*/);

	// interface
	/// set the robot to that position, calculates the Axis
	bool setTo(const Base::Placement &To);
	bool setAxis(int Axis,float Value);
	float getAxis(int Axis);
	/// calculate the new Tcp out of the Axis
	bool calcTcp(void);
	Base::Placement getTcp(void);

    void exportChain(const char* FileName);
    void importChain(const char* FileName);
    


protected:
	KDL::Chain Kinematic;
	KDL::JntArray Actuall;
	KDL::Frame Tcp;

	float MaxAngle[6];
	float MinAngle[6];

};

} //namespace Part


#endif // PART_TOPOSHAPE_H
