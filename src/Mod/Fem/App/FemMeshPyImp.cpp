
#include "PreCompiled.h"

#include <SMESH_Gen.hxx>
#include <SMESH_Mesh.hxx>
#include <SMDS_VolumeTool.hxx>

#include <TopoDS_Shape.hxx>

#include <Base/VectorPy.h>
#include <Base/PlacementPy.h>

#include <Mod/Part/App/TopoShapePy.h>
#include <Mod/Part/App/TopoShape.h>

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

// ===== Methods ============================================================

PyObject* FemMeshPy::setShape(PyObject *args)
{
   PyObject *pcObj;
   if (!PyArg_ParseTuple(args, "O!", &(Part::TopoShapePy::Type), &pcObj))
        return NULL;

    TopoDS_Shape shape = static_cast<Part::TopoShapePy*>(pcObj)->getTopoShapePtr()->_Shape;
    getFemMeshPtr()->myMesh->ShapeToMesh(shape);
    Py_Return;
}

PyObject* FemMeshPy::read(PyObject *args)
{
    char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    getFemMeshPtr()->read(filename);
    Py_Return;
}

PyObject* FemMeshPy::write(PyObject *args)
{
    char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    getFemMeshPtr()->write(filename);
    Py_Return;
}

// ===== Atributes ============================================================

Py::Int FemMeshPy::getNodeCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbNodes());
}

Py::Int FemMeshPy::getEdgeCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbEdges());
}

Py::Int FemMeshPy::getFacesCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbFaces());
}

Py::Int FemMeshPy::getTriangleCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbTriangles());
}

Py::Int FemMeshPy::getQuadrangleCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbQuadrangles());
}

Py::Int FemMeshPy::getPolygonCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbPolygons());
}

Py::Int FemMeshPy::getVolumeCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbVolumes());
}

Py::Int FemMeshPy::getTetraCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbTetras());
}

Py::Int FemMeshPy::getHexaCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbHexas());
}

Py::Int FemMeshPy::getPyramidCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbPyramids());
}

Py::Int FemMeshPy::getPrismCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbPrisms());
}

Py::Int FemMeshPy::getPolyhedronCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbPolyhedrons());
}

Py::Int FemMeshPy::getSubMeshCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbSubMesh());
}

Py::Int FemMeshPy::getGroupCount(void) const
{
    return Py::Int(getFemMeshPtr()->myMesh->NbGroup());
}

// ===== custom attributes ============================================================

PyObject *FemMeshPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int FemMeshPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


