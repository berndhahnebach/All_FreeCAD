/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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

#ifndef _PreComp_
# include <sstream>
#endif

#include <Mod/Mesh/App/WildMagic4/Wm4Delaunay3.h>
#include <Mod/Mesh/App/WildMagic4/Wm4Vector3.h>

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Builder3D.h>

#include <App/MatrixPy.h>

#include "MeshPy.h"
#include "Mesh.h"
#include "MeshAlgos.h"
#include "Core/TopoAlgorithm.h"
#include "Core/MeshKernel.h"
#include "Core/MeshIO.h"
#include "Core/Info.h"
#include "Core/Evaluation.h"
#include "Core/Iterator.h"
#include "Core/SetOperations.h"
#include <Mod/Part/App/TopologyPy.h>


using namespace Mesh;
using namespace MeshCore;


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyDoc_STRVAR(mesh_doc,
"mesh() -- Create an empty mesh object.\n"
"\n"
"This class allows to manipulate the mesh object by adding new faces, deleting faces, importing from an STL file,\n"
"transforming the mesh and much more.\n"
"For a complete overview of what can be done see also the documentation of mesh.\n"
"A mesh object cannot be added to an existing document directly. Therefore the document must create an object\n"
"with a property class that supports meshes.\n"
"Example:\n"
"  m = Mesh.mesh()\n"
"  ... # Manipulate the mesh\n"
"  d = FreeCAD.activeDocument() # Get a reference to the actie document\n"
"  f = d.addObject(\"Mesh::Feature\", \"Mesh\") # Create a mesh feature\n"
"  f.Mesh = m # Assign the mesh object to the internal property\n"
"  d.recompute()\n");

