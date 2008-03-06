
#include "PreCompiled.h"

#ifndef _PreComp_
# include <sstream>
#endif

#include <Base/Matrix.h>
#include <App/MatrixPy.h>

#include "Document.h"
#include "ViewProviderExtern.h"

// inclusion of the generated files (generated out of DocumentPy.xml)
#include "DocumentPy.h"
#include "DocumentPy.cpp"

using namespace Gui;

// returns a string which represent the object e.g. when printed in python
const char *DocumentPy::representation(void) const
{
    std::stringstream str;
    str << "<GUI Document object at " << getDocumentPtr() << ">";

    // Note: As the return type is 'const char*' we cannot create a temporary char array neither on the stack because the array would be freed
    // when leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class. This, however, is not a problem as long as we only
    // use this method in _repr().
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}


PyObject*  DocumentPy::show(PyObject *args)
{
    char *psFeatStr;
    if (!PyArg_ParseTuple(args, "s;Name of the Feature to show have to be given!",&psFeatStr))     // convert args: Python->C 
        return NULL;  // NULL triggers exception 

    PY_TRY {
        getDocumentPtr()->setShow(psFeatStr);  
        Py_Return;
    } PY_CATCH;
}

PyObject*  DocumentPy::hide(PyObject *args)
{
    char *psFeatStr;
    if (!PyArg_ParseTuple(args, "s;Name of the Feature to hide have to be given!",&psFeatStr))     // convert args: Python->C 
        return NULL;  // NULL triggers exception 

    PY_TRY {
        getDocumentPtr()->setHide(psFeatStr);  
        Py_Return;
    } PY_CATCH;
}

PyObject*  DocumentPy::setPos(PyObject *args)
{
    char *psFeatStr;
    Base::Matrix4D mat;
    PyObject *pcMatObj;
    if (!PyArg_ParseTuple(args, "sO!;Name of the Feature and the transformation matrix have to be given!",
                          &psFeatStr,
                          &(App::MatrixPy::Type), &pcMatObj))     // convert args: Python->C 
        return NULL;  // NULL triggers exception 

    mat = ((App::MatrixPy*)pcMatObj)->value();

    PY_TRY {
        getDocumentPtr()->setPos(psFeatStr,mat);  
        Py_Return;
    } PY_CATCH;
}

PyObject*  DocumentPy::addAnnotation(PyObject *args)
{
    char *psAnnoName,*psFileName,*psModName=0;
    if (!PyArg_ParseTuple(args, "ss|s;Name of the Annotation and a file name have to be given!",&psAnnoName,&psFileName,&psModName))     // convert args: Python->C 
        return NULL;  // NULL triggers exception 

    PY_TRY {
        ViewProviderExtern *pcExt = new ViewProviderExtern();

        pcExt->setModeByFile(psModName?psModName:"Main",psFileName);
        pcExt->adjustDocumentName(getDocumentPtr()->getDocument()->getName());

        getDocumentPtr()->setAnnotationViewProvider(psAnnoName,pcExt);

        Py_Return;

    } PY_CATCH;
}

PyObject*  DocumentPy::update(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 

    PY_TRY {
        getDocumentPtr()->update();
        Py_Return;
    } PY_CATCH;
}

PyObject*  DocumentPy::getObject(PyObject *args)
{
    char *sName;
    if (!PyArg_ParseTuple(args, "s",&sName))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    PY_TRY {
        ViewProvider *pcView = getDocumentPtr()->getViewProviderByName(sName);
	    if (pcView)
		    return pcView->getPyObject();
        else {
            Py_Return;
        }
    } PY_CATCH;
}

PyObject*  DocumentPy::activeObject(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 

    PY_TRY {
        App::DocumentObject *pcFtr = getDocumentPtr()->getDocument()->getActiveObject();
        if (pcFtr) {
            ViewProvider *pcView = getDocumentPtr()->getViewProvider(pcFtr);
	        return pcView->getPyObject();
        } else {
		    Py_Return;
        }
    } PY_CATCH;
}

PyObject*  DocumentPy::activeView(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    PY_TRY {
        Gui::MDIView  *pcView = getDocumentPtr()->getActiveView();
        if (pcView){
            // already incremented in getPyObject().
            return pcView->getPyObject();
        } else {
            Py_Return;
        }
    } PY_CATCH;
}

Py::Object DocumentPy::getActiveObject(void) const
{
    App::DocumentObject *object = getDocumentPtr()->getDocument()->getActiveObject();
    if (object) {
        ViewProvider *viewObj = getDocumentPtr()->getViewProvider(object);
        return Py::Object(viewObj->getPyObject());
    } else {
        return Py::None();
    }
}

void  DocumentPy::setActiveObject(Py::Object arg)
{
    throw Py::AttributeError("'Document' object attribute 'ActiveObject' is read-only");
}

Py::Object DocumentPy::getActiveView(void) const
{
    Gui::MDIView *view = getDocumentPtr()->getActiveView();
    if (view) {
        // already incremented in getPyObject().
        return Py::Object(view->getPyObject());
    } else {
        return Py::None();
    }
}

void  DocumentPy::setActiveView(Py::Object arg)
{
    throw Py::AttributeError("'Document' object attribute 'ActiveView' is read-only");
}

PyObject *DocumentPy::getCustomAttributes(const char* attr) const
{
    // Note: Here we want to return only a document object if its
    // name matches 'attr'. However, it is possible to have an object
    // with the same name as an attribute. If so, we return 0 as other-
    // wise it wouldn't be possible to address this attribute any more.
    // The object must then be addressed by the getObject() method directly.
    PyObject *method = Py_FindMethod(this->Methods, const_cast<DocumentPy*>(this), attr);
    if (method) {
        Py_DECREF(method);
        return 0;
    } else if (PyErr_Occurred()) {
        PyErr_Clear();
    }
    // search for an object with this name
    ViewProvider* obj = getDocumentPtr()->getViewProviderByName(attr);
    return (obj ? obj->getPyObject() : 0);
}

int DocumentPy::setCustomAttributes(const char* attr, PyObject *)
{
    // Note: Here we want to return only a document object if its
    // name matches 'attr'. However, it is possible to have an object
    // with the same name as an attribute. If so, we return 0 as other-
    // wise it wouldn't be possible to address this attribute any more.
    // The object must then be addressed by the getObject() method directly.
    PyObject *method = Py_FindMethod(this->Methods, this, attr);
    if (method) {
        Py_DECREF(method);
        return 0;
    } else if (PyErr_Occurred()) {
        PyErr_Clear();
    }
    ViewProvider* obj = getDocumentPtr()->getViewProviderByName(attr);
    if (obj) {
        std::stringstream str;
        str << "'Document' object attribute '" << attr 
            << "' must not be set this way" << std::ends;
        throw Py::AttributeError(str.str());
    }
    
    return 0;
}


