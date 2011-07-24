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
#include <iostream>
#include <algorithm>

#include "GCS.h"
#include "qp_eq.h"

namespace GCS
{

///////////////////////////////////////
// Solver
///////////////////////////////////////

/*
JacobianMatrix::JacobianMatrix()
: matrix()
{
}

double JacobianMatrix::operator() (Constraint *constr, double *param)
{
    std::map<Constraint *,MAP_pD_D >:: const_iterator row;
    row = matrix.find(constr);
    if (row != matrix.end()) {
        MAP_pD_D::const_iterator element;
        element = row->second.find(param);
        if (element != row->second.end())
          return element->second;
    }
    return 0.;
}

void JacobianMatrix::set(Constraint *constr, double *param, double value)
{
    matrix[constr][param] = value;
}

void JacobianMatrix::remove(Constraint *constr)
{
    matrix.erase(constr);
}
*/


// System
System::System()
: clist(0),
  c2p(), p2c(),
  subsystems(0),
  reference(),
  init(false)
{
}

System::System(std::vector<Constraint *> clist_)
: c2p(), p2c(),
  subsystems(0),
  reference(),
  init(false)
{
    // create own (shallow) copy of constraints
    for (std::vector<Constraint *>::iterator constr=clist_.begin();
         constr != clist_.end(); ++constr) {
        Constraint *newconstr;
        switch ((*constr)->getTypeId()) {
            case Equal: {
                ConstraintEqual *oldconstr = static_cast<ConstraintEqual *>(*constr);
                newconstr = new ConstraintEqual(*oldconstr);
                break;
            }
            case P2PDistance: {
                ConstraintP2PDistance *oldconstr = static_cast<ConstraintP2PDistance *>(*constr);
                newconstr = new ConstraintP2PDistance(*oldconstr);
                break;
            }
            case P2PAngle: {
                ConstraintP2PAngle *oldconstr = static_cast<ConstraintP2PAngle *>(*constr);
                newconstr = new ConstraintP2PAngle(*oldconstr);
                break;
            }
            case P2LDistance: {
                ConstraintP2LDistance *oldconstr = static_cast<ConstraintP2LDistance *>(*constr);
                newconstr = new ConstraintP2LDistance(*oldconstr);
                break;
            }
            case PointOnLine: {
                ConstraintPointOnLine *oldconstr = static_cast<ConstraintPointOnLine *>(*constr);
                newconstr = new ConstraintPointOnLine(*oldconstr);
                break;
            }
            case Parallel: {
                ConstraintParallel *oldconstr = static_cast<ConstraintParallel *>(*constr);
                newconstr = new ConstraintParallel(*oldconstr);
                break;
            }
            case Perpendicular: {
                ConstraintPerpendicular *oldconstr = static_cast<ConstraintPerpendicular *>(*constr);
                newconstr = new ConstraintPerpendicular(*oldconstr);
                break;
            }
            case None:
                break;
        }
        if (newconstr)
            addConstraint(newconstr);
    }
}

System::~System()
{
    clear();
}

void System::clear()
{
    clearReference();
    clearSubSystems();
    free(clist);
    c2p.clear();
    p2c.clear();
}

void System::clearLevel(int level)
{
    std::vector<Constraint *> constrvec;
    for (std::vector<Constraint *>::const_iterator
         constr=clist.begin(); constr != clist.end(); ++constr) {
        if ((*constr)->getPriority() == level)
            constrvec.push_back(*constr);
    }
    for (std::vector<Constraint *>::const_iterator
         constr=constrvec.begin(); constr != constrvec.end(); ++constr) {
        removeConstraint(*constr);
    }
}

int System::addConstraint(Constraint *constr)
{
    clearReference();

    clist.push_back(constr);
    VEC_pD constr_params = constr->params();
    for (VEC_pD::const_iterator param=constr_params.begin();
         param != constr_params.end(); ++param) {
//        jacobi.set(constr, *param, 0.);
        c2p[constr].push_back(*param);
        p2c[*param].push_back(constr);
    }
    return clist.size()-1;
}

void System::removeConstraint(Constraint *constr)
{
    clearReference();
    clearSubSystems();

    std::vector<Constraint *>::iterator it;
    it = std::find(clist.begin(), clist.end(), constr);
    clist.erase(it);

    VEC_pD constr_params = c2p[constr];
    for (VEC_pD::const_iterator param=constr_params.begin();
         param != constr_params.end(); ++param) {
        std::vector<Constraint *> &constraints = p2c[*param];
        it = std::find(constraints.begin(), constraints.end(), constr);
        constraints.erase(it);
    }
    c2p.erase(constr);

    std::vector<Constraint *> constrvec;
    constrvec.push_back(constr);
    free(constrvec);
}

int System::addConstraintEqual(double *param1, double *param2, int level)
{
    Constraint *constr = new ConstraintEqual(param1, param2);
    constr->setPriority(level);
    return addConstraint(constr);
}

int System::addConstraintP2PDistance(Point &p1, Point &p2, double *distance, int level)
{
    Constraint *constr = new ConstraintP2PDistance(p1, p2, distance);
    constr->setPriority(level);
    return addConstraint(constr);
}

int System::addConstraintP2PAngle(Point &p1, Point &p2, double *angle,
                                  double incr_angle, int level)
{
    Constraint *constr = new ConstraintP2PAngle(p1, p2, angle, incr_angle);
    constr->setPriority(level);
    return addConstraint(constr);
}

int System::addConstraintP2PAngle(Point &p1, Point &p2, double *angle, int level)
{
    return addConstraintP2PAngle(p1, p2, angle, 0.);
}

int System::addConstraintP2LDistance(Point &p, Line &l, double *distance, int level)
{
    Constraint *constr = new ConstraintP2LDistance(p, l, distance);
    constr->setPriority(level);
    return addConstraint(constr);
}

int System::addConstraintPointOnLine(Point &p, Line &l, int level)
{
    Constraint *constr = new ConstraintPointOnLine(p, l);
    constr->setPriority(level);
    return addConstraint(constr);
}

int System::addConstraintParallel(Line &l1, Line &l2, int level)
{
    Constraint *constr = new ConstraintParallel(l1, l2);
    constr->setPriority(level);
    return addConstraint(constr);
}

int System::addConstraintPerpendicular(Line &l1, Line &l2, int level)
{
    Constraint *constr = new ConstraintPerpendicular(l1, l2);
    constr->setPriority(level);
    return addConstraint(constr);
}

int System::addConstraintP2PCoincident(Point &p1, Point &p2, int level)
{
           addConstraintEqual(p1.x, p2.x, level);
    return addConstraintEqual(p1.y, p2.y, level);
}

int System::addConstraintHorizontal(Line &l, int level)
{
    return addConstraintEqual(l.p1.y, l.p2.y, level);
}

int System::addConstraintHorizontal(Point &p1, Point &p2, int level)
{
    return addConstraintEqual(p1.y, p2.y, level);
}

int System::addConstraintVertical(Line &l, int level)
{
    return addConstraintEqual(l.p1.x, l.p2.x, level);
}

int System::addConstraintVertical(Point &p1, Point &p2, int level)
{
    return addConstraintEqual(p1.x, p2.x, level);
}

int System::addConstraintCoordinateX(Point &p, double *x, int level)
{
    return addConstraintEqual(p.x, x, level);
}

int System::addConstraintCoordinateY(Point &p, double *y, int level)
{
    return addConstraintEqual(p.y, y, level);
}

int System::addConstraintArcRules(Arc &a, int level)
{
           addConstraintP2PAngle(a.center, a.start, a.startAngle, level);
           addConstraintP2PAngle(a.center, a.end, a.endAngle, level);
           addConstraintP2PDistance(a.center, a.start, a.rad, level);
    return addConstraintP2PDistance(a.center, a.end, a.rad, level);
}

int System::addConstraintPointOnCircle(Point &p, Circle &c, int level)
{
    return addConstraintP2PDistance(p, c.center, c.rad, level);
}

int System::addConstraintPointOnArc(Point &p, Arc &a, int level)
{
    return addConstraintP2PDistance(p, a.center, a.rad, level);
}

int System::addConstraintTangent(Line &l, Circle &c, int level)
{
    return addConstraintP2LDistance(c.center, l, c.rad, level);
}

int System::addConstraintTangent(Line &l, Arc &a, int level)
{
    return addConstraintP2LDistance(a.center, l, a.rad, level);
}

int System::addConstraintLine2Arc(Point &p1, Point &p2, Arc &a, int level)
{
    addConstraintP2PCoincident(p2, a.start, level);
    double incr_angle = *(a.startAngle) < *(a.endAngle) ? M_PI/2 : -M_PI/2;
    return addConstraintP2PAngle(p1, p2, a.startAngle, incr_angle, level);
}

int System::addConstraintArc2Line(Arc &a, Point &p1, Point &p2, int level)
{
    addConstraintP2PCoincident(p1, a.end, level);
    double incr_angle = *(a.startAngle) < *(a.endAngle) ? M_PI/2 : -M_PI/2;
    return addConstraintP2PAngle(p1, p2, a.endAngle, incr_angle, level);
}

void System::initSolution(VEC_pD &params)
{
    // - Stores the current parameters in the vector "reference"
    // - Identifies the equality constraints with priority level >= 0
    //   and prepares a corresponding system reduction
    // - Organizes the rest of constraints into two subsystems for
    //   priority levels >=0 and < 0 respectively and applies the
    //   system reduction specified in the previous step
    // - Analyses the constrainess grad of the system and provides
    //   feedback

    clearReference();
    for (VEC_pD::const_iterator param=params.begin();
         param != params.end(); ++param)
        reference[*param] = **param;

    // identification of equality constraints and parameter reduction
    std::set<Constraint *> eliminated;  // constraints that will be eliminated through reduction
    reductionmap.clear();
    {
        VEC_pD reduced_params=params;
        MAP_pD_I params_index;
        for (int i=0; i < int(params.size()); ++i)
            params_index[params[i]] = i;

        for (std::vector<Constraint *>::const_iterator constr=clist.begin();
            constr != clist.end(); ++constr) {
            if ((*constr)->getPriority() >= 0 && (*constr)->getTypeId() == Equal) {
                MAP_pD_I::const_iterator it1,it2;
                it1 = params_index.find((*constr)->params()[0]);
                it2 = params_index.find((*constr)->params()[1]);
                if (it1 != params_index.end() && it2 != params_index.end()) {
                    eliminated.insert(*constr);
                    double *p_kept = reduced_params[it1->second];
                    double *p_replaced = reduced_params[it2->second];
                    for (int i=0; i < int(params.size()); ++i)
                       if (reduced_params[i] == p_replaced)
                           reduced_params[i] = p_kept;
                }
            }
        }
        for (int i=0; i < int(params.size()); ++i)
            if (params[i] != reduced_params[i])
                reductionmap[params[i]] = reduced_params[i];
    }
 
    int i=0;
    std::vector<Constraint *> clist0, clist1;
    for (std::vector<Constraint *>::const_iterator constr=clist.begin();
         constr != clist.end(); ++constr, i++) {
        if (eliminated.count(*constr) == 0) {
            if ((*constr)->getPriority() >= 0)
                clist0.push_back(*constr);
            else
                clist1.push_back(*constr);
        }
    }

    clearSubSystems();
    if (clist0.size() > 0)
        subsystems.push_back(new SubSystem(clist0, params, reductionmap));
    if (clist1.size() > 0)
        subsystems.push_back(new SubSystem(clist1, params, reductionmap));

    init = true;
}

void System::clearReference()
{
    init = false;
    reference.clear();
}

void System::resetToReference()
{
    for (MAP_pD_D::const_iterator it=reference.begin();
         it != reference.end(); ++it)
        *(it->first) = it->second;
}

int System::solve(VEC_pD &params, int isFine)
{
    initSolution(params);
    return solve(isFine);
}

int System::solve(int isFine)
{
    if (subsystems.size() == 0)
        return 0;
    else if (subsystems.size() == 1) {
        resetToReference();
        return solve(subsystems[0], isFine);
    }
    else {
        resetToReference();
        solve(subsystems[0], isFine);
        return solve(subsystems[0], subsystems[1], isFine);
    }
}

int System::solve(SubSystem *subsys, int isFine)
{
    int xsize = subsys->pSize();
    if (xsize == 0)
        return Success;

    subsys->redirectParams();

    Eigen::MatrixXd D = Eigen::MatrixXd::Identity(xsize, xsize);
    Eigen::VectorXd x(xsize);
    Eigen::VectorXd xdir(xsize);
    Eigen::VectorXd grad(xsize);
    Eigen::VectorXd h(xsize);
    Eigen::VectorXd y(xsize);
    Eigen::VectorXd Dy(xsize);

    // Initial unknowns vector and initial gradient vector
    subsys->getParams(x);
    subsys->calcGrad(grad);

    // Initial search direction oposed to gradient (steepest-descent)
    xdir = -grad;
    lineSearch(subsys, xdir);
    double err = subsys->error();

    h = x;
    subsys->getParams(x);
    h = x - h; // = x - xold

    double convergence = (isFine > 0) ? XconvergenceFine : XconvergenceRough;
    int maxIterNumber = MaxIterations * xsize;

    for (int iter=1; iter < maxIterNumber; iter++) {

        if (h.norm() <= convergence || err <= smallF)
            break;

        y = grad;
        subsys->calcGrad(grad);
        y = grad - y; // = grad - gradold

        double hty = h.dot(y);
        //make sure that hty is never 0
        if (hty == 0)
            hty = .0000000001;

        Dy = D * y;

        double ytDy = y.dot(Dy);

        //Now calculate the BFGS update on D
        D += (1.+ytDy/hty)/hty * h * h.transpose();
        D -= 1./hty * (h * Dy.transpose() + Dy * h.transpose());

        xdir = -D * grad;
        lineSearch(subsys, xdir);
        err = subsys->error();

        h = x;
        subsys->getParams(x);
        h = x - h; // = x - xold
    }

    subsys->revertParams();

    if (err <= smallF)
        return Success;
    if (h.norm() <= convergence)
        return Converged;
    return Failed;
}

// The following solver variant solves a system compound of two subsystems
// treating the first of them as of higher priority than the second
int System::solve(SubSystem *subsysA, SubSystem *subsysB, int isFine)
{
    int xsizeA = subsysA->pSize();
    int xsizeB = subsysB->pSize();
    int csizeA = subsysA->cSize();

    VEC_pD plist(xsizeA+xsizeB);
    {
        VEC_pD plistA, plistB;
        subsysA->getParamList(plistA);
        subsysB->getParamList(plistB);

        std::sort(plistA.begin(),plistA.end());
        std::sort(plistB.begin(),plistB.end());

        VEC_pD::const_iterator it;
        it = std::set_union(plistA.begin(),plistA.end(),
                            plistB.begin(),plistB.end(),plist.begin());
        plist.resize(it-plist.begin());
    }
    int xsize = plist.size();

    Eigen::MatrixXd B = Eigen::MatrixXd::Identity(xsize, xsize);
    Eigen::MatrixXd JA(csizeA, xsize);
    Eigen::MatrixXd Y,Z;

    Eigen::VectorXd resA(csizeA);
    Eigen::VectorXd lambda(csizeA), lambda0(csizeA), lambdadir(csizeA);
    Eigen::VectorXd x(xsize), x0(xsize), xdir(xsize), xdir1(xsize);
    Eigen::VectorXd grad(xsize);
    Eigen::VectorXd h(xsize);
    Eigen::VectorXd y(xsize);
    Eigen::VectorXd Bh(xsize);

    // We assume that there are no common constraints in subsysA and subsysB
    subsysA->redirectParams();
    subsysB->redirectParams();

    subsysB->getParams(plist,x);
    subsysA->getParams(plist,x);
    subsysB->setParams(plist,x);  // just to ensure that A and B are synchronized

    subsysB->calcGrad(plist,grad);
    subsysA->calcJacobi(plist,JA);
    subsysA->calcResidual(resA);

    double convergence = (isFine > 0) ? XconvergenceFine : XconvergenceRough;
    int maxIterNumber = MaxIterations * xsize;

    double mu = 0;
    lambda.setZero();
    for (int iter=1; iter < maxIterNumber; iter++) {
        int status = qp_eq(B, grad, JA, resA, xdir, Y, Z);
        if (status)
            break;

        x0 = x;
        lambda0 = lambda;
        lambda = Y.transpose() * (B * xdir + grad);
        lambdadir = lambda - lambda0;

        // line search
        {
            double eta=0.25;
            double tau=0.5;
            double rho=0.5;
            double alpha=1;
            alpha = std::min(alpha, subsysA->maxStep(plist,xdir));

            // Eq. 18.32
            // double mu = lambda.lpNorm<Eigen::Infinity>() + 0.01;
            // Eq. 18.33
            // double mu =  grad.dot(xdir) / ( (1.-rho) * resA.lpNorm<1>());
            // Eq. 18.36
            mu =  std::max(mu,
                           (grad.dot(xdir) +  std::max(0., 0.5*xdir.dot(B*xdir))) /
                           ( (1. - rho) * resA.lpNorm<1>() ) );

            // Eq. 18.27
            double f0 = subsysB->error() + mu * resA.lpNorm<1>();

            // Eq. 18.29
            double deriv = grad.dot(xdir) - mu * resA.lpNorm<1>();

            x = x0 + alpha * xdir;
            subsysA->setParams(plist,x);
            subsysB->setParams(plist,x);
            subsysA->calcResidual(resA);
            double f = subsysB->error() + mu * resA.lpNorm<1>();

            // line search, Eq. 18.28
            bool first = true;
            while (f > f0 + eta * alpha * deriv) {
                if (first) { // try a second order step
//                    xdir1 = JA.jacobiSvd(Eigen::ComputeThinU |
//                                         Eigen::ComputeThinV).solve(-resA);
                    xdir1 = -Y*resA;
                    x += xdir1; // = x0 + alpha * xdir + xdir1
                    subsysA->setParams(plist,x);
                    subsysB->setParams(plist,x);
                    subsysA->calcResidual(resA);
                    f = subsysB->error() + mu * resA.lpNorm<1>();
                    if (f < f0 + eta * alpha * deriv)
                        break;
                }
                alpha = tau * alpha;
                x = x0 + alpha * xdir;
                subsysA->setParams(plist,x);
                subsysB->setParams(plist,x);
                subsysA->calcResidual(resA);
                f = subsysB->error() + mu * resA.lpNorm<1>();
            }
            lambda = lambda0 + alpha * lambdadir;
 
        }
        h = x - x0;

        y = grad - JA.transpose() * lambda;
        {
            subsysB->calcGrad(plist,grad);
            subsysA->calcJacobi(plist,JA);
            subsysA->calcResidual(resA);
        }
        y = grad - JA.transpose() * lambda - y; // Eq. 18.13

        if (iter > 1) {
            double yTh = y.dot(h);
            if (yTh != 0) {
                Bh = B * h;
                //Now calculate the BFGS update on B
                B += 1./yTh * y * y.transpose();
                B -= 1./h.dot(Bh) * (Bh * Bh.transpose());
            }
        }

        if (h.norm() <= convergence && subsysA->error() <= smallF)
            break;
    }

    int ret;
    if (subsysA->error() <= smallF)
        ret = Success;
    else if (h.norm() <= convergence)
        ret = Converged;
    else
        ret = Failed;

    subsysA->revertParams();
    subsysB->revertParams();
    return ret;

}

void System::getSubSystems(std::vector<SubSystem *> &subsysvec)
{
    subsysvec = subsystems;
}

void System::applySolution()
{
    if (subsystems.size() > 1)
        subsystems[1]->applySolution();
    if (subsystems.size() > 0)
        subsystems[0]->applySolution();

    for (MAP_pD_pD::const_iterator it=reductionmap.begin();
         it != reductionmap.end(); ++it)
        *(it->first) = *(it->second);

}

void System::clearSubSystems()
{
    init = false;
    free(subsystems);
}





double lineSearch(SubSystem *subsys, Eigen::VectorXd &xdir)
{
    double f1,f2,f3,alpha1,alpha2,alpha3,alphaStar;

    Eigen::VectorXd x0, x;

    //Save initial values
    subsys->getParams(x0);

    //Start at the initial position alpha1 = 0
    alpha1 = 0.;
    f1 = subsys->error();

    //Take a step of alpha2 = 1
    alpha2 = 1.;
    x = x0 + alpha2 * xdir;
    subsys->setParams(x);
    f2 = subsys->error();

    //Take a step of alpha3 = 2*alpha2
    alpha3 = alpha2*2;
    x = x0 + alpha3 * xdir;
    subsys->setParams(x);
    f3 = subsys->error();

    //Now reduce or lengthen alpha2 and alpha3 until the minimum is
    //Bracketed by the triplet f1>f2<f3
    while (f2 > f1 || f2 > f3) {
        if (f2 > f1) {
            //If f2 is greater than f1 then we shorten alpha2 and alpha3 closer to f1
            //Effectively both are shortened by a factor of two.
            alpha3 = alpha2;
            f3 = f2;
            alpha2 = alpha2 / 2;
            x = x0 + alpha2 * xdir;
            subsys->setParams(x);
            f2 = subsys->error();
        }
        else if (f2 > f3) {
            //If f2 is greater than f3 then we increase alpha2 and alpha3 away from f1
            //Effectively both are lengthened by a factor of two.
            alpha2 = alpha3;
            f2 = f3;
            alpha3 = alpha3 * 2;
            x = x0 + alpha3 * xdir;
            subsys->setParams(x);
            f3 = subsys->error();
        }
    }
    //Get the alpha for the minimum f of the quadratic approximation
    alphaStar = alpha2 + ((alpha2-alpha1)*(f1-f3))/(3*(f1-2*f2+f3));

    //Guarantee that the new alphaStar is within the bracket
    if (alphaStar >= alpha3 || alphaStar <= alpha1)
        alphaStar = alpha2;

    if (alphaStar != alphaStar)
        alphaStar = 0.;

    //Take a final step to alphaStar
    x = x0 + alphaStar * xdir;
    subsys->setParams(x);

    return alphaStar;
}


void free(VEC_pD &doublevec)
{
    for (VEC_pD::iterator it = doublevec.begin();
         it != doublevec.end(); ++it)
        if (*it) delete *it;
    doublevec.clear();
}

void free(std::vector<Constraint *> &constrvec)
{
    for (std::vector<Constraint *>::iterator constr=constrvec.begin();
         constr != constrvec.end(); ++constr) {
        if (*constr) {
            switch ((*constr)->getTypeId()) {
                case Equal:
                    delete static_cast<ConstraintEqual *>(*constr);
                    break;
                case P2PDistance:
                    delete static_cast<ConstraintP2PDistance *>(*constr);
                    break;
                case P2PAngle:
                    delete static_cast<ConstraintP2PAngle *>(*constr);
                    break;
                case P2LDistance:
                    delete static_cast<ConstraintP2LDistance *>(*constr);
                    break;
                case PointOnLine:
                    delete static_cast<ConstraintPointOnLine *>(*constr);
                    break;
                case Parallel:
                    delete static_cast<ConstraintParallel *>(*constr);
                    break;
                case Perpendicular:
                    delete static_cast<ConstraintPerpendicular *>(*constr);
                    break;
                case None:
                default:
                    delete *constr;
            }
        }
    }
    constrvec.clear();
}

void free(std::vector<SubSystem *> &subsysvec)
{
    for (std::vector<SubSystem *>::iterator it=subsysvec.begin();
         it != subsysvec.end(); ++it)
        if (*it) delete *it;
    subsysvec.clear();
}


} //namespace GCS
