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
# include <QPixmap>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoSwitch.h>
# include <Inventor/nodes/SoTransform.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Matrix.h>

#include "ViewProvider.h"
#include "ViewProviderPy.h"
#include "BitmapFactory.h"
#include "View3DInventor.h"

using namespace std;
using namespace Gui;


//**************************************************************************
//**************************************************************************
// ViewProvider
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

PROPERTY_SOURCE_ABSTRACT(Gui::ViewProvider, App::PropertyContainer)


       
ViewProvider::ViewProvider() : _iActualMode(-1), _pyObject(0)
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
  pcModeSwitch->whichChild = _iActualMode;
}


ViewProvider::~ViewProvider()
{
  if (_pyObject)
  {
    _pyObject->setInvalid();
    _pyObject->DecRef();
  }

  pcRoot->unref();
  pcTransform->unref();
  pcModeSwitch->unref();
}

void ViewProvider::update(void)
{
    // Hide the object temporarily to speed up the update
    bool vis = this->isShow();
    if (vis) hide();
    updateData();
    if (vis) show();
}

QIcon ViewProvider::getIcon(void) const
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

void ViewProvider::addDisplayMaskMode( SoNode *node, const char* type )
{
  _sDisplayMaskModes[ type ] = pcModeSwitch->getNumChildren();
  pcModeSwitch->addChild( node );
}

void ViewProvider::setDisplayMaskMode( const char* type )
{
  std::map<std::string, int>::const_iterator it = _sDisplayMaskModes.find( type );
  if ( it != _sDisplayMaskModes.end() )
    pcModeSwitch->whichChild = it->second;
  else
    pcModeSwitch->whichChild = -1;
  _iActualMode = pcModeSwitch->whichChild.getValue();
}

std::vector<std::string> ViewProvider::getDisplayMaskModes() const
{
  std::vector<std::string> types;
  for ( std::map<std::string, int>::const_iterator it = _sDisplayMaskModes.begin(); it != _sDisplayMaskModes.end(); ++it )
    types.push_back( it->first );
  return types;
}

/**
 * If you add new viewing modes in @ref getModes() then you need to reimplement also setMode() to handle these
 * new modes by setting the appropriate display mode.
 */
void ViewProvider::setDisplayMode(const char* ModeName)
{
  _sCurrentMode = ModeName;
}

std::string ViewProvider::getActiveDisplayMode(void) const
{
  return _sCurrentMode;
}

void ViewProvider::hide(void)
{
  pcModeSwitch->whichChild = -1;
}

void ViewProvider::show(void)
{
  pcModeSwitch->whichChild = _iActualMode;
}

bool ViewProvider::isShow(void) const
{
  return pcModeSwitch->whichChild.getValue() != -1;

}

std::string ViewProvider::toString() const
{
    return View3DInventor::writeNodesToString(pcRoot);
}

PyObject* ViewProvider::getPyObject()
{
  if (!_pyObject)
    _pyObject = new ViewProviderPy(this);
  _pyObject->IncRef();
  return _pyObject;
}

