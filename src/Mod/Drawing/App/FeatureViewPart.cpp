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
# include <sstream>

#endif


#include <HLRBRep_Algo.hxx>
#include <TopoDS_Shape.hxx>
#include <HLRTopoBRep_OutLiner.hxx>
//#include <BRepAPI_MakeOutLine.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_ShapeBounds.hxx>
#include <HLRBRep_HLRToShape.hxx>


#include <Base/Exception.h>
#include <Base/FileInfo.h>

#include "FeatureViewPart.h"

using namespace Drawing;


//===========================================================================
// FeatureView
//===========================================================================

PROPERTY_SOURCE(Drawing::FeatureViewPart, Drawing::FeatureView)


FeatureViewPart::FeatureViewPart(void) 
{
  static const char *group = "Shape view";

  ADD_PROPERTY_TYPE(Source ,(0),group,App::Prop_None,"Shape to view");
}

FeatureViewPart::~FeatureViewPart()
{
}

int FeatureViewPart::execute(void)
{
    /*  Exemples d'utilisation :

       1- avec deux TopoDS_Shape*/

  Handle(HLRBRep_Algo) Hider = new HLRBRep_Algo();
  TopoDS_Shape S1,S2;
  // initialiser les TopoDS_Shape.
  HLRAlgo_Projector Prj;
  // initialiser le HLRAlgo_Projector
  Standard_Integer nbIso = 0;
  Hider->Add(S1,nbIso);
  Hider->Add(S2,nbIso);
  Hider->Projector(Prj);
  Hider->Update();
  Hider->Hide();
  // retirer le TopoDS_Shape S1
  Hider->Remove(Hider->Index(S1));

  delete Hider;
       //2- avec un HLRTopoBRep_OutLiner

  //Hider = new HLRBRep_Algo();
  //TopoDS_Shape S;
  //// initialiser le TopoDS_Shape.
  //HLRTopoBRep_OutLiner OS;
  //BRepAPI_MakeOutLine OutL;
  //HLRAlgo_Projector Prj;
  //// initialiser le HLRAlgo_Projector
  //Standard_Integer nbIso = 0;
  //OS.OriginalShape(S);
  //OS.Fill(Prj,nbIso,OutL);
  //TopoDS_Shape result = OS.OutLinedShape();
  //// ce resultat contient les IsoLines et les contours apparents.
  //Hider->Load(OS,nbIso);
  //Hider->Projector(Prj);
  //Hider->Update();
  //Hider->Hide();

     //Lien entre les nouveaux Shapes et les anciens.

  TopoDS_Shape Sh,SOld,SNew;
  Standard_Integer index = Hider->Index(Sh);
  SOld = Hider->ShapeBounds(index).Shape()
      ->DataStructure().NewSOldS(SNew);

     //Lecture des rsultats :

     //  1- gnrale

  {
  HLRBRep_HLRToShape Tool(Hider);
  TopoDS_Shape V  = Tool.VCompound       ();// artes vives       vues
  TopoDS_Shape V1 = Tool.Rg1LineVCompound();// artes rgulires  vues
  TopoDS_Shape VN = Tool.RgNLineVCompound();// artes de couture  vues
  TopoDS_Shape VO = Tool.OutLineVCompound();// contours apparents vus
  TopoDS_Shape VI = Tool.IsoLineVCompound();// isoparamtriques   vues
  TopoDS_Shape H  = Tool.HCompound       ();// artes vives       caches
  TopoDS_Shape H1 = Tool.Rg1LineHCompound();// artes rgulires  caches
  TopoDS_Shape HN = Tool.RgNLineHCompound();// artes de couture  caches
  TopoDS_Shape HO = Tool.OutLineHCompound();// contours apparents cachs
  TopoDS_Shape HI = Tool.IsoLineHCompound();// isoparamtriques   caches
  }
       ///2- sur tous les edges d'un TopoDS_Shape
  {
  TopoDS_Shape S;
  HLRBRep_HLRToShape Tool(Hider);
  TopoDS_Shape V  = Tool.VCompound       (S);// artes vives       vues
  TopoDS_Shape V1 = Tool.Rg1LineVCompound(S);// artes rgulires  vues
  TopoDS_Shape VN = Tool.RgNLineVCompound(S);// artes de couture  vues
  TopoDS_Shape VO = Tool.OutLineVCompound(S);// contours apparents vus
  TopoDS_Shape VI = Tool.IsoLineVCompound(S);// isoparamtriques   vues
  TopoDS_Shape H  = Tool.HCompound       (S);// artes vives       caches
  TopoDS_Shape H1 = Tool.Rg1LineHCompound(S);// artes rgulires  caches
  TopoDS_Shape HN = Tool.RgNLineHCompound(S);// artes de couture  caches
  TopoDS_Shape HO = Tool.OutLineHCompound(S);// contours apparents cachs
  TopoDS_Shape HI = Tool.IsoLineHCompound(S);// isoparamtriques   caches
  }

  return 0;
}


//
//PyObject *Feature::getPyObject(void)
//{
// if(PythonObject.is(Py::_None())){
//    // ref counter is set to 1
//    PythonObject.set(new PartFeaturePy(this),true);
//  }
//  return Py::new_reference_to(PythonObject); 
//}


