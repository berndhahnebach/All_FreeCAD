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


#ifndef __ViewProviderCurveNet_H__
#define __ViewProviderCurveNet_H__

#include "ViewProvider.h"


class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;

namespace Gui {
  class View3DInventorViewer;
  class SoFCSelection;
}

namespace PartGui {


class AppPartGuiExport ViewProviderCurveNet:public ViewProviderInventorPart
{
public:
  /// constructor
  ViewProviderCurveNet();
  /// destructor
  virtual ~ViewProviderCurveNet();


  virtual void attache(App::Feature *);

  /// set the viewing mode
  virtual void setMode(const char* ModeName){};
  /// returns a vector of all possible modes
  virtual std::vector<std::string> getModes(void){return std::vector<std::string>();}
  /// Update the Part representation
  virtual void update(const ChangeType&){}

  virtual void setEdit(void);
  virtual void unsetEdit(void);

  virtual bool handleEvent(const SoEvent * const ev,Gui::View3DInventorViewer &Viewer);

protected:
  struct Node {
    Gui::SoFCSelection  *pcHighlight;
    SoTransform    *pcTransform;
  };

  std::list<Node> NodeList;

  bool bInEdit;

  /// root of the edge and vertex points
  SoSeparator *EdgeRoot, *VertexRoot;

  Standard_Boolean computeEdges   (SoSeparator* root, const TopoDS_Shape &myShape);
  Standard_Boolean computeVertices(SoSeparator* root, const TopoDS_Shape &myShape);

};

} // namespace PartGui


#endif // __VIEWPROVIDERPART_H__

