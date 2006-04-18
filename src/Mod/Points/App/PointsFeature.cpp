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
  showMode.setValue("Point");
}

Feature::~Feature()
{
}

void Feature::Save (Base::Writer &writer) const
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

void Feature::SaveDocFile (Base::Writer &writer) const
{
  const PointKernel& kernel = _Points.getKernel();
  unsigned long uCtPts = kernel.size();
  writer.write((const char*)&uCtPts, sizeof(unsigned long));
  writer.write((const char*)&(kernel[0]), uCtPts*sizeof(Base::Vector3f));
}

void Feature::RestoreDocFile(Base::Reader &reader)
{
  PointKernel& kernel = _Points.getKernel();
  kernel.clear();

  unsigned long uCtPts;
  reader.read((char*)&uCtPts, sizeof(unsigned long));
  kernel.resize(uCtPts);
  reader.read((char*)&(kernel[0]), uCtPts*sizeof(Base::Vector3f));
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

// ------------------------------------------------------------------

PROPERTY_SOURCE(Points::Export, Points::Feature)

Export::Export(void)
{
  ADD_PROPERTY(Sources ,(0));
  ADD_PROPERTY(FileName,(""));
  ADD_PROPERTY(Format  ,(""));
}

int Export::execute(void)
{
  // ask for write permission
  Base::FileInfo fi(FileName.getValue());
  Base::FileInfo di(fi.dirPath().c_str());
	if ( fi.exists() && fi.isWritable() == false || di.exists() == false || di.isWritable() == false )
  {
    setError("No write permission for file '%s'",FileName.getValue());
    return 1;
  }

  std::ofstream str( FileName.getValue(), std::ios::out | std::ios::binary );

  if ( fi.hasExtension("asc") )
  {
    const std::vector<App::DocumentObject*>& features = Sources.getValues();
    for ( std::vector<App::DocumentObject*>::const_iterator it = features.begin(); it != features.end(); ++it )
    {
      Feature *pcFeat  = dynamic_cast<Feature*>(*it);
      const PointKernel& kernel = pcFeat->getPoints().getKernel();

      str << "# " << pcFeat->name.getValue() << " Number of points: " << kernel.size() << std::endl;
      for ( PointKernel::const_iterator it = kernel.begin(); it != kernel.end(); ++it )
        str << it->x << " " << it->y << " " << it->z << std::endl;
    }
  }
  else
  {
    setError("File format '%s' not supported", fi.extension().c_str());
    return 1;
  }

  return 0;
}

// ------------------------------------------------------------------

PROPERTY_SOURCE(Points::Transform, Points::Feature)

Transform::Transform()
{
  ADD_PROPERTY(Source, (0));
  ADD_PROPERTY(Trnsfrm, (Base::Matrix4D()));
}

Transform::~Transform()
{
}

int Transform::execute(void)
{
  Feature *pcPoints  = dynamic_cast<Feature*>(Source.getValue());
  _Points = pcPoints->getPoints();
  _Points.transform( Trnsfrm.getValue() );

  const Base::Matrix4D& cMat = Trnsfrm.getValue();
  Base::Console().Message("Transform [[%.2f, %.2f, %.2f, %.2f],[%.2f, %.2f, %.2f, %.2f],[%.2f, %.2f, %.2f, %.2f],[%.2f, %.2f, %.2f, %.2f]]\n",
                          cMat[0][0],cMat[0][1],cMat[0][2],cMat[0][3],cMat[1][0],cMat[1][1],cMat[1][2],cMat[1][3],
                          cMat[2][0],cMat[2][1],cMat[2][2],cMat[2][3],cMat[3][0],cMat[3][1],cMat[3][2],cMat[3][3]);

  return 0;
}
