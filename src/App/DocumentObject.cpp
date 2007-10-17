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
  : _pDoc(0)
{
    // FIXME: Each document object has two names associated, its 'name' property and the internal name stored inside
    // the document it belongs to. The name property is claimed to be the user name, e.g. to display in the tree view
    // while the internal name is used to address an object over its document. Thus, the internal name must be unique but
    // the value of the name property not necessarily. 
    // However, whereever we work with a document object we pass the value of the name property instead of using its internal
    // name (see getNameInDocument()) which is wrong as the name property and internal name may differ.
    // On the other hand the method getNameInDocument() is rather slow as we need to iterate always over an internal map and
    // should thus be avoided.
    // I think the best way would be to define the name property to always be set to the internal name as stored in the document 
    // and to have a second name property in its associated view provider class for display purposes. Thus, the name property of 
    // the document object must be read-only.
    // So, we don't need to change client code where we use the name property and can access the correct internal name in constant 
    // time. This makes the method getNameInDocument() obsolete, then.
    ADD_PROPERTY_TYPE(name,("Unnamed"),"Base",Prop_ReadOnly,"User name of the object (UTF8)");
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
