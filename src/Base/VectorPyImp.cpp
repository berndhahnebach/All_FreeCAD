
#include "PreCompiled.h"

#ifndef _PreComp_
# include <sstream>
#endif

#include "Vector3D.h"

// inclusion of the generated files (generated out of VectorPy.xml)
#include "VectorPy.h"
#include "VectorPy.cpp"

using namespace Base;

// returns a string which represent the object e.g. when printed in python
const char *VectorPy::representation(void) const
{
    VectorPy::PointerType ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    std::stringstream str;
    str << "Vector (";
    str << ptr->x << ", "<< ptr->y << ", "<< ptr->z;
    str << ")";

    // Note: As the return type is 'const char*' we cannot create a temporary char array neither on the stack because the array would be freed
    // when leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class. This, however, is not a problem as long as we only
    // use this method in _repr().
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

// constructor method
int VectorPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    double  x=0.0,y=0.0,z=0.0;
    PyObject *object;
    VectorPy::PointerType ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    if (PyArg_ParseTuple(args, "|ddd", &x,&y,&z)) {
        ptr->Set(x,y,z);
    }
    else if (PyArg_ParseTuple(args,"O!",&(Base::VectorPy::Type), &object)) {
        PyErr_Clear(); // set by PyArg_ParseTuple()
        // Note: must be static_cast, not reinterpret_cast
        *ptr = *(static_cast<Base::VectorPy*>(object)->getVectorPtr());
    }
    else if (PyArg_ParseTuple(args,"O!",&(PyTuple_Type), &object)) {
        PyErr_Clear(); // set by PyArg_ParseTuple()
        try {
            Py::Tuple tuple(object);
            ptr->x = (float)Py::Float(tuple.getItem(0));
            ptr->y = (float)Py::Float(tuple.getItem(1));
            ptr->z = (float)Py::Float(tuple.getItem(2));
        }
        catch (const Py::Exception&) {
            return -1;
        }
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Either three floats, tuple or Vector expected");
        return -1;
    }

    return 0;
}

PyObject*  VectorPy::add(PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O!", &(VectorPy::Type), &obj))
        return 0;

    VectorPy* vec = static_cast<VectorPy*>(obj);

    VectorPy::PointerType this_ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    VectorPy::PointerType vect_ptr = reinterpret_cast<VectorPy::PointerType>(vec->_pcTwinPointer);

    Base::Vector3d v = (*this_ptr) + (*vect_ptr);
    return new VectorPy(v);
}

PyObject*  VectorPy::sub(PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O!", &(VectorPy::Type), &obj))
        return 0;

    VectorPy* vec = static_cast<VectorPy*>(obj);

    VectorPy::PointerType this_ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    VectorPy::PointerType vect_ptr = reinterpret_cast<VectorPy::PointerType>(vec->_pcTwinPointer);

    Base::Vector3d v = (*this_ptr) - (*vect_ptr);
    return new VectorPy(v);
}

PyObject*  VectorPy::scale(PyObject *args)
{
    double factorX, factorY, factorZ;
    if (!PyArg_ParseTuple(args, "ddd", &factorX, &factorY, &factorZ))
        return 0;
    VectorPy::PointerType ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    ptr->Scale(factorX, factorY, factorZ);
    Py_Return;
}

PyObject*  VectorPy::dot(PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O!", &(VectorPy::Type), &obj))
        return 0;

    VectorPy* vec = static_cast<VectorPy*>(obj);

    VectorPy::PointerType this_ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    VectorPy::PointerType vect_ptr = reinterpret_cast<VectorPy::PointerType>(vec->_pcTwinPointer);

    Py::Float mult((*this_ptr) * (*vect_ptr));
    return Py::new_reference_to(mult);
}

PyObject*  VectorPy::cross(PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O!", &(VectorPy::Type), &obj))
        return 0;

    VectorPy* vec = static_cast<VectorPy*>(obj);

    VectorPy::PointerType this_ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    VectorPy::PointerType vect_ptr = reinterpret_cast<VectorPy::PointerType>(vec->_pcTwinPointer);

    Base::Vector3d v = (*this_ptr) % (*vect_ptr);
    return new VectorPy(v);
}

PyObject*  VectorPy::getAngle(PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O!", &(VectorPy::Type), &obj))
        return 0;

    VectorPy* vec = static_cast<VectorPy*>(obj);

    VectorPy::PointerType this_ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    VectorPy::PointerType vect_ptr = reinterpret_cast<VectorPy::PointerType>(vec->_pcTwinPointer);

    Py::Float angle(this_ptr->GetAngle(*vect_ptr));
    return Py::new_reference_to(angle);
}

PyObject*  VectorPy::normalize(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return 0;
    VectorPy::PointerType ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    if (ptr->Length() < 1.0e-6) {
        PyErr_SetString(PyExc_Exception, "Cannot normalize null vector");
        return 0;
    }

    ptr->Normalize();
    Py_Return;
}

