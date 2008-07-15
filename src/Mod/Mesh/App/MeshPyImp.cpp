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
#include <Base/Builder3D.h>

#include "Mesh.h"
#include "MeshPy.h"
#include "MeshPointPy.h"
#include "FacetPy.h"
#include "MeshPy.cpp"
#include "Core/Algorithm.h"
#include "Core/Iterator.h"
#include "Core/Elements.h"
#include "Core/Grid.h"

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
    else if (PyTuple_Check(pcObj)) {
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

PyObject *MeshPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of MeshPy and the Twin object 
    return new MeshPy(new MeshObject);
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
    float Float;
    if (!PyArg_ParseTuple(args, "f",&Float))
        return NULL;

    PY_TRY {
        Mesh().offsetSpecial2(Float);
    } PY_CATCH;

    Py_Return; 
}

PyObject*  MeshPy::offsetSpecial(PyObject *args)
{
    float Float,zmin,zmax;
    if (!PyArg_ParseTuple(args, "fff",&Float,&zmin,&zmax))			 
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
    double x,y,z;
    if (!PyArg_ParseTuple(args, "ddd",&x,&y,&z))
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
        Mesh().addFacet(MeshCore::MeshGeomFacet(
                        Base::Vector3f((float)x1,(float)y1,(float)z1),
                        Base::Vector3f((float)x2,(float)y2,(float)z2),
                        Base::Vector3f((float)x3,(float)y3,(float)z3)));
    } PY_CATCH;

    Py_Return;
}

