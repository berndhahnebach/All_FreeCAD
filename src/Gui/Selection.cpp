/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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
# include <assert.h>
# include <string>
# include <boost/signals.hpp>
# include <boost/bind.hpp>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Selection.h"
#include <Base/Exception.h>
#include <Base/Console.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <App/DocumentObjectPy.h>


using namespace Gui;
using namespace std;

std::vector<SelectionSingleton::SelObj> SelectionSingleton::getCompleteSelection() const
{
    std::vector<SelObj> temp;
    SelObj tempSelObj;

    for( list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It) {
        tempSelObj.DocName  = It->DocName.c_str();
        tempSelObj.FeatName = It->FeatName.c_str();
        tempSelObj.SubName  = It->SubName.c_str();
        tempSelObj.TypeName = It->TypeName.c_str();
        tempSelObj.pObject  = It->pObject;
        tempSelObj.pDoc     = It->pDoc;
        temp.push_back(tempSelObj);
    }

    return temp;
}

std::vector<SelectionSingleton::SelObj> SelectionSingleton::getSelection(const char* pDocName) const
{
    std::vector<SelObj> temp;
    SelObj tempSelObj;

    App::Document *pcDoc;
    string DocName;

    pcDoc = getDocument(pDocName);

    //FIXME: We should abandon all the 'active' document stuff because this make quite a lot of troubles.
    //Thus it should only be able to access (an existing) document stuff by its name. For Python there might
    //be an active document but not for internal stuff. (Werner 20060517)
    if(!pcDoc)
        return temp;

    for(std::list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It) {
        if (It->pDoc == pcDoc) {
            tempSelObj.DocName  = It->DocName.c_str();
            tempSelObj.FeatName = It->FeatName.c_str();
            tempSelObj.SubName  = It->SubName.c_str();
            tempSelObj.TypeName = It->TypeName.c_str();
            tempSelObj.pObject    = It->pObject;
            tempSelObj.pDoc     = It->pDoc;
            temp.push_back(tempSelObj);
        }
    }

    return temp;
}

vector<App::DocumentObject*> SelectionSingleton::getObjectsOfType(const Base::Type& typeId, const char* pDocName) const
{
    std::vector<App::DocumentObject*> temp;
    App::Document *pcDoc;

    pcDoc = getDocument(pDocName);

    //FIXME: We should abandon all the 'active' document stuff because this make quite a lot of troubles.
    //Thus it should only be able to access (an existing) document stuff by its name. For Python there might
    //be an active document but not for internal stuff. (Werner 20060517)
    if (!pcDoc)
        return temp;

    for (std::list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It) {
        if (It->pDoc == pcDoc && It->pObject && It->pObject->getTypeId().isDerivedFrom(typeId)) {
            temp.push_back(It->pObject);
        }
    }

    return temp;
}

std::vector<App::DocumentObject*> SelectionSingleton::getObjectsOfType(const char* typeName, const char* pDocName) const
{
    Base::Type typeId = Base::Type::fromName(typeName);
    if (typeId == Base::Type::badType())
        return std::vector<App::DocumentObject*>();
    return getObjectsOfType(typeId, pDocName);
}

unsigned int SelectionSingleton::countObjectsOfType(const Base::Type& typeId, const char* pDocName) const
{
    unsigned int iNbr=0;
    App::Document *pcDoc;

    pcDoc = getDocument(pDocName);

    //FIXME: We should abandon all the 'active' document stuff because this make quite a lot of troubles.
    //Thus it should only be able to access (an existing) document stuff by its name. For Python there might
    //be an active document but not for internal stuff. (Werner 20060517)
    if (!pcDoc)
        return 0;

    for (std::list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It) {
        if (It->pDoc == pcDoc && It->pObject && It->pObject->getTypeId().isDerivedFrom(typeId)) {
            iNbr++;
        }
    }

    return iNbr;
}

unsigned int SelectionSingleton::countObjectsOfType(const char* typeName, const char* pDocName) const
{
    Base::Type typeId = Base::Type::fromName(typeName);
    if (typeId == Base::Type::badType())
        return 0;
    return countObjectsOfType(typeId, pDocName);
}

