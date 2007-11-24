/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#include <App/VectorPy.h>
#include <Base/Handle.h>

#include "Mesh.h"
#include "MeshPy.h"
#include "MeshPy.cpp"

using namespace Mesh;

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
MeshPy::MeshPy(MeshObject *pcMeshObject, PyTypeObject *T)
    : ComplexGeoDataPy(pcMeshObject, T), _meshObject(pcMeshObject)
{
}

PyObject *MeshPy::PyMake(PyTypeObject  *ignored, PyObject *args, PyObject *kwds)  // Python wrapper
{
    return new MeshPy(new MeshObject);
}

int MeshPy::PyInit(PyObject* self, PyObject* args, PyObject*)
{
    PyObject *pcObj=0;
    if (!PyArg_ParseTuple(args, "|O", &pcObj))     // convert args: Python->C 
        return -1;                             // NULL triggers exception

    // if no mesh is given
    if (!pcObj) return 0;
    if (PyObject_TypeCheck(pcObj, &(MeshPy::Type))) {
        *(static_cast<MeshPy*>(self)->_meshObject) = *(static_cast<MeshPy*>(pcObj)->_meshObject);
    }
    else if (PyList_Check(pcObj)) {
        static_cast<MeshPy*>(self)->addFacets(args);
    }
    else if (PyString_Check(pcObj)) {
        static_cast<MeshPy*>(self)->_meshObject->load(PyString_AsString(pcObj));
    }

    return 0;
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
MeshPy::~MeshPy()                                // Everything handled in parent
{
}

//--------------------------------------------------------------------------
// MeshPy representation
//--------------------------------------------------------------------------
PyObject *MeshPy::_repr(void)
{
    return Py_BuildValue("s", _meshObject->representation().c_str());
}

void MeshPy::setMesh(MeshObject* pcMesh)
{
    _meshObject = pcMesh;
}

MeshObject* MeshPy::getMesh(void) const
{
    return &(*_meshObject);
}

// returns a string which represent the object e.g. when printed in python
const char *MeshPy::representation(void) const
{
    return "<MeshObject object>";
}

PyObject*  MeshPy::read(PyObject *args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;                         

    PY_TRY {
        _meshObject->load(Name);
    } PY_CATCH;
    
    Py_Return; 
}

PyObject*  MeshPy::write(PyObject *args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;                         

    PY_TRY {
        _meshObject->save(Name);
    } PY_CATCH;
    
    Py_Return; 
}

PyObject*  MeshPy::offset(PyObject *args)
{
    double Float;
    if (!PyArg_ParseTuple(args, "d",&Float))
        return NULL;

    PY_TRY {
        _meshObject->offsetSpecial2(Float);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::offsetSpecial(PyObject *args)
{
    double Float,zmin,zmax;
    if (!PyArg_ParseTuple(args, "ddd",&Float,&zmin,&zmax))			 
        return NULL;                         

    PY_TRY {
        _meshObject->offsetSpecial(Float,zmax,zmin);  
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::unite(PyObject *args)
{
    MeshPy   *pcObject;
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    pcObject = (MeshPy*)pcObj;

    PY_TRY {
        _meshObject->unite(*pcObject->_meshObject);
    } PY_CATCH;

    Py_Return;
}

PyObject*  MeshPy::intersect(PyObject *args)
{
    MeshPy   *pcObject;
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    pcObject = (MeshPy*)pcObj;

    PY_TRY {
        _meshObject->intersect(*pcObject->_meshObject);
    } PY_CATCH;

    Py_Return;
}

PyObject*  MeshPy::difference(PyObject *args)
{
    MeshPy   *pcObject;
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    pcObject = (MeshPy*)pcObj;

    PY_TRY {
      _meshObject->subtract(*pcObject->_meshObject);
    } PY_CATCH;

    Py_Return;
}

PyObject*  MeshPy::inner(PyObject *args)
{
    MeshPy   *pcObject;
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    pcObject = (MeshPy*)pcObj;

    PY_TRY {
      _meshObject->inner(*pcObject->_meshObject);
    } PY_CATCH;

    Py_Return;
}

PyObject*  MeshPy::outer(PyObject *args)
{
    MeshPy   *pcObject;
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    pcObject = (MeshPy*)pcObj;

    PY_TRY {
      _meshObject->outer(*pcObject->_meshObject);
    } PY_CATCH;

    Py_Return;
}

PyObject*  MeshPy::coarsen(PyObject *args)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject*  MeshPy::translate(PyObject *args)
{
    float x,y,z;
    if (!PyArg_ParseTuple(args, "fff",&x,&y,&z))
        return NULL;

    PY_TRY {
        Base::Matrix4D m;
        m.move(x,y,z);
        _meshObject->applyTransform(m);
    } PY_CATCH;

    Py_Return;
}

PyObject*  MeshPy::rotate(PyObject *args)
{
    float x,y,z;
    if (!PyArg_ParseTuple(args, "fff",&x,&y,&z))
        return NULL;

    PY_TRY {
        Base::Matrix4D m;
        m.rotX(x);
        m.rotY(y);
        m.rotZ(z);
        _meshObject->applyTransform(m);
    } PY_CATCH;

    Py_Return;
}

PyObject*  MeshPy::transformToEigen(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    _meshObject->transformToEigenSystem();
    Py_Return;
}

PyObject*  MeshPy::addFacet(PyObject *args)
{
    double x1,y1,z1,x2,y2,z2,x3,y3,z3;
    if (!PyArg_ParseTuple(args, "ddddddddd",&x1,&y1,&z1,&x2,&y2,&z2,&x3,&y3,&z3))
        return NULL;

    PY_TRY {
        _meshObject->addFacet(MeshCore::MeshGeomFacet(Base::Vector3f(x1,y1,z1),
                                                      Base::Vector3f(x2,y2,z2),
                                                      Base::Vector3f(x3,y3,z3)));
    } PY_CATCH;

    Py_Return;
}

PyObject*  MeshPy::addFacets(PyObject *args)
{
    PyObject *list;

    std::vector<MeshCore::MeshGeomFacet> facets;
    if (!PyArg_ParseTuple(args, "O!: list of vectors (3 of them defined a facet)", &PyList_Type, &list)) {
        Py_Error(PyExc_Exception, "need list of vectors (3 of them defined a facet)");
        return NULL;
    }
    
    if (PyList_Check(list)) {
        int k = 0;
        MeshCore::MeshGeomFacet facet;
        for (int i = 0; i < PyList_Size(list); i++) {
            PyObject *vec = PyList_GetItem(list, i);
            if (PyList_Check(vec)) {
                if (PyList_Size(vec) == 3) {
                    for (int j = 0; j < 3; j++) {
                        PyObject *val = PyList_GetItem(vec, j);
                        if (PyFloat_Check(val)) {
                            float f = PyFloat_AsDouble(val);
                            facet._aclPoints[k][j] = f;
                        }
                        else {
                            Py_Error(PyExc_Exception, "vector needs 3 double values");
                            return NULL; // not a double
                        }
                    }
                }
                else {
                    Py_Error(PyExc_Exception, "vector needs 3 double values");
                    return NULL; // vector needs 3 doubles
                }
            }
            else {
                Py_Error(PyExc_Exception, "inner list should be 3 doubles as list");
                return NULL; // not a vector
            }
        
            k++;
            if (k == 3) {
                k = 0;
                facet.CalcNormal();
                facets.push_back(facet);
            }
        }
    }
    else {
        Py_Error(PyExc_Exception, "need a list of 3 double values");
        return NULL; // not a list
    }

    PyErr_Clear();
    _meshObject->addFacets(facets);
    Py_Return;
}

PyObject*  MeshPy::clear(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    _meshObject->clear();
    Py_Return;
}

PyObject*  MeshPy::isSolid(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    bool ok = _meshObject->isSolid();
    return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PyObject*  MeshPy::hasNonManifolds(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    bool ok = _meshObject->hasNonManifolds();
    return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PyObject*  MeshPy::removeNonManifolds(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    _meshObject->removeNonManifolds();
    Py_Return
}

PyObject*  MeshPy::hasSelfIntersections(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    bool ok = _meshObject->hasSelfIntersections();
    return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PyObject*  MeshPy::fixSelfIntersections(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    try {
        _meshObject->removeSelfIntersections();
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_Exception, e.what());
        return NULL;
    }
    Py_Return;
}

PyObject*  MeshPy::flipNormals(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        _meshObject->flipNormals();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::hasNonUnifomOrientedFacets(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    bool ok = _meshObject->countNonUnifomOrientedFacets() > 0;
    return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PyObject*  MeshPy::countNonUnifomOrientedFacets(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    unsigned long count = _meshObject->countNonUnifomOrientedFacets();
    return Py_BuildValue("k", count); 
}

PyObject*  MeshPy::harmonizeNormals(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        _meshObject->harmonizeNormals();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::countComponents(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    unsigned long count = _meshObject->countComponents();
    return Py_BuildValue("k",count);
}

PyObject*  MeshPy::removeComponents(PyObject *args)
{
    unsigned long count;
    if (!PyArg_ParseTuple(args, "k", &count))
        return NULL;                         

    PY_TRY {
        if (count > 0) {
            _meshObject->removeComponents(count);
        }
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::fillupHoles(PyObject *args)
{
    unsigned long len;
    float area;
    if (!PyArg_ParseTuple(args, "kf", &len, &area))
        return NULL;                         

    PY_TRY {
        _meshObject->fillupHoles(len, area);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::fixIndices(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        _meshObject->validateIndices();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::fixDeformations(PyObject *args)
{
    float fMaxAngle;
    if (!PyArg_ParseTuple(args, "f", &fMaxAngle))
        return NULL;

    PY_TRY {
        _meshObject->validateDeformations(fMaxAngle);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::fixDegenerations(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        _meshObject->validateDegenerations();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::removeDuplicatedPoints(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        _meshObject->removeDuplicatedPoints();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::removeDuplicatedFacets(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        _meshObject->removeDuplicatedFacets();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::refine(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        _meshObject->refine();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::optimizeTopology(PyObject *args)
{
    float fMaxAngle;
    if (!PyArg_ParseTuple(args, "f; specify the maximum allowed angle between the normals of two adjacent facets", &fMaxAngle))
        return NULL;

    PY_TRY {
        _meshObject->optimizeTopology(fMaxAngle);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::optimizeEdges(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        _meshObject->optimizeEdges();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::splitEdges(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        _meshObject->splitEdges();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::splitEdge(PyObject *args)
{
    unsigned long facet, neighbour;
    PyObject* vertex;
    if (!PyArg_ParseTuple(args, "kkO!", &facet, &neighbour, &App::VectorPy::Type, &vertex))
        return NULL;

    Base::Vector3f v = static_cast<App::VectorPy*>(vertex)->value();

    const MeshCore::MeshKernel& kernel = _meshObject->getKernel();
    PY_TRY {
        if (facet < 0 || facet >= kernel.CountFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
        if (neighbour < 0 || neighbour >= kernel.CountFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }

        const MeshCore::MeshFacet& rclF = kernel.GetFacets()[facet];
        if (rclF._aulNeighbours[0] != neighbour && rclF._aulNeighbours[1] != neighbour &&
            rclF._aulNeighbours[2] != neighbour) {
            PyErr_SetString(PyExc_IndexError, "No adjacent facets");
            return NULL;
        }
        
        _meshObject->splitEdge(facet, neighbour, v);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::splitFacet(PyObject *args)
{
    unsigned long facet;
    PyObject* vertex1;
    PyObject* vertex2;
    if (!PyArg_ParseTuple(args, "kO!O!", &facet, &App::VectorPy::Type, &vertex1, 
                                                  &App::VectorPy::Type, &vertex2))
        return NULL;

    Base::Vector3f v1 = static_cast<App::VectorPy*>(vertex1)->value();
    Base::Vector3f v2 = static_cast<App::VectorPy*>(vertex2)->value();

    const MeshCore::MeshKernel& kernel = _meshObject->getKernel();
    PY_TRY {
        if (facet < 0 || facet >= kernel.CountFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
        
        _meshObject->splitFacet(facet, v1, v2);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::swapEdge(PyObject *args)
{
    unsigned long facet, neighbour;
    if (!PyArg_ParseTuple(args, "kk", &facet, &neighbour))
        return NULL;

    const MeshCore::MeshKernel& kernel = _meshObject->getKernel();
    PY_TRY {
        if (facet < 0 || facet >= kernel.CountFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
        if (neighbour < 0 || neighbour >= kernel.CountFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
  
        const MeshCore::MeshFacet& rclF = kernel.GetFacets()[facet];
        if (rclF._aulNeighbours[0] != neighbour && rclF._aulNeighbours[1] != neighbour &&
            rclF._aulNeighbours[2] != neighbour) {
            PyErr_SetString(PyExc_IndexError, "No adjacent facets");
            return NULL;
        }
        
        _meshObject->swapEdge(facet, neighbour);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::collapseEdge(PyObject *args)
{
    unsigned long facet, neighbour;
    if (!PyArg_ParseTuple(args, "kk", &facet, &neighbour))
        return NULL;

    const MeshCore::MeshKernel& kernel = _meshObject->getKernel();
    PY_TRY {
        if (facet < 0 || facet >= kernel.CountFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
        if (neighbour < 0 || neighbour >= kernel.CountFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
  
        const MeshCore::MeshFacet& rclF = kernel.GetFacets()[facet];
        if (rclF._aulNeighbours[0] != neighbour && rclF._aulNeighbours[1] != neighbour &&
            rclF._aulNeighbours[2] != neighbour) {
            PyErr_SetString(PyExc_IndexError, "No adjacent facets");
            return NULL;
        }
        
        _meshObject->collapseEdge(facet, neighbour);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::collapseFacet(PyObject *args)
{
    unsigned long facet;
    if (!PyArg_ParseTuple(args, "k", &facet))
        return NULL;

    PY_TRY {
        if (facet < 0 || facet >= _meshObject->countFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
  
        _meshObject->collapseFacet(facet);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::insertVertex(PyObject *args)
{
    unsigned long facet;
    PyObject* vertex;
    if (!PyArg_ParseTuple(args, "kO!", &facet, &App::VectorPy::Type, &vertex))
        return NULL;

    Base::Vector3f v = static_cast<App::VectorPy*>(vertex)->value();

    PY_TRY {
        if (facet < 0 || facet >= _meshObject->countFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
        
        _meshObject->insertVertex(facet, v);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::snapVertex(PyObject *args)
{
    unsigned long facet;
    PyObject* vertex;
    if (!PyArg_ParseTuple(args, "kO!", &facet, &App::VectorPy::Type, &vertex))
        return NULL;

    Base::Vector3f v = static_cast<App::VectorPy*>(vertex)->value();

    PY_TRY {
        if (facet < 0 || facet >= _meshObject->countFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
        
        _meshObject->snapVertex(facet, v);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::printInfo(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    return Py_BuildValue("s", _meshObject->topologyInfo().c_str());
}

PyObject*  MeshPy::collapseFacets(PyObject *args)
{
    PyObject *pcObj=0;
    if (!PyArg_ParseTuple(args, "O", &pcObj))     // convert args: Python->C 
        return 0;                             // NULL triggers exception

    // if no mesh is given
    if (PyList_Check(pcObj)) {
        std::vector<unsigned long> facets;
        for (int i = 0; i < PyList_Size(pcObj); i++) {
            PyObject *idx = PyList_GetItem(pcObj, i);
            if (PyInt_Check(idx)){
                unsigned long iIdx = PyInt_AsLong(idx);
                facets.push_back(iIdx);
            }
            else {
                Py_Error(PyExc_Exception, "list of integers needed");
            }
        }

        _meshObject->collapseFacets(facets);
    }
    else {
        Py_Error(PyExc_Exception, "List of Integers needed");
    }

    Py_Return; 
}

Py::Int MeshPy::getCountPoints(void) const
{
    return Py::Int((long)_meshObject->countPoints());
}

Py::Int MeshPy::getCountFacets(void) const
{
    return Py::Int((long)_meshObject->countFacets());
}

PyObject *MeshPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int MeshPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}