PyObject*  MeshPy::addFacets(PyObject *args)
{
    PyObject *list;
    if (PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) {
        std::vector<MeshCore::MeshGeomFacet> facets;
        MeshCore::MeshGeomFacet facet;
        int k = 0;
        for (int i = 0; i < PyList_Size(list); i++) {
            PyObject *vec = PyList_GetItem(list, i);
            if (PyList_Check(vec)) {
                if (PyList_Size(vec) == 3) {
                    for (int j = 0; j < 3; j++) {
                        PyObject *val = PyList_GetItem(vec, j);
                        if (PyFloat_Check(val)) {
                            float f = (float)PyFloat_AsDouble(val);
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

        getMeshObjectPtr()->addFacets(facets);
        Py_Return;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!", &PyTuple_Type, &list)) {
        Py::Tuple tuple(list);
        Py::List list_v(tuple.getItem(0));
        std::vector<Base::Vector3f> vertices;
        union PyType_Object pyVertType = {&(Base::VectorPy::Type)};
        Py::Type vType(pyVertType.o);
        for (Py::List::iterator it = list_v.begin(); it != list_v.end(); ++it) {
            if ((*it).isType(vType)) {
                Base::Vector3d v = static_cast<Base::VectorPy*>((*it).ptr())->value();
                vertices.push_back(Base::Vector3f((float)v.x,(float)v.y,(float)v.z));
            }
        }

        Py::List list_f(tuple.getItem(1));
        MeshCore::MeshFacetArray faces;
        for (Py::List::iterator it = list_f.begin(); it != list_f.end(); ++it) {
            Py::Tuple f(*it);
            MeshCore::MeshFacet face;
            face._aulPoints[0] = (long)Py::Int(f.getItem(0));
            face._aulPoints[1] = (long)Py::Int(f.getItem(1));
            face._aulPoints[2] = (long)Py::Int(f.getItem(2));
            faces.push_back(face);
        }

        getMeshObjectPtr()->addFacets(faces, vertices);

        Py_Return;
    }

    PyErr_SetString(PyExc_Exception, "either expect\n"
        "-- [Vector] (3 of them define a facet)\n"
        "-- ([Vector],[(int,int,int)])");
    return NULL;
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
    int level=0;
    if (!PyArg_ParseTuple(args, "kf|i", &len, &area, &level))
        return NULL;                         

    PY_TRY {
        Mesh().fillupHoles(len, area, level);
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

PyObject*  MeshPy::foraminate(PyObject *args)
{
    PyObject* pnt_p;
    PyObject* dir_p;
    if (!PyArg_ParseTuple(args, "OO", &pnt_p, &dir_p))
        return NULL;

    try {
        Py::Tuple pnt_t(pnt_p);
        Py::Tuple dir_t(dir_p);
        Base::Vector3f pnt((float)Py::Float(pnt_t.getItem(0)),
                           (float)Py::Float(pnt_t.getItem(1)),
                           (float)Py::Float(pnt_t.getItem(2)));
        Base::Vector3f dir((float)Py::Float(dir_t.getItem(0)),
                           (float)Py::Float(dir_t.getItem(1)),
                           (float)Py::Float(dir_t.getItem(2)));

        Base::Vector3f res;
        MeshCore::MeshFacetIterator f_it(getMeshObjectPtr()->getKernel());
        int index = 0;

        Py::Dict dict;
        for (f_it.Begin(); f_it.More(); f_it.Next(), index++) {
            if (f_it->Foraminate(pnt, dir, res)) {
                Py::Tuple tuple(3);
                tuple.setItem(0, Py::Float(res.x));
                tuple.setItem(1, Py::Float(res.y));
                tuple.setItem(2, Py::Float(res.z));
                dict.setItem(Py::Int(index), tuple);
            }
        }

        return Py::new_reference_to(dict);
    }
    catch (const Py::Exception&) {
        return 0;
    }
}

PyObject*  MeshPy::smooth(PyObject *args)
{
    int iter=1;
    float d_max=FLOAT_MAX;
    if (!PyArg_ParseTuple(args, "|if", &iter,&d_max))
        return NULL;

    PY_TRY {
        getMeshObjectPtr()->smooth(iter, d_max);
    } PY_CATCH;

    Py_Return; 
}

PyObject* MeshPy::nearestFacetOnRay(PyObject *args)
{
    PyObject* pnt_p;
    PyObject* dir_p;
    if (!PyArg_ParseTuple(args, "OO", &pnt_p, &dir_p))
        return NULL;

    try {
        Py::Tuple pnt_t(pnt_p);
        Py::Tuple dir_t(dir_p);
        Py::Dict dict;
        Base::Vector3f pnt((float)Py::Float(pnt_t.getItem(0)),
                           (float)Py::Float(pnt_t.getItem(1)),
                           (float)Py::Float(pnt_t.getItem(2)));
        Base::Vector3f dir((float)Py::Float(dir_t.getItem(0)),
                           (float)Py::Float(dir_t.getItem(1)),
                           (float)Py::Float(dir_t.getItem(2)));

        unsigned long index = 0;
        Base::Vector3f res;
        MeshCore::MeshAlgorithm alg(getMeshObjectPtr()->getKernel());

#if 0 // for testing only
        MeshCore::MeshFacetGrid grid(getMeshObjectPtr()->getKernel(),10);
        // With grids we might search in the opposite direction, too
        if (alg.NearestFacetOnRay(pnt,  dir, grid, res, index) ||
            alg.NearestFacetOnRay(pnt, -dir, grid, res, index)) {
#else
        if (alg.NearestFacetOnRay(pnt, dir, res, index)) {
#endif
            Py::Tuple tuple(3);
            tuple.setItem(0, Py::Float(res.x));
            tuple.setItem(1, Py::Float(res.y));
            tuple.setItem(2, Py::Float(res.z));
            dict.setItem(Py::Int((int)index), tuple);
        }

#if 0 // for testing only
        char szBuf[200];
        std::ofstream str("grid_test.iv");
        Base::InventorBuilder builder(str);
        MeshCore::MeshGridIterator g_it(grid);
        for (g_it.Init(); g_it.More(); g_it.Next()) {
            Base::BoundBox3f box = g_it.GetBoundBox();
            unsigned long uX,uY,uZ;
            g_it.GetGridPos(uX,uY,uZ);
            builder.addBoundingBox(Base::Vector3f(box.MinX,box.MinY, box.MinZ),
                                   Base::Vector3f(box.MaxX,box.MaxY, box.MaxZ));
            sprintf(szBuf, "(%lu,%lu,%lu)", uX, uY, uZ);
            builder.addText(box.CalcCenter(), szBuf);
        }
        builder.addSingleArrow(pnt-20.0f*dir, pnt+10.0f*dir);
        builder.close();
        str.close();
#endif

        return Py::new_reference_to(dict);
    }
    catch (const Py::Exception&) {
        return 0;
    }
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
