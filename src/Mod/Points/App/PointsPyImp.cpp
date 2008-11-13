
#include "PreCompiled.h"

#include "Mod/Points/App/Points.h"
#include <Base/VectorPy.h>

// inclusion of the generated files (generated out of PointsPy.xml)
#include "PointsPy.h"
#include "PointsPy.cpp"

using namespace Points;

// returns a string which represents the object e.g. when printed in python
const char *PointsPy::representation(void) const
{
    return "<PointKernel object>";
}

PyObject *PointsPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of PointsPy and the Twin object 
    return new PointsPy(new PointKernel);
}

// constructor method
int PointsPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject *pcObj=0;
    if (!PyArg_ParseTuple(args, "|O", &pcObj))     // convert args: Python->C 
        return -1;                             // NULL triggers exception

    // if no mesh is given
    if (!pcObj) return 0;
    if (PyObject_TypeCheck(pcObj, &(PointsPy::Type))) {
        *getPointKernelPtr() = *(static_cast<PointsPy*>(pcObj)->getPointKernelPtr());
    }
    else if (PyList_Check(pcObj)) {
        addPoints(args);
    }
    else if (PyTuple_Check(pcObj)) {
        addPoints(args);
    }
    else if (PyString_Check(pcObj)) {
        getPointKernelPtr()->load(PyString_AsString(pcObj));
    }

    return 0;
}


PyObject* PointsPy::read(PyObject * args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;                         

    PY_TRY {
        getPointKernelPtr()->load(Name);
    } PY_CATCH;
    
    Py_Return; 
}

PyObject* PointsPy::write(PyObject * args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;                         

    PY_TRY {
        getPointKernelPtr()->save(Name);
    } PY_CATCH;
    
    Py_Return; 
}

PyObject* PointsPy::addPoints(PyObject * args)
{
    PyObject *list;
    double v[3];
    if (PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) {
        int k = 0;
        for (int i = 0; i < PyList_Size(list); i++) {
            PyObject *vec = PyList_GetItem(list, i);            
            if (PyList_Check(vec)) {
                if (PyList_Size(vec) == 3) {
                    for (int j = 0; j < 3; j++) {
                        PyObject *val = PyList_GetItem(vec, j);
                        if (PyFloat_Check(val)) {
                            double f = PyFloat_AsDouble(val);
                            v[j] = f;
                        }
                        else {
                            Py_Error(PyExc_Exception, "vector needs 3 double values");
                            return NULL; // not a double
                        }
                    }
                    getPointKernelPtr()->push_back(Base::Vector3d(v[0],v[1],v[2]));
                }
                else {
                    Py_Error(PyExc_Exception, "vector needs 3 double values");
                    return NULL; // vector needs 3 doubles
                }
            }else if (PyObject_TypeCheck(vec, &(Base::VectorPy::Type))) {
                getPointKernelPtr()->push_back(*(static_cast<Base::VectorPy*>(vec)->getVectorPtr()));
            }else {
                Py_Error(PyExc_Exception, "inner list should be 3 doubles as list");
                return NULL; // not a vector
            }
        
         }

        Py_Return;
    }

    //PyErr_Clear();
    //if (PyArg_ParseTuple(args, "O!", &PyTuple_Type, &list)) {
    //    Py::Tuple tuple(list);
    //    Py::List list_v(tuple.getItem(0));
    //    union PyType_Object pyVertType = {&(Base::VectorPy::Type)};
    //    Py::Type vType(pyVertType.o);
    //    for (Py::List::iterator it = list_v.begin(); it != list_v.end(); ++it) {
    //        if ((*it).isType(vType)) {
    //            Base::Vector3d v = static_cast<Base::VectorPy*>((*it).ptr())->value();
    //            getPointKernelPtr()->addPoint(v);

    //        }
    //    }

    //    Py_Return;
    //}

    PyErr_SetString(PyExc_Exception, "either expect\n"
        "-- [Vector,...] \n"
        "-- [[x,y,z],...]");
    return NULL;
}

Py::Int PointsPy::getCountPoints(void) const
{
    return Py::Int((long)getPointKernelPtr()->size());
}

Py::List PointsPy::getPoints(void) const
{
    Py::List PointList;
    unsigned int Index=0;
    PointKernel* points = getPointKernelPtr();
    for (PointKernel::const_point_iterator it = points->begin(); it != points->end(); ++it) {
        PointList.append(Py::Object(new Base::VectorPy(*it)));
    }
    return PointList;
}

PyObject *PointsPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int PointsPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


