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
	ForcePoint(POINT_FOR_ENTITY(he, 0), x, y);
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

void SketchFlatInterface::getCurvePoints(std::vector<Base::Vector3d> &coords,int curve)
{
	SketchCurve *c = &(SK->curve[curve]);
	double chordTol = 0.1;

    int pts = 0;
    int iters = 0;

    double from = 0.02;
    double finalTo = 0.98;

    double tryTo = finalTo;

    int pwls0 = SK->pwls;

    while(from < (finalTo - 0.001)) {
        double xi, yi;      // Starting point of the line we are considering
        double xf, yf;      // Ending point of the line we are considering
        double xm, ym;      // A point on the curve midway between start, end
        double xml, yml;    // The midpoint of the line we are considering
        
        if(c->x.A != 0 || c->y.A != 0) {
            // A cubic might pass through the midpoint of the line connecting 
            // its endpoints, but deviate from that line elsewhere.
            if(tryTo - from > 0.1) {
                tryTo = min(finalTo, from + 0.1);
            }
        }

        CurveEval(c, from, &xi, &yi);
        CurveEval(c, tryTo, &xf, &yf);
        CurveEval(c, (from + tryTo)/2, &xm, &ym);

        xml = (xi + xf)/2;
        yml = (yi + yf)/2;

        if(Distance(xm, ym, xml, yml) < chordTol) {
            // Looks good
			coords.push_back(Base::Vector3d(xi,yi,0));
            coords.push_back(Base::Vector3d(xf,yf,0));

            from = tryTo;
            tryTo = finalTo;
            pts++;
        } else {
            tryTo = from + (tryTo - from)/2;
            // And try again
        }

        iters++;
        if(pts > 200 || iters > 1000) {
            // If we get too many points trying to plot the thing cleverly
            // and adaptively, then give up and just generate 200 evenly
            // spaced points.
            SK->pwls = pwls0;
            double t;
            CurveEval(c, 0, &xi, &yi);
            double steps = 200;
            double dt = 1.0/steps;
            for(t = dt; t < 1 + dt; t += dt) {
                CurveEval(c, t, &xf, &yf);
				coords.push_back(Base::Vector3d(xi,yi,0));
                coords.push_back(Base::Vector3d(xf,yf,0));
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
}


void SketchFlatInterface::forcePoint(int point, double x, double y)
{
	ForcePoint(point, x, y);
}

void SketchFlatInterface::solve(void)
{
	Solve();
}











//**************************************************************************



