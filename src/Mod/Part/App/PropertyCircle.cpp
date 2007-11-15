/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Exception.h>

#include "PropertyCircle.h"
#include "CirclePy.h"

using namespace Part;

TYPESYSTEM_SOURCE(Part::PropertyCircle , App::Property);

PropertyCircle::PropertyCircle()
{
}

PropertyCircle::~PropertyCircle()
{
}

void PropertyCircle::setValue(const gp_Circ& circ)
{
    aboutToSetValue();
    _circle = circ;
    hasSetValue();
}

const gp_Circ& PropertyCircle::getValue(void) const 
{
    return _circle;
}

PyObject *PropertyCircle::getPyObject(void)
{
    return new CirclePy(this->_circle);
}

void PropertyCircle::setPyObject(PyObject *value)
{
    if (PyObject_TypeCheck(value, &(CirclePy::Type))) {
        CirclePy  *pcObject = (CirclePy*)value;
        setValue( pcObject->value() );
    } 
    else {
        std::string error = std::string("type must be 'Circle', not ");
        error += value->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

App::Property *PropertyCircle::Copy(void) const
{
    PropertyCircle* p = new PropertyCircle();
    p->_circle = this->_circle;
    return p;
}

void PropertyCircle::Paste(const App::Property &from)
{
    aboutToSetValue();
    _circle = dynamic_cast<const PropertyCircle&>(from)._circle;
    hasSetValue();
}

unsigned int PropertyCircle::getMemSize (void) const
{
    return sizeof(gp_Circ);
}

void PropertyCircle::Save (Base::Writer &writer) const
{
    gp_Ax1 axis = _circle.Axis();
    gp_Dir dir = axis.Direction();
    gp_Pnt loc = axis.Location();
    Standard_Real fRad = _circle.Radius();
    writer.Stream() << writer.ind() << "<PropertyCircle Radius=\"" <<  fRad << "\" PosX=\"" <<  loc.X() << "\" PosY=\"" <<  loc.Y() << "\" PosZ=\"" <<  loc.Z() 
                    << writer.ind() << "\" DirX=\"" <<  dir.X() << "\" DirY=\"" <<  dir.Y() << "\" DirZ=\"" <<  dir.Z() <<"\"/>" << std::endl;
}

void PropertyCircle::Restore(Base::XMLReader &reader)
{
    // read my element
    reader.readElement("PropertyCircle");

    // get the value of my Attribute
    Standard_Real fR = reader.getAttributeAsFloat("Radius");
    Standard_Real pX = reader.getAttributeAsFloat("PosX");
    Standard_Real pY = reader.getAttributeAsFloat("PosY");
    Standard_Real pZ = reader.getAttributeAsFloat("PosZ");
    Standard_Real dX = reader.getAttributeAsFloat("DirX");
    Standard_Real dY = reader.getAttributeAsFloat("DirY");
    Standard_Real dZ = reader.getAttributeAsFloat("DirZ");

    gp_Dir dir(dX, dY, dZ);
    gp_Pnt loc(pX, pY, pZ);
    gp_Ax1 axis(loc, dir);
    aboutToSetValue();
    _circle.SetAxis(axis);
    _circle.SetRadius(fR);
    hasSetValue();
}
