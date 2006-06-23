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
# include <Inventor/actions/SoRayPickAction.h> 
#endif

#include <Base/Parameter.h>

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
#ifndef COIN_COLOR_STYLE
    unsigned long highlight = (unsigned long)(highlightColor.getPackedValue());
    int r = (highlight >> 24)&0xff;
    int g = (highlight >> 16)&0xff;
    int b = (highlight >>  8)&0xff;
    int a = (highlight)&0xff;
    highlight = (b << 16) | (g << 8) | r;
    highlight = hGrp->GetInt("HighlightColor", highlight);
    b = (highlight >> 16)&0xff;
    g = (highlight >>  8)&0xff;
    r = (highlight)&0xff;
    highlight = (r << 24) | (g << 16) | (b << 8) | a;
    highlightColor.setPackedValue((uint32_t)highlight, transparency);
    pcHighlight->colorHighlight.setValue(highlightColor);
#else
    unsigned long highlight = (unsigned long)(highlightColor.getPackedValue());
    int a = (highlight)&0xff;
    highlight = hGrp->GetInt("HighlightColor", highlight);
    highlight += a;
    highlightColor.setPackedValue((uint32_t)highlight, transparency);
    pcHighlight->colorHighlight.setValue(highlightColor);
#endif
  }
  if (disableSel) {
    pcHighlight->selectionMode = Gui::SoFCSelection::SEL_OFF;
    pcHighlight->style = Gui::SoFCSelection::BOX;
  } else {
    // Do the same with the selection color
    SbColor selectionColor = pcHighlight->colorSelection.getValue();
#ifndef COIN_COLOR_STYLE
    unsigned long selection = (unsigned long)(selectionColor.getPackedValue());
    int r = (selection >> 24)&0xff;
    int g = (selection >> 16)&0xff;
    int b = (selection >>  8)&0xff;
    int a = (selection)&0xff;
    selection = (b << 16) | (g << 8) | r;
    selection = hGrp->GetInt("SelectionColor", selection);
    b = (selection >> 16)&0xff;
    g = (selection >>  8)&0xff;
    r = (selection)&0xff;
    selection = (r << 24) | (g << 16) | (b << 8) | a;
    selectionColor.setPackedValue((uint32_t)selection, transparency);
    pcHighlight->colorSelection.setValue(selectionColor);
#else
    unsigned long selection = (unsigned long)(selectionColor.getPackedValue());
    int a = (selection)&0xff;
    selection = hGrp->GetInt("SelectionColor", selection);
    selection += a;
    selectionColor.setPackedValue((uint32_t)selection, transparency);
    pcHighlight->colorSelection.setValue(selectionColor);
#endif
  }
}

ViewProviderFeature::~ViewProviderFeature()
{
  pcHighlight->unref();
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
  return (pick ? pick->copy() : 0);
}
