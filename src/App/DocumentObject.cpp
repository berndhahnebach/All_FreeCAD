/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de)          *
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

#include "Document.h"
#include "DocumentObject.h"
#include "DocumentObjectPy.h"
#define new DEBUG_CLIENTBLOCK
using namespace App;


PROPERTY_SOURCE(App::DocumentObject, App::PropertyContainer)


//===========================================================================
// DocumentObject
//===========================================================================

DocumentObject::DocumentObject(void)
:_pDoc(0)
{
  ADD_PROPERTY(name,("Unnamed"));
  touchTime.setToActual();
  touchViewTime.setToActual();

}

DocumentObject::~DocumentObject(void)
{
  //if(pcObjectPy)
  //{
  //  pcObjectPy->setInvalid();
  //  pcObjectPy->DecRef();
  //}
}

App::Document &DocumentObject::getDocument(void) const
{
  return *_pDoc;
}

void DocumentObject::setDocument(App::Document* doc)
{
  _pDoc=doc;
}

void DocumentObject::onBevorChange(const Property* prop)
{
  if(_pDoc)
    _pDoc->onBevorChangeProperty(this,prop);
}

  /// get called by the container when a Proptery was changed
void DocumentObject::onChanged(const Property* prop)
{
  if(_pDoc)
    _pDoc->onChangedProperty(this,prop);
}

PyObject *DocumentObject::getPyObject(void)
{
  if(PythonObject.is(Py::_None())){
    // ref counter is set to 1
    PythonObject = new DocumentObjectPy(this);
  }
  return Py::new_reference_to(PythonObject); 
}

void DocumentObject::Touch(void)
{
  touchTime.setToActual();
}

void DocumentObject::TouchView(void)
{
  touchViewTime.setToActual();
}
