
#include "PreCompiled.h"

#include <Base/Matrix.h>
#include <App/MatrixPy.h>

#include "Document.h"
#include "ViewProviderExtern.h"

// inclusion of the generated files (generated out of DocumentPy.xml)
#include "DocumentPy.h"
#include "DocumentPy.cpp"

using namespace Gui;

// returns a string which represent the object e.g. when printed in python
const char *DocumentPy::representation(void)
{
	return "DocumentPy";
}


PyObject*  DocumentPy::show(PyObject *args)
{
    char *psFeatStr;
    if (!PyArg_ParseTuple(args, "s;Name of the Feature to show have to be given!",&psFeatStr))     // convert args: Python->C 
        return NULL;  // NULL triggers exception 

    PY_TRY {
        getDocumentObject()->setShow(psFeatStr);  
        Py_Return;
    } PY_CATCH;
}

PyObject*  DocumentPy::hide(PyObject *args)
{
    char *psFeatStr;
    if (!PyArg_ParseTuple(args, "s;Name of the Feature to hide have to be given!",&psFeatStr))     // convert args: Python->C 
        return NULL;  // NULL triggers exception 

    PY_TRY {
        getDocumentObject()->setHide(psFeatStr);  
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
        getDocumentObject()->setPos(psFeatStr,mat);  
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
        pcExt->adjustDocumentName(getDocumentObject()->getDocument()->getName());

        getDocumentObject()->setAnotationViewProvider(psAnnoName,pcExt);

        Py_Return;

    } PY_CATCH;
}

PyObject*  DocumentPy::update(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 

    PY_TRY {
        getDocumentObject()->update();
        Py_Return;
    } PY_CATCH;
}

PyObject*  DocumentPy::activeObject(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 

    PY_TRY {
        App::DocumentObject *pcFtr = getDocumentObject()->getDocument()->getActiveObject();
        if (pcFtr) {
            ViewProvider *pcView = getDocumentObject()->getViewProvider(pcFtr);
	        return pcView->getPyObject();
        } else {
		    Py_Error(PyExc_Exception,"No active Object");
        }
    } PY_CATCH;
}

PyObject*  DocumentPy::getObject(PyObject *args)
{
    char *sName;
    if (!PyArg_ParseTuple(args, "s",&sName))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    PY_TRY {
        ViewProvider *pcView = getDocumentObject()->getViewProviderByName(sName);
	    if (pcView)
		    return pcView->getPyObject();
        else {
            Py_Return;
        }
    } PY_CATCH;
}

PyObject*  DocumentPy::activeView(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    PY_TRY {
        Gui::MDIView  *pcView = getDocumentObject()->getActiveView();
        if (pcView){
            // already incremented in getPyObject().
            return pcView->getPyObject();
        } else {
            Py_Return;
        }
    } PY_CATCH;
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
    ViewProvider* obj = getDocumentObject()->getViewProviderByName(attr);
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
    ViewProvider* obj = getDocumentObject()->getViewProviderByName(attr);
    if (obj)
    {
        char szBuf[200];
        snprintf(szBuf, 200, "'Document' object attribute '%s' must not be set this way", attr);
        throw Py::AttributeError(szBuf); 
    }
    
    return 0;
}


