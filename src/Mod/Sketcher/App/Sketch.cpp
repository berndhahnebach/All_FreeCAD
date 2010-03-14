/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2010     *
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
#include <Base/Exception.h>


#include "Sketch.h"
#include <math.h>

#include <iostream>


using namespace Sketcher;
using namespace Base;
using namespace Part;


TYPESYSTEM_SOURCE(Sketcher::Sketch, Base::Persistence)


Sketch::Sketch()
{
}
Sketch::~Sketch()
{
    for(std::vector<double*>::iterator it;it!=Parameters.end();++it)
        delete *it;
}

// Geometry adding ==========================================================

int Sketch::addGeometry(Part::GeomCurve *geo)
{
    
    if(geo->getTypeId()== GeomLineSegment::getClassTypeId()){ // add a line
        GeomLineSegment *lineSeg = dynamic_cast<GeomLineSegment*>(geo);
        // create the definition struct for that geom
        return addLineSegment(*lineSeg);
    } else {
        Base::Exception("Sketch::addGeometry(): Unknown or unsoported type added to a sketch");
    }
}


int Sketch::addLineSegment(Part::GeomLineSegment lineSegment)
{
    // create our own copy
    GeomLineSegment *lineSeg = new GeomLineSegment(lineSegment);
    // create the definition struct for that geom
    GeoDef def;
    def.geo  = lineSeg;
    def.type = Line;

    // get the points from the line
    Base::Vector3d start = lineSeg->getStartPoint();
    Base::Vector3d end   = lineSeg->getEndPoint();
    // set the parameter for the solver
    def.parameterStartIndex = Parameters.size();
    Parameters.push_back(new double(start.x));
    Parameters.push_back(new double(start.y));
    Parameters.push_back(new double(end.x));
    Parameters.push_back(new double(end.y));

    // set the points for later constraints
    point p1,p2;
    p1.x = Parameters[def.parameterStartIndex+0];
    p1.y = Parameters[def.parameterStartIndex+1];
    p2.x = Parameters[def.parameterStartIndex+2];
    p2.y = Parameters[def.parameterStartIndex+3];
    def.pointStartIndex = Points.size();
    Points.push_back(p1);
    Points.push_back(p2);

    // set the line for later constraints
    line l;
    l.p1 = p1;
    l.p2 = p2;
    def.lineStartIndex = Lines.size();
    Lines.push_back(l);

    // store complete set
    Geoms.push_back(def);
    
    // return the position of the newly added geometry
    return Geoms.size()-1;
}

// constraint adding ==========================================================

int Sketch::addHorizontalConstraint(int geoIndex, const char* name)
{
    // index out of bounds?
    assert(geoIndex < Geoms.size());
    // constraint the right type?
    assert(Geoms[geoIndex].type = Line);

    Constraint constrain;
    constrain.constrain.type = horizontal;
    constrain.constrain.line1 = Lines[Geoms[geoIndex].lineStartIndex];
    if(name)
        constrain.name = name;

    Const.push_back(constrain);

    return Const.size()-1;
}

int Sketch::addVerticalConstraint(int geoIndex, const char* name)
{

    return Const.size()-1;
}


int Sketch::solve(void) {


	return 0;
}

// Persistance implementer -------------------------------------------------

unsigned int Sketch::getMemSize (void) const
{
	return 0;
}

void Sketch::Save (Writer &writer) const
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

void Sketch::Restore(XMLReader &reader)
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

#if 0

using namespace std;

double parameters[100],constants[100];
double *P,*C;

point points[100];
line lines[100];

circle circles[100];
arc arcs[100];
constraint cons[100];


int Sketch::solve(void) {
	P= parameters;

	point origin;
	double zero = 0;
	origin.x = &zero;
	origin.y = &zero;

	constants[0]=6;
	constants[1]=6;
	constants[2]=2;
	constants[3]=M_PI/3;
	constants[4]=3;

	points[0].x = &parameters[0];
	points[0].y = &parameters[1];
	points[1].x = &parameters[2];
	points[1].y = &parameters[3];
	points[2].x = &parameters[4];
	points[2].y = &parameters[5];
	points[3].x = &parameters[6];
	points[3].y = &parameters[7];
	points[4].x = &constants[0];
	points[4].y = &constants[1];




	lines[0].p1 = points[0];
	lines[0].p2 = points[1];
	lines[1].p1 = points[1];
	lines[1].p2 = points[2];


	cons[0].type = horizontal;
	cons[0].line1 = lines[0];

	cons[1].type = vertical;
	cons[1].line1 = lines[1];

	cons[2].type = pointOnPoint;
	cons[2].point1 = points[2];
	cons[2].point2 = points[4];

	cons[3].type = pointOnPoint;
	cons[3].point1 = points[3];
	cons[3].point2 = points[4];

	for(int i=0;i<1;i++)
	{
	parameters[0]=0;//1x
	parameters[1]=0;//y
	parameters[2]=5;//x
	parameters[3]=0;//y
	parameters[4]=6;//xstart
	parameters[5]=5;//y

	parameters[6]=6;//xend
	parameters[7]=5;//y



	int sol;



	double  *pparameters[100];

	for(i=0;i<100;i++)
	{
		pparameters[i] = &parameters[i];
	}

    Solver s;
    sol=s.solve(pparameters ,8,cons,4,rough);

	if(sol==succsess)
	{
		cout<<"A good Solution was found"<<endl;
	}

	else if(sol==noSolution)
	{
		cout<<"No valid Solutions were found from this start point"<<endl;
	}
	for(int i=0;i<8;i++)
	{
		cout<<"Point"<<*pparameters[i]<<endl;
	}
	double hey = parameters[0]*parameters[2]+parameters[1]*parameters[3];
	cout<<"dot product: "<<hey<<endl;

	double gradF[20];
		double *ggradF[20];


		for(int i=0;i<20;i++)
			{
			gradF[i]=0;
			ggradF[i]=&gradF[i];
			}

		//derivatives(pparameters, gradF, 4, cons,3);

		for(int i=0;i<4;i++) cout<<"GradF["<<i<<"]: "<<*ggradF[i]<<endl;



	}
	//end


	return 0;
}

#endif