PyObject*  VectorPy::projectToLine(PyObject *args)
{
    PyObject *base, *line;
    if (!PyArg_ParseTuple(args, "OO",&base, &line))
        return 0;
    if (!PyObject_TypeCheck(base, &(VectorPy::Type))) {
        PyErr_SetString(PyExc_TypeError, "First arg must be Vector");
        return 0;
    }
    if (!PyObject_TypeCheck(line, &(VectorPy::Type))) {
        PyErr_SetString(PyExc_TypeError, "Second arg must be Vector");
        return 0;
    }

    VectorPy* base_vec = static_cast<VectorPy*>(base);
    VectorPy* line_vec = static_cast<VectorPy*>(line);

    VectorPy::PointerType this_ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    VectorPy::PointerType base_ptr = reinterpret_cast<VectorPy::PointerType>(base_vec->_pcTwinPointer);
    VectorPy::PointerType line_ptr = reinterpret_cast<VectorPy::PointerType>(line_vec->_pcTwinPointer);

    this_ptr->ProjToLine(*base_ptr, *line_ptr);
    Py_Return;
}

PyObject*  VectorPy::projectToPlane(PyObject *args)
{
    PyObject *base, *line;
    if (!PyArg_ParseTuple(args, "OO",&base, &line))
        return 0;
    if (!PyObject_TypeCheck(base, &(VectorPy::Type))) {
        PyErr_SetString(PyExc_TypeError, "First arg must be Vector");
        return 0;
    }
    if (!PyObject_TypeCheck(line, &(VectorPy::Type))) {
        PyErr_SetString(PyExc_TypeError, "Second arg must be Vector");
        return 0;
    }

    VectorPy* base_vec = static_cast<VectorPy*>(base);
    VectorPy* line_vec = static_cast<VectorPy*>(line);

    VectorPy::PointerType this_ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    VectorPy::PointerType base_ptr = reinterpret_cast<VectorPy::PointerType>(base_vec->_pcTwinPointer);
    VectorPy::PointerType line_ptr = reinterpret_cast<VectorPy::PointerType>(line_vec->_pcTwinPointer);

    this_ptr->ProjToPlane(*base_ptr, *line_ptr);
    Py_Return;
}

PyObject*  VectorPy::distanceToLine(PyObject *args)
{
    PyObject *base, *line;
    if (!PyArg_ParseTuple(args, "OO",&base, &line))
        return 0;
    if (!PyObject_TypeCheck(base, &(VectorPy::Type))) {
        PyErr_SetString(PyExc_TypeError, "First arg must be Vector");
        return 0;
    }
    if (!PyObject_TypeCheck(line, &(VectorPy::Type))) {
        PyErr_SetString(PyExc_TypeError, "Second arg must be Vector");
        return 0;
    }

    VectorPy* base_vec = static_cast<VectorPy*>(base);
    VectorPy* line_vec = static_cast<VectorPy*>(line);

    VectorPy::PointerType this_ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    VectorPy::PointerType base_ptr = reinterpret_cast<VectorPy::PointerType>(base_vec->_pcTwinPointer);
    VectorPy::PointerType line_ptr = reinterpret_cast<VectorPy::PointerType>(line_vec->_pcTwinPointer);

    Py::Float dist(this_ptr->DistanceToLine(*base_ptr, *line_ptr));
    return Py::new_reference_to(dist);
}

PyObject*  VectorPy::distanceToPlane(PyObject *args)
{
    PyObject *base, *line;
    if (!PyArg_ParseTuple(args, "OO",&base, &line))
        return 0;
    if (!PyObject_TypeCheck(base, &(VectorPy::Type))) {
        PyErr_SetString(PyExc_TypeError, "First arg must be Vector");
        return 0;
    }
    if (!PyObject_TypeCheck(line, &(VectorPy::Type))) {
        PyErr_SetString(PyExc_TypeError, "Second arg must be Vector");
        return 0;
    }

    VectorPy* base_vec = static_cast<VectorPy*>(base);
    VectorPy* line_vec = static_cast<VectorPy*>(line);

    VectorPy::PointerType this_ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    VectorPy::PointerType base_ptr = reinterpret_cast<VectorPy::PointerType>(base_vec->_pcTwinPointer);
    VectorPy::PointerType line_ptr = reinterpret_cast<VectorPy::PointerType>(line_vec->_pcTwinPointer);

    Py::Float dist(this_ptr->DistanceToPlane(*base_ptr, *line_ptr));
    return Py::new_reference_to(dist);
}

Py::Float VectorPy::getLength(void) const
{
    VectorPy::PointerType ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    return Py::Float(ptr->Length());
}

void  VectorPy::setLength(Py::Float arg)
{
    VectorPy::PointerType ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    double len = ptr->Length();
    if (len < 1.0e-6) {
        throw Py::Exception(std::string("Cannot set length of null vector"));
    }

    double val = (double)arg/len;
    ptr->x *= val;
    ptr->y *= val;
    ptr->z *= val;
}

Py::Float VectorPy::getx(void) const
{
    VectorPy::PointerType ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    return Py::Float(ptr->x);
}

void  VectorPy::setx(Py::Float arg)
{
    VectorPy::PointerType ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    ptr->x = (double)arg;
}

Py::Float VectorPy::gety(void) const
{
    VectorPy::PointerType ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    return Py::Float(ptr->y);
}

void  VectorPy::sety(Py::Float arg)
{
    VectorPy::PointerType ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    ptr->y = (double)arg;
}

Py::Float VectorPy::getz(void) const
{
    VectorPy::PointerType ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    return Py::Float(ptr->z);
}

void  VectorPy::setz(Py::Float arg)
{
    VectorPy::PointerType ptr = reinterpret_cast<VectorPy::PointerType>(_pcTwinPointer);
    ptr->z = (double)arg;
}

PyObject *VectorPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int VectorPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


