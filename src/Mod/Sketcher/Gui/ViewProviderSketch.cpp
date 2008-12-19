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
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoLineSet.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoVertexProperty.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Parameter.h>

#include "ViewProviderSketch.h"


//#include "Tree.h"



using namespace SketcherGui;
using namespace std;


//**************************************************************************
// Construction/Destruction

PROPERTY_SOURCE(SketcherGui::ViewProviderSketch, PartGui::ViewProviderPart)

       
ViewProviderSketch::ViewProviderSketch()
{
 /*   ADD_PROPERTY(ShowGrid,(true));


    GridRoot = new SoSeparator();
    GridRoot->ref();

    pcRoot->addChild(GridRoot);*/
 
    sPixmap = "PartFeatureImport";
}

ViewProviderSketch::~ViewProviderSketch()
{
  //GridRoot->unref();
}



// **********************************************************************************


void ViewProviderSketch::updateData(const App::Property* prop)
{
    ViewProviderPart::updateData(prop);

 
}

void ViewProviderSketch::onChanged(const App::Property* prop)
{
    // call father
    ViewProviderPart::onChanged(prop);

 /*   if (prop == &ShowGrid) {
        if(ShowGrid.getValue())
            createGrid();
        else
            GridRoot->removeAllChildren();
    }*/
}

void ViewProviderSketch::attach(App::DocumentObject *pcFeat)
{
    ViewProviderPart::attach(pcFeat);

    createGrid();


}

void ViewProviderSketch::setEdit(void)
{

}

void ViewProviderSketch::unsetEdit(void)
{

}

