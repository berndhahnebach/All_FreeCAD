/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <Inventor/actions/SoHandleEventAction.h>
# include <Inventor/actions/SoWriteAction.h>
# include <Inventor/events/SoLocation2Event.h>
# include <Inventor/nodes/SoCamera.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
# include <Inventor/nodes/SoPerspectiveCamera.h>
#endif


#include "View3DInventor.h"
#include "View3DPy.h"

#include <Base/PyExport.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include "Application.h"
#include "SoFCSelectionAction.h"
#include "View3DInventorViewer.h"

#include <Base/PyCXX/Objects.hxx>

using Base::Console;
using Base::streq;
using namespace Gui;


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject View3DPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"View3D",				/*tp_name*/
	sizeof(View3DPy),			/*tp_basicsize*/
	0,						/*tp_itemsize*/
	/* methods */
	PyDestructor,	  		/*tp_dealloc*/
	0,			 			/*tp_print*/
	__getattr, 				/*tp_getattr*/
	__setattr, 				/*tp_setattr*/
	0,						/*tp_compare*/
	__repr,					/*tp_repr*/
	0,						/*tp_as_number*/
	0,						/*tp_as_sequence*/
	0,						/*tp_as_mapping*/
	0,						/*tp_hash*/
	0,						/*tp_call */
  0,                                                /*tp_str  */
  0,                                                /*tp_getattro*/
  0,                                                /*tp_setattro*/
  /* --- Functions to access object as input/output buffer ---------*/
  0,                                                /* tp_as_buffer */
  /* --- Flags to define presence of optional/expanded features */
  Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS,        /*tp_flags */
  "About View3D",                                 /*tp_doc */
  0,                                                /*tp_traverse */
  0,                                                /*tp_clear */
  0,                                                /*tp_richcompare */
  0,                                                /*tp_weaklistoffset */
  0,                                                /*tp_iter */
  0,                                                /*tp_iternext */
  View3DPy::Methods,                                /*tp_methods */
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

PyDoc_STRVAR(view_getCursorPos_doc,
"getCursorPos() -> tuple of integers\n"
"\n"
"Return the current cursor position relative to the coordinate system of the\n"
"viewport region.\n");

PyDoc_STRVAR(view_getObjectInfo_doc,
"getObjectInfoPos(tuple of integers) -> dictionary or None\n"
"\n"
"Return a dictionary with the name of document, object and component. The\n"
"dictionary also contains the coordinates of the appropriate 3d point of\n"
"the underlying geometry in the scenegraph.\n"
"If no geometry was found 'None' is returned, instead.\n");

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef View3DPy::Methods[] = {
//  {"DocType",      (PyCFunction) sPyDocType,         Py_NEWARGS},
  PYMETHODEDEF(message)
  PYMETHODEDEF(fitAll)
  PYMETHODEDEF(viewBottom)
  PYMETHODEDEF(viewFront)
  PYMETHODEDEF(viewLeft)
  PYMETHODEDEF(viewRear)
  PYMETHODEDEF(viewRight)
  PYMETHODEDEF(viewTop)
  PYMETHODEDEF(viewAxometric)
  PYMETHODEDEF(saveImage)
  PYMETHODEDEF(getCamera)
  PYMETHODEDEF(setCamera)
  PYMETHODEDEF(getCameraType)
  PYMETHODEDEF(setCameraType)
  PYMETHODEDEF(listCameraTypes)
  PYMETHODEDEF(dump)
  PYMETHODEDEF(setStereoType)
  PYMETHODEDEF(getStereoType)
  PYMETHODEDEF(listStereoTypes)
  {"getCursorPos",  (PyCFunction) sgetCursorPos,  Py_NEWARGS, view_getCursorPos_doc },
  {"getObjectInfo", (PyCFunction) sgetObjectInfo, Py_NEWARGS, view_getObjectInfo_doc},
  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject View3DPy::Parents[] = {&View3DPy::Type,&PyObjectBase::Type, NULL};     

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
View3DPy::View3DPy(View3DInventor *pcView, PyTypeObject *T)
 : PyObjectBase(0, T), _pcView(pcView)
{
}

PyObject *View3DPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new View3DPy(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
View3DPy::~View3DPy()						// Everything handled in parent
{
} 


//--------------------------------------------------------------------------
// View3DPy representation
//--------------------------------------------------------------------------
PyObject *View3DPy::_repr(void)
{
	return Py_BuildValue("s", "3D View");
}
//--------------------------------------------------------------------------
// View3DPy Attributes
//--------------------------------------------------------------------------
PyObject *View3DPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
			 _getattr_up(PyObjectBase); 						
} 

int View3DPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
		return PyObjectBase::_setattr(attr, value); 	// send up to parent
} 



