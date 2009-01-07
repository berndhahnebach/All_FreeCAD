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
#endif


#include "ComplexGeoData.h"

using namespace Data;

TYPESYSTEM_SOURCE_ABSTRACT(Data::Segment , Base::BaseClass);


TYPESYSTEM_SOURCE_ABSTRACT(Data::ComplexGeoData , Base::Persistence);


ComplexGeoData::ComplexGeoData(void)
{
}

ComplexGeoData::~ComplexGeoData(void)
{
}

Data::Segment* ComplexGeoData::getSubElementByName(const char* Name) const
{
    int i=0;
    std::string temp;
    while (Name[i] != 0)
    {
        if (Name[i]>=48 && Name[i]<=57)
            break;
        temp.push_back(Name[i]);
        i++;
    }
    int n = atoi(&(Name[i]));
    return getSubElement(temp.c_str(),n);
}

void ComplexGeoData::applyTransform(const Base::Matrix4D& rclTrf)
{
    setTransform(rclTrf * getTransform());
}

void ComplexGeoData::applyTranslation(const Base::Vector3d& mov)
{
    Base::Matrix4D mat;
    mat.move(mov);
    setTransform(mat * getTransform());
}

void ComplexGeoData::applyRotation(const Base::Rotation& rot)
{
    Base::Matrix4D mat;
    rot.getValue(mat);
    setTransform(mat * getTransform());
}

void ComplexGeoData::setPlacement(const Base::Placement& rclPlacement)
{
    setTransform(rclPlacement.toMatrix());
}

Base::Placement ComplexGeoData::getPlacement() const
{
    Base::Placement trf;
    Base::Matrix4D mat = getTransform();
    Base::Rotation rot(mat);
    trf._rot = rot;
    trf._pos.x = mat[0][3];
    trf._pos.y = mat[1][3];
    trf._pos.z = mat[2][3];
    return trf;
}
