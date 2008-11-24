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

#include <Base/Writer.h>

#include "Document.h"
#include "DocumentObject.h"
#include "DocumentObjectPy.h"
#define new DEBUG_CLIENTBLOCK
using namespace App;


PROPERTY_SOURCE(App::DocumentObject, App::PropertyContainer)

DocumentObjectExecReturn *DocumentObject::StdReturn = 0;

//===========================================================================
// DocumentObject
//===========================================================================

DocumentObject::DocumentObject(void)
  : _pDoc(0),pcNameInDocument(0)
{
    ADD_PROPERTY_TYPE(Label,("Unnamed"),"Base",Prop_None,"User name of the object (UTF8)");
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

DocumentObjectExecReturn *DocumentObject::execute(void)
{
    return DocumentObject::StdReturn;
}

short DocumentObject::mustExecute(void) const
{
    return 0;
}

const char* DocumentObject::getStatusString(void) const
{
    if (isError()) {
        const char* text = getDocument()->getErrorDescription(this);
        return text ? text : "Error";
    }
    else if (isTouched())
        return "Touched";
    else
        return "Valid";
}

const char *DocumentObject::getNameInDocument(void) const
{
    // Note: It can happen that we query the internal name of an object even if it is not
    // part of a document (anymore). This is the case e.g. if we have a reference in Python 
    // to an object that has been removed from the document. In this case we should rather
    // return 0.
    //assert(pcNameInDocument);
    if (!pcNameInDocument) return 0;
    return pcNameInDocument->c_str();
 /* 
    std::map<std::string,DocumentObject*>::const_iterator It = _pDoc->ObjectMap.begin();
    while(It != _pDoc->ObjectMap.end() && It->second!= this) ++It;
    assert(It != _pDoc->ObjectMap.end());
    return It->first.c_str();
   */ 
}

void DocumentObject::onLoseLinkToObject(DocumentObject*)
{

}

App::Document *DocumentObject::getDocument(void) const
{
    return _pDoc;
}

void DocumentObject::setDocument(App::Document* doc)
{
    _pDoc=doc;
}

void DocumentObject::onBeforeChange(const Property* prop)
{
    if(_pDoc)
        _pDoc->onBeforeChangeProperty(this,prop);
}

/// get called by the container when a Property was changed
void DocumentObject::onChanged(const Property* prop)
{
    if(_pDoc)
        _pDoc->onChangedProperty(this,prop);
    if (prop->getType() & Prop_Output)
        return;
    // set object touched
    StatusBits.set(0);
}

PyObject *DocumentObject::getPyObject(void)
{
    if(PythonObject.is(Py::_None())){
        // ref counter is set to 1
        PythonObject = Py::Object(new DocumentObjectPy(this),true);
    }
    return Py::new_reference_to(PythonObject); 
}

void DocumentObject::touch(void)
{
    //touchTime.setToActual();
    StatusBits.set(0);
}

void DocumentObject::Save (Base::Writer &writer) const
{
    writer.Name = this->getNameInDocument();
    App::PropertyContainer::Save(writer);
}

/*
void DocumentObject::TouchView(void)
{
    touchViewTime.setToActual();
}
*/
