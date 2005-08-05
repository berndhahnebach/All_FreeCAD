/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2005     *
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
# include <fcntl.h>
# include <TopTools_HSequenceOfShape.hxx>
# include <IGESControl_Writer.hxx>
# include <IGESControl_Reader.hxx>
# include <TopoDS_Shape.hxx>
# include <TFunction_Logbook.hxx>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include "FeaturePartCurveNet.h"
#include "PartAlgos.h"


using namespace Part;


FeaturePartCurveNet::FeaturePartCurveNet()
{
  _lineMaterial.ambientColor.set(0.2f,0.8f,0.2f);
  _lineMaterial.diffuseColor.set(0.1f,0.8f,0.1f);
  _lineMaterial.specularColor.set(0.0f,0.0f,0.0f);
  _lineMaterial.emissiveColor.set(0.0f,0.0f,0.0f);
  _lineMaterial.shininess = 0.0f;
  _lineMaterial.transparency = 0.0f;
  _lineSize = 4.0f;

  _pointMaterial.ambientColor.set(0.9f,0.9f,0.9f);
  _pointMaterial.diffuseColor.set(0.8f,0.8f,0.8f);;
  _pointMaterial.specularColor.set(0.0f,0.0f,0.0f);
  _pointMaterial.emissiveColor.set(0.0f,0.0f,0.0f);
  _pointMaterial.shininess = 0.3f;
  _pointSize = 0.05f;
  _showMode = "Flat";
}


void FeaturePartCurveNet::initFeature(void)
{
	addProperty("String","FileName");
}

int FeaturePartCurveNet::execute(TFunction_Logbook& log)
{
  try{

    std::string FileName = getPropertyString("FileName");

    int i=open(FileName.c_str(),O_RDONLY);
	  if( i != -1)
	  {
		  close(i);
	  }else{
      Base::Console().Log("FeaturePartImportIges::Execute() not able to open %s!\n",FileName.c_str());
		  return 1;
	  }

	  setShape(PartAlgos::Load(FileName.c_str()));
  }
  catch(...){
    Base::Console().Error("FeaturePartImportIges::Execute() failed!");
    return 1;
  }

  return 0;
}

