/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <BRepBuilderAPI_MakeEdge.hxx>
# include <gp_Circ.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Edge.hxx>
#endif

#include "FeaturePartCircle.h"

using namespace Part;

PROPERTY_SOURCE(Part::Circle, Part::Primitive)


Circle::Circle()
{
    ADD_PROPERTY(Radius,(2.0f));
    ADD_PROPERTY(Angle0,(0.0f));
    ADD_PROPERTY(Angle1,(2*F_PI));
    gp_Circ c;c.SetRadius(1.0);
    ADD_PROPERTY(Circ,(c));
}

Circle::~Circle()
{
}

short Circle::mustExecute() const
{
    if (Angle0.isTouched() ||
        Angle1.isTouched() ||
        Radius.isTouched())
        return 1;
    if (Circ.isTouched())
        return 1;
    return Part::Feature::mustExecute();
}

App::DocumentObjectExecReturn *Circle::execute(void)
{
    Base::Vector3f loc = this->Location.getValue();
    Base::Vector3f dir = this->Axis.getValue();

    gp_Ax1 axis;
    axis.SetLocation(gp_Pnt(loc.x, loc.y, loc.z));
    axis.SetDirection(gp_Dir(dir.x, dir.y, dir.z));
    gp_Circ circle;
    circle.SetAxis(axis);
    circle.SetRadius(this->Radius.getValue());
    
    BRepBuilderAPI_MakeEdge clMakeEdge(circle, this->Angle0.getValue(), this->Angle1.getValue());
    TopoDS_Edge edge = TopoDS::Edge(clMakeEdge.Shape());
    this->Shape.setValue(edge);

    return App::DocumentObject::StdReturn;
}

void Circle::onChanged(const App::Property* prop)
{
    if (prop == &Circ) {
        this->Radius.setValue((float)Circ.getValue().Radius());
        gp_Pnt loc = Circ.getValue().Location();
        this->Location.setValue((float)loc.X(),(float)loc.Y(),(float)loc.Z());
        gp_Ax1 axis = Circ.getValue().Axis();
        gp_Dir dir = axis.Direction();
        this->Axis.setValue((float)dir.X(),(float)dir.Y(),(float)dir.Z());
    }

    Part::Feature::onChanged(prop);
}

#if 1
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Exception.h>

#include "CirclePy.h"

using namespace Part;

TYPESYSTEM_SOURCE(Part::PropertyCircle , App::Property);

PropertyCircle::PropertyCircle()
  : _circle(gp_Circ())
{
}

PropertyCircle::~PropertyCircle()
{
}

void PropertyCircle::setValue(const Geom_Circle& circ)
{
    aboutToSetValue();
    _circle = circ;
    hasSetValue();
}

const Geom_Circle& PropertyCircle::getValue(void) const 
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
        CirclePy  *pcObject = static_cast<CirclePy*>(value);
        Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast
            (pcObject->getGeomCirclePtr()->handle());
        setValue(Geom_Circle(circle->Circ()));
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
#endif
