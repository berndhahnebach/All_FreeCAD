/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2009     *
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


#include "FemMesh.h"


using namespace Fem;
using namespace Base;
//using namespace KDL;


TYPESYSTEM_SOURCE(Fem::FemMesh , Base::Persistence);

FemMesh::FemMesh()
{

}

FemMesh::FemMesh(const FemMesh& Mesh)

{
    
}

FemMesh::~FemMesh()
{

}

FemMesh &FemMesh::operator=(const FemMesh& Trac)
{
    return *this;
}



unsigned int FemMesh::getMemSize (void) const
{
	return 0;
}

void FemMesh::Save (Writer &writer) const
{
    //writer.Stream() << writer.ind() << "<FemMesh count=\"" <<  getSize() <<"\">" << std::endl;
    //writer.incInd();
    //for(unsigned int i = 0;i<getSize(); i++)
    //    vpcWaypoints[i]->Save(writer);
    //writer.decInd();
    //writer.Stream() << writer.ind() << "</FemMesh>" << std::endl ;

}

void FemMesh::Restore(XMLReader &reader)
{
    //vpcWaypoints.clear();
    //// read my element
    //reader.readElement("FemMesh");
    //// get the value of my Attribute
    //int count = reader.getAttributeAsInteger("count");
    //vpcWaypoints.resize(count);

    //for (int i = 0; i < count; i++) {
    //    Waypoint *tmp = new Waypoint();
    //    tmp->Restore(reader);
    //    vpcWaypoints[i] = tmp;
    //}
    //generateFemMesh();
}




 
