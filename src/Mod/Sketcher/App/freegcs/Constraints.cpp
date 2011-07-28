/***************************************************************************
 *   Copyright (c) Konstantinos Poulios      (logari81@gmail.com) 2011     *
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

#include <cmath>
#include "Constraints.h"

namespace GCS
{

///////////////////////////////////////
// Constraints
///////////////////////////////////////

Constraint::Constraint()
: origpvec(0), pvec(0), scale(1.), priority(0)
{
}

void Constraint::redirectParams(MAP_pD_pD redirectionmap)
{
    int i=0;
    for (VEC_pD::iterator param=origpvec.begin();
         param != origpvec.end(); ++param, i++) {
        MAP_pD_pD::const_iterator it = redirectionmap.find(*param);
        if (it != redirectionmap.end())
            pvec[i] = it->second;
    }
}

void Constraint::revertParams()
{
    pvec = origpvec;
}

ConstraintType Constraint::getTypeId()
{
    return None;
}

void Constraint::rescale(double coef)
{
    scale = coef * 1.;
}

double Constraint::error()
{
    return 0.;
}

double Constraint::grad(double *param)
{
    return 0.;
}

double Constraint::maxStep(MAP_pD_D &dir, double lim)
{
    return lim;
}

// Equal
ConstraintEqual::ConstraintEqual(double *p1, double *p2)
{
    pvec.push_back(p1);
    pvec.push_back(p2);
    origpvec = pvec;
    rescale();
}

ConstraintType ConstraintEqual::getTypeId()
{
    return Equal;
}

void ConstraintEqual::rescale(double coef)
{
    scale = coef * 1.;
}

double ConstraintEqual::error()
{
    return scale * (*param1() - *param2());
}

double ConstraintEqual::grad(double *param)
{
    double deriv=0.;
    if (param == param1()) deriv += 1;
    if (param == param2()) deriv += -1;
    return scale * deriv;
}

// Difference
ConstraintDifference::ConstraintDifference(double *p1, double *p2, double *d)
{
    pvec.push_back(p1);
    pvec.push_back(p2);
    pvec.push_back(d);
    origpvec = pvec;
    rescale();
}

ConstraintType ConstraintDifference::getTypeId()
{
    return Difference;
}

void ConstraintDifference::rescale(double coef)
{
    scale = coef * 1.;
}

double ConstraintDifference::error()
{
    return scale * (*param2() - *param1() - *difference());
}

double ConstraintDifference::grad(double *param)
{
    double deriv=0.;
    if (param == param1()) deriv += -1;
    if (param == param2()) deriv += 1;
    if (param == difference()) deriv += -1;
    return scale * deriv;
}

// P2PDistance
ConstraintP2PDistance::ConstraintP2PDistance(Point &p1, Point &p2, double *d)
{
    pvec.push_back(p1.x);
    pvec.push_back(p1.y);
    pvec.push_back(p2.x);
    pvec.push_back(p2.y);
    pvec.push_back(d);
    origpvec = pvec;
    rescale();
}

ConstraintType ConstraintP2PDistance::getTypeId()
{
    return P2PDistance;
}

void ConstraintP2PDistance::rescale(double coef)
{
    scale = coef * 1.;
}

double ConstraintP2PDistance::error()
{
    double dx = (*p1x() - *p2x());
    double dy = (*p1y() - *p2y());
    double d = sqrt(dx*dx + dy*dy);
    double dist  = *distance();
    return scale * (d - dist);
}

double ConstraintP2PDistance::grad(double *param)
{
    double deriv=0.;
    if (param == p1x() || param == p1y() ||
        param == p2x() || param == p2y()) {
        double dx = (*p1x() - *p2x());
        double dy = (*p1y() - *p2y());
        double d = sqrt(dx*dx + dy*dy);
        if (param == p1x()) deriv += dx/d;
        if (param == p1y()) deriv += dy/d;
        if (param == p2x()) deriv += -dx/d;
        if (param == p2y()) deriv += -dy/d;
    }
    if (param == distance()) deriv += -1.;
    return scale * deriv;
}

double ConstraintP2PDistance::maxStep(MAP_pD_D &dir, double lim)
{
    MAP_pD_D::iterator it;
    // distance() >= 0
    it = dir.find(distance());
    if (it != dir.end()) {
        if (it->second < 0.)
            lim = std::min(lim, -(*distance()) / it->second);
    }
    // restrict actual distance change
    double ddx=0.,ddy=0.;
    it = dir.find(p1x());
    if (it != dir.end()) ddx += it->second;
    it = dir.find(p1y());
    if (it != dir.end()) ddy += it->second;
    it = dir.find(p2x());
    if (it != dir.end()) ddx -= it->second;
    it = dir.find(p2y());
    if (it != dir.end()) ddy -= it->second;
    double dd = sqrt(ddx*ddx+ddy*ddy);
    double dist  = *distance();
    if (dd > dist) {
        double dx = (*p1x() - *p2x());
        double dy = (*p1y() - *p2y());
        double d = sqrt(dx*dx + dy*dy);
        if (dd > d)
            lim = std::min(lim, std::max(d,dist)/dd);
    }
    return lim;
}

// P2PAngle
ConstraintP2PAngle::ConstraintP2PAngle(Point &p1, Point &p2, double *a, double da_)
: da(da_)
{
    pvec.push_back(p1.x);
    pvec.push_back(p1.y);
    pvec.push_back(p2.x);
    pvec.push_back(p2.y);
    pvec.push_back(a);
    origpvec = pvec;
    rescale();
}

ConstraintType ConstraintP2PAngle::getTypeId()
{
    return P2PAngle;
}

void ConstraintP2PAngle::rescale(double coef)
{
    scale = coef * 1.;
}

double ConstraintP2PAngle::error()
{
    double dx = (*p2x() - *p1x());
    double dy = (*p2y() - *p1y());
    double a = *angle() + da;
    double ca = cos(a);
    double sa = sin(a);
    double x = dx*ca + dy*sa;
    double y = -dx*sa + dy*ca;
    return scale * atan2(y,x);
}

double ConstraintP2PAngle::grad(double *param)
{
    double deriv=0.;
    if (param == p1x() || param == p1y() ||
        param == p2x() || param == p2y() || param == angle()) {
        double dx = (*p2x() - *p1x());
        double dy = (*p2y() - *p1y());
        double a = *angle() + da;
        double ca = cos(a);
        double sa = sin(a);
        double x = dx*ca + dy*sa;
        double y = -dx*sa + dy*ca;
        double r2 = dx*dx+dy*dy;
        dx = -y/r2;
        dy = x/r2;
        if (param == p1x()) deriv += (-ca*dx + sa*dy);
        if (param == p1y()) deriv += (-sa*dx - ca*dy);
        if (param == p2x()) deriv += ( ca*dx - sa*dy);
        if (param == p2y()) deriv += ( sa*dx + ca*dy);
        if (param == angle()) deriv += -1;
    }
    return scale * deriv;
}

double ConstraintP2PAngle::maxStep(MAP_pD_D &dir, double lim)
{
    // step(angle()) <= pi/18 = 10°
    MAP_pD_D::iterator it = dir.find(angle());
    if (it != dir.end()) {
        double step = std::abs(it->second);
        if (step > M_PI/18.)
            lim = std::min(lim, (M_PI/18.) / step);
    }
    return lim;
}

// P2LDistance
ConstraintP2LDistance::ConstraintP2LDistance(Point &p, Line &l, double *d)
{
    pvec.push_back(p.x);
    pvec.push_back(p.y);
    pvec.push_back(l.p1.x);
    pvec.push_back(l.p1.y);
    pvec.push_back(l.p2.x);
    pvec.push_back(l.p2.y);
    pvec.push_back(d);
    origpvec = pvec;
    rescale();
}

ConstraintType ConstraintP2LDistance::getTypeId()
{
    return P2LDistance;
}

void ConstraintP2LDistance::rescale(double coef)
{
    scale = coef;
}

double ConstraintP2LDistance::error()
{
    double x0=*p0x(), x1=*p1x(), x2=*p2x();
    double y0=*p0y(), y1=*p1y(), y2=*p2y();
    double dist = *distance();
    double dx = x2-x1;
    double dy = y2-y1;
    double d = sqrt(dx*dx+dy*dy);
    double area = std::abs(-x0*dy+y0*dx+x1*y2-x2*y1); // = x1y2 - x2y1 - x0y2 + x2y0 + x0y1 - x1y0 = 2*(triangle area)
    return scale * (area/d - dist);
}

double ConstraintP2LDistance::grad(double *param)
{
    double deriv=0.;
    // darea/dx0 = (y1-y2)      darea/dy0 = (x2-x1)
    // darea/dx1 = (y2-y0)      darea/dy1 = (x0-x2)
    // darea/dx2 = (y0-y1)      darea/dy2 = (x1-x0)
    if (param == p0x() || param == p0y() ||
        param == p1x() || param == p1y() ||
        param == p2x() || param == p2y()) {
        double x0=*p0x(), x1=*p1x(), x2=*p2x();
        double y0=*p0y(), y1=*p1y(), y2=*p2y();
        double dx = x2-x1;
        double dy = y2-y1;
        double d2 = dx*dx+dy*dy;
        double d = sqrt(d2);
        double area = -x0*dy+y0*dx+x1*y2-x2*y1;
        if (param == p0x()) deriv += (y1-y2) / d;
        if (param == p0y()) deriv += (x2-x1) / d ;
        if (param == p1x()) deriv += ((y2-y0)*d + (dx/d)*area) / d2;
        if (param == p1y()) deriv += ((x0-x2)*d + (dy/d)*area) / d2;
        if (param == p2x()) deriv += ((y0-y1)*d - (dx/d)*area) / d2;
        if (param == p2y()) deriv += ((x1-x0)*d - (dy/d)*area) / d2;
        if (area < 0)
            deriv *= -1;
    }
    if (param == distance())
        deriv += -1;
    return scale * deriv;
}

double ConstraintP2LDistance::maxStep(MAP_pD_D &dir, double lim)
{
    MAP_pD_D::iterator it;
    // distance() >= 0
    it = dir.find(distance());
    if (it != dir.end()) {
        if (it->second < 0.)
            lim = std::min(lim, -(*distance()) / it->second);
    }
    // restrict actual area change
    double darea=0.;
    double x0=*p0x(), x1=*p1x(), x2=*p2x();
    double y0=*p0y(), y1=*p1y(), y2=*p2y();
    it = dir.find(p0x());
    if (it != dir.end()) darea += (y1-y2) * it->second;
    it = dir.find(p0y());
    if (it != dir.end()) darea += (x2-x1) * it->second;
    it = dir.find(p1x());
    if (it != dir.end()) darea += (y2-y0) * it->second;
    it = dir.find(p1y());
    if (it != dir.end()) darea += (x0-x2) * it->second;
    it = dir.find(p2x());
    if (it != dir.end()) darea += (y0-y1) * it->second;
    it = dir.find(p2y());
    if (it != dir.end()) darea += (x1-x0) * it->second;

    darea = std::abs(darea);
    if (darea > 0.) {
        double dx = x2-x1;
        double dy = y2-y1;
        double area = 0.3*(*distance())*sqrt(dx*dx+dy*dy);
        if (darea > area) {
            area = std::max(area, 0.3*std::abs(-x0*dy+y0*dx+x1*y2-x2*y1));
            if (darea > area)
                lim = std::min(lim, area/darea);
        }
    }
    return lim;
}

// PointOnLine
ConstraintPointOnLine::ConstraintPointOnLine(Point &p, Line &l)
{
    pvec.push_back(p.x);
    pvec.push_back(p.y);
    pvec.push_back(l.p1.x);
    pvec.push_back(l.p1.y);
    pvec.push_back(l.p2.x);
    pvec.push_back(l.p2.y);
    origpvec = pvec;
    rescale();
}

ConstraintType ConstraintPointOnLine::getTypeId()
{
    return PointOnLine;
}

void ConstraintPointOnLine::rescale(double coef)
{
    scale = coef;
}

double ConstraintPointOnLine::error()
{
    double x0=*p0x(), x1=*p1x(), x2=*p2x();
    double y0=*p0y(), y1=*p1y(), y2=*p2y();
    double dx = x2-x1;
    double dy = y2-y1;
    double area = -x0*dy+y0*dx+x1*y2-x2*y1; // = x1y2 - x2y1 - x0y2 + x2y0 + x0y1 - x1y0 = 2*(triangle area)
    return scale * area;
}

double ConstraintPointOnLine::grad(double *param)
{
    double deriv=0.;
    // darea/dx0 = (y1-y2)      darea/dy0 = (x2-x1)
    // darea/dx1 = (y2-y0)      darea/dy1 = (x0-x2)
    // darea/dx2 = (y0-y1)      darea/dy2 = (x1-x0)
    if (param == p0x()) deriv += (*p1y()-*p2y());
    if (param == p0y()) deriv += (*p2x()-*p1x());
    if (param == p1x()) deriv += (*p2y()-*p0y());
    if (param == p1y()) deriv += (*p0x()-*p2x());
    if (param == p2x()) deriv += (*p0y()-*p1y());
    if (param == p2y()) deriv += (*p1x()-*p0x());
    return scale * deriv;
}

// Parallel
ConstraintParallel::ConstraintParallel(Line &l1, Line &l2)
{
    pvec.push_back(l1.p1.x);
    pvec.push_back(l1.p1.y);
    pvec.push_back(l1.p2.x);
    pvec.push_back(l1.p2.y);
    pvec.push_back(l2.p1.x);
    pvec.push_back(l2.p1.y);
    pvec.push_back(l2.p2.x);
    pvec.push_back(l2.p2.y);
    origpvec = pvec;
    rescale();
}

ConstraintType ConstraintParallel::getTypeId()
{
    return Parallel;
}

void ConstraintParallel::rescale(double coef)
{
    scale = coef * 0.01;
}

double ConstraintParallel::error()
{
    double dx1 = (*l1p1x() - *l1p2x());
    double dy1 = (*l1p1y() - *l1p2y());
    double dx2 = (*l2p1x() - *l2p2x());
    double dy2 = (*l2p1y() - *l2p2y());
    return scale * (dx1*dy2 - dx2*dy1);
}

double ConstraintParallel::grad(double *param)
{
    double deriv=0.;
    if (param == l1p1x()) deriv += (*l2p1y() - *l2p2y()); // = dy2
    if (param == l1p2x()) deriv += -(*l2p1y() - *l2p2y()); // = -dy2
    if (param == l1p1y()) deriv += -(*l2p1x() - *l2p2x()); // = -dx2
    if (param == l1p2y()) deriv += (*l2p1x() - *l2p2x()); // = dx2

    if (param == l2p1x()) deriv += -(*l1p1y() - *l1p2y()); // = -dy1
    if (param == l2p2x()) deriv += (*l1p1y() - *l1p2y()); // = dy1
    if (param == l2p1y()) deriv += (*l1p1x() - *l1p2x()); // = dx1
    if (param == l2p2y()) deriv += -(*l1p1x() - *l1p2x()); // = -dx1

    return scale * deriv;
}

// Perpendicular
ConstraintPerpendicular::ConstraintPerpendicular(Line &l1, Line &l2)
{
    pvec.push_back(l1.p1.x);
    pvec.push_back(l1.p1.y);
    pvec.push_back(l1.p2.x);
    pvec.push_back(l1.p2.y);
    pvec.push_back(l2.p1.x);
    pvec.push_back(l2.p1.y);
    pvec.push_back(l2.p2.x);
    pvec.push_back(l2.p2.y);
    origpvec = pvec;
    rescale();
}

ConstraintType ConstraintPerpendicular::getTypeId()
{
    return Perpendicular;
}

void ConstraintPerpendicular::rescale(double coef)
{
    scale = coef * 1.;
}

double ConstraintPerpendicular::error()
{
    double dx1 = (*l1p1x() - *l1p2x());
    double dy1 = (*l1p1y() - *l1p2y());
    double dx2 = (*l2p1x() - *l2p2x());
    double dy2 = (*l2p1y() - *l2p2y());
    return scale * (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1+dy1*dy1)*(dx2*dx2+dy2*dy2));
}

double ConstraintPerpendicular::grad(double *param)
{
    double deriv=0.;
    if (param == l1p1x() || param == l1p2x() || param == l1p1y() || param == l1p2y() ||
        param == l2p1x() || param == l2p2x() || param == l2p1y() || param == l2p2y()) {
        double dx1 = (*l1p1x() - *l1p2x());
        double dy1 = (*l1p1y() - *l1p2y());
        double dx2 = (*l2p1x() - *l2p2x());
        double dy2 = (*l2p1y() - *l2p2y());
        double dd1 = dx1*dx1+dy1*dy1;
        double dd2 = dx2*dx2+dy2*dy2;
        double d1 = sqrt(dd1);
        double d2 = sqrt(dd2);
        double dp = dx1*dx2 + dy1*dy2;
        if (param == l1p1x()) deriv += (d1*dx2 - dp*dx1/d1) / (dd1*d2);
        if (param == l1p2x()) deriv += -(d1*dx2 - dp*dx1/d1) / (dd1*d2);
        if (param == l1p1y()) deriv += (d1*dy2 - dp*dy1/d1) / (dd1*d2);
        if (param == l1p2y()) deriv += -(d1*dy2 - dp*dy1/d1) / (dd1*d2);

        if (param == l2p1x()) deriv += (d2*dx1 - dp*dx2/d2) / (dd2*d1);
        if (param == l2p2x()) deriv += -(d2*dx1 - dp*dx2/d2) / (dd2*d1);
        if (param == l2p1y()) deriv += (d2*dy1 - dp*dy2/d2) / (dd2*d1);
        if (param == l2p2y()) deriv += -(d2*dy1 - dp*dy2/d2) / (dd2*d1);
    }
    return scale * deriv;
}


} //namespace GCS
