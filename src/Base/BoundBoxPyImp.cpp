
#include "PreCompiled.h"

#include "Base/BoundBox.h"

// inclusion of the generated files (generated out of BoundBoxPy.xml)
#include "BoundBoxPy.h"
#include "VectorPy.h"
#include "BoundBoxPy.cpp"

using namespace Base;

// returns a string which represent the object e.g. when printed in python
const char *BoundBoxPy::representation(void) const
{
    std::stringstream str;
    str << "BoundBox (";
    str << getBoundBoxPtr()->MinX << ", "
        << getBoundBoxPtr()->MinY << ", "
        << getBoundBoxPtr()->MinZ << ", "
        << getBoundBoxPtr()->MaxX << ", "
        << getBoundBoxPtr()->MaxY << ", "
        << getBoundBoxPtr()->MaxZ ;
    str << ")";

    // Note: As the return type is 'const char*' we cannot create a temporary char array neither on the stack because the array would be freed
    // when leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class. This, however, is not a problem as long as we only
    // use this method in _repr().
    static std::string buf;
    buf = str.str();
    return buf.c_str();
}

PyObject *BoundBoxPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of BoundBoxPy and the Twin object 
    return new BoundBoxPy(new BoundBox3d);
}

// constructor method
int BoundBoxPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    double xMin=0.0,yMin=0.0,zMin=0.0,xMax=0.0,yMax=0.0,zMax=0.0;
    PyObject *object1, *object2;
    BoundBoxPy::PointerType ptr = getBoundBoxPtr();
    if (PyArg_ParseTuple(args, "|dddddd", &xMin, &yMin, &zMin, &xMax, &yMax, &zMax)) {
        ptr->MaxX = xMax;
        ptr->MaxY = yMax;
        ptr->MaxZ = zMax;
        ptr->MinX = xMin;
        ptr->MinY = yMin;
        ptr->MinZ = zMin;
    }
    else if (PyArg_ParseTuple(args,"O!O!",&(Base::VectorPy::Type), &object1,&(Base::VectorPy::Type), &object2)) {
        PyErr_Clear(); // set by PyArg_ParseTuple()
        // Note: must be static_cast, not reinterpret_cast
        ptr->Add(*(static_cast<Base::VectorPy*>(object1)->getVectorPtr()));
        ptr->Add(*(static_cast<Base::VectorPy*>(object2)->getVectorPtr()));
    }
    else if (PyArg_ParseTuple(args,"O!",&(Base::BoundBoxPy::Type), &object1)) {
        PyErr_Clear(); // set by PyArg_ParseTuple()
        // Note: must be static_cast, not reinterpret_cast
        *ptr = *(static_cast<Base::BoundBoxPy*>(object1)->getBoundBoxPtr());
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Either six floats, two instances of Vector or instance of BoundBox expected");
        return -1;
    }

    return 0;
}

PyObject*  BoundBoxPy::add(PyObject *args)
{
    double x,y,z;
    PyObject *object;
    if (PyArg_ParseTuple(args, "ddd", &x,&y,&z)) {
        BoundBox().Add(Vector3d(x,y,z));
    }
    else if (PyArg_ParseTuple(args,"O!",&(Base::VectorPy::Type), &object)) {
        PyErr_Clear();
        BoundBox().Add(*(static_cast<Base::VectorPy*>(object)->getVectorPtr()));
    }
    else if (PyArg_ParseTuple(args,"O!;Need a Vector, BoundBox or three floats as argument",&(Base::BoundBoxPy::Type), &object)) {
        PyErr_Clear();
        BoundBox().Add(*(static_cast<Base::BoundBoxPy*>(object)->getBoundBoxPtr()));
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Either three floats, instance of Vector or instance of BoundBox expected");
        return 0;
    }

    Py_Return;
}

PyObject*  BoundBoxPy::isIntersection(PyObject *args)
{
    double x,y,z;
    PyObject *object,*object2;
    Py::Bool retVal;
    if (PyArg_ParseTuple(args, "ddd", &x,&y,&z)) {
        retVal = BoundBox().IsInBox(Vector3d(x,y,z));
    }
    else if (PyArg_ParseTuple(args,"O!",&(Base::VectorPy::Type), &object)) {
        PyErr_Clear();
        retVal = BoundBox().IsInBox(*(static_cast<Base::VectorPy*>(object)->getVectorPtr()));
    }
    else if (PyArg_ParseTuple(args,"O!O!",&(Base::VectorPy::Type), &object,&(Base::VectorPy::Type), &object2)) {
        PyErr_Clear();
        retVal = BoundBox().IsCutLine(
            *(static_cast<Base::VectorPy*>(object )->getVectorPtr()),
            *(static_cast<Base::VectorPy*>(object2)->getVectorPtr()));
    }
    else if (PyArg_ParseTuple(args,"O!;Need vector, bounding box or three floats as argument",&(Base::BoundBoxPy::Type), &object)) {
        PyErr_Clear();
        retVal = BoundBox().IsInBox(*(static_cast<Base::BoundBoxPy*>(object)->getBoundBoxPtr()));
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Either three floats, Vector(s) or BoundBox expected");
        return 0;
    }

    return Py::new_reference_to(retVal);
}

