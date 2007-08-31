/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>                              *
 *	 Human Rezai <human.rezai@eads.net>                                    * 
 *   Mohamad Najib Muhammad Noor <najib_bean@yahoo.co.uk>                  *
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
/**************APPROX.H*********************
*Class Approximate, inheriting from Routines
*Dependancies:- BOOST, ATLAS, UMFPACK, BLAS
*				LAPACK
********************************************/

#ifndef APPROX_H
#define APPROX_H

#ifndef NDEBUG //This is for faster matrix operations. In fact, some checks are turned off in the uBlas functions
#define NDEBUG   
#endif
/*******MAIN INCLUDE*********/
#include "routine.h"

/*******BOOST********/
#include <boost/numeric/ublas/matrix.hpp>

/*****NAMESPACE******/
using namespace boost::numeric;

/*! \class Approximate 
	\brief The main class for the approximate routine
	
	Inheriting the Routines class defined in Routines.h,it takes a mesh structure and tolerance level as it's input parameter.
	
	As output, it gives out the following NURBS info:-
		Control Points, Knot U, Knot V, Order U, Order V
	
	where Control Points, Knot U, Knot V are of type std::vectors, Order U and Order V of type int

	In this program, it will be directly converted into a topo surface from the given information
 */
class Approximate : protected Routines
{
public:
	Approximate(const MeshCore::MeshKernel &m, std::vector<double> &_Cntrl, std::vector<double> &_KnotU, std::vector<double> &_KnotV,
		 int &_OrderU, int &_OrderV, double tol);

protected:
	void ParameterBoundary();
	void ParameterInnerPoints();
	void ErrorApprox();
	void ApproxMain();
	void Reparam();
	void eFair2(ublas::compressed_matrix<double> &E_Matrix);
	void ComputeError(int &h, double eps_1, double eps_2, double &max_error, 
		 double &av, double &c2, std::vector <double> &err_w);
	void ExtendNurb(double c2, int h);
	void ReorderNeighbourList(std::set<MeshCore::MeshPointArray::_TConstIterator> &pnt, 
		std::set<MeshCore::MeshFacetArray::_TConstIterator> &face, std::vector<unsigned long> &nei,unsigned long CurInd);
	//void RemakeList(std::vector<MyMesh::VertexHandle> &v_neighbour);
	
private:
	MeshCore::MeshKernel LocalMesh;  //Local Mesh 
	MeshCore::MeshKernel ParameterMesh;   //Parameterized Mesh - ONLY USED FOR VISUALIZING TO CHECK FOR PARAMETERIZATION ERRORS
	int NumOfPoints;    //Info about the Mesh
	int NumOfInnerPoints;
	int NumOfOuterPoints;
	double tolerance;  //error level
	ublas::vector<double> ParameterX;   //Parameterized Coordinate Lists
	ublas::vector<double> ParameterY;
	ublas::vector<double> BoundariesX;  //Parametrized Boundaries' Coordinate List
	ublas::vector<double> BoundariesY;
	std::vector<double> UnparamBoundariesX;   //Original Boundaries' Coordinate List
	std::vector<double> UnparamBoundariesY;
	std::vector<double> UnparamBoundariesZ;
	std::vector<double> UnparamX; //Original Coordinate Lists
	std::vector<double> UnparamY;
	std::vector<double> UnparamZ;
	
	std::vector<int> mapper;
	std::list< std::vector <unsigned long> > BoundariesIndex;
	std::list< std::vector <Base::Vector3f> > BoundariesPoints;

	//NURBS
	NURBS MainNurb;

	//Bounding box
	double MinX;
	double MaxX;
	double MinY;
	double MaxY;
	
};

#endif  /*APPROX_H DEFINED*/