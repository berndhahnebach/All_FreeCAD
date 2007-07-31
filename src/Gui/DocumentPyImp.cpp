
#include "PreCompiled.h"

#include <Base/Matrix.h>
#include <App/MatrixPy.h>

#include "Document.h"
#include "ViewProviderExtern.h"

// inclusion of the generated files (generated out of DocumentPy.xml)
#include "DocumentPy.h"
#include "DocumentPy.cpp"

using namespace Gui;


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
	return 0;
}

int DocumentPy::setCustomAttributes(const char* attr, PyObject *obj)
{
	return 0; 
}


