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

#include <Base/VectorPy.h>

#include <Mod/Part/App/Geometry.h>
#include <Mod/Part/App/GeometryCurvePy.h>
#include <Mod/Part/App/LinePy.h>


#include "Sketch.h"
#include "Constraint.h"
#include <math.h>

#include <iostream>


using namespace Sketcher;
using namespace Base;
using namespace Part;


TYPESYSTEM_SOURCE(Sketcher::Sketch, Base::Persistence)

const int IntGeoOffset(3); 

Sketch::Sketch()
{
    // add the root point at 0,0
    addPoint(Base::Vector3d());
    // add x,y axis
    Part::GeomLineSegment axis;
    axis.setPoints(Base::Vector3d(0,0,0),Base::Vector3d(100,0,0));
    addLineSegment(axis);
    axis.setPoints(Base::Vector3d(0,0,0),Base::Vector3d(0,100,0));
    addLineSegment(axis);

    // set them to construction elements

}
Sketch::~Sketch()
{
    for(std::vector<double*>::iterator it=Parameters.begin();it!=Parameters.end();++it)
        delete *it;
}

// Geometry adding ==========================================================

int Sketch::addGeometry(const Part::Geometry *geo)
{
    
    if(geo->getTypeId()== GeomLineSegment::getClassTypeId()){ // add a line
        const GeomLineSegment *lineSeg = dynamic_cast<const GeomLineSegment*>(geo);
        // create the definition struct for that geom
        return addLineSegment(*lineSeg);
    } else {
        Base::Exception("Sketch::addGeometry(): Unknown or unsoported type added to a sketch");
        return 0; 
    }
}

void Sketch::addGeometry(const std::vector<Part::Geometry *> geo)
{
    for(std::vector<Part::Geometry *>::const_iterator it = geo.begin();it!=geo.end();++it)
        addGeometry(*it);
    
}

int Sketch::addPoint(Base::Vector3d newPoint)
{
    // create the definition struct for that geom
    GeoDef def;
    def.geo  = 0;
    def.type = Point;
    def.construction = false;

    // set the parameter for the solver
    def.parameterStartIndex = Parameters.size();
    Parameters.push_back(new double(newPoint.x));
    Parameters.push_back(new double(newPoint.y));
 
    // set the points for later constraints
    point p1;
    p1.x = Parameters[def.parameterStartIndex+0];
    p1.y = Parameters[def.parameterStartIndex+1];
    def.pointStartIndex = Points.size();
    Points.push_back(p1);

    // store complete set
    Geoms.push_back(def);
    
    // return the position of the newly added geometry
    return Geoms.size()-1;
}

int Sketch::addLine(const Part::GeomLineSegment &line)
{


    // return the position of the newly added geometry
    return Geoms.size()-1;
}

