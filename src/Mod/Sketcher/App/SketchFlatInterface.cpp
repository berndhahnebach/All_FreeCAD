/***************************************************************************
 *   Copyright (c) Jürgen Riegel	        <FreeCAD@juergen-riegel.net    *
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

#include <Base/Console.h>

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "SketchFlatInterface.h"
#include "SketchFlat/sketchflat.h"


using namespace Sketcher;

void GenerateCurves(void);
void CurveEval(SketchCurve *c, double t, double *xp, double *yp);


bool SketchFlatInterface::bAlive = false;

//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
SketchFlatInterface::SketchFlatInterface()
{
	// The SketchFlat solver is not build ot have 2 instances!
	assert(!bAlive);
	bAlive=true;

}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
SketchFlatInterface::~SketchFlatInterface()
{
	bAlive=false;
}

//**************************************************************************
// import export

bool SketchFlatInterface::save(const char* FileName)
{
    return (SaveToFile(const_cast<char*>(FileName))?true:false);
}

bool SketchFlatInterface::load(const char* FileName)
{
    return (LoadFromFile(const_cast<char*>(FileName))?true:false);
}


//**************************************************************************
// Methodes to add stuff

/**
 */
unsigned int SketchFlatInterface::addLine(double x, double y)
{
	hEntity he;
	he = SketchAddEntity(ENTITY_LINE_SEGMENT);
	ForcePoint(POINT_FOR_ENTITY(he, 0), x*1000, y*1000);
    return POINT_FOR_ENTITY(he, 1);
}


//**************************************************************************
// render helper

void SketchFlatInterface::setUpRendering(void)
{
    SK->pwls = 0;

    // First, create the various curves.
    GenerateCurves();

    // Then break them down to piecewise linear segments. The chord
    // tolerance with which we do this is caller-configurable.
   
    // And adaptive-pwl each curve.
 /*   int i;
    for(i = 0; i < SK->curves; i++) {
        GeneratePwlsFromCurve(&(SK->curve[i]), chordTol);
    }*/
}

int SketchFlatInterface::nbrOfCurves(void)
{
	return SK->curves;
}

void SketchFlatInterface::getCurvePoints(std::vector<Base::Vector3d> &coords,bool &Construction, int curve)
{
	SketchCurve *c = &(SK->curve[curve]);
	double chordTol = 0.5;

    int pts = 0;
    int iters = 0;

    double from = 0.02;
    double finalTo = 0.98;

    double tryTo = finalTo;

    int pwls0 = SK->pwls;

	Construction = c->construction != 0;

    while(from < (finalTo - 0.001)) {
        double xi, yi;      // Starting point of the line we are considering
        double xf, yf;      // Ending point of the line we are considering
        double xm, ym;      // A point on the curve midway between start, end
        double xml, yml;    // The midpoint of the line we are considering
        
        if(c->x.A != 0 || c->y.A != 0) {
            // A cubic might pass through the midpoint of the line connecting 
            // its endpoints, but deviate from that line elsewhere.
            if(tryTo - from > 0.1) {
                tryTo = std::min<double>(finalTo, from + 0.1);
            }
        }

        CurveEval(c, from, &xi, &yi);
        CurveEval(c, tryTo, &xf, &yf);
        CurveEval(c, (from + tryTo)/2, &xm, &ym);

        xml = (xi + xf)/2;
        yml = (yi + yf)/2;

        if(Distance(xm, ym, xml, yml) < chordTol) {
            // Looks good
			coords.push_back(Base::Vector3d(xi/1000.0,yi/1000.0,0));
            coords.push_back(Base::Vector3d(xf/1000.0,yf/1000.0,0));

            from = tryTo;
            tryTo = finalTo;
            pts++;
        } else {
            tryTo = from + (tryTo - from)/2;
            // And try again
        }

        iters++;
        if(pts >15 || iters > 30) {
            // If we get too many points trying to plot the thing cleverly
            // and adaptively, then give up and just generate 200 evenly
            // spaced points.
			coords.clear();
            SK->pwls = pwls0;
            double t;
            CurveEval(c, 0, &xi, &yi);
            double steps = 20;
            double dt = 1.0/steps;
            for(t = dt; t < 1 + dt; t += dt) {
                CurveEval(c, t, &xf, &yf);
				coords.push_back(Base::Vector3d(xi/1000.0,yi/1000.0,0));
                coords.push_back(Base::Vector3d(xf/1000.0,yf/1000.0,0));
                xi = xf;
                yi = yf;
            }
            break;
        }
    }
}
int SketchFlatInterface::nbrOfPoints(void)
{
	return SK->points;
}

