/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de)          *
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
  ADD_PROPERTY_TYPE(name,("Unnamed"),"Base",Prop_None);
  touchTime.setToActual();
  touchViewTime.setToActual();

}

DocumentObject::~DocumentObject(void)
{
  if (!PythonObject.is(Py::_None())){
    // Remark: The API of Py::Object has been changed to set whether the wrapper owns the passed 
    // Python object or not. In the constructor we forced the wrapper to own the object so we need
    // not to dec'ref the Python object any more.
    // But we must still invalidate the Python object because it need not to be
    // destructed right now because the interpreter can own several references to it.
    Base::PyObjectBase* obj = (Base::PyObjectBase*)PythonObject.ptr();
    // Call before decrementing the reference counter, otherwise a heap error can occur
    obj->setInvalid();
  }
}

const std::string &DocumentObject::getNameInDocument(void) const
{
  std::map<std::string,DocumentObject*>::const_iterator It = _pDoc->ObjectMap.begin();
  while(It != _pDoc->ObjectMap.end() && It->second!= this) ++It;
  return It->first;
}

void DocumentObject::onLoseLinkToObject(DocumentObject*)
{

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
  // set object touched
  StatusBits.set(0);
}

PyObject *DocumentObject::getPyObject(void)
{
  if(PythonObject.is(Py::_None())){
    // ref counter is set to 1
    PythonObject.set(new DocumentObjectPy(this),true);
  }
  return Py::new_reference_to(PythonObject); 
}

void DocumentObject::touch(void)
{
  touchTime.setToActual();
  StatusBits.set(0);
}

void DocumentObject::TouchView(void)
{
  touchViewTime.setToActual();
}
