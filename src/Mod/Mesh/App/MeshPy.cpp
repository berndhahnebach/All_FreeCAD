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
# include <Wm3Delaunay3.h>
# include <Wm3Vector3.h>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>

using Base::Console;


#include "MeshPy.h"
#include "Mesh.h"
#include "MeshAlgos.h"
#include "Core/MeshKernel.h"
#include "Core/MeshIO.h"
#include "Core/Info.h"
#include "Core/Evaluation.h"
#include "Core/Iterator.h"
#include <App/Topology.h>

using namespace Mesh;
using namespace MeshCore;


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject MeshPy::Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                      /*ob_size*/
  "Mesh",        /*tp_name*/
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
  Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS,        /*tp_flags */
  "About PyObjectBase",                             /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  0,                                                /*tp_methods */
  0,                                                /*tp_members */
  0,                                                /*tp_getset */
  &Base::PyObjectBase::Type,                        /*tp_base */
  0,                                                /*tp_dict */
  0,                                                /*tp_descr_get */
  0,                                                /*tp_descr_set */
  0,                                                /*tp_dictoffset */
  0,                                                /*tp_init */
  0,                                                /*tp_alloc */
  0,                                                /*tp_new */
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
  PYMETHODEDEF(pointCount)
  PYMETHODEDEF(faceCount)
  PYMETHODEDEF(read)
  PYMETHODEDEF(write)
  PYMETHODEDEF(offset)
  PYMETHODEDEF(calcVertexNormales)
  PYMETHODEDEF(calcVertexCurvature)
  PYMETHODEDEF(calcFaceCurvature)
  PYMETHODEDEF(Union)
  PYMETHODEDEF(intersect)
  PYMETHODEDEF(diff)
  PYMETHODEDEF(coarsen)
  PYMETHODEDEF(translate)
  PYMETHODEDEF(rotate)
  PYMETHODEDEF(transformToEigen)
  PYMETHODEDEF(scale)
  PYMETHODEDEF(addFacet)
  PYMETHODEDEF(clear)
  PYMETHODEDEF(copy)
  PYMETHODEDEF(hasConsistentOrientation)
  PYMETHODEDEF(isSolid)
  PYMETHODEDEF(hasNonManifolds)
  PYMETHODEDEF(testDelaunay)
  PYMETHODEDEF(makeCutToolFromShape)
  PYMETHODEDEF(cutOuter)
  PYMETHODEDEF(cutInner)
  {NULL, NULL}    /* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject MeshPy::Parents[] = {&Base::PyObjectBase::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
MeshPy::MeshPy(MeshWithProperty *pcMesh,bool ReferencedMesh, PyTypeObject *T)
: Base::PyObjectBase(T), _pcMesh(pcMesh),_bReferencedMesh(ReferencedMesh)
{
  Base::Console().Log("Create MeshPy: %p \n",this);
}

PyObject *MeshPy::PyMake(PyObject *ignored, PyObject *args)  // Python wrapper
{
  //return new MeshPy(name, n, tau, gamma);      // Make new Python-able object
  return 0;
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
MeshPy::~MeshPy()           // Everything handled in parent
{
  Base::Console().Log("Destroy MeshPy: %p \n",this);
  if(!_bReferencedMesh) delete _pcMesh;
} 

//--------------------------------------------------------------------------
// MeshPy representation
//--------------------------------------------------------------------------
PyObject *MeshPy::_repr(void)
{
  std::stringstream a;
  MeshInfo info(*_pcMesh->getKernel());
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


void MeshPy::setMesh(MeshWithProperty *pcMesh)
{
  _pcMesh = pcMesh;
}

MeshWithProperty *MeshPy::getMesh(void)
{
  return _pcMesh;
}

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(MeshPy,makeCutToolFromShape)
{
  App::TopoShapePy   *pcObject;
  PyObject *pcObj;
  if (!PyArg_ParseTuple(args, "O!", &(App::TopoShapePy::Type), &pcObj))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (App::TopoShapePy*)pcObj;

  MeshWithProperty *M = new MeshWithProperty();
  PY_TRY {
    TopoDS_Shape aShape = pcObject->getShape();

    
    MeshAlgos::cutByShape(aShape, _pcMesh,M);  

  } PY_CATCH;

  return new MeshPy(M);
}

PYFUNCIMP_D(MeshPy,pointCount)
{
  return Py_BuildValue("i",_pcMesh->getKernel()->CountPoints()); 
}

PYFUNCIMP_D(MeshPy,faceCount)
{
  return Py_BuildValue("i",_pcMesh->getKernel()->CountFacets()); 
}

PYFUNCIMP_D(MeshPy,write)
{
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         

  PY_TRY {
    MeshAlgos::writeBin(_pcMesh,Name);
  } PY_CATCH;

  Py_Return; 
}

PYFUNCIMP_D(MeshPy,read)
{
  const char* Name;
  if (! PyArg_ParseTuple(args, "s",&Name))			 
    return NULL;                         

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
    MeshAlgos::offset(_pcMesh,Float);  
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

PYFUNCIMP_D(MeshPy,calcVertexNormales)
{
  PY_TRY {
    MeshAlgos::calcVertexNormales(_pcMesh);  
  } PY_CATCH;
  
  Py_Return;

}

PYFUNCIMP_D(MeshPy,calcVertexCurvature)
{
  PY_TRY {
    MeshAlgos::calcVertexCurvature(_pcMesh);  
  } PY_CATCH;
  
  Py_Return;

}

PYFUNCIMP_D(MeshPy,calcFaceCurvature)
{
  PY_TRY {
    MeshAlgos::calcFaceCurvature(_pcMesh);  
  } PY_CATCH;
  
  Py_Return;

}

PYFUNCIMP_D(MeshPy,Union)
{
 	MeshPy   *pcObject;
  PyObject *pcObj;
  if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (MeshPy*)pcObj;

  PY_TRY {
    MeshWithProperty* m = pcObject->_pcMesh;
    MeshAlgos::boolean(_pcMesh,m,_pcMesh,0);
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
    MeshWithProperty* m = pcObject->_pcMesh;
    MeshAlgos::boolean(_pcMesh,m,_pcMesh,1);  
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
    MeshWithProperty* m = pcObject->_pcMesh;
    MeshAlgos::boolean(_pcMesh,m,_pcMesh,2);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,cutOuter)
{
 	MeshPy   *pcObject;
  PyObject *pcObj;
  if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (MeshPy*)pcObj;

  PY_TRY {
    MeshWithProperty* m = pcObject->_pcMesh;
    MeshAlgos::boolean(_pcMesh,m,_pcMesh,4);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,cutInner)
{
 	MeshPy   *pcObject;
  PyObject *pcObj;
  if (!PyArg_ParseTuple(args, "O!", &(MeshPy::Type), &pcObj))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  pcObject = (MeshPy*)pcObj;

  PY_TRY {
    MeshWithProperty* m = pcObject->_pcMesh;
    MeshAlgos::boolean(_pcMesh,m,_pcMesh,3);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,translate)
{
  double x,y,z;
  if (! PyArg_ParseTuple(args, "ddd",&x,&y,&z))			 
    return NULL;                         

  PY_TRY {
    Matrix4D m;
    m.SetMoveX(x);
    m.SetMoveY(y);
    m.SetMoveZ(z);
    _pcMesh->transform(m);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,rotate)
{
  double x,y,z;
  if (! PyArg_ParseTuple(args, "ddd",&x,&y,&z))			 
    return NULL;                         

  PY_TRY {
    Matrix4D m;
    m.SetRotX(x);
    m.SetRotY(y);
    m.SetRotZ(z);
    _pcMesh->transform(m);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,transformToEigen)
{
  PyObject* pcBool;
  if (!PyArg_ParseTuple(args, "O", &pcBool))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  bool ok = (pcBool == Py_True ? true : false);
  MeshEigensystem cMeshEval( *_pcMesh->getKernel() );
  switch ( cMeshEval.Validate( ok ) )
  {
  case MeshEvaluation::Fixed:
    ok = true;
    break;
  case MeshEvaluation::Valid:
  case MeshEvaluation::Invalid:
  default:
    ok = false;
    break;
  }

  return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PYFUNCIMP_D(MeshPy,scale)
{
  double s;
  if (! PyArg_ParseTuple(args, "d",&s))			 
    return NULL;                         

  PY_TRY {
    Matrix4D m;
    m.SetScaleX(s);
    m.SetScaleY(s);
    m.SetScaleZ(s);
    _pcMesh->transform(m);  
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,addFacet)
{
  double x1,y1,z1,x2,y2,z2,x3,y3,z3;
  if (! PyArg_ParseTuple(args, "ddddddddd",&x1,&y1,&z1,&x2,&y2,&z2,&x3,&y3,&z3))			 
    return NULL;                         

  PY_TRY {
    _pcMesh->getKernel()->AddFacet(MeshGeomFacet(Vector3D(x1,y1,z1),
                                                 Vector3D(x2,y2,z2),
                                                 Vector3D(x3,y3,z3)));
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,clear)
{
  PY_TRY {
    _pcMesh->clear();
  } PY_CATCH;

  Py_Return;
}

PYFUNCIMP_D(MeshPy,copy)
{
  PY_TRY {
   return new MeshPy(new MeshWithProperty(*_pcMesh));
  } PY_CATCH;
}

PYFUNCIMP_D(MeshPy,hasConsistentOrientation)
{
  std::string txt;
  MeshEvalNormals cMeshEval( *_pcMesh->getKernel() );
  switch ( cMeshEval.Validate() )
  {
  case MeshEvaluation::Valid:
    txt = "True";
    break;
  case MeshEvaluation::Fixed:
    txt = "Tried to fix";
    break;
  case MeshEvaluation::Invalid:
    txt = "False";
    break;
  default:
    txt = "no information";
    break;
  }

  return Py_BuildValue("s",txt.c_str()); 
}

PYFUNCIMP_D(MeshPy,isSolid)
{
  bool ok;
  MeshEvalSolid cMeshEval( *_pcMesh->getKernel() );
  switch ( cMeshEval.Validate() )
  {
  case MeshEvaluation::Valid:
    ok = true;
    break;
  case MeshEvaluation::Fixed:
  case MeshEvaluation::Invalid:
  default:
    ok = false;
    break;
  }

  return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PYFUNCIMP_D(MeshPy,hasNonManifolds)
{
  bool ok;
  MeshEvalTopology cMeshEval( *_pcMesh->getKernel() );
  switch ( cMeshEval.Validate() )
  {
  case MeshEvaluation::Valid:
    ok = false;
    break;
  case MeshEvaluation::Fixed:
  case MeshEvaluation::Invalid:
  default:
    ok = true;
    break;
  }

  return Py_BuildValue("O", (ok ? Py_True : Py_False)); 
}

PYFUNCIMP_D(MeshPy,testDelaunay)
{
  PY_TRY {
    MeshPy* pyMesh = new MeshPy(new MeshWithProperty(*_pcMesh));
    
    // get all points
    MeshKernel* pMesh = pyMesh->getMesh()->getKernel();
    std::vector< Wm3::Vector3<float> > aPnts;
    MeshPointIterator cPIt( *pMesh );
    for ( cPIt.Init(); cPIt.More(); cPIt.Next() )
    {
      Wm3::Vector3<float> cP( cPIt->x, cPIt->y, cPIt->z );
      aPnts.push_back( cP );
    }

    Wm3::Delaunay3<float> triaDel(aPnts.size(), &(aPnts[0]), MESH_MIN_PT_DIST, false);
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

      MeshKernel& kernel = *(pyMesh->getMesh()->getKernel());
      kernel = aFaces;
    }

//    delete [] idx;

    return pyMesh;
  } PY_CATCH;
}