void SketchFlatInterface::getPoint(int Nbr,double &x,double &y)
{
    EvalPoint(SK->point[Nbr], &x, &y);
	x=x/1000.0;
	y=y/1000.0;
}

int SketchFlatInterface::getPoint(int Nbr)
{
    return SK->point[Nbr];
}

int SketchFlatInterface::nbrOfLines(void)
{
	return SK->lines;
}
void SketchFlatInterface::getLine(int Nbr,double &x0, double &y0, double &dx, double &dy)
{
	LineToParametric(SK->line[Nbr], &x0, &y0, &dx, &dy);
	x0=x0/1000.0;
	y0=y0/1000.0;
	dx=dx/1000.0;
	dy=dy/1000.0;
}


void SketchFlatInterface::forcePoint(int point, double x, double y)
{
	ForcePoint(point, x*1000.0, y*1000.0);
}

void SketchFlatInterface::solve(void)
{
	Solve();
}

std::string SketchFlatInterface::getGeo(void)
{
	for(int i = 0; i<SK->entities; ++i)
	{
		SketchEntity e=SK->entity[i];


	}
	return std::string();
}

const char* gce_ErrorStatusText(gce_ErrorType et)
{
    switch (et)
    {
    case gce_Done:
        return "Construction was successful";
    case gce_ConfusedPoints:
        return "Two points are coincident";
    case gce_NegativeRadius:
        return "Radius value is negative";
    case gce_ColinearPoints:
        return "Three points are collinear";
    case gce_IntersectionError:
        return "Intersection cannot be computed";
    case gce_NullAxis:
        return "Axis is undefined";
    case gce_NullAngle:
        return "Angle value is invalid (usually null)";
    case gce_NullRadius:
        return "Radius is null";
    case gce_InvertAxis:
        return "Axis value is invalid";
    case gce_BadAngle:
        return "Angle value is invalid";
    case gce_InvertRadius:
        return "Radius value is incorrect (usually with respect to another radius)";
    case gce_NullFocusLength:
        return "Focal distance is null";
    case gce_NullVector:
        return "Vector is null";
    case gce_BadEquation:
        return "Coefficients are incorrect (applies to the equation of a geometric object)";
    default:
        return "Creation of geometry failed";
    }
}

