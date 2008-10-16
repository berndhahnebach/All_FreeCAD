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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <Inventor/SbColor.h>
# include <Inventor/fields/SoSFColor.h> 
# include <Inventor/nodes/SoCamera.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoDirectionalLight.h>
# include <Inventor/SoPickedPoint.h>
# include <Inventor/actions/SoRayPickAction.h> 
#endif

#include <Base/Parameter.h>
#include <App/Document.h>

#include "Window.h"
#include "SoFCSelection.h"
#include "ViewProviderFeature.h"
#include "View3DInventorViewer.h"


using namespace Gui;


PROPERTY_SOURCE(Gui::ViewProviderFeature, Gui::ViewProviderDocumentObject)

ViewProviderFeature::ViewProviderFeature()
{
  // Create the selection node
  pcHighlight = new SoFCSelection();
  pcHighlight->ref();

  float transparency;
  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("View");

  // switch off preselection
  bool disablePre = hGrp->GetBool("DisablePreselection", false);
  bool disableSel = hGrp->GetBool("DisableSelection", false);
  if (disablePre) {
    pcHighlight->highlightMode = Gui::SoFCSelection::OFF;
  } else {
    // Search for a user defined value with the current color as default
    SbColor highlightColor = pcHighlight->colorHighlight.getValue();
    unsigned long highlight = (unsigned long)(highlightColor.getPackedValue());
    int a = (highlight)&0xff;
    highlight = hGrp->GetUnsigned("HighlightColor", highlight);
    highlight += a;
    highlightColor.setPackedValue((uint32_t)highlight, transparency);
    pcHighlight->colorHighlight.setValue(highlightColor);
  }
  if (disableSel) {
    pcHighlight->selectionMode = Gui::SoFCSelection::SEL_OFF;
    pcHighlight->style = Gui::SoFCSelection::BOX;
  } else {
    // Do the same with the selection color
    SbColor selectionColor = pcHighlight->colorSelection.getValue();
    unsigned long selection = (unsigned long)(selectionColor.getPackedValue());
    int a = (selection)&0xff;
    selection = hGrp->GetUnsigned("SelectionColor", selection);
    selection += a;
    selectionColor.setPackedValue((uint32_t)selection, transparency);
    pcHighlight->colorSelection.setValue(selectionColor);
  }
}

ViewProviderFeature::~ViewProviderFeature()
{
  pcHighlight->unref();
}

void ViewProviderFeature::attach(App::DocumentObject *pcObj)
{
  ViewProviderDocumentObject::attach(pcObj);
  pcHighlight->objectName = pcObj->getNameInDocument();
  pcHighlight->documentName = pcObj->getDocument()->getName();
  pcHighlight->subElementName = "Main";
}

SoPickedPointList ViewProviderFeature::getPickedPoints(const SbVec2s& pos, const View3DInventorViewer& viewer,bool pickAll) const
{
  SoSeparator* root = new SoSeparator;
  root->ref();
  root->addChild(viewer.getHeadlight());
  root->addChild(viewer.getCamera());
  root->addChild(this->pcHighlight);

  SoRayPickAction rp(viewer.getViewportRegion());
  rp.setPickAll(pickAll);
  rp.setPoint(pos);
  rp.apply(root);
  root->unref();

  // returns a copy of the list
  return rp.getPickedPointList();
}

SoPickedPoint* ViewProviderFeature::getPickedPoint(const SbVec2s& pos, const View3DInventorViewer& viewer) const
{
  SoSeparator* root = new SoSeparator;
  root->ref();
  root->addChild(viewer.getHeadlight());
  root->addChild(viewer.getCamera());
  root->addChild(this->pcHighlight);

  SoRayPickAction rp(viewer.getViewportRegion());
  rp.setPoint(pos);
  rp.apply(root);
  root->unref();

  // returns a copy of the point
  SoPickedPoint* pick = rp.getPickedPoint();
  //return (pick ? pick->copy() : 0); // needs the same instance of CRT under MS Windows
  return (pick ? new SoPickedPoint(*pick) : 0);
}