//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PYFUNCIMP_D(View3DPy,message)
{ 
  const char **ppReturn = 0;
  char *psMsgStr;
  if (!PyArg_ParseTuple(args, "s;Message string needed (string)",&psMsgStr))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
   
    if(_pcView->onMsg(psMsgStr,ppReturn) && ppReturn)
    {
      // create string out of ppReturn....
      Py_Return;
    }
    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(View3DPy,fitAll)
{
 if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
   
    _pcView->_viewer->viewAll();
    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(View3DPy,viewBottom)
{
 if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
    SoCamera* cam = _pcView->_viewer->getCamera();
    cam->orientation.setValue(-1, 0, 0, 0);
    _pcView->_viewer->viewAll();
    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(View3DPy,viewFront)
{
 if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
    float root = (float)(sqrt(2.0)/2.0);
    SoCamera* cam = _pcView->_viewer->getCamera();
    cam->orientation.setValue(-root, 0, 0, -root);
    _pcView->_viewer->viewAll();
    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(View3DPy,viewLeft)
{
 if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
    SoCamera* cam = _pcView->_viewer->getCamera();
    cam->orientation.setValue(0.5, 0.5, 0.5, 0.5);
    _pcView->_viewer->viewAll();
    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(View3DPy,viewRear)
{
 if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
    float root = (float)(sqrt(2.0)/2.0);
    SoCamera* cam = _pcView->_viewer->getCamera();
    cam->orientation.setValue(0, root, root, 0);
    _pcView->_viewer->viewAll();
    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(View3DPy,viewRight)
{
 if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
    SoCamera* cam = _pcView->_viewer->getCamera();
    cam->orientation.setValue(-0.5, 0.5, 0.5, -0.5);
    _pcView->_viewer->viewAll();
    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(View3DPy,viewTop)
{
 if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
    SoCamera* cam = _pcView->_viewer->getCamera();
    cam->orientation.setValue(0, 0, 0, 1);
    _pcView->_viewer->viewAll();
    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(View3DPy,viewAxometric)
{
 if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
    float root = (float)(sqrt(3.0)/4.0);
    SoCamera* cam = _pcView->_viewer->getCamera();
    cam->orientation.setValue(-0.333333f, -0.166666f, -0.333333f, -root);
    _pcView->_viewer->viewAll();
    Py_Return;

  }PY_CATCH;
} 

PYFUNCIMP_D(View3DPy,saveImage)
{ 
  char *cFileName,*cImageType="Current",*cComment="$MIBA";
  int w=-1,h=-1,t;

  if (!PyArg_ParseTuple(args, "s|iiss",&cFileName,&w,&h,&cImageType,&cComment))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {

#ifdef __GNUC__
    if(strcasecmp(cImageType,"Current")==0)
      t=0;
    else if(strcasecmp(cImageType,"Black")==0)
      t=1;
    else if(strcasecmp(cImageType,"White")==0)
      t=2;
    else if(strcasecmp(cImageType,"Transparent")==0)
      t=3;
    else 
      Py_Error(PyExc_Exception,"Parameter 4 have to be (Current|Black|White|Transparent)");
#else
    if(_stricmp(cImageType,"Current")==0)
      t=0;
    else if(_stricmp(cImageType,"Black")==0)
      t=1;
    else if(_stricmp(cImageType,"White")==0)
      t=2;
    else if(_stricmp(cImageType,"Transparent")==0)
      t=3;
    else 
      Py_Error(PyExc_Exception,"Parameter 4 have to be (Current|Black|White|Transparent)");
#endif

    QColor c;
    _pcView->_viewer->makeScreenShot(cFileName,w,h,t,cComment);
    Py_Return;

  }PY_CATCH;
}

PYFUNCIMP_D(View3DPy,getCamera)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
    SoOutput out;
    char buffer[512];
    out.setBuffer(buffer, 512, 0);

    SoWriteAction wa(&out);
    SoCamera * cam = _pcView->_viewer->getCamera();
    wa.apply(cam);
    return Py_BuildValue("s", buffer);
  }PY_CATCH;
}

PYFUNCIMP_D(View3DPy,setCamera)
{
  char* buffer;
  if (!PyArg_ParseTuple(args, "s", &buffer))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
    _pcView->setCamera(buffer);
    Py_Return;
  }PY_CATCH;
}

//FIXME: Once View3DInventor inherits from PropertyContainer we can use PropertyEnumeration.
const char* CameraTypeEnums[]= {"Orthographic","Perspective",NULL};

PYFUNCIMP_D(View3DPy,getCameraType)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
    SoCamera* cam = _pcView->_viewer->getCamera();
    if (!cam) {
      PyErr_SetString(PyExc_AssertionError, "No camera set!");
      return NULL;
    } else if ( cam->getTypeId() == SoOrthographicCamera::getClassTypeId() ) {
      return Py_BuildValue("s", CameraTypeEnums[0]);
    } else if ( cam->getTypeId() == SoPerspectiveCamera::getClassTypeId() ) {
      return Py_BuildValue("s", CameraTypeEnums[1]);
    } else {
      return Py_BuildValue("s", "Unknown camera type");
    }
  }PY_CATCH;
}

PYFUNCIMP_D(View3DPy,setCameraType)
{
  int cameratype=-1;
  if (!PyArg_ParseTuple(args, "i", &cameratype)) {    // convert args: Python->C 
    char* modename;
    PyErr_Clear();
    if (!PyArg_ParseTuple(args, "s", &modename))
      return NULL;  // NULL triggers exception
    for ( int i=0; i<2; i++ ) {
      if ( strncmp(CameraTypeEnums[i],modename,20) == 0 ) {
        cameratype = i;
        break;
      }
    }

    if ( cameratype < 0 ) {
      PyErr_Format(PyExc_NameError, "Unknown camera type '%s'", modename);
      return NULL;
    }
  }

  PY_TRY {
    if (cameratype < 0 || cameratype > 1)
      throw Base::Exception("Out of range");
    if (cameratype==0)
      _pcView->_viewer->setCameraType(SoOrthographicCamera::getClassTypeId());
    else
      _pcView->_viewer->setCameraType(SoPerspectiveCamera::getClassTypeId());
    Py_Return;
  }PY_CATCH;
}

PYFUNCIMP_D(View3DPy,listCameraTypes)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 
  PY_TRY {
    PyObject* pyList = PyList_New(2);
    for ( int i=0; i<2; i++ )
    {
      PyObject* str = PyString_FromString(CameraTypeEnums[i]);
      PyList_SetItem(pyList, i, str);
    }

    return pyList;
  }PY_CATCH;
}

PYFUNCIMP_D(View3DPy,dump)
{
  char* filename;
  if (!PyArg_ParseTuple(args, "s", &filename))     // convert args: Python->C 
    return NULL;  // NULL triggers exception 

  PY_TRY {
    _pcView->dump(filename);
    Py_Return;
  }PY_CATCH;
}

//FIXME: Once View3DInventor inherits from PropertyContainer we can use PropertyEnumeration.
const char* StereoTypeEnums[]= {"None","Anaglyph","QuadBuffer","InterleavedRows","InterleavedColumns",NULL};

PYFUNCIMP_D(View3DPy,setStereoType)
{
  int stereomode=-1;
  if (!PyArg_ParseTuple(args, "i", &stereomode)) {    // convert args: Python->C 
    char* modename;
    PyErr_Clear();
    if (!PyArg_ParseTuple(args, "s", &modename))
      return NULL;  // NULL triggers exception
    for ( int i=0; i<5; i++ ) {
      if ( strncmp(StereoTypeEnums[i],modename,20) == 0 ) {
        stereomode = i;
        break;
      }
    }

    if ( stereomode < 0 ) {
      PyErr_Format(PyExc_NameError, "Unknown stereo type '%s'", modename);
      return NULL;
    }
  }

  PY_TRY {
#if (SOQT_MAJOR_VERSION >= 1 && SOQT_MINOR_VERSION >= 2)
    if (stereomode < 0 || stereomode > 4)
      throw Base::Exception("Out of range");
    SoQtViewer::StereoType mode = SoQtViewer::StereoType(stereomode);
    _pcView->_viewer->setStereoType(mode);
#else
    throw Base::Exception("Stereo types not supported. Use SoQt 1.2.x or later!");
#endif
    Py_Return;
  }PY_CATCH;
}

PYFUNCIMP_D(View3DPy,getStereoType)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;  // NULL triggers exception

  PY_TRY {
#if (SOQT_MAJOR_VERSION >= 1 && SOQT_MINOR_VERSION >= 2)
    int mode = (int)(_pcView->_viewer->getStereoType()); 
    return Py_BuildValue("s", StereoTypeEnums[mode]);
#else
    throw Base::Exception("Stereo types not supported. Use SoQt 1.2.x or later!");
#endif
    Py_Return;
  }PY_CATCH;
}

