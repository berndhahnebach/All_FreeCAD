
#include "PreCompiled.h"

#include "DocumentObject.h"

// inclusion of the generated files (generated out of DocumentObjectPy.xml)
#include "DocumentObjectPy.h"
#include "DocumentObjectPy.cpp"

using namespace App;

// returns a string which represent the object e.g. when printed in python
const char *DocumentObjectPy::representation(void)
{
    return "DocumentObjectPy";
}

PyObject*  DocumentObjectPy::getInternalName(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))   // convert args: Python->C 
        return NULL;                   // NULL triggers exception 

    DocumentObject* object = this->getDocumentObjectObject();
    return PyString_FromString(object->getNameInDocument().c_str());
}

PyObject*  DocumentObjectPy::touch(PyObject *args)
{
    getDocumentObjectObject()->touch();
    Py_Return;
}

Py::List DocumentObjectPy::getState(void) const
{
    return Py::List();
}

PyObject *DocumentObjectPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int DocumentObjectPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    // search in PropertyList
    Property *prop = getDocumentObjectObject()->getPropertyByName(attr);
    if (prop) {
        // Read-only attributes must not be set over its Python interface
        short Type =  getDocumentObjectObject()->getPropertyType(prop);
        if (Type & Prop_ReadOnly) {
            std::stringstream s;
            s << "'DocumentObject' attribute '" << attr << "' is read-only"; 
            throw Py::AttributeError(s.str());
        }

        prop->setPyObject(obj);
        return 1;
    } 

    return 0;
}
