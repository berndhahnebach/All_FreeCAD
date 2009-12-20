
#include "PreCompiled.h"

#include <Base/VectorPy.h>
#include <Base/PlacementPy.h>
#include "Mod/Fem/App/FemMesh.h"

// inclusion of the generated files (generated out of FemMeshPy.xml)
#include "FemMeshPy.h"
#include "FemMeshPy.cpp"


using namespace Fem;

// returns a string which represents the object e.g. when printed in python
std::string FemMeshPy::representation(void) const
{
    std::stringstream str;
 /*   str.precision(5);
    str << "FemMesh [";
    str << "size:" << getFemMeshPtr()->getSize() << " ";
    str << "length:" << getFemMeshPtr()->getLength() << " ";
    str << "duration:" << getFemMeshPtr()->getDuration() << " ";
    str << "]";*/

    return str.str();
}

PyObject *FemMeshPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of FemMeshPy and the Twin object 
    return new FemMeshPy(new FemMesh);
}

// constructor method
int FemMeshPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    //PyObject *pcObj=0;
    //if (!PyArg_ParseTuple(args, "|O!", &(PyList_Type), &pcObj))
    //    return -1;

    //if (pcObj) {
    //    Py::List list(pcObj);
    //    bool first = true;
    //    for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
    //        if (PyObject_TypeCheck((*it).ptr(), &(Fem::WaypointPy::Type))) {
    //            Fem::Waypoint &wp = *static_cast<Fem::WaypointPy*>((*it).ptr())->getWaypointPtr();
    //            getFemMeshPtr()->addWaypoint(wp);
    //        }
    //    }
    //}
    //getFemMeshPtr()->generateFemMesh();
    return 0;
}




PyObject *FemMeshPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int FemMeshPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


