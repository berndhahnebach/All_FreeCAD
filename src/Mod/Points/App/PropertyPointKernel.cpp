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
#include <Base/Writer.h>

#include "PropertyPointKernel.h"
#include "PointsPy.h"

using namespace Points;

TYPESYSTEM_SOURCE(Points::PropertyPointKernel , App::Property);

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
  if ( writer.isForceXML() ) {
  } else {
    writer.Stream() << writer.ind() << "<Points file=\"" << writer.addFile(getName(), this) << "\"/>" << std::endl;
  }
}

void PropertyPointKernel::Restore(Base::XMLReader &reader)
{
  reader.readElement("Points");
  std::string file (reader.getAttribute("file") );

  if ( file == "" ) {
  } else {
    // initate a file read
    reader.addFile(file.c_str(),this);
  }
}

void PropertyPointKernel::SaveDocFile (Base::Writer &writer) const
{
  unsigned long uCtPts = _cPoints.size();
  writer.Stream().write((const char*)&uCtPts, sizeof(unsigned long));
  writer.Stream().write((const char*)&(_cPoints[0]), uCtPts*sizeof(Base::Vector3f));
}

void PropertyPointKernel::RestoreDocFile(Base::Reader &reader)
{
  _cPoints.clear();

  unsigned long uCtPts;
  reader.read((char*)&uCtPts, sizeof(unsigned long));
  _cPoints.resize(uCtPts);
  reader.read((char*)&(_cPoints[0]), uCtPts*sizeof(Base::Vector3f));
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

void PropertyPointKernel::removeIndices( const std::vector<unsigned long>& uIndices )
{
  // We need a sorted array
  std::vector<unsigned long> uSortedInds = uIndices;
  std::sort(uSortedInds.begin(), uSortedInds.end());

  assert( uSortedInds.size() <= _cPoints.size() );
  if ( uSortedInds.size() > _cPoints.size() )
    return;

  std::vector<Base::Vector3f> remainValue;
  remainValue.reserve(_cPoints.size() - uSortedInds.size());

  std::vector<unsigned long>::iterator pos = uSortedInds.begin();
  for ( std::vector<Base::Vector3f>::const_iterator it = _cPoints.begin(); it != _cPoints.end(); ++it ) {
    unsigned long index = it - _cPoints.begin();
    if (pos == uSortedInds.end())
      remainValue.push_back( *it );
    else if (index != *pos)
      remainValue.push_back( *it );
    else 
      pos++;
  }

  setValue(remainValue);
}

void PropertyPointKernel::transform(const Base::Matrix4D &rclMat)
{
  aboutToSetValue();
  for (std::vector<Base::Vector3f>::iterator it = _cPoints.begin(); it != _cPoints.end(); ++it)
    *it = rclMat * (*it);
  hasSetValue();
}