PYFUNCIMP_D(View3DPy,listStereoTypes)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 
  PY_TRY {
    PyObject* pyList = PyList_New(5);
    for ( int i=0; i<5; i++ )
    {
      PyObject* str = PyString_FromString(StereoTypeEnums[i]);
      PyList_SetItem(pyList, i, str);
    }

    return pyList;
  }PY_CATCH;
} 

PYFUNCIMP_D(View3DPy,getCursorPos)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 
  try {
    QPoint pos = _pcView->mapFromGlobal(QCursor::pos());
    Py::Tuple tuple(2);
    tuple.setItem(0, Py::Int(pos.x()));
    tuple.setItem(1, Py::Int(_pcView->height()-pos.y()-1));
    return Py::new_reference_to(tuple); // increment ref counter
  } catch (const Py::Exception&) {
    return NULL;
  }
}

PYFUNCIMP_D(View3DPy,getObjectInfo)
{
  PyObject* object;
  if (!PyArg_ParseTuple(args, "O", &object))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 

  try {
    //Note: For gcc (4.2) we need the 'const' keyword to avoid the compiler error:
    //conversion from 'Py::seqref<Py::Object>' to non-scalar type 'Py::Int' requested
    //We should report this problem to the PyCXX project as in the documentation an 
    //example without the 'const' keyword is used.
    //Or we can also write Py::Int x(tuple[0]);
    const Py::Tuple tuple(object);
    Py::Int x = tuple[0];
    Py::Int y = tuple[1];

    Gui::SoFCDocumentObjectEvent ev;
    ev.setPosition(SbVec2s((long)x,(long)y));
    SoHandleEventAction action(_pcView->getViewer()->getViewportRegion());
    action.setEvent(&ev);
    action.apply(_pcView->getViewer()->getSceneManager()->getSceneGraph());

    if (action.isHandled()) {
      Py::Dict dict;
      dict.setItem("Document", Py::String(ev.getDocumentName().getString()));
      dict.setItem("Object", Py::String(ev.getObjectName().getString()));
      dict.setItem("Component", Py::String(ev.getComponentName().getString()));
      dict.setItem("x", Py::Float(ev.getPoint()[0]));
      dict.setItem("y", Py::Float(ev.getPoint()[1]));
      dict.setItem("z", Py::Float(ev.getPoint()[2]));
      return Py::new_reference_to(dict);  // increment ref counter
    } else {
      return Py::new_reference_to(Py::None());
    }
  } catch (const Py::Exception&) {
    return NULL;
  }
}