PyTypeObject MeshPy::Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                      /*ob_size*/
  "Mesh.mesh",        /*tp_name*/
  sizeof(MeshPy),  /*tp_basicsize*/
  0,                      /*tp_itemsize*/
                          /* methods */
  PyDestructor,           /*tp_dealloc*/
  0,                      /*tp_print*/
  __getattr,              /*tp_getattr*/
  __setattr,              /*tp_setattr*/
  0,                      /*tp_compare*/
  __repr,                 /*tp_repr*/
  0,                      /*tp_as_number*/
  0,                      /*tp_as_sequence*/
  0,                      /*tp_as_mapping*/
  0,                      /*tp_hash*/
  0,                      /*tp_call */
  0,                                                /*tp_str  */
  0,                                                /*tp_getattro*/
  0,                                                /*tp_setattro*/
  /* --- Functions to access object as input/output buffer ---------*/
  0,                                                /* tp_as_buffer */
  /* --- Flags to define presence of optional/expanded features */
  Py_TPFLAGS_HAVE_CLASS,                            /*tp_flags */
  mesh_doc,                                         /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  MeshPy::Methods,                                  /*tp_methods */
  0,                                                /*tp_members */
  0,                                                /*tp_getset */
  &Base::PyObjectBase::Type,                        /*tp_base */
  0,                                                /*tp_dict */
  0,                                                /*tp_descr_get */
  0,                                                /*tp_descr_set */
  0,                                                /*tp_dictoffset */
  MeshPy::PyInit,                                   /*tp_init */
  0,                                                /*tp_alloc */
  MeshPy::PyMake,                                   /*tp_new */
  0,                                                /*tp_free   Low-level free-memory routine */
  0,                                                /*tp_is_gc  For PyObject_IS_GC */
  0,                                                /*tp_bases */
  0,                                                /*tp_mro    method resolution order */
  0,                                                /*tp_cache */
  0,                                                /*tp_subclasses */
  0                                                 /*tp_weaklist */
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef MeshPy::Methods[] = {
  {"pointCount", (PyCFunction) spointCount, Py_NEWARGS, 
   "Return the number of vertices of the mesh object."},
  {"faceCount", (PyCFunction) sfaceCount, Py_NEWARGS, 
   "Return the number of faces of the mesh object."},
  {"read", (PyCFunction) sread, Py_NEWARGS, 
   "Read in a mesh object from an STL file."},
  {"write", (PyCFunction) swrite, Py_NEWARGS, 
   "Write the mesh object into an STL file."},
  PYMETHODEDEF(offset)
  PYMETHODEDEF(offsetSpecial)
  {"unite", (PyCFunction) sunite, Py_NEWARGS, 
   "deprecated -- use union instead"},
  {"union", (PyCFunction) sunite, Py_NEWARGS, 
   "Union of this and the given mesh object."},
  {"intersect", (PyCFunction) sintersect, Py_NEWARGS, 
   "Intersection of this and the given mesh object."},
  {"diff", (PyCFunction) sdiff, Py_NEWARGS, 
   "Difference of this and the given mesh object."},
  PYMETHODEDEF(outer)
  PYMETHODEDEF(inner)
  PYMETHODEDEF(coarsen)
  PYMETHODEDEF(translate)
  PYMETHODEDEF(rotate)
  PYMETHODEDEF(transformToEigen)
  PYMETHODEDEF(scale)
  PYMETHODEDEF(transform)
  PYMETHODEDEF(addFacet)
  PYMETHODEDEF(addFacets)
  PYMETHODEDEF(clear)
  PYMETHODEDEF(copy)
  PYMETHODEDEF(isSolid)
  PYMETHODEDEF(hasNonManifolds)
  PYMETHODEDEF(testDelaunay)
  PYMETHODEDEF(makeCutToolFromShape)
  PYMETHODEDEF(flipNormals)
  PYMETHODEDEF(hasNonUnifomOrientedFacets)
  PYMETHODEDEF(countNonUnifomOrientedFacets)
  PYMETHODEDEF(harmonizeNormals)
  PYMETHODEDEF(countComponents)
  PYMETHODEDEF(removeComponents)
  PYMETHODEDEF(fillupHoles)
  PYMETHODEDEF(refine)
  PYMETHODEDEF(optimizeTopology)
  PYMETHODEDEF(optimizeEdges)
  PYMETHODEDEF(splitEdge)
  {NULL, NULL}    /* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject MeshPy::Parents[] = {&Base::PyObjectBase::Type, NULL};     

PyObject *MeshPy::PyMake(PyTypeObject  *ignored, PyObject *args, PyObject *kwds)  // Python wrapper
{
  return new MeshPy();
}

int MeshPy::PyInit(PyObject* self, PyObject* args, PyObject*)
{
  PyObject *pcObj=0;
  if (!PyArg_ParseTuple(args, "|O", &pcObj))     // convert args: Python->C 
    return -1;                             // NULL triggers exception

  // if no mesh is given
  if (!pcObj) return 0;
  if (PyObject_TypeCheck(pcObj, &(MeshPy::Type))) 
  {
    MeshPy* pcMesh = static_cast<MeshPy*>(pcObj);
    *(static_cast<MeshPy*>(self)->_pcMesh) = *(pcMesh->_pcMesh);
  }
  else if (PyList_Check(pcObj))
  {
    ((MeshPy*)self)->addFacets(args);
  }
  else if (PyString_Check(pcObj))
  {
    std::auto_ptr<MeshCore::MeshKernel> apcKernel(new MeshCore::MeshKernel());
    MeshInput aReader(*apcKernel);
        
    aReader.LoadAny(PyString_AsString(pcObj));
    // Mesh is okay
    ((MeshPy*)self)->_pcMesh = apcKernel.release();
  }

  return 0;
}

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
MeshPy::MeshPy(PyTypeObject *T)
: Base::PyObjectBase(0,T)
{
  _pcMesh = new MeshCore::MeshKernel();
}

MeshPy::MeshPy(MeshCore::MeshKernel* pcMesh, PyTypeObject *T)
: Base::PyObjectBase(0,T)
{
  // As long as we don't have a reference mechanism working we must copy the data, otherwise we run into SEGFAULTS
  _pcMesh = new MeshCore::MeshKernel();
  _pcMesh->operator = (*pcMesh);
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
MeshPy::~MeshPy()           // Everything handled in parent
{
  delete _pcMesh;
} 

//--------------------------------------------------------------------------
// MeshPy representation
//--------------------------------------------------------------------------
PyObject *MeshPy::_repr(void)
{
  std::stringstream a;
  MeshInfo info(*_pcMesh);
  info.GeneralInformation( a );
  return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// MeshPy Attributes
//--------------------------------------------------------------------------
PyObject *MeshPy::_getattr(char *attr)     // __getattr__ function: note only need to handle new state
{ 
  try{
    if (Base::streq(attr, "XXXX"))
      return Py_BuildValue("i",1); 
    else
        _getattr_up(PyObjectBase);
  }catch(...){
    Py_Error(PyExc_Exception,"Error in get Attribute");
  }
} 

int MeshPy::_setattr(char *attr, PyObject *value) // __setattr__ function: note only need to handle new state
{ 
  if (Base::streq(attr, "XXXX"))           // settable new state
    return 1;
  else 
    return PyObjectBase::_setattr(attr, value); 
} 


void MeshPy::setMesh(MeshCore::MeshKernel* pcMesh)
{
  if(pcMesh)
  {
    if(_pcMesh)
      delete _pcMesh;
    _pcMesh = pcMesh;
  }
}

const MeshCore::MeshKernel& MeshPy::getMesh(void) const
{
  return *_pcMesh;
}

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(MeshPy,makeCutToolFromShape)
{
#if 0
  Part::TopoShapePy   *pcObject;
  PyObject *pcObj;
  if (!PyArg_ParseTuple(args, "O!", &(Part::TopoShapePy::Type), &pcObj))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (Part::TopoShapePy*)pcObj;

  MeshKernel *M = new MeshKernel();
  PY_TRY {
    TopoDS_Shape aShape = pcObject->getShape();

    
    MeshAlgos::cutByShape(aShape, _pcMesh,M);  

  } PY_CATCH;

  return new MeshPy(M);
#else
  Base::Console().Error("Linker error: Part::TopoShapePy\n");
  Py_Return;
#endif
}

PYFUNCIMP_D(MeshPy,pointCount)
{
  return Py_BuildValue("i",_pcMesh->CountPoints()); 
}

PYFUNCIMP_D(MeshPy,faceCount)
{
  return Py_BuildValue("i",_pcMesh->CountFacets()); 
}

PYFUNCIMP_D(MeshPy,write)
{
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         

  PY_TRY {
    MeshOutput aWriter(*_pcMesh);
    aWriter.SaveAny(Name);
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,read)
{
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         

  Base::FileInfo File(Name);
  
  // checking on the file
  if(!File.isReadable())
    Py_Error(PyExc_Exception,"File to load not existing or not readable");

  PY_TRY {
    MeshAlgos::read(_pcMesh,Name);
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,offset)
{
  double Float;
  if (! PyArg_ParseTuple(args, "d",&Float))			 
    return NULL;                         

  PY_TRY {
    MeshAlgos::offsetSpecial2(_pcMesh,Float);  
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,offsetSpecial)
{
  double Float,zmin,zmax;
  if (! PyArg_ParseTuple(args, "ddd",&Float,&zmin,&zmax))			 
    return NULL;                         

  PY_TRY {
    MeshAlgos::offsetSpecial(_pcMesh,Float,zmax,zmin);  
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,flipNormals)
{
  if (! PyArg_ParseTuple(args, ""))			 
    return NULL;                         

  PY_TRY {
    MeshTopoAlgorithm Algo(*_pcMesh);
    Algo.FlipNormals();
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,harmonizeNormals)
{
  if (! PyArg_ParseTuple(args, ""))			 
    return NULL;                         

  PY_TRY {
    MeshTopoAlgorithm Algo(*_pcMesh);
    Algo.HarmonizeNormals();
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,countComponents)
{
  if (! PyArg_ParseTuple(args, ""))			 
    return NULL;                         

  std::vector<std::vector<unsigned long> > segments;
  PY_TRY {
    MeshComponents comp(*_pcMesh);
    comp.SearchForComponents(MeshComponents::OverEdge,segments);
  } PY_CATCH;

  return Py_BuildValue("i",segments.size());
}

PYFUNCIMP_D(MeshPy,removeComponents)
{
  int count;
  if (! PyArg_ParseTuple(args, "i", &count))			 
    return NULL;                         

  PY_TRY {
    if ( count > 0 ) {
      MeshTopoAlgorithm(*_pcMesh).RemoveComponents( (unsigned long)count );
    }
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,fillupHoles)
{
  int len;
  if (! PyArg_ParseTuple(args, "i", &len))			 
    return NULL;                         

  PY_TRY {
    MeshTopoAlgorithm topalg(*_pcMesh);
    topalg.FillupHoles((unsigned long)len);
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,refine)
{
  if (! PyArg_ParseTuple(args, ""))			 
    return NULL;                         

  PY_TRY {
    unsigned long cnt = _pcMesh->CountFacets();
    MeshFacetIterator cF(*_pcMesh);
    MeshTopoAlgorithm topalg(*_pcMesh);
    for ( unsigned long i=0; i<cnt; i++ ) {
      cF.Set(i);
      if ( !cF->IsDeformed() )
        topalg.InsertVertexAndSwapEdge(i, cF->GetGravityPoint(), 0.1f);
    }
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,optimizeTopology)
{
  float fMaxAngle;
  if (! PyArg_ParseTuple(args, "f; specify the maximum allowed angle between the normals of two adjacent facets", &fMaxAngle))			 
    return NULL;                         

  PY_TRY {
    MeshTopoAlgorithm topalg(*_pcMesh);
    topalg.OptimizeTopology(fMaxAngle);
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,optimizeEdges)
{
  if (! PyArg_ParseTuple(args, ""))			 
    return NULL;                         

  PY_TRY {
    MeshTopoAlgorithm topalg(*_pcMesh);
    topalg.AdjustEdgesToCurvatureDirection();
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,splitEdge)
{
  if (! PyArg_ParseTuple(args, ""))			 
    return NULL;                         

  PY_TRY {
    std::vector<std::pair<unsigned long, unsigned long> > adjacentFacet;
    MeshAlgorithm alg(*_pcMesh);
    alg.ResetFacetFlag(MeshFacet::VISIT);
    const MeshFacetArray& rFacets = _pcMesh->GetFacets();
    for ( MeshFacetArray::_TConstIterator pF = rFacets.begin(); pF != rFacets.end(); ++pF )
    {
      int id=2;
      if ( pF->_aulNeighbours[id] != ULONG_MAX ) {
        const MeshFacet& rFace = rFacets[pF->_aulNeighbours[id]];
        if ( !pF->IsFlag(MeshFacet::VISIT) && !rFace.IsFlag(MeshFacet::VISIT) ) {
          pF->SetFlag(MeshFacet::VISIT);
          rFace.SetFlag(MeshFacet::VISIT);
          adjacentFacet.push_back(std::make_pair(pF-rFacets.begin(), pF->_aulNeighbours[id]));
        }
      }
    }
    
    MeshFacetIterator cIter(*_pcMesh);
    MeshTopoAlgorithm topalg(*_pcMesh);
    for ( std::vector<std::pair<unsigned long, unsigned long> >::iterator it = adjacentFacet.begin(); it != adjacentFacet.end(); ++it )
    {
      cIter.Set(it->first);
      Base::Vector3f mid = 0.5f*(cIter->_aclPoints[0]+cIter->_aclPoints[2]);
      topalg.SplitEdge(it->first, it->second, mid);
    }
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,coarsen)
{
  double Float;
  if (! PyArg_ParseTuple(args, "d",&Float))			 
    return NULL;                         

  PY_TRY {
    MeshAlgos::coarsen(_pcMesh,Float);  
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,unite)
{
  MeshPy   *pcObject;
  PyObject *pcObj;
  if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (MeshPy*)pcObj;

  PY_TRY {
    MeshKernel& m = *(pcObject->_pcMesh);
    MeshCore::SetOperations setOp(*_pcMesh, m, *_pcMesh, MeshCore::SetOperations::Union, 1.0e-5);
    setOp.Do();
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,intersect)
{
  MeshPy   *pcObject;
  PyObject *pcObj;
  if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (MeshPy*)pcObj;

  PY_TRY {
    MeshKernel& m = *(pcObject->_pcMesh);
    MeshCore::SetOperations setOp(*_pcMesh, m, *_pcMesh, MeshCore::SetOperations::Intersect, 1.0e-5);
    setOp.Do();
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,diff)
{
  MeshPy   *pcObject;
  PyObject *pcObj;
  if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (MeshPy*)pcObj;

  PY_TRY {
    MeshKernel& m = *(pcObject->_pcMesh);
    MeshCore::SetOperations setOp(*_pcMesh, m, *_pcMesh, MeshCore::SetOperations::Difference, 1.0e-5);
    setOp.Do();
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,outer)
{
  MeshPy   *pcObject;
  PyObject *pcObj;
  if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (MeshPy*)pcObj;

  PY_TRY {
    MeshKernel& m = *(pcObject->_pcMesh);
    MeshCore::SetOperations setOp(*_pcMesh, m, *_pcMesh, MeshCore::SetOperations::Outer, 1.0e-5);
    setOp.Do();
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,inner)
{
  MeshPy   *pcObject;
  PyObject *pcObj;
  if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (MeshPy*)pcObj;

  PY_TRY {
    MeshKernel& m = *(pcObject->_pcMesh);
    MeshCore::SetOperations setOp(*_pcMesh, m, *_pcMesh, MeshCore::SetOperations::Inner, 1.0e-5);
    setOp.Do();
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,translate)
{
  float x,y,z;
  if (! PyArg_ParseTuple(args, "fff",&x,&y,&z))			 
    return NULL;                         

  PY_TRY {
    Base::Matrix4D m;
    m.move(x,y,z);
    _pcMesh->Transform(m);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,rotate)
{
  float x,y,z;
  if (! PyArg_ParseTuple(args, "fff",&x,&y,&z))			 
    return NULL;                         

  PY_TRY {
    Base::Matrix4D m;
    m.rotX(x);
    m.rotY(y);
    m.rotZ(z);
    _pcMesh->Transform(m);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,transformToEigen)
{
  if (! PyArg_ParseTuple(args, ""))			 
    return NULL;                         
  MeshEigensystem cMeshEval( *_pcMesh );
  cMeshEval.Evaluate();
  _pcMesh->Transform(cMeshEval.Transform());
  Py_Return;
}

PYFUNCIMP_D(MeshPy,scale)
{
  double s;
  if (! PyArg_ParseTuple(args, "d",&s))			 
    return NULL;                         

  PY_TRY {
    Base::Matrix4D m;
    m.scale(s,s,s);
    _pcMesh->Transform(m);  
  } PY_CATCH;

  Py_Return;
}


PYFUNCIMP_D(MeshPy,transform)
{
  Base::Matrix4D mat;
  PyObject *pcMatObj;

  if (PyArg_ParseTuple(args, "O!: a transform matrix (Matrix) is needed", &(App::MatrixPy::Type), &pcMatObj) )     // convert args: Python->C 
  {
    mat = ((App::MatrixPy*)pcMatObj)->value();
    PyErr_Clear();
  }
  else
    return NULL;

  PY_TRY
  {
    _pcMesh->Transform(mat);
  }
  PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,addFacet)
{
  double x1,y1,z1,x2,y2,z2,x3,y3,z3;
  if (! PyArg_ParseTuple(args, "ddddddddd",&x1,&y1,&z1,&x2,&y2,&z2,&x3,&y3,&z3))			 
    return NULL;                         

  PY_TRY {
    _pcMesh->AddFacet(MeshGeomFacet(Base::Vector3f(x1,y1,z1),
                                  Base::Vector3f(x2,y2,z2),
                                  Base::Vector3f(x3,y3,z3)));
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,addFacets)
{
  PyObject *list;

  vector<MeshGeomFacet> facets;
  if (PyArg_ParseTuple(args, "O!: list of vectors (3 of them defined a facet)", &PyList_Type, &list))     
  {
    if (PyList_Check(list))
    {
      int k = 0;
      MeshGeomFacet facet;
      for (int i = 0; i < PyList_Size(list); i++)
      {
        PyObject *vec = PyList_GetItem(list, i);
        if (PyList_Check(vec))
        {
          if (PyList_Size(vec) == 3)
          {
            for (int j = 0; j < 3; j++)
            {
              PyObject *val = PyList_GetItem(vec, j);
              if (PyFloat_Check(val))
              {                
                float f = PyFloat_AsDouble(val);
                facet._aclPoints[k][j] = f;
              }
              else
              {
                Py_Error(PyExc_Exception, "vector needs 3 double values");
                return NULL; // not a double
              }
            }
          }
          else
          {
            Py_Error(PyExc_Exception, "vector needs 3 double values");
            return NULL; // vector needs 3 doubles
          }
        }
        else
        {
          Py_Error(PyExc_Exception, "inner list should be 3 doubles as list");
          return NULL; // not a vector
        }
        k++;
        if (k == 3)
        {
          k = 0;
          facet.CalcNormal();
          facets.push_back(facet);
        }
        
      }    
    }
    else
    {
      Py_Error(PyExc_Exception, "need a list of 3 double values");
      return NULL; // not a list
    }

    PyErr_Clear();


  }
  else
  {
    Py_Error(PyExc_Exception, "need list of vectors (3 of them defined a facet)");
    return NULL;
  }

  PY_TRY {
    *(_pcMesh) = facets;
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,clear)
{
  PY_TRY {
    _pcMesh->Clear();
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,copy)
{
  PY_TRY {
    return new MeshPy( _pcMesh );
  } PY_CATCH;
}

PYFUNCIMP_D(MeshPy,hasNonUnifomOrientedFacets)
{
  MeshEvalOrientation cMeshEval( *_pcMesh );
  bool ok = cMeshEval.Evaluate();
  return Py_BuildValue("O", (ok ? Py_False : Py_True)); 
}

PYFUNCIMP_D(MeshPy,countNonUnifomOrientedFacets)
{
  MeshCore::MeshEvalOrientation cMeshEval(* _pcMesh );
  std::vector<unsigned long> inds = cMeshEval.GetIndices();
  return Py_BuildValue("i", inds.size()); 
}

PYFUNCIMP_D(MeshPy,isSolid)
{
  MeshEvalSolid cMeshEval( *_pcMesh );
  bool ok = cMeshEval.Evaluate();
  return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PYFUNCIMP_D(MeshPy,hasNonManifolds)
{
  MeshEvalTopology cMeshEval( *_pcMesh );
  bool ok = !cMeshEval.Evaluate();
  return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PYFUNCIMP_D(MeshPy,testDelaunay)
{
  PY_TRY {
    MeshPy* pyMesh = new MeshPy();
    
    // get all points
    std::vector< Wm4::Vector3<float> > aPnts;
    MeshPointIterator cPIt( *_pcMesh );
    for ( cPIt.Init(); cPIt.More(); cPIt.Next() )
    {
      Wm4::Vector3<float> cP( cPIt->x, cPIt->y, cPIt->z );
      aPnts.push_back( cP );
    }

    Wm4::Delaunay3<float> triaDel(aPnts.size(), &(aPnts[0]), MESH_MIN_PT_DIST, false, Wm4::Query::QT_INTEGER);
    int cnt; int* idx;

    if ( triaDel.GetHull(cnt, idx) )
    {
      std::vector<MeshGeomFacet> aFaces;
      for ( int i=0; i<cnt; i++ )
      {
        MeshGeomFacet face;
        for ( int j=0; j<3; j++ )
        {
          face._aclPoints[j].Set(aPnts[*idx].X(), aPnts[*idx].Y(), aPnts[*idx].Z());
          *idx++;
        }

        aFaces.push_back( face );
      }

      MeshKernel& kernel = *(pyMesh->_pcMesh);
      kernel = aFaces;
    }

//    delete [] idx;

    return pyMesh;
  } PY_CATCH;
}
