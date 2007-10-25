
#include "PreCompiled.h"

#include "BaseClass.h"

// inclusion of the generated files (generated out of BaseClassPy.xml)
#include "BaseClassPy.h"
#include "BaseClassPy.cpp"

using namespace Base;

// returns a string which represent the object e.g. when printed in python
const char *BaseClassPy::representation(void)
{
    return "The Python binding base class";
}


PyObject*  BaseClassPy::isDerivedFrom(PyObject *args)
{
    char *name;
    if (!PyArg_ParseTuple(args, "s", &name))     // convert args: Python->C 
        return NULL;                    // NULL triggers exception

    Base::Type type = Base::Type::fromName(name);
    if (type != Base::Type::badType() && getBaseClassObject()->getTypeId().isDerivedFrom(type)) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

PyObject*  BaseClassPy::getAllDerivedFrom(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                    // NULL triggers exception
    
    std::vector<Base::Type> ary;
    Base::Type::getAllDerivedFrom(getBaseClassObject()->getTypeId(), ary);
    Py::List res;
    for (std::vector<Base::Type>::iterator it = ary.begin(); it != ary.end(); ++it)
        res.append(Py::String(it->getName()));
    return Py::new_reference_to(res);
}

Py::String BaseClassPy::getType(void) const
{
    return Py::String(std::string(getBaseClassObject()->getTypeId().getName()));
}

Py::Int BaseClassPy::getModule(void) const
{
    return Py::Int();
}

PyObject *BaseClassPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int BaseClassPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}


