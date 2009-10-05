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
# include <BRepPrimAPI_MakeBox.hxx>
# include <Precision.hxx>
#endif


#include <Base/Console.h>
#include <Base/Reader.h>
#include "FeaturePartBox.h"


using namespace Part;


PROPERTY_SOURCE(Part::Box, Part::Primitive)


Box::Box()
{
    ADD_PROPERTY_TYPE(Length,(100.0f),"Box",App::Prop_None,"The length of the box");
    ADD_PROPERTY_TYPE(Width ,(100.0f),"Box",App::Prop_None,"The width of the box");
    ADD_PROPERTY_TYPE(Height,(100.0f),"Box",App::Prop_None,"The height of the box");
}

short Box::mustExecute() const
{
    if (Length.isTouched() ||
        Height.isTouched() ||
        Width.isTouched() )
        return 1;
    return Primitive::mustExecute();
}

App::DocumentObjectExecReturn *Box::execute(void)
{
    double L = Length.getValue();
    double W = Width.getValue();
    double H = Height.getValue();

    if (L < Precision::Confusion())
        return new App::DocumentObjectExecReturn("Length of box too small");

    if (W < Precision::Confusion())
        return new App::DocumentObjectExecReturn("Width of box too small");

    if (H < Precision::Confusion())
        return new App::DocumentObjectExecReturn("Height of box too small");

    try {
        // Build a box using the dimension and position attributes
        gp_Pnt pnt(Location.getValue().x,
                   Location.getValue().y,
                   Location.getValue().z);
        gp_Dir dir(Axis.getValue().x,
                   Axis.getValue().y,
                   Axis.getValue().z);
        BRepPrimAPI_MakeBox mkBox(gp_Ax2(pnt,dir), L, W, H);
        TopoDS_Shape ResultShape = mkBox.Shape();
        this->Shape.setValue(ResultShape);
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    }

    return App::DocumentObject::StdReturn;
}

/**
 * This method was added for backward-compatibility. In former versions
 * of Box we had the properties x,y,z and l,h,w which have changed to
 * Location -- as replacement for x,y and z and Length, Height and Width.
 */
void Box::Restore(Base::XMLReader &reader)
{
    reader.readElement("Properties");
    int Cnt = reader.getAttributeAsInteger("Count");

    bool location_xyz = false;
    App::PropertyDistance x,y,z;
    for (int i=0 ;i<Cnt;i++) {
        reader.readElement("Property");
        const char* PropName = reader.getAttribute("name");
        const char* TypeName = reader.getAttribute("type");
        App::Property* prop = getPropertyByName(PropName);
        if (!prop) {
            // in case this comes from an old document we must use the new properties
            if (strcmp(PropName, "l") == 0)
                prop = getPropertyByName("Length");
            else if (strcmp(PropName, "w") == 0)
                prop = getPropertyByName("Height"); // by mistake w was considered as height
            else if (strcmp(PropName, "h") == 0)
                prop = getPropertyByName("Width"); // by mistake h was considered as width
            else if (strcmp(PropName, "x") == 0) {
                location_xyz = true;
                prop = &x;
            }
            else if (strcmp(PropName, "y") == 0) {
                location_xyz = true;
                prop = &y;
            }
            else if (strcmp(PropName, "z") == 0) {
                location_xyz = true;
                prop = &z;
            }
        }
        // NOTE: We must also check the type of the current property because a subclass
        // of PropertyContainer might change the type of a property but not its name.
        // In this case we would force to read-in a wrong property type and the behaviour
        // would be undefined.
        if (prop && strcmp(prop->getTypeId().getName(), TypeName) == 0)
            prop->Restore(reader);

        reader.readEndElement("Property");
    }

    if (location_xyz) {
        this->Location.setValue(x.getValue(),y.getValue(),z.getValue());
    }

    reader.readEndElement("Properties");
}

void Box::onChanged(const App::Property* prop)
{
    if(prop == &Length ||prop == &Width ||prop == &Height ||prop == &Location ||prop == &Axis ){
        App::DocumentObjectExecReturn *ret = execute();
        if (ret != App::DocumentObject::StdReturn)
            delete ret;
    }
    Part::Primitive::onChanged(prop);
}
