/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Writer.h>


#include "PointsFeature.h"
#include "PointsFeaturePy.h"

using namespace Points;


//===========================================================================
// Feature
//===========================================================================

PROPERTY_SOURCE(Points::Feature, App::AbstractFeature)

Feature::Feature()
{
  // set default display mode
  _showMode = "Point";
}

Feature::~Feature()
{
}

void Feature::Save (Base::Writer &writer)
{
  // save parent
  AbstractFeature::Save(writer);

  if(writer.isForceXML())
  {
  }else{
    writer << writer.ind() << "<Points file=\"" << writer.addFile(name.getValue(), this) << "\"/>" << std::endl;
  }
}

void Feature::Restore(Base::XMLReader &reader)
{
  // load parent
  AbstractFeature::Restore(reader);
  reader.readElement("Points");
  std::string file (reader.getAttribute("file") );

  if (file == "")
  {
  }else{
    // initate a file read
    reader.addFile(file.c_str(),this);
  }
}

void Feature::SaveDocFile (Base::Writer &writer)
{
  const PointKernel& kernel = _Points.getKernel();
  unsigned long uCtPts = kernel.size();
  writer.write((const char*)&uCtPts, sizeof(unsigned long));
  writer.write((const char*)&(kernel[0]), uCtPts*sizeof(Base::Vector3D));
}

void Feature::RestoreDocFile(Base::Reader &reader)
{
  PointKernel& kernel = _Points.getKernel();
  kernel.clear();

  unsigned long uCtPts;
  reader.read((char*)&uCtPts, sizeof(unsigned long));
  kernel.resize(uCtPts);
  reader.read((char*)&(kernel[0]), uCtPts*sizeof(Base::Vector3D));
}


int Feature::execute(void)
{
  return 0;
}

void Feature::setPoints(const PointsWithProperty& New)
{
  _Points = New;
  Touch();
}

Base::PyObjectBase *Feature::GetPyObject(void)
{
  return new PointsFeaturePy(this);
}

