
#include "PreCompiled.h"

#include "MeshPoint.h"



// inclusion of the generated files (generated out of MeshPointPy.xml)
#include "MeshPointPy.h"
#include "MeshPointPy.cpp"

using namespace Mesh;

// returns a string which represent the object e.g. when printed in python
const char *MeshPointPy::representation(void) const
{
    MeshPointPy::PointerType ptr = getMeshPointPtr();
    std::stringstream str;
    str << "MeshPoint (";
    if(ptr->isBound())
        str << ptr->x << ", "<< ptr->y << ", "<< ptr->z << ", Idx=" << ptr->Index;
    else
        str << ptr->x << ", "<< ptr->y << ", "<< ptr->z ;
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
int MeshPointPy::PyInit(PyObject* args, PyObject*k)
{
    double  x=0.0,y=0.0,z=0.0;
    if (!PyArg_ParseTuple(args, "|ddd", &x,&y,&z))
        return -1;

    getMeshPointPtr()->Set(x,y,z);
    return 0;
}


PyObject*  MeshPointPy::unbound(PyObject *args)
{

    getMeshPointPtr()->Index = UINT_MAX;
    getMeshPointPtr()->Mesh = 0;
    Py_Return;
}

PyObject*  MeshPointPy::move(PyObject *args)
{
    if(! getMeshPointPtr()->isBound() )
        PyErr_SetString(PyExc_NotImplementedError, "This object is not bounded to a mesh, so no topological operation is possible!");

    double  x=0.0,y=0.0,z=0.0;
    PyObject *object;
    Base::Vector3d vec;
    if (PyArg_ParseTuple(args, "ddd", &x,&y,&z))
        vec.Set(x,y,z);
    else if (PyArg_ParseTuple(args,"O!",&(Base::VectorPy::Type), &object))
        vec = *(reinterpret_cast<Base::VectorPy*>(object)->getVectorPtr());
    else
        return 0;

    getMeshPointPtr()->Mesh->movePoint(getMeshPointPtr()->Index,vec);

    Py_Return;
}

Py::Int MeshPointPy::getIndex(void) const
{
    return Py::Int((long) getMeshPointPtr()->Index);
}

Py::Int MeshPointPy::getBound(void) const
{
    return Py::Int(getMeshPointPtr()->Index != UINT_MAX ? 1:0);
}

Py::Object MeshPointPy::getNormal(void) const
{
    if(! getMeshPointPtr()->isBound() )
        PyErr_SetString(PyExc_NotImplementedError, "This object is not bounded to a mesh, so no topological operation is possible!");

    return Py::Object(new VectorPy(new Vector3d(getMeshPointPtr()->Mesh->getPointNormal(getMeshPointPtr()->Index))));
}


PyObject *MeshPointPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int MeshPointPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}


