/***************************************************************************
 *   Copyright (c) 2005 Imetric 3D GmbH                                    *
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
# include <set>
# include <vector>
#endif

#include <Base/Sequencer.h>

#include "Builder.h"
#include "MeshKernel.h"

using namespace MeshCore;


MeshBuilder::MeshBuilder (MeshKernel& kernel) : _meshKernel(kernel)
{
}

MeshBuilder::~MeshBuilder (void)
{
}

void MeshBuilder::Initialize (long ctFacets, bool deletion)
{
  if (deletion)
  {
	  _meshKernel._aclFacetArray.clear();
	  _meshKernel._aclPointArray.clear();
	  _ptIdx = 0;
  }
  else
  {
    for (MeshPointArray::_TConstIterator it1 = _meshKernel._aclPointArray.begin(); it1 != _meshKernel._aclPointArray.end(); it1++)
    {
      _points.insert(*it1);
    }
    _ptIdx = _meshKernel._aclPointArray.size();
  }

	// Base::SequencerLauncher seq("create mesh structure...", ctFacets);
	Base::Sequencer().start("create mesh structure...", ctFacets * 2);
}

void MeshBuilder::AddFacet (const MeshGeomFacet& facet)
{
	AddFacet(facet._aclPoints[0], facet._aclPoints[1], facet._aclPoints[2], facet.GetNormal());
}

void MeshBuilder::AddFacet (const Vector3D& pt1, const Vector3D& pt2, const Vector3D& pt3, const Vector3D& normal)
{
	Vector3D facetPoints[4] = { pt1, pt2, pt3, normal };
	AddFacet(facetPoints);
}


void MeshBuilder::AddFacet (Vector3D* facetPoints)
{
	Base::Sequencer().next(true); // allow to cancel

	// adjust circulation direction
  if ((((facetPoints[1] - facetPoints[0]) % (facetPoints[2] - facetPoints[0])) * facetPoints[3]) < 0.0f)
	{		
		std::swap(facetPoints[1], facetPoints[2]);
	}


	MeshFacet mf;
  int i = 0;
	for (i = 0; i < 3; i++)
	{
		MeshPoint pt(facetPoints[i]);			
		std::set<MeshPoint>::iterator p = _points.find(pt);
		if (p == _points.end())
		{
			mf._aulPoints[i] = _ptIdx;
			_meshKernel._aclPointArray.push_back(pt);
      pt._ulProp = _ptIdx++;
			_points.insert(pt);			
		}
		else
			mf._aulPoints[i] = p->_ulProp;
	}		

  // check for degenerated facet (one edge has length 0)
  if ((mf._aulPoints[0] == mf._aulPoints[1]) || (mf._aulPoints[0] == mf._aulPoints[2]) || (mf._aulPoints[1] == mf._aulPoints[2]))
    return;

  _meshKernel._aclFacetArray.push_back(mf);
}

void MeshBuilder::SetNeighbourhood ()
{
  std::set<Edge> edges;

  int facetIdx = 0;

  for (MeshFacetArray::_TIterator it = _meshKernel._aclFacetArray.begin(); it != _meshKernel._aclFacetArray.end(); it++)
  {
  	Base::Sequencer().next(true); // allow to cancel

    MeshFacet& mf = *it;

	  for (int i = 0; i < 3; i++)
	  {
		  Edge edge(mf._aulPoints[i], mf._aulPoints[(i+1)%3], facetIdx);

		  std::set<Edge>::iterator e = edges.find(edge);
		  if (e != edges.end())
		  { // edge exists, set neighbourhood
        MeshFacet& mf1 = _meshKernel._aclFacetArray[e->facetIdx];

        if (mf1._aulPoints[0] == edge.pt1)
        {
          if (mf1._aulPoints[1] == edge.pt2)
            mf1._aulNeighbours[0] = facetIdx;
          else
            mf1._aulNeighbours[2] = facetIdx;
        }
        else if (mf1._aulPoints[0] == edge.pt2)
        {
          if (mf1._aulPoints[1] == edge.pt1)
            mf1._aulNeighbours[0] = facetIdx;
          else
            mf1._aulNeighbours[2] = facetIdx;
        }
        else
          mf1._aulNeighbours[1] = facetIdx;

        mf._aulNeighbours[i] = e->facetIdx;
		  }
      else
		  {  // new edge
			  edges.insert(edge);
		  }
	  }

    facetIdx++;
  }
}

void MeshBuilder::Finish ()
{
  _points.clear();
  _points.swap(_points);

  SetNeighbourhood();

  // release some memory
  _meshKernel._aclFacetArray.swap(_meshKernel._aclFacetArray);
  _meshKernel._aclPointArray.swap(_meshKernel._aclPointArray);
  _meshKernel.RecalcBoundBox();

	Base::Sequencer().stop();
}
