/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2008     *
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
#include <Base/Reader.h>


#include "Constraint.h"
#include "SketchFlatInterface.h"


using namespace Sketcher;
using namespace Base;


TYPESYSTEM_SOURCE(Sketcher::Constraint, Base::Persistance)


Constraint::Constraint()
{
}
Constraint::~Constraint()
{
}

unsigned int Constraint::getMemSize (void) const
{
	return 0;
}

void Constraint::Save (Writer &writer) const
{
 /*   for(unsigned int i=0;i<6;i++){
        Base::Placement Tip = toPlacement(Kinematic.getSegment(i).getFrameToTip());
	    writer.Stream() << writer.ind() << "<Axis "
                        << "Px=\""          <<  Tip.getPosition().x  << "\" " 
                        << "Py=\""          <<  Tip.getPosition().y  << "\" "
                        << "Pz=\""          <<  Tip.getPosition().z  << "\" "
					    << "Q0=\""          <<  Tip.getRotation()[0] << "\" "
                        << "Q1=\""          <<  Tip.getRotation()[1] << "\" "
                        << "Q2=\""          <<  Tip.getRotation()[2] << "\" "
                        << "Q3=\""          <<  Tip.getRotation()[3] << "\" "
                        << "maxAngle=\""    <<  MaxAngle[i]		     << "\" "
                        << "minAngle=\""    <<  MinAngle[i]			 << "\" "
                        << "AxisVelocity=\""<<  Velocity[i]          << "\" "
                        << "Pos=\""         <<  Actuall(i)           << "\"/>"
					                      
                        << std::endl;
    }*/

}

void Constraint::Restore(XMLReader &reader)
{
    //Chain Temp;
    //Base::Placement Tip;

    //for(unsigned int i=0;i<6;i++){
    //    // read my Element
    //    reader.readElement("Axis");
    //    // get the value of the placement
    //    Tip =    Base::Placement(Base::Vector3d(reader.getAttributeAsFloat("Px"),
    //                                            reader.getAttributeAsFloat("Py"),
    //                                            reader.getAttributeAsFloat("Pz")),
    //                             Base::Rotation(reader.getAttributeAsFloat("Q0"),
    //                                            reader.getAttributeAsFloat("Q1"),
    //                                            reader.getAttributeAsFloat("Q2"),
    //                                            reader.getAttributeAsFloat("Q3")));
    //    Temp.addSegment(Segment(Joint(Joint::RotZ),toFrame(Tip)));


    //    MaxAngle[i]   = reader.getAttributeAsFloat("maxAngle");
    //    MinAngle[i]	  = reader.getAttributeAsFloat("minAngle");
    //    if(reader.hasAttribute("AxisVelocity"))
    //        Velocity[i] = reader.getAttributeAsFloat("AxisVelocity");
    //    else
    //        Velocity[i] = 156.0;
    //    Actuall(i) = reader.getAttributeAsFloat("Pos");
    //}
    //Kinematic = Temp;

    //calcTcp();

}


