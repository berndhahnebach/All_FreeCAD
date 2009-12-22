/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <sstream>
#endif


#include <strstream>
#include <Base/Console.h>
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Stream.h>

#include "FemMeshProperty.h"
#include "FemMeshPy.h"

using namespace Fem;

TYPESYSTEM_SOURCE(Fem::PropertyFemMesh , App::Property);

PropertyFemMesh::PropertyFemMesh()
{
}

PropertyFemMesh::~PropertyFemMesh()
{
}

void PropertyFemMesh::setValue(const FemMesh& sh)
{
    aboutToSetValue();
    _FemMesh = sh;
    hasSetValue();
}


const FemMesh &PropertyFemMesh::getValue(void)const 
{
    return _FemMesh;
}


Base::BoundBox3d PropertyFemMesh::getBoundingBox() const
{
    return _FemMesh.getBoundBox();
}


PyObject *PropertyFemMesh::getPyObject(void)
{
    return new FemMeshPy(new FemMesh(_FemMesh));
}

void PropertyFemMesh::setPyObject(PyObject *value)
{
    if (PyObject_TypeCheck(value, &(FemMeshPy::Type))) {
        FemMeshPy *pcObject = static_cast<FemMeshPy*>(value);
        setValue(*pcObject->getFemMeshPtr());
    }
    else {
        std::string error = std::string("type must be 'FemMesh', not ");
        error += value->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

App::Property *PropertyFemMesh::Copy(void) const
{
    PropertyFemMesh *prop = new PropertyFemMesh();
    prop->_FemMesh = this->_FemMesh;
 
    return prop;
}

void PropertyFemMesh::Paste(const App::Property &from)
{
    aboutToSetValue();
    _FemMesh = dynamic_cast<const PropertyFemMesh&>(from)._FemMesh;
    hasSetValue();
}

unsigned int PropertyFemMesh::getMemSize (void) const
{
    return _FemMesh.getMemSize();
}

void PropertyFemMesh::Save (Base::Writer &writer) const
{
    _FemMesh.Save(writer);
}

void PropertyFemMesh::Restore(Base::XMLReader &reader)
{
    Fem::FemMesh temp;
    temp.Restore(reader);
    setValue(temp);
}



