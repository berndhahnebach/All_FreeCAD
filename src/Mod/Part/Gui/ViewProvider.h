/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef __VIEWPROVIDERPART_H__
#define __VIEWPROVIDERPART_H__

#include "../../../Gui/ViewProviderFeature.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;

namespace PartGui {


class AppPartGuiExport ViewProviderInventorPart:public Gui::ViewProviderInventorFeature
{
public:
  /// constructor
  ViewProviderInventorPart();
  /// destructor
  virtual ~ViewProviderInventorPart();


  virtual void attach(App::Feature *);

  /// returns a vector of all possible modes
  virtual std::vector<std::string> getModes(void);
  /// Update the Part representation
  //virtual void update(const ChangeType&);

  virtual void updateData(void);

protected:
  Standard_Boolean computeFaces   (SoSeparator* root, const TopoDS_Shape &myShape);
  Standard_Boolean computeEdges   (SoSeparator* root, const TopoDS_Shape &myShape);
  Standard_Boolean computeVertices(SoSeparator* root, const TopoDS_Shape &myShape);

  void transferToArray(const TopoDS_Face& aFace,SbVec3f** vertices,SbVec3f** vertexnormals, long** cons,int &nbNodesInFace,int &nbTriInFace );

  // nodes for the data representation
  SoSeparator *EdgeRoot;
  SoSeparator *FaceRoot;
  SoSeparator *VertexRoot;


  // settings stuff
  ParameterGrp::handle hGrp;
  float fMeshDeviation;     
  bool  bNoPerVertexNormals;
//  long  lHilightColor;      
  bool  bQualityNormals;    


};

} // namespace PartGui


#endif // __VIEWPROVIDERPART_H__

