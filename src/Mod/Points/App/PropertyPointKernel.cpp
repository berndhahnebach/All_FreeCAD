/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <math.h>
# include <iostream>
#endif

#include <Base/Exception.h>
#include <Base/Matrix.h>
#include <Base/Stream.h>
#include <Base/Writer.h>

#include "PropertyPointKernel.h"
#include "PointsPy.h"

using namespace Points;

TYPESYSTEM_SOURCE(Points::PropertyPointKernel , App::PropertyComplexGeoData);

PropertyPointKernel::PropertyPointKernel()
{

}

PropertyPointKernel::~PropertyPointKernel()
{
}

void PropertyPointKernel::setValue(const PointKernel& m)
{
    aboutToSetValue();
    _cPoints = m;
    hasSetValue();
}

const PointKernel& PropertyPointKernel::getValue(void) const 
{
    return _cPoints;
}

Base::BoundBox3d PropertyPointKernel::getBoundingBox() const
{
    Base::BoundBox3d box;
    for (PointKernel::const_iterator it = _cPoints.begin(); it != _cPoints.end(); ++it)
        box.Add(*it);
    return box;
}

void PropertyPointKernel::getFaces(std::vector<Base::Vector3d> &Points,
                                   std::vector<Data::ComplexGeoData::FacetTopo> &Topo,
                                   float Accuracy, uint16_t flags) const
{
    // no 3d triangulator implemented ==> do nothing
}

PyObject *PropertyPointKernel::getPyObject(void)
{
    return new PointsPy(_cPoints);
}

void PropertyPointKernel::setPyObject(PyObject *value)
{
    if (PyObject_TypeCheck(value, &(PointsPy::Type))) {
        PointsPy  *pcObject = (PointsPy*)value;
        setValue( pcObject->getPoints());
    }
    else {
        std::string error = std::string("type must be 'Points', not ");
        error += value->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

void PropertyPointKernel::Save (Base::Writer &writer) const
{
    _cPoints.Save(writer);
}

void PropertyPointKernel::Restore(Base::XMLReader &reader)
{
    reader.readElement("Points");
    std::string file (reader.getAttribute("file") );

    if (!file.empty()) {
        // initate a file read
        reader.addFile(file.c_str(),this);
    }
    if(reader.DocumentSchema > 3)
    {
        std::string Matrix (reader.getAttribute("mtrx") );
        Base::Matrix4D mtrx;
        mtrx.fromString(Matrix);

        aboutToSetValue();
        _cPoints.setTransform(mtrx);
        hasSetValue();
    }
}

void PropertyPointKernel::RestoreDocFile(Base::Reader &reader)
{
    aboutToSetValue();
    _cPoints.RestoreDocFile(reader);
    hasSetValue();
}

App::Property *PropertyPointKernel::Copy(void) const 
{
    PropertyPointKernel* prop = new PropertyPointKernel();
    prop->_cPoints = this->_cPoints;
    return prop;
}

void PropertyPointKernel::Paste(const App::Property &from)
{
    aboutToSetValue();
    const PropertyPointKernel& prop = dynamic_cast<const PropertyPointKernel&>(from);
    this->_cPoints = prop._cPoints;
    hasSetValue();
}

unsigned int PropertyPointKernel::getMemSize (void) const
{
    return sizeof(Base::Vector3f) * this->_cPoints.size();
}

//void PropertyPointKernel::removeIndices( const std::vector<unsigned long>& uIndices )
//{
//    // We need a sorted array
//    std::vector<unsigned long> uSortedInds = uIndices;
//    std::sort(uSortedInds.begin(), uSortedInds.end());
//
//    assert( uSortedInds.size() <= _cPoints.size() );
//    if ( uSortedInds.size() > _cPoints.size() )
//        return;
//
//    std::vector<Base::Vector3f> remainValue;
//    remainValue.reserve(_cPoints.size() - uSortedInds.size());
//
//    std::vector<unsigned long>::iterator pos = uSortedInds.begin();
//    for ( std::vector<Base::Vector3f>::const_iterator it = _cPoints.begin(); it != _cPoints.end(); ++it ) {
//        unsigned long index = it - _cPoints.begin();
//        if (pos == uSortedInds.end())
//            remainValue.push_back( *it );
//        else if (index != *pos)
//            remainValue.push_back( *it );
//        else 
//        pos++;
//    }
//
//    setValue(remainValue);
//}

void PropertyPointKernel::transform(const Base::Matrix4D &rclMat)
{
    _cPoints.setTransform(rclMat);
}