bool SelectionSingleton::setPreselect(const char* pDocName, const char* pObjectName, const char* pSubName, float x, float y, float z)
{
    if (DocName != "")
        rmvPreselect();

    DocName = pDocName;
    FeatName= pObjectName;
    SubName = pSubName;
    hx = x;
    hy = y;
    hz = z;

    SelectionChanges Chng;

    Chng.pDocName  = DocName.c_str();
    Chng.pObjectName = FeatName.c_str();
    Chng.pSubName  = SubName.c_str();
    Chng.x = x;
    Chng.y = y;
    Chng.z = z;
    Chng.Type = SelectionChanges::SetPreselect;


    Notify(Chng);

    //Base::Console().Log("Sel : Add preselect %s \n",pObjectName);

    // allows the preselection
    return true;
}

void SelectionSingleton::rmvPreselect()
{
    if (DocName == "")
        return;

    SelectionChanges Chng;
    Chng.pDocName  = DocName.c_str();
    Chng.pObjectName = FeatName.c_str();
    Chng.pSubName  = SubName.c_str();
    Chng.Type = SelectionChanges::RmvPreselect;

    Notify(Chng);

    DocName = "";
    FeatName= "";
    SubName = "";
    hx = 0;
    hy = 0;
    hz = 0;

    //Base::Console().Log("Sel : Rmv preselect \n");
}

App::Document* SelectionSingleton::getDocument(const char* pDocName) const
{
    if (pDocName)
        return App::GetApplication().getDocument(pDocName);
    else
        return App::GetApplication().getActiveDocument();
}

bool SelectionSingleton::addSelection(const char* pDocName, const char* pObjectName, const char* pSubName, float x, float y, float z)
{
    // already in ?
    if (isSelected(pDocName, pObjectName, pSubName))
        return true;

    _SelObj temp;

    temp.pDoc = getDocument(pDocName);

    if (temp.pDoc) {
        if(pObjectName)
            temp.pObject = temp.pDoc->getObject(pObjectName);
        else
            temp.pObject = 0;

        temp.DocName  = pDocName;
        temp.FeatName = pObjectName?pObjectName:"";
        temp.SubName  = pSubName ?pSubName :"";
        temp.x        = x;
        temp.y        = y;
        temp.z        = z;

        if (temp.pObject)
            temp.TypeName = temp.pObject->getTypeId().getName();

        _SelList.push_back(temp);

        SelectionChanges Chng;

        Chng.pDocName  = pDocName;
        Chng.pObjectName = pObjectName?pObjectName:"";
        Chng.pSubName  = pSubName ?pSubName :"";
        Chng.x         = x;
        Chng.y         = y;
        Chng.z         = z;
        Chng.Type      = SelectionChanges::AddSelection;


        Notify(Chng);

        Base::Console().Log("Sel : Add Selection \"%s.%s.%s(%f,%f,%f)\"\n",pDocName,pObjectName,pSubName,x,y,z);

        // allow selection
        return true;
    }
    else { // neither an existing nor active document available 
        assert(0);
        return false;
    }
}

void SelectionSingleton::rmvSelection(const char* pDocName, const char* pObjectName, const char* pSubName)
{
    std::vector<SelectionChanges> rmvList;

    for (std::list<_SelObj>::iterator It = _SelList.begin();It != _SelList.end();) {
        if ((It->DocName == pDocName && !pObjectName) ||
            (It->DocName == pDocName && pObjectName && It->FeatName == pObjectName && !pSubName) ||
            (It->DocName == pDocName && pObjectName && It->FeatName == pObjectName && pSubName && It->SubName == pSubName))
        {
            // save in tmp. string vars
            std::string tmpDocName = It->DocName;
            std::string tmpFeaName = It->FeatName;
            std::string tmpSubName = It->SubName;

            // destroy the _SelObj item
            It = _SelList.erase(It);

            SelectionChanges Chng;
            Chng.pDocName  = tmpDocName.c_str();
            Chng.pObjectName = tmpFeaName.c_str();
            Chng.pSubName  = tmpSubName.c_str();
            Chng.Type      = SelectionChanges::RmvSelection;

            Notify(Chng);
      
            rmvList.push_back(Chng);
            Base::Console().Log("Sel : Rmv Selection \"%s.%s.%s\"\n",pDocName,pObjectName,pSubName);
        }
        else {
            ++It;
        }
    }
}

