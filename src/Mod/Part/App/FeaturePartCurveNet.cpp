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
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include "FeaturePartCurveNet.h"


using namespace Part;

PROPERTY_SOURCE(Part::CurveNet, Part::Feature)

CurveNet::CurveNet()
{
//  _pointMaterial.ambientColor.set(0.9f,0.9f,0.9f);
//  _pointMaterial.diffuseColor.set(0.8f,0.8f,0.8f);;
//  _pointMaterial.specularColor.set(0.0f,0.0f,0.0f);
//  _pointMaterial.emissiveColor.set(0.0f,0.0f,0.0f);
//  _pointMaterial.shininess = 0.3f;

  ADD_PROPERTY(FileName,(""));
}



App::DocumentObjectExecReturn *CurveNet::execute(void)
{
  /*
  try{

    std::string FileName = getPropertyString("FileName");

    int i=_open(FileName.c_str(),O_RDONLY);
	  if( i != -1)
	  {
		  _close(i);
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
*/
//  setShape(TopoDS_Shape());

    return App::DocumentObject::StdReturn;
}

