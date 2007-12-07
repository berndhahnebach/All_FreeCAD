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

#include <Base/VectorPy.h>
#include <Base/Handle.h>

#include "Mesh.h"
#include "MeshPy.h"
#include "MeshPointPy.h"
#include "FacetPy.h"
#include "MeshPy.cpp"
#include "Core/Iterator.h"

using namespace Mesh;


int MeshPy::PyInit(PyObject* args, PyObject*)
{
    PyObject *pcObj=0;
    if (!PyArg_ParseTuple(args, "|O", &pcObj))     // convert args: Python->C 
        return -1;                             // NULL triggers exception

    // if no mesh is given
    if (!pcObj) return 0;
    if (PyObject_TypeCheck(pcObj, &(MeshPy::Type))) {
        Mesh() = static_cast<MeshPy*>(pcObj)->Mesh();
    }
    else if (PyList_Check(pcObj)) {
        addFacets(args);
    }
    else if (PyString_Check(pcObj)) {
        Mesh().load(PyString_AsString(pcObj));
    }

    return 0;
}

// returns a string which represent the object e.g. when printed in python
const char *MeshPy::representation(void) const
{
    // Note: As the return type is 'const char*' we cannot create a temporary char array neither on the stack because the array would be freed
    // when leaving the scope nor on the heap because we would have a memory leak.
    // So we use a static array that is used by all instances of this class. This, however, is not a problem as long as we only
    // use this method in _repr().
    MeshPy::PointerType ptr = reinterpret_cast<MeshPy::PointerType>(_pcTwinPointer);
    static std::string buf;
    buf = ptr->representation();
    return buf.c_str();
}

PyObject*  MeshPy::read(PyObject *args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;                         

    PY_TRY {
        Mesh().load(Name);
    } PY_CATCH;
    
    Py_Return; 
}

PyObject*  MeshPy::write(PyObject *args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;                         

    PY_TRY {
        Mesh().save(Name);
    } PY_CATCH;
    
    Py_Return; 
}