void SelectionSingleton::clearSelection(const char* pDocName)
{
    App::Document* pDoc;
    pDoc = getDocument(pDocName);

    // the document 'pDocName' has already been removed
    if (!pDoc && !pDocName) {
        clearCompleteSelection();
    }
    else {
        std::string docName;
        if (pDocName)
            docName = pDocName;
        else
            docName = pDoc->getName(); // active document
        std::list<_SelObj> selList;
        for (std::list<_SelObj>::iterator it = _SelList.begin(); it != _SelList.end(); ++it) {
            if (it->DocName != docName)
                selList.push_back(*it);
        }

        _SelList = selList;

        SelectionChanges Chng;
        Chng.Type = SelectionChanges::ClrSelection;
        Chng.pDocName = docName.c_str();

        Notify(Chng);

        Base::Console().Log("Sel : Clear selection\n");
    }
}

void SelectionSingleton::clearCompleteSelection()
{
    _SelList.clear();

    SelectionChanges Chng;
    Chng.Type = SelectionChanges::ClrSelection;
    Chng.pDocName = "";


    Notify(Chng);

    Base::Console().Log("Sel : Clear selection\n");
}

bool SelectionSingleton::isSelected(const char* pDocName, const char* pObjectName, const char* pSubName) const
{
    const char* tmpDocName = pDocName ? pDocName : "";
    const char* tmpFeaName = pObjectName ? pObjectName : "";
    const char* tmpSubName = pSubName ? pSubName : "";
    for (std::list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It)
        if (It->DocName == tmpDocName && It->FeatName == tmpFeaName && It->SubName == tmpSubName)
            return true;
    return false;
}

bool SelectionSingleton::isSelected(App::DocumentObject* obj) const
{
    if (!obj) return false;

    for(list<_SelObj>::const_iterator It = _SelList.begin();It != _SelList.end();++It) {
        if (It->pObject == obj)
            return true;
    }

    return false;
}
void SelectionSingleton::slotDeletedObject(App::DocumentObject& Obj)
{
    // remove also from the selection, if selected
    Selection().rmvSelection( Obj.getDocument().getName(), Obj.getNameInDocument() );
}

void SelectionSingleton::slotRenamedObject(App::DocumentObject& Obj)
{
    // compare internals with the document and change them if needed
    App::Document& pDoc = Obj.getDocument();
    for (std::list<_SelObj>::iterator it = _SelList.begin(); it != _SelList.end(); ++it) {
        if (it->pDoc == &pDoc) {
            it->DocName = pDoc.getName();
        }
    }
}


//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
SelectionSingleton::SelectionSingleton()
{
    App::GetApplication().signalDeletedObject.connect(boost::bind(&Gui::SelectionSingleton::slotDeletedObject, this, _1));
    App::GetApplication().signalRenamedObject.connect(boost::bind(&Gui::SelectionSingleton::slotRenamedObject, this, _1));
}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
SelectionSingleton::~SelectionSingleton()
{
}


SelectionSingleton* SelectionSingleton::_pcSingleton = NULL;

SelectionSingleton& SelectionSingleton::instance(void)
{
    if (_pcSingleton == NULL)
        _pcSingleton = new SelectionSingleton;
    return *_pcSingleton;
}

void SelectionSingleton::destruct (void)
{
    if (_pcSingleton != NULL)
        delete _pcSingleton;
    _pcSingleton = 0;
}
/*
void SelectionSingleton::addObject(App::DocumentObject *f)
{
  _ObjectSet.insert(f);

}

void SelectionSingleton::removeObject(App::DocumentObject *f)
{
  _ObjectSet.erase(f);


}
*/


//**************************************************************************
// Python stuff

