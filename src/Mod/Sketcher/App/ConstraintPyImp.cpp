
#include "PreCompiled.h"
#include <strstream>
#include "Mod/Sketcher/App/Constraint.h"

// inclusion of the generated files (generated out of ConstraintPy.xml)
#include "ConstraintPy.h"
#include "ConstraintPy.cpp"

using namespace Sketcher;

PyObject *ConstraintPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of ConstraintPy and the Twin object 
    return new ConstraintPy(new Constraint);
}

// constructor method
int ConstraintPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    if (PyArg_ParseTuple(args, "")) {
        return 0;
    }

    PyErr_Clear();

    char *ConstraintType;
    int  FirstIndex =-1;
    int  FirstPos   =-1;
    int  SecondIndex=-1;
    int  SecondPos  =-1;

    if (PyArg_ParseTuple(args, "si", &ConstraintType, &FirstIndex)) {
        if(strcmp("Horizontal",ConstraintType) == 0 ){
            this->getConstraintPtr()->Type = Horizontal ;
        } else if(strcmp("Vertical",ConstraintType) == 0 ){
            this->getConstraintPtr()->Type = Vertical ;
        }else{
            this->getConstraintPtr()->Type = None ;
        }
        this->getConstraintPtr()->First = FirstIndex;
        return 0;
    }

    PyErr_Clear();

    if (PyArg_ParseTuple(args, "siiii", &ConstraintType, &FirstIndex,&FirstPos, &SecondIndex,&SecondPos)) {
        if(strcmp("Coincident",ConstraintType) == 0 ){
            this->getConstraintPtr()->Type = Coincident ;
        } else {
            this->getConstraintPtr()->Type = None ;
        }
        this->getConstraintPtr()->First     = FirstIndex;
        this->getConstraintPtr()->FirstPos  = (Sketcher::PointPos) FirstPos;
        this->getConstraintPtr()->Second    = SecondIndex;
        this->getConstraintPtr()->SecondPos = (Sketcher::PointPos) SecondPos;
        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "Line constructor accepts:\n"
    "-- empty parameter list\n"
    "-- Constraint type and index\n");
    return -1;

}

// returns a string which represents the object e.g. when printed in python
std::string ConstraintPy::representation(void) const
{
    std::stringstream result;
    result << "<Constraint " ;
    switch(this->getConstraintPtr()->Type) {
        case None: result << "'None'>";break;
        case Vertical: result << "'Vertical' (" << getConstraintPtr()->First << ")>";break;
        case Horizontal: result << "'Horizontal' (" << getConstraintPtr()->First << ")>";break;
        case Coincident: result << "'Coincident'>";break;
        default  : result << "'?'>";break;
    }
    return result.str();
}


Py::Int ConstraintPy::getFirst(void) const
{
    return Py::Int(this->getConstraintPtr()->First);
}

void  ConstraintPy::setFirst(Py::Int arg)
{
    this->getConstraintPtr()->First = arg;
}

Py::Int ConstraintPy::getSecond(void) const
{
    return Py::Int(this->getConstraintPtr()->Second);
}

void  ConstraintPy::setSecond(Py::Int arg)
{
    this->getConstraintPtr()->Second = arg;
}


PyObject *ConstraintPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int ConstraintPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