Part::TopoShape SketchFlatInterface::getGeoAsShape(void)
{
	int point=-1;
    hPoint pt0=0, pt1=0, pt2=0;
    hParam prm0=0;
	bool bFirst=true;
	double x,y,R;
	//struct points2D {
	//	points2D(double x,double y):x(x),y(y){}

	//	double x;
	//	double y;
	//};
	//for(int i=0; i<nbrOfPoints();i++){
	//	getPoint(i,x,y);
	//}


	//std::vector<points2D> points;

	Part::TopoShape result;
	BRepBuilderAPI_MakeWire mkWire;

	for(int i = 0; i<SK->entities; ++i)
	{
		Handle_Geom_Curve curve;
		if(SK->entity[i].construction)
			continue;

		switch(SK->entity[i].type){
			case ENTITY_DATUM_POINT  :      
			case ENTITY_DATUM_LINE   :  
				break;
			case ENTITY_LINE_SEGMENT :{  
				pt0 = POINT_FOR_ENTITY(SK->entity[i].id, 0);
				pt1 = POINT_FOR_ENTITY(SK->entity[i].id, 1);
					
				EvalPoint(pt0, &x, &y);
				gp_Pnt V1(x/1000.0,y/1000.0,0);
				//points.push_back(points2D(x,y));
				EvalPoint(pt1, &x, &y);
				gp_Pnt V2(x/1000.0,y/1000.0,0);
				//points.push_back(points2D(x,y));
				//if (V1 == V2) Standard_Failure::Raise("Both points are equal");
				GC_MakeSegment ms(V1, V2);
				if (!ms.IsDone()) 
					throw Base::Exception(gce_ErrorStatusText(ms.Status()));
				curve = ms.Value();

				break;
									  }
			case ENTITY_CIRCLE       :   
			{
				pt0 = POINT_FOR_ENTITY(SK->entity[i].id, 0);
				prm0 = PARAM_FOR_ENTITY(SK->entity[i].id, 0);

				EvalPoint(pt0, &x, &y);
				gp_Pnt V1(x/1000.0,y/1000.0,0);

				R = EvalParam(prm0);
				
				GC_MakeCircle circ(V1,gp_Dir(0,0,1),R/1000.0);
				if (!circ.IsDone()) 
					throw Base::Exception(gce_ErrorStatusText(circ.Status()));
				curve = circ.Value();

	            break;
			}
			case ENTITY_CIRCULAR_ARC :{  
				pt0 = POINT_FOR_ENTITY(SK->entity[i].id, 0);
				pt1 = POINT_FOR_ENTITY(SK->entity[i].id, 1);
				pt2 = POINT_FOR_ENTITY(SK->entity[i].id, 2);
					
				EvalPoint(pt0, &x, &y);
				gp_Pnt V1(x/1000.0,y/1000.0,0);

				//points.push_back(points2D(x,y));
				EvalPoint(pt1, &x, &y);
				gp_Pnt V2(x/1000.0,y/1000.0,0);
				//points.push_back(points2D(x,y));
				EvalPoint(pt2, &x, &y);
				gp_Pnt V3(x/1000.0,y/1000.0,0);
				//points.push_back(points2D(x,y));
				double rad = V3.Distance(V2);
				gp_Circ circ(gp_Ax2(V3,gp_Dir(0,0,1),gp_Dir(1,0,0)),rad);
            
				//GC_MakeCircle circ(V1,gp_Dir(0,0,1),V1.Distance(V2));
				//GC_MakeArcOfCircle arc(circ,0.0,3.1415,true);
				GC_MakeArcOfCircle arc(circ,V2,V1,true);
            
				if (!arc.IsDone()) 
					throw Base::Exception(gce_ErrorStatusText(arc.Status()));
                
				curve = arc.Value();

				break;
									  }
			case ENTITY_CUBIC_SPLINE : 
			case ENTITY_TTF_TEXT     :   
			case ENTITY_IMPORTED     :
				break;
		}
		if(!curve.IsNull()){
            BRepBuilderAPI_MakeEdge mkEdge(curve, curve->FirstParameter(), curve->LastParameter());
			mkWire.Add(mkEdge);
			//if(bFirst){
			//	result._Shape = mkEdge.Edge();
			//	bFirst=false;
			//}else{
			//	BRepAlgoAPI_Fuse mkFuse(result._Shape, mkEdge.Edge());
			//	if (!mkFuse.IsDone()) 
			//		throw Base::Exception("SketchFlatInterface::getGeoAsShape(): Can not fuse Shape!");
			//	result._Shape = mkFuse.Shape();
			//}
		}

	}
	if(mkWire.IsDone())
		result._Shape = mkWire.Wire();
	else
		Base::Console().Warning("Sketch not created, possibly inner Wires!");

	return result;
}










//**************************************************************************