// SelectionSingleton Methods						// Methods structure
PyMethodDef SelectionSingleton::Methods[] = {
    {"addSelection",         (PyCFunction) SelectionSingleton::sAddSelection, 1, 
     "addSelection(object) -- Add an object to the selection"},
    {"removeSelection",      (PyCFunction) SelectionSingleton::sRemoveSelection, 1,
     "removeSelection(object) -- Remove an object from the selection"},
    {"clearSelection"  ,     (PyCFunction) SelectionSingleton::sClearSelection, 1,
     "clearSelection([string]) -- Clear the selection\n"
     "Clear the selection to the given document name. If no document is\n"
     "given the complete selection is cleared."},
    {"isSelected",           (PyCFunction) SelectionSingleton::sIsSelected, 1,
     "isSelected(object) -- Check if a given object is selected"},
    {"getSelection",         (PyCFunction) SelectionSingleton::sGetSelection, 1,
     "getSelection([string]) -- Return a list of selected objets\n"
     "Return a list of selected objects for a given document name. If no\n"
     "document is given the complete selection is returned."},
    {NULL, NULL, 0, NULL}		/* Sentinel */
};

PyObject *SelectionSingleton::sAddSelection(PyObject * /*self*/, PyObject *args, PyObject * /*kwd*/)
{
    PyObject *object;
    if (!PyArg_ParseTuple(args, "O!", &(App::DocumentObjectPy::Type), &object))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    App::DocumentObjectPy* docObjPy = static_cast<App::DocumentObjectPy*>(object);
    App::DocumentObject* docObj = docObjPy->getDocumentObjectPtr();
    if (!docObj || !docObj->getNameInDocument()) {
        PyErr_SetString(PyExc_Exception, "Cannot check invalid object");
        return NULL;
    }

    Selection().addSelection(docObj->getDocument().getName(), docObj->getNameInDocument());

    Py_Return;
}

PyObject *SelectionSingleton::sRemoveSelection(PyObject * /*self*/, PyObject *args, PyObject * /*kwd*/)
{
    PyObject *object;
    if (!PyArg_ParseTuple(args, "O!", &(App::DocumentObjectPy::Type), &object))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    App::DocumentObjectPy* docObjPy = static_cast<App::DocumentObjectPy*>(object);
    App::DocumentObject* docObj = docObjPy->getDocumentObjectPtr();
    if (!docObj || !docObj->getNameInDocument()) {
        PyErr_SetString(PyExc_Exception, "Cannot check invalid object");
        return NULL;
    }

    Selection().rmvSelection(docObj->getDocument().getName(), docObj->getNameInDocument());

    Py_Return;
}

PyObject *SelectionSingleton::sClearSelection(PyObject * /*self*/, PyObject *args, PyObject * /*kwd*/)
{
    char *documentName=0;
    if (!PyArg_ParseTuple(args, "|s", &documentName))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception
    documentName ? Selection().clearSelection(documentName) : Selection().clearCompleteSelection();
    Py_Return;
}

PyObject *SelectionSingleton::sIsSelected(PyObject * /*self*/, PyObject *args, PyObject * /*kwd*/)
{
    PyObject *object;
    if (!PyArg_ParseTuple(args, "O!", &(App::DocumentObjectPy::Type), &object))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    App::DocumentObjectPy* docObj = static_cast<App::DocumentObjectPy*>(object);
    bool ok = Selection().isSelected(docObj->getDocumentObjectPtr());
    return Py_BuildValue("O", (ok ? Py_True : Py_False));
}

PyObject *SelectionSingleton::sGetSelection(PyObject * /*self*/, PyObject *args, PyObject * /*kwd*/)
{
    char *documentName=0;
    if (!PyArg_ParseTuple(args, "|s", &documentName))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception

    std::vector<SelectionSingleton::SelObj> sel;
    if (documentName)
        sel = Selection().getSelection(documentName);
    else
        sel = Selection().getCompleteSelection();

    try {
        Py::List list;
        for (std::vector<SelectionSingleton::SelObj>::iterator it = sel.begin(); it != sel.end(); ++it) {
            list.append(Py::Object(it->pObject->getPyObject()));
        }
        return Py::new_reference_to(list);
    }
    catch (Py::Exception&) {
        return 0;
    }
}