PyObject*  BoundBoxPy::enlarge(PyObject *args)
{
    double s;
    if (!PyArg_ParseTuple(args, "d;Need float parameter to enlarge", &s))
        return 0;
    BoundBox().Enlarge(s);
    Py_Return;
}

PyObject*  BoundBoxPy::getIntersectionPoint(PyObject *args)
{
    PyObject *object,*object2;
    if (PyArg_ParseTuple(args,"O!O!:Need base and direction vector",&(Base::VectorPy::Type), &object,&(Base::VectorPy::Type), &object2)) {
        Base::Vector3d point = BoundBox().IntersectionPoint(
            *(static_cast<Base::VectorPy*>(object)->getVectorPtr()),
            *(static_cast<Base::VectorPy*>(object2)->getVectorPtr()));
        // IsInBox() doesn't handle border points correctly
        BoundBoxPy::PointerType bb = getBoundBoxPtr();
        if ((bb->MinX <= point.x && bb->MaxX >= point.x) &&
            (bb->MinY <= point.y && bb->MaxY >= point.y) &&
            (bb->MinZ <= point.z && bb->MaxZ >= point.z)) {
            return new VectorPy(point);
        }
        else {
            PyErr_SetString(PyExc_Exception, "No intersection");
            return 0;
        }
    }
    else
        return 0;
}

PyObject*  BoundBoxPy::move(PyObject *args)
{
   double x,y,z;
    PyObject *object;
    Base::Vector3d vec;

    if (PyArg_ParseTuple(args, "ddd", &x,&y,&z)) {
        vec = Vector3d(x,y,z);
    }
    else if (PyArg_ParseTuple(args,"O!:Need vector to move",&(Base::VectorPy::Type), &object)) {
        PyErr_Clear();
        vec = *(static_cast<Base::VectorPy*>(object)->getVectorPtr());
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Either three floats or vector expected");
        return 0;
    }

    BoundBox().MoveX(vec.x);
    BoundBox().MoveY(vec.y);
    BoundBox().MoveZ(vec.z);

    Py_Return;
}

PyObject*  BoundBoxPy::isCutPlane(PyObject *args)
{
    PyObject *object,*object2;
    Py::Bool retVal;

    if (PyArg_ParseTuple(args,"O!O!:Need base and normal vector of a plane",&(Base::VectorPy::Type), &object,&(Base::VectorPy::Type), &object2))
        retVal = BoundBox().IsCutPlane(
            *(static_cast<Base::VectorPy*>(object)->getVectorPtr()),
            *(static_cast<Base::VectorPy*>(object2)->getVectorPtr()));
    else
        return 0;

    return Py::new_reference_to(retVal);
}

Py::Object BoundBoxPy::getCenter(void) const
{
    return Py::Object(new VectorPy(new Vector3d(BoundBox().CalcCenter())));
}

Py::Float BoundBoxPy::getXMax(void) const
{
    return Py::Float(BoundBox().MaxX);
}

void  BoundBoxPy::setXMax(Py::Float arg)
{
    BoundBox().MaxX = arg;
}

Py::Float BoundBoxPy::getYMax(void) const
{
    return Py::Float(BoundBox().MaxY);
}

void  BoundBoxPy::setYMax(Py::Float arg)
{
    BoundBox().MaxY = arg;
}

Py::Float BoundBoxPy::getZMax(void) const
{
    return Py::Float(BoundBox().MaxZ);
}

void  BoundBoxPy::setZMax(Py::Float arg)
{
    BoundBox().MaxZ = arg;
}

Py::Float BoundBoxPy::getXMin(void) const
{
    return Py::Float(BoundBox().MinX);
}

void  BoundBoxPy::setXMin(Py::Float arg)
{
    BoundBox().MinX = arg;
}

Py::Float BoundBoxPy::getYMin(void) const
{
    return Py::Float(BoundBox().MinY);
}

void  BoundBoxPy::setYMin(Py::Float arg)
{
    BoundBox().MinY = arg;
}

Py::Float BoundBoxPy::getZMin(void) const
{
    return Py::Float(BoundBox().MinZ);
}

void  BoundBoxPy::setZMin(Py::Float arg)
{
    BoundBox().MinZ = arg;
}

Py::Float BoundBoxPy::getXLength(void) const
{
    return Py::Float(BoundBox().LengthX());
}

Py::Float BoundBoxPy::getYLength(void) const
{
    return Py::Float(BoundBox().LengthY());
}

Py::Float BoundBoxPy::getZLength(void) const
{
    return Py::Float(BoundBox().LengthZ());
}

Py::Float BoundBoxPy::getDiagonalLength(void) const
{
    return Py::Float(BoundBox().CalcDiagonalLength());
}

PyObject *BoundBoxPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int BoundBoxPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


