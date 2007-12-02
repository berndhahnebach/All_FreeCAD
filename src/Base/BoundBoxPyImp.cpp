
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

// constructor method
int BoundBoxPy::PyInit(PyObject* args, PyObject*k)
{
    double xMin=0.0,yMin=0.0,zMin=0.0,xMax=0.0,yMax=0.0,zMax=0.0;
    if (!PyArg_ParseTuple(args, "|dddddd", &xMin, &yMin, &zMin, &xMax, &yMax, &zMax))
        return -1;

    getBoundBoxPtr()->MaxX = xMax;
    getBoundBoxPtr()->MaxY = yMax;
    getBoundBoxPtr()->MaxZ = zMax;
    getBoundBoxPtr()->MinX = xMin;
    getBoundBoxPtr()->MinY = yMin;
    getBoundBoxPtr()->MinZ = zMin;

    return 0;
}


PyObject*  BoundBoxPy::add(PyObject *args)
{
    double x,y,z;
    PyObject *object;
    if (PyArg_ParseTuple(args, "ddd", &x,&y,&z))
        BoundBox().Add(Vector3d(x,y,z));
    else if (PyArg_ParseTuple(args,"O!",&(Base::VectorPy::Type), &object))
        BoundBox().Add( *(reinterpret_cast<Base::VectorPy*>(object)->getVectorPtr()) );
    else if (PyArg_ParseTuple(args,"O!;Need a Vector, BoundBox or three floats as argument",&(Base::BoundBoxPy::Type), &object))
        BoundBox().Add( *(reinterpret_cast<Base::BoundBoxPy*>(object)->getBoundBoxPtr()));
    else
        return 0;
    Py_Return;
}

PyObject*  BoundBoxPy::isIntersection(PyObject *args)
{
    double x,y,z;
    PyObject *object,*object2;
    Py::Int retVal;
    if (PyArg_ParseTuple(args, "ddd", &x,&y,&z))
        retVal = BoundBox().IsInBox(Vector3d(x,y,z))?1:0;
    else if (PyArg_ParseTuple(args,"O!",&(Base::VectorPy::Type), &object))
        retVal = BoundBox().IsInBox(*(reinterpret_cast<Base::VectorPy*>(object)->getVectorPtr()))?1:0 ;
    else if (PyArg_ParseTuple(args,"O!O!",&(Base::VectorPy::Type), &object,&(Base::VectorPy::Type), &object2))
        retVal = BoundBox().IsCutLine(
            *(reinterpret_cast<Base::VectorPy*>(object)->getVectorPtr()),
            *(reinterpret_cast<Base::VectorPy*>(object2)->getVectorPtr())
                                     )?1:0 ;
    else if (PyArg_ParseTuple(args,"O!;Need a Vector, BoundBox or three floats as argument",&(Base::BoundBoxPy::Type), &object))
        retVal = BoundBox().IsInBox(*(reinterpret_cast<Base::BoundBoxPy*>(object)->getBoundBoxPtr()))?1:0 ;
    else
        return 0;
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
    if (PyArg_ParseTuple(args,"O!O!:Need Base and Direction Vector",&(Base::VectorPy::Type), &object,&(Base::VectorPy::Type), &object2))
        return new VectorPy(new Vector3d(BoundBox().IntersectionPoint(
            *(reinterpret_cast<Base::VectorPy*>(object)->getVectorPtr()),
            *(reinterpret_cast<Base::VectorPy*>(object2)->getVectorPtr())
                                     )));
    else
        return 0;

}

PyObject*  BoundBoxPy::move(PyObject *args)
{
   double x,y,z;
    PyObject *object;
    Base::Vector3d vec;

    if (PyArg_ParseTuple(args, "ddd", &x,&y,&z))
        vec = Vector3d(x,y,z);
    else if (PyArg_ParseTuple(args,"O!:Need a Vector to move",&(Base::VectorPy::Type), &object))
        vec = *(reinterpret_cast<Base::VectorPy*>(object)->getVectorPtr()) ;
    else 
        return 0;

    BoundBox().MoveX(vec.x);
    BoundBox().MoveY(vec.y);
    BoundBox().MoveZ(vec.z);

    Py_Return;
}

PyObject*  BoundBoxPy::isCutPlane(PyObject *args)
{
    PyObject *object,*object2;
    Py::Int retVal;

    if (PyArg_ParseTuple(args,"O!O!:Need Base and Normal Vector of a Plane",&(Base::VectorPy::Type), &object,&(Base::VectorPy::Type), &object2))
        retVal = BoundBox().IsCutPlane(
            *(reinterpret_cast<Base::VectorPy*>(object)->getVectorPtr()),
            *(reinterpret_cast<Base::VectorPy*>(object2)->getVectorPtr())
            )?1:0;
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
    return Py::Float();
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

PyObject *BoundBoxPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int BoundBoxPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}