PyObject*  MeshPy::offset(PyObject *args)
{
    double Float;
    if (!PyArg_ParseTuple(args, "d",&Float))
        return NULL;

    PY_TRY {
        Mesh().offsetSpecial2(Float);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::offsetSpecial(PyObject *args)
{
    double Float,zmin,zmax;
    if (!PyArg_ParseTuple(args, "ddd",&Float,&zmin,&zmax))			 
        return NULL;                         

    PY_TRY {
        Mesh().offsetSpecial(Float,zmax,zmin);  
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
        Mesh().unite(pcObject->Mesh());
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
        Mesh().intersect(pcObject->Mesh());
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
      Mesh().subtract(pcObject->Mesh());
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
      Mesh().inner(pcObject->Mesh());
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
      Mesh().outer(pcObject->Mesh());
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
        Mesh().applyTransform(m);
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
        Mesh().applyTransform(m);
    } PY_CATCH;

    Py_Return;
}

PyObject*  MeshPy::transformToEigen(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    Mesh().transformToEigenSystem();
    Py_Return;
}

PyObject*  MeshPy::addFacet(PyObject *args)
{
    double x1,y1,z1,x2,y2,z2,x3,y3,z3;
    if (!PyArg_ParseTuple(args, "ddddddddd",&x1,&y1,&z1,&x2,&y2,&z2,&x3,&y3,&z3))
        return NULL;

    PY_TRY {
        Mesh().addFacet(MeshCore::MeshGeomFacet(Base::Vector3f(x1,y1,z1),
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
    Mesh().addFacets(facets);
    Py_Return;
}

PyObject*  MeshPy::clear(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    Mesh().clear();
    Py_Return;
}

PyObject*  MeshPy::isSolid(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    bool ok = Mesh().isSolid();
    return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PyObject*  MeshPy::hasNonManifolds(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    bool ok = Mesh().hasNonManifolds();
    return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PyObject*  MeshPy::removeNonManifolds(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    Mesh().removeNonManifolds();
    Py_Return
}

PyObject*  MeshPy::hasSelfIntersections(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    bool ok = Mesh().hasSelfIntersections();
    return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PyObject*  MeshPy::fixSelfIntersections(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    try {
        Mesh().removeSelfIntersections();
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
        Mesh().flipNormals();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::hasNonUnifomOrientedFacets(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    bool ok = Mesh().countNonUnifomOrientedFacets() > 0;
    return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PyObject*  MeshPy::countNonUnifomOrientedFacets(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    unsigned long count = Mesh().countNonUnifomOrientedFacets();
    return Py_BuildValue("k", count); 
}

PyObject*  MeshPy::harmonizeNormals(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        Mesh().harmonizeNormals();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::countComponents(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    unsigned long count = Mesh().countComponents();
    return Py_BuildValue("k",count);
}

PyObject*  MeshPy::removeComponents(PyObject *args)
{
    unsigned long count;
    if (!PyArg_ParseTuple(args, "k", &count))
        return NULL;                         

    PY_TRY {
        if (count > 0) {
            Mesh().removeComponents(count);
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
        Mesh().fillupHoles(len, area);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::fixIndices(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        Mesh().validateIndices();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::fixDeformations(PyObject *args)
{
    float fMaxAngle;
    if (!PyArg_ParseTuple(args, "f", &fMaxAngle))
        return NULL;

    PY_TRY {
        Mesh().validateDeformations(fMaxAngle);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::fixDegenerations(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        Mesh().validateDegenerations();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::removeDuplicatedPoints(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        Mesh().removeDuplicatedPoints();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::removeDuplicatedFacets(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        Mesh().removeDuplicatedFacets();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::refine(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        Mesh().refine();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::optimizeTopology(PyObject *args)
{
    float fMaxAngle;
    if (!PyArg_ParseTuple(args, "f; specify the maximum allowed angle between the normals of two adjacent facets", &fMaxAngle))
        return NULL;

    PY_TRY {
        Mesh().optimizeTopology(fMaxAngle);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::optimizeEdges(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        Mesh().optimizeEdges();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::splitEdges(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    PY_TRY {
        Mesh().splitEdges();
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::splitEdge(PyObject *args)
{
    unsigned long facet, neighbour;
    PyObject* vertex;
    if (!PyArg_ParseTuple(args, "kkO!", &facet, &neighbour, &Base::VectorPy::Type, &vertex))
        return NULL;

    Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(vertex);
    Base::Vector3d* val = pcObject->getVectorPtr();
    Base::Vector3f v((float)val->x,(float)val->y,(float)val->z);

    const MeshCore::MeshKernel& kernel = Mesh().getKernel();
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
        
        Mesh().splitEdge(facet, neighbour, v);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::splitFacet(PyObject *args)
{
    unsigned long facet;
    PyObject* vertex1;
    PyObject* vertex2;
    if (!PyArg_ParseTuple(args, "kO!O!", &facet, &Base::VectorPy::Type, &vertex1, 
                                                 &Base::VectorPy::Type, &vertex2))
        return NULL;

    Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(vertex1);
    Base::Vector3d* val = pcObject->getVectorPtr();
    Base::Vector3f v1((float)val->x,(float)val->y,(float)val->z);

    pcObject = static_cast<Base::VectorPy*>(vertex2);
    val = pcObject->getVectorPtr();
    Base::Vector3f v2((float)val->x,(float)val->y,(float)val->z);

    const MeshCore::MeshKernel& kernel = Mesh().getKernel();
    PY_TRY {
        if (facet < 0 || facet >= kernel.CountFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
        
        Mesh().splitFacet(facet, v1, v2);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::swapEdge(PyObject *args)
{
    unsigned long facet, neighbour;
    if (!PyArg_ParseTuple(args, "kk", &facet, &neighbour))
        return NULL;

    const MeshCore::MeshKernel& kernel = Mesh().getKernel();
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
        
        Mesh().swapEdge(facet, neighbour);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::collapseEdge(PyObject *args)
{
    unsigned long facet, neighbour;
    if (!PyArg_ParseTuple(args, "kk", &facet, &neighbour))
        return NULL;

    const MeshCore::MeshKernel& kernel = Mesh().getKernel();
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
        
        Mesh().collapseEdge(facet, neighbour);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::collapseFacet(PyObject *args)
{
    unsigned long facet;
    if (!PyArg_ParseTuple(args, "k", &facet))
        return NULL;

    PY_TRY {
        if (facet < 0 || facet >= Mesh().countFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
  
        Mesh().collapseFacet(facet);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::insertVertex(PyObject *args)
{
    unsigned long facet;
    PyObject* vertex;
    if (!PyArg_ParseTuple(args, "kO!", &facet, &Base::VectorPy::Type, &vertex))
        return NULL;

    Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(vertex);
    Base::Vector3d* val = pcObject->getVectorPtr();
    Base::Vector3f v((float)val->x,(float)val->y,(float)val->z);

    PY_TRY {
        if (facet < 0 || facet >= Mesh().countFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
        
        Mesh().insertVertex(facet, v);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::snapVertex(PyObject *args)
{
    unsigned long facet;
    PyObject* vertex;
    if (!PyArg_ParseTuple(args, "kO!", &facet, &Base::VectorPy::Type, &vertex))
        return NULL;

    Base::VectorPy  *pcObject = static_cast<Base::VectorPy*>(vertex);
    Base::Vector3d* val = pcObject->getVectorPtr();
    Base::Vector3f v((float)val->x,(float)val->y,(float)val->z);

    PY_TRY {
        if (facet < 0 || facet >= Mesh().countFacets()) {
            PyErr_SetString(PyExc_IndexError, "Facet index out of range");
            return NULL;
        }
        
        Mesh().snapVertex(facet, v);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::printInfo(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    return Py_BuildValue("s", Mesh().topologyInfo().c_str());
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

        Mesh().collapseFacets(facets);
    }
    else {
        Py_Error(PyExc_Exception, "List of Integers needed");
    }

    Py_Return; 
}

Py::Int MeshPy::getCountPoints(void) const
{
    return Py::Int((long)Mesh().countPoints());
}

Py::Int MeshPy::getCountFacets(void) const
{
    return Py::Int((long)Mesh().countFacets());
}

PyObject *MeshPy::getCustomAttributes(const char* attr) const
{
    return 0;
}

int MeshPy::setCustomAttributes(const char* attr, PyObject *obj)
{
    return 0; 
}

Py::List MeshPy::getPoints(void) const
{
    Py::List PointList;
    MeshCore::MeshPointIterator it = Mesh().PointIterator();
    unsigned int Index=0;
    for (it.Init(); it.More(); it.Next(),Index++)
        PointList.append(Py::Object(new MeshPointPy(new MeshPoint(it.asVector3d(),getMeshObjectPtr(),Index))));

    return PointList;
}

Py::List MeshPy::getFacets(void) const
{
    Py::List FacetList;
    for (MeshObject::FacetIter it = getMeshObjectPtr()->facets_begin(); it != getMeshObjectPtr()->facets_end(); ++it) {
        FacetList.append(Py::Object(new FacetPy(new Facet(*it))));
    }
    return FacetList;
}
