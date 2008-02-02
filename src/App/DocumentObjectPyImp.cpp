
#include "PreCompiled.h"

#include "DocumentObject.h"

// inclusion of the generated files (generated out of DocumentObjectPy.xml)
#include "DocumentObjectPy.h"
#include "DocumentObjectPy.cpp"

using namespace App;

// returns a string which represent the object e.g. when printed in python
const char *DocumentObjectPy::representation(void) const
{
    return "<Document object>";
}

Py::String DocumentObjectPy::getName(void) const
{
    DocumentObject* object = this->getDocumentObjectPtr();
    const char* internal = object->getNameInDocument();
    if (!internal) {
        throw Py::RuntimeError(std::string("This object is currently not part of a document"));
    }
    return Py::String(std::string(internal));
}

PyObject*  DocumentObjectPy::touch(PyObject */*args*/)
{
    getDocumentObjectPtr()->touch();
    Py_Return;
}

Py::List DocumentObjectPy::getState(void) const
{
    DocumentObject* object = this->getDocumentObjectPtr();
    Py::List list;
    bool uptodate = true;
    if (object->isTouched()) {
        uptodate = false;
        list.append(Py::String("Touched"));
    }
    if (object->isError()) {
        uptodate = false;
        list.append(Py::String("Invalid"));
    }
    if (uptodate) {
        list.append(Py::String("Up-to-date"));
    }
    return list;
}

PyObject *DocumentObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int DocumentObjectPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    // search in PropertyList
    Property *prop = getDocumentObjectPtr()->getPropertyByName(attr);
    if (prop) {
        // Read-only attributes must not be set over its Python interface
        short Type =  getDocumentObjectPtr()->getPropertyType(prop);
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
