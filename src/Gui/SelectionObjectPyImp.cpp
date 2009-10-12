
#include "PreCompiled.h"

#include "Gui/SelectionObject.h"
#include "App/Document.h"
#include "App/DocumentObject.h"
#include "App/Application.h"

// inclusion of the generated files (generated out of SelectionObjectPy.xml)
#include "SelectionObjectPy.h"
#include "SelectionObjectPy.cpp"

using namespace Gui;

// returns a string which represents the object e.g. when printed in python
std::string SelectionObjectPy::representation(void) const
{
    return "<SelectionObject object>";
}



PyObject* SelectionObjectPy::remove(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* SelectionObjectPy::isA(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}



Py::String SelectionObjectPy::getObjectName(void) const
{
    return Py::String(getSelectionObjectPtr()->getFeatName());
}

Py::String SelectionObjectPy::getSubElementName(void) const
{
    return Py::String(/*getSelectionObjectPtr()->getSubName()*/);
}

Py::String SelectionObjectPy::getFullName(void) const
{
	std::string buf;
	//buf = getSelectionObjectPtr()->getDocName();
	//buf += ".";
	//buf += getSelectionObjectPtr()->getFeatName();
	//if(getSelectionObjectPtr()->getSubName()){
	//	buf += ".";
	//	buf += getSelectionObjectPtr()->getSubName();
	//}
    return Py::String(buf.c_str());
}

Py::String SelectionObjectPy::getDocumentName(void) const
{
    return Py::String(getSelectionObjectPtr()->getDocName());
}

Py::Object SelectionObjectPy::getDocument(void) const
{
	//App::GetApplication()->getDocument
    return Py::Object();
}

Py::Object SelectionObjectPy::getObject(void) const
{
    return Py::Object(getSelectionObjectPtr()->getObject()->getPyObject());
}

Py::Object SelectionObjectPy::getSubObject(void) const
{
    return Py::Object();
}

Py::Boolean SelectionObjectPy::getHasSubObject(void) const
{
    return Py::Boolean();
}

PyObject *SelectionObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int SelectionObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


