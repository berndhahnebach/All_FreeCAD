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
# include <TFunction_Logbook.hxx>
# include <TopoDS_Shape.hxx>
#endif


#include "../../../Base/Console.h"
#include "FeatureRayExportPov.h"


using namespace Raytracing;

void FeatureRayExportPov::InitLabel(const TDF_Label &rcLabel)
{
//	Base::Console().Log("PartBoxFeature::InitLabel()\n");

	AddProperty("Float","x","0.0");
	AddProperty("Float","y","0.0");
	AddProperty("Float","z","0.0");
	AddProperty("Float","l","100.0");
	AddProperty("Float","h","100.0");
	AddProperty("Float","w","100.0");

}

/*
bool PartBoxFeature::MustExecute(const TFunction_Logbook& log)
{
	Base::Console().Log("PartBoxFeature::MustExecute()\n");
	return false;
}
*/
Standard_Integer FeatureRayExportPov::Execute(void)
{
	Base::Console().Log("PartBoxFeature::Execute()\n");

/*  cout << GetFloatProperty("x") << endl;
  cout << GetFloatProperty("y") << endl;
  cout << GetFloatProperty("z") << endl;
  cout << GetFloatProperty("l") << endl;
  cout << GetFloatProperty("h") << endl;
  cout << GetFloatProperty("w") << endl;*/

	double x = GetFloatProperty("x");
  double y = GetFloatProperty("y");
  double z = GetFloatProperty("z");

  try{
	// Build a box using the dimension and position attributes
	BRepPrimAPI_MakeBox mkBox( gp_Pnt( x, y, z),
                             GetFloatProperty("l"),
                             GetFloatProperty("h"),
                             GetFloatProperty("w"));

  TopoDS_Shape ResultShape = mkBox.Shape();


	SetShape(ResultShape);

  }
  catch(...){
    return 1;
  }

  return 0;
}


void FeatureRayExportPov::Validate(void)
{
	Base::Console().Log("PartBoxFeature::Validate()\n");
 
  // We validate the object label ( Label() ), all the arguments and the results of the object:
  log.SetValid(Label(), Standard_True);


}