int Sketch::addLineSegment(const Part::GeomLineSegment &lineSegment)
{
    // create our own copy
    GeomLineSegment *lineSeg = new GeomLineSegment(lineSegment);
    // create the definition struct for that geom
    GeoDef def;
    def.geo  = lineSeg;
    def.type = Line;
    def.construction = false;

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

int Sketch::addArc(const Part::GeomTrimmedCurve &circleSegment)
{
    
    // return the position of the newly added geometry
    return Geoms.size()-1;
}

int Sketch::addCircle(const GeomCircle &cir)
{
    // create our own copy
    GeomCircle *circ = new GeomCircle(cir);
    // create the definition struct for that geom
    GeoDef def;
    def.geo  = circ;
    def.type = Circle;
    def.construction = false;

    // get the point from the line
    Base::Vector3d center = circ->getCenter();
    double radius         = circ->getRadius();
 
    // set the parameter for the solver
    def.parameterStartIndex = Parameters.size();
    Parameters.push_back(new double(center.x));
    Parameters.push_back(new double(center.y));
    Parameters.push_back(new double(radius));

    // set the points for later constraints
    point p1;
    p1.x = Parameters[def.parameterStartIndex+0];
    p1.y = Parameters[def.parameterStartIndex+1];
    def.pointStartIndex = Points.size();
    Points.push_back(p1);

    // add the radius parameter
    double *r = Parameters[def.parameterStartIndex+2];

    // set the circel for later constraints
    circle c;
    c.center = p1;
    c.rad    = r;
    def.circleStartIndex = Circles.size();
    Circles.push_back(c);

    // store complete set
    Geoms.push_back(def);
    
    // return the position of the newly added geometry
    return Geoms.size()-1;
}

int Sketch::addEllipse(const Part::GeomEllipse &ellibse)
{
    
    // return the position of the newly added geometry
    return Geoms.size()-1;
}

std::vector<Part::Geometry *> Sketch::getGeometry(bool withConstrucionElements) const
{
    std::vector<Part::Geometry *> temp(Geoms.size()-IntGeoOffset);
    int i=0;
    std::vector<GeoDef>::const_iterator it=Geoms.begin();
    // skeep the default elements
    it += IntGeoOffset;
         
    for(;it!=Geoms.end();++it,i++)
        if(!it->construction || withConstrucionElements)
          temp[i] = it->geo->clone();

    return temp;
}

Py::Tuple Sketch::getPyGeometry(void) const
{

    Py::Tuple tuple(Geoms.size());
    int i=0;
    std::vector<GeoDef>::const_iterator it=Geoms.begin();
    // skeep the default elements
    it += IntGeoOffset;

    for(;it!=Geoms.end();++it,i++) {
        if(it->type == Line){
            GeomLineSegment *lineSeg = dynamic_cast<GeomLineSegment*>(it->geo);
            tuple[i] = Py::Object(new LinePy(lineSeg));
        }else if(it->type == Point){
            Base::Vector3d temp(*(Points[Geoms[i].pointStartIndex].x),*(Points[Geoms[i].pointStartIndex].y),0);
            tuple[i] = Py::Object(new VectorPy(temp));
        }else {
            assert(0); // not implemented type in the sketch!
        }
    }
    return tuple;
}

 void Sketch::setConstruction(int geoIndex,bool isConstruction)
 {
    // index out of bounds?
    assert(geoIndex < (int)Geoms.size());

    Geoms[geoIndex].construction = isConstruction;

 }

bool  Sketch::getConstruction(int geoIndex) const
 {
    // index out of bounds?
    assert(geoIndex < (int)Geoms.size());

    return Geoms[geoIndex].construction;

 }

// constraint adding ==========================================================

void Sketch::addConstraints(const std::vector<Constraint *> &ConstraintList)
{
    // constraints on nothing makes no sense 
    assert((int)Geoms.size()>0);

    for(std::vector<Constraint *>::const_iterator it = ConstraintList.begin();it!=ConstraintList.end();++it){
        switch ((*it)->Type){
            case Horizontal:
                addHorizontalConstraint((*it)->First,(*it)->Name.c_str());
                break;
            case Vertical:
                addVerticalConstraint((*it)->First,(*it)->Name.c_str());
                break;
            case Coincident:
                addPointCoincidentConstraint((*it)->First,(*it)->FirstPos,(*it)->Second,(*it)->SecondPos,(*it)->Name.c_str());
                break;
        }
    }
}

int Sketch::addHorizontalConstraint(int geoIndex, const char* name)
{
    // index out of bounds?
    assert(geoIndex < (int)Geoms.size());
    // constraint the right type?
    assert(Geoms[geoIndex].type == Line);

    ConstrainDef constrain;
    constrain.constrain.type = horizontal;
    constrain.constrain.line1 = Lines[Geoms[geoIndex].lineStartIndex];
    if(name)
        constrain.name = name;

    Const.push_back(constrain);

    return Const.size()-1;
}

int Sketch::addVerticalConstraint(int geoIndex, const char* name)
{
    // index out of bounds?
    assert(geoIndex < (int)Geoms.size());
    // constraint the right type?
    assert(Geoms[geoIndex].type == Line);

    // creat the constraint and fill it up
    ConstrainDef constrain;
    constrain.constrain.type = vertical;
    constrain.constrain.line1 = Lines[Geoms[geoIndex].lineStartIndex];
    if(name)
        constrain.name = name;

    Const.push_back(constrain);

    return Const.size()-1;
}

int Sketch::addPointCoincidentConstraint(int geoIndex1,PointPos Pos1,int geoIndex2,PointPos Pos2, const char* name)
{   
    // index out of bounds?
    assert(geoIndex1 < (int)Geoms.size());
    assert(geoIndex2 < (int)Geoms.size());
    // constraint the right type?
    assert(Geoms[geoIndex1].type == Line || Geoms[geoIndex1].type == Point);
    assert(Geoms[geoIndex2].type == Line || Geoms[geoIndex2].type == Point);
    // in case of point only the first point is allowed
    assert(Geoms[geoIndex1].type != Point || Pos1 !=  end);
    assert(Geoms[geoIndex2].type != Point || Pos2 !=  end);

    // creat the constraint and fill it up
    ConstrainDef constrain;
    constrain.constrain.type = pointOnPoint;
    if(Pos1 == start)
        constrain.constrain.point1 = Points[Geoms[geoIndex1].pointStartIndex];
    else
        constrain.constrain.point1 = Points[Geoms[geoIndex1].pointStartIndex+1];
    if(Pos2 == start)
        constrain.constrain.point2 = Points[Geoms[geoIndex2].pointStartIndex];
    else
        constrain.constrain.point2 = Points[Geoms[geoIndex2].pointStartIndex+1];

    if(name)
        constrain.name = name;

    Const.push_back(constrain);

    return Const.size()-1;
}


// solving ==========================================================

void _redirectPoint(point &c,double &fixedValue1,double &fixedValue2,double * fixed[2]){
    if(fixed[0] && c.x == fixed[0]){
        // copy the fixes values
        fixedValue1 = *(c.x);
        fixedValue2 = *(c.y);
        // point the constraint to the fixed values
        c.x = &(fixedValue1);
        c.y = &(fixedValue2);
    }
}

int Sketch::solve(double * fixed[2]) {

    Solver s;
    double fixedValue1=0,fixedValue2=0;


    // copy the constraints and handling the fixed constraint ################
    std::vector<constraint> constraints(Const.size());
    int i=0;
    for(std::vector<ConstrainDef>::iterator it=Const.begin();it!=Const.end();++it,i++){
        constraints[i] = it->constrain;
        if(fixed[0]){
            // exchange the fixed points in the constraints
            _redirectPoint(constraints[i].point1,fixedValue1,fixedValue2,fixed);
            _redirectPoint(constraints[i].point1,fixedValue1,fixedValue2,fixed);
            _redirectPoint(constraints[i].line1.p1,fixedValue1,fixedValue2,fixed);
            _redirectPoint(constraints[i].line1.p2,fixedValue1,fixedValue2,fixed);
            _redirectPoint(constraints[i].line2.p1,fixedValue1,fixedValue2,fixed);
            _redirectPoint(constraints[i].line2.p2,fixedValue1,fixedValue2,fixed);
        }
    }

    // solving with solvesketch ############################################
    int ret = s.solve(&Parameters[0],Parameters.size(),&constraints[0],Const.size(),0);

    // set back the fixed parameters no matter what the solver did with it
    if(fixed[0]){
        *(fixed[0]) = fixedValue1;
        *(fixed[1]) = fixedValue2;
    }

    // if successfully solve write the parameter back
    if(ret == 0){
        int i=0;
        for(std::vector<GeoDef>::const_iterator it=Geoms.begin();it!=Geoms.end();++it,i++){
            if(it->type == Line){
                GeomLineSegment *lineSeg = dynamic_cast<GeomLineSegment*>(it->geo);
                lineSeg->setPoints(
                         Vector3d(
                             *Parameters[it->parameterStartIndex+0],
                             *Parameters[it->parameterStartIndex+1],
                             0.0),
                         Vector3d(
                             *Parameters[it->parameterStartIndex+2],
                             *Parameters[it->parameterStartIndex+3],
                             0.0)
                     );
            }

        }
    }


	return ret;
}

int Sketch::solve(void) 
{
    double * fixed[2]={0,0};
    return solve(fixed);
}

int Sketch::movePoint(int geoIndex1,PointPos Pos1,Base::Vector3d toPoint)
{
    // list of the two fixed parameters (point)
    double * fixed[2]={0,0};

    if(Pos1 == start){
        *(Points[Geoms[geoIndex1].pointStartIndex].x) = toPoint.x;
        *(Points[Geoms[geoIndex1].pointStartIndex].y) = toPoint.y;
        fixed [0] = Points[Geoms[geoIndex1].pointStartIndex].x;
        fixed [1] = Points[Geoms[geoIndex1].pointStartIndex].y;
    }else{
        *(Points[Geoms[geoIndex1].pointStartIndex+1].x) = toPoint.x;
        *(Points[Geoms[geoIndex1].pointStartIndex+1].y) = toPoint.y;
        fixed [0] = Points[Geoms[geoIndex1].pointStartIndex+1].x;
        fixed [1] = Points[Geoms[geoIndex1].pointStartIndex+1].y;
    }

    return solve(fixed);
}

TopoShape Sketch::toShape(void)
{
    TopoShape result;
    std::vector<GeoDef>::const_iterator it=Geoms.begin();
    // skeep the default elements
    it += IntGeoOffset;

    bool first = true;
    for(;it!=Geoms.end();++it){
        if(!it->construction){
            TopoDS_Shape sh = it->geo->toShape();
            if (first) {
                first = false;
                result._Shape = sh;
            }
            else {
                result._Shape = result.fuse(sh);
            }
        }
    }
    return result;
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