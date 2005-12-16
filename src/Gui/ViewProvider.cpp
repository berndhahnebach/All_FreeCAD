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
# include <qlistview.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoTransform.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Matrix.h>

#include "ViewProvider.h"
#include "BitmapFactory.h"
#include "Tree.h"



# include <Inventor/nodes/SoSwitch.h>


using namespace std;
using namespace Gui;



//**************************************************************************
//**************************************************************************
// ViewProvider
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


       
ViewProvider::ViewProvider()
{
  pcRoot = new SoSeparator();
  pcRoot->ref();
  pcModeSwitch = new SoSwitch();
  pcModeSwitch->ref();
  pcTransform  = new SoTransform();
  pcTransform->ref();
  pcRoot->addChild(pcTransform);
  pcRoot->addChild(pcModeSwitch);
  sPixmap = "px";

}


ViewProvider::~ViewProvider()
{
  pcRoot->unref();
  pcTransform->unref();
  pcModeSwitch->unref();
}

QPixmap ViewProvider::getIcon(void) const
{
  return Gui::BitmapFactory().pixmap(sPixmap);
}



void ViewProvider::setTransformation(const Base::Matrix4D &rcMatrix)
{

  double dMtrx[16];
  
  rcMatrix.getGLMatrix(dMtrx);

  pcTransform->setMatrix(SbMatrix(dMtrx[0], dMtrx[1], dMtrx[2],  dMtrx[3],
                                  dMtrx[4], dMtrx[5], dMtrx[6],  dMtrx[7],
                                  dMtrx[8], dMtrx[9], dMtrx[10], dMtrx[11],
                                  dMtrx[12],dMtrx[13],dMtrx[14], dMtrx[15]));
}

void ViewProvider::setTransformation(const SbMatrix &rcMatrix)
{
  pcTransform->setMatrix(rcMatrix);
}


void ViewProvider::setMode(const char* ModeName)
{
  // collect all modes (with subclasses)
  vector<string> modes = getModes();
  vector<string>::iterator p;

  p = find(modes.begin(),modes.end(),ModeName);

  if(p!=modes.end())
    setMode(p-modes.begin());
  else
    Base::Console().Warning("Unknown mode '%s' in ViewProvider::setMode(), ignored\n", ModeName);

}

void ViewProvider::setMode(int Mode)
{
  pcModeSwitch->whichChild = Mode;
  _iActualMode = Mode;
}


int ViewProvider::getMode(void)
{
 return pcModeSwitch->whichChild.getValue();
}


std::string ViewProvider::getModeName(void)
{
  return getModes()[getMode()];
}

void ViewProvider::hide(void)
{
  if(pcModeSwitch->whichChild.getValue() != -1)
    _iActualMode = pcModeSwitch->whichChild.getValue();
  pcModeSwitch->whichChild = -1;
}

void ViewProvider::show(void)
{
  pcModeSwitch->whichChild = _iActualMode;

}

bool ViewProvider::isShow(void)
{
  return pcModeSwitch->whichChild.getValue() != -1;

}


