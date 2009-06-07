
#include "PreCompiled.h"
#ifndef _PreComp_
# include <Inventor/nodes/SoNode.h>
#endif

#include "Gui/ViewProviderPythonFeature.h"
#include <Base/Interpreter.h>

// inclusion of the generated files (generated out of ViewProviderPythonFeaturePy.xml)
#include "ViewProviderPythonFeaturePy.h"
#include "ViewProviderPythonFeaturePy.cpp"

using namespace Gui;

// returns a string which represents the object e.g. when printed in python
const char *ViewProviderPythonFeaturePy::representation(void) const
{
    return "<ViewProviderPythonFeature object>";
}

PyObject* ViewProviderPythonFeaturePy::addDisplayMode(PyObject * args)
{
    char* mode;
    PyObject* obj;
    if (!PyArg_ParseTuple(args, "Os", &obj, &mode))
        return NULL;

    void* ptr = 0;
    try {
        Base::Interpreter().convertSWIGPointerObj("SoNode *", obj, &ptr, 0);
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return 0;
    }

    PY_TRY {
        SoNode* node = reinterpret_cast<SoNode*>(ptr);
        getViewProviderPythonFeaturePtr()->addDisplayMode(node,mode);
        Py_Return;
    } PY_CATCH;
}

PyObject *ViewProviderPythonFeaturePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int ViewProviderPythonFeaturePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
