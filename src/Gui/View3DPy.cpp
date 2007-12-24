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

#ifndef __InventorAll__
# include "InventorAll.h"
#endif


#include "View3DInventor.h"
#include "View3DPy.h"
#include "ViewProviderExtern.h"
#include "Document.h"

#include <Base/PyExport.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/PyCXX/Objects.hxx>
#include <Base/VectorPy.h>
#include "Application.h"
#include "SoFCSelectionAction.h"
#include "View3DInventorViewer.h"

#include "swigpyrun.h"

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
  {"getCursorPos",  (PyCFunction) sgetCursorPos,  Py_NEWARGS, 
    "getCursorPos() -> tuple of integers\n"
    "\n"
    "Return the current cursor position relative to the coordinate system of the\n"
    "viewport region.\n" },
  {"getObjectInfo", (PyCFunction) sgetObjectInfo, Py_NEWARGS, 
    "getObjectInfoPos(tuple of integers) -> dictionary or None\n"
    "\n"
    "Return a dictionary with the name of document, object and component. The\n"
    "dictionary also contains the coordinates of the appropriate 3d point of\n"
    "the underlying geometry in the scenegraph.\n"
    "If no geometry was found 'None' is returned, instead.\n"},
  PYMETHODEDEF(getSize)
  {"getPoint", (PyCFunction) sgetPoint, Py_NEWARGS, 
    "getPoint(pixel coords (as integer)) -> 3D vector\n"
    "\n"
    "Return the according 3D point on the focal plane to the given 2D point (in\n"
    "pixel coordinates).\n"},
  PYMETHODEDEF(addEventCallback)
  PYMETHODEDEF(removeEventCallback)
  PYMETHODEDEF(setAnnotation)
  PYMETHODEDEF(removeAnnotation)
  PYMETHODEDEF(getSceneGraph)
  PYMETHODEDEF(addEventCallbackSWIG)
  PYMETHODEDEF(removeEventCallbackSWIG)
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

PYFUNCIMP_D(View3DPy,getSize)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 
    try {
        SbVec2s size = _pcView->getViewer()->getSize();
        Py::Tuple tuple(2);
        tuple.setItem(0, Py::Int(size[0]));
        tuple.setItem(1, Py::Int(size[1]));
        return Py::new_reference_to(tuple); // increment ref counter
    } catch (const Py::Exception&) {
        return NULL;
    }
}

PYFUNCIMP_D(View3DPy,getPoint)
{
    int x,y;
    if (!PyArg_ParseTuple(args, "ii", &x, &y))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 
    try {
        const SbViewportRegion& vp = _pcView->getViewer()->getViewportRegion();

        SbVec2f siz = vp.getViewportSize();
        float dX, dY; siz.getValue(dX, dY);

        float fRatio = vp.getViewportAspectRatio();
        float pX = (float)x / float(vp.getViewportSizePixels()[0]);
        float pY = (float)y / float(vp.getViewportSizePixels()[1]);

        // now calculate the real points respecting aspect ratio information
        //
        if (fRatio > 1.0f) {
            pX = ( pX - 0.5f*dX ) * fRatio + 0.5f*dX;
        }
        else if (fRatio < 1.0f) {
            pY = ( pY - 0.5f*dY ) / fRatio + 0.5f*dY;
        }
        
        SoCamera* pCam = _pcView->getViewer()->getCamera();  
        SbViewVolume  vol = pCam->getViewVolume(); 
        
        SbLine line; SbVec3f pt;
        SbPlane focalPlane = vol.getPlane(pCam->focalDistance.getValue());
        vol.projectPointToLine(SbVec2f(pX,pY), line);
        focalPlane.intersect(line, pt);
        
        return new Base::VectorPy(Base::Vector3f(pt[0], pt[1], pt[2]));
    } catch (const Py::Exception&) {
        return NULL;
    }
}

void View3DPy::eventCallback(void * ud, SoEventCallback * n)
{
    try {
        Py::Dict dict;
        const SoEvent* e = n->getEvent();
        if (!e) return; // invalid event
        // Type
        dict.setItem("Type", Py::String(std::string(e->getTypeId().getName().getString())));
        // Time
        dict.setItem("Time", Py::String(std::string(e->getTime().formatDate().getString())));
        SbVec2s p = n->getEvent()->getPosition();
        Py::Tuple pos(2);
        pos.setItem(0, Py::Int(p[0]));
        pos.setItem(1, Py::Int(p[1]));
        // Position
        dict.setItem("Position", pos);
        // Shift, Ctrl, Alt down
        dict.setItem("ShiftDown", Py::Object((e->wasShiftDown() ? Py_True : Py_False)));
        dict.setItem("CtrlDown",  Py::Object((e->wasCtrlDown()  ? Py_True : Py_False)));
        dict.setItem("AltDown",   Py::Object((e->wasAltDown()   ? Py_True : Py_False)));
        if (e->isOfType(SoButtonEvent::getClassTypeId())) {
            std::string state;
            const SoButtonEvent* be = static_cast<const SoButtonEvent*>(e);
            switch (be->getState()) {
                case SoButtonEvent::UP:
                    state = "UP";
                    break;
                case SoButtonEvent::DOWN:
                    state = "DOWN";
                    break;
                default:
                    state = "UNKNOWN";
                    break;
            }

            dict.setItem("State", Py::String(state));
        }
        if (e->isOfType(SoKeyboardEvent::getClassTypeId())) {
            const SoKeyboardEvent* ke = static_cast<const SoKeyboardEvent*>(e);
            switch (ke->getKey()) {
                case SoKeyboardEvent::ANY:
                    dict.setItem("Key", Py::String("ANY"));
                    break;
                case SoKeyboardEvent::UNDEFINED:
                    dict.setItem("Key", Py::String("UNDEFINED"));
                    break;
                case SoKeyboardEvent::LEFT_SHIFT:
                case SoKeyboardEvent::RIGHT_SHIFT:
                    dict.setItem("Key", Py::String("SHIFT"));
                    break;
                case SoKeyboardEvent::LEFT_CONTROL:
                case SoKeyboardEvent::RIGHT_CONTROL:
                    dict.setItem("Key", Py::String("CONTROL"));
                    break;
                case SoKeyboardEvent::LEFT_ALT:
                case SoKeyboardEvent::RIGHT_ALT:
                    dict.setItem("Key", Py::String("ALT"));
                    break;
                case SoKeyboardEvent::HOME:
                    dict.setItem("Key", Py::String("HOME"));
                    break;
                case SoKeyboardEvent::LEFT_ARROW:
                    dict.setItem("Key", Py::String("LEFT_ARROW"));
                    break;
                case SoKeyboardEvent::UP_ARROW:
                    dict.setItem("Key", Py::String("UP_ARROW"));
                    break;
                case SoKeyboardEvent::RIGHT_ARROW:
                    dict.setItem("Key", Py::String("RIGHT_ARROW"));
                    break;
                case SoKeyboardEvent::DOWN_ARROW:
                    dict.setItem("Key", Py::String("DOWN_ARROW"));
                    break;
                case SoKeyboardEvent::PAGE_UP:
                    dict.setItem("Key", Py::String("PAGE_UP"));
                    break;
                case SoKeyboardEvent::PAGE_DOWN:
                    dict.setItem("Key", Py::String("PAGE_DOWN"));
                    break;
                case SoKeyboardEvent::END:
                    dict.setItem("Key", Py::String("END"));
                    break;
                case SoKeyboardEvent::PAD_ENTER:
                    dict.setItem("Key", Py::String("PAD_ENTER"));
                    break;
                case SoKeyboardEvent::PAD_F1:
                    dict.setItem("Key", Py::String("PAD_F1"));
                    break;
                case SoKeyboardEvent::PAD_F2:
                    dict.setItem("Key", Py::String("PAD_F2"));
                    break;
                case SoKeyboardEvent::PAD_F3:
                    dict.setItem("Key", Py::String("PAD_F3"));
                    break;
                case SoKeyboardEvent::PAD_F4:
                    dict.setItem("Key", Py::String("PAD_F4"));
                    break;
                case SoKeyboardEvent::PAD_0:
                    dict.setItem("Key", Py::String("PAD_0"));
                    break;
                case SoKeyboardEvent::PAD_1:
                    dict.setItem("Key", Py::String("PAD_1"));
                    break;
                case SoKeyboardEvent::PAD_2:
                    dict.setItem("Key", Py::String("PAD_2"));
                    break;
                case SoKeyboardEvent::PAD_3:
                    dict.setItem("Key", Py::String("PAD_3"));
                    break;
                case SoKeyboardEvent::PAD_4:
                    dict.setItem("Key", Py::String("PAD_4"));
                    break;
                case SoKeyboardEvent::PAD_5:
                    dict.setItem("Key", Py::String("PAD_5"));
                    break;
                case SoKeyboardEvent::PAD_6:
                    dict.setItem("Key", Py::String("PAD_6"));
                    break;
                case SoKeyboardEvent::PAD_7:
                    dict.setItem("Key", Py::String("PAD_7"));
                    break;
                case SoKeyboardEvent::PAD_8:
                    dict.setItem("Key", Py::String("PAD_8"));
                    break;
                case SoKeyboardEvent::PAD_9:
                    dict.setItem("Key", Py::String("PAD_9"));
                    break;
                case SoKeyboardEvent::PAD_ADD:
                    dict.setItem("Key", Py::String("PAD_ADD"));
                    break;
                case SoKeyboardEvent::PAD_SUBTRACT:
                    dict.setItem("Key", Py::String("PAD_SUBTRACT"));
                    break;
                case SoKeyboardEvent::PAD_MULTIPLY:
                    dict.setItem("Key", Py::String("PAD_MULTIPLY"));
                    break;
                case SoKeyboardEvent::PAD_DIVIDE:
                    dict.setItem("Key", Py::String("PAD_DIVIDE"));
                    break;
                case SoKeyboardEvent::PAD_TAB:
                    dict.setItem("Key", Py::String("PAD_TAB"));
                    break;
                case SoKeyboardEvent::PAD_DELETE:
                    dict.setItem("Key", Py::String("PAD_DELETE"));
                    break;
                case SoKeyboardEvent::F1:
                    dict.setItem("Key", Py::String("F1"));
                    break;
                case SoKeyboardEvent::F2:
                    dict.setItem("Key", Py::String("F2"));
                    break;
                case SoKeyboardEvent::F3:
                    dict.setItem("Key", Py::String("F3"));
                    break;
                case SoKeyboardEvent::F4:
                    dict.setItem("Key", Py::String("F4"));
                    break;
                case SoKeyboardEvent::F5:
                    dict.setItem("Key", Py::String("F5"));
                    break;
                case SoKeyboardEvent::F6:
                    dict.setItem("Key", Py::String("F6"));
                    break;
                case SoKeyboardEvent::F7:
                    dict.setItem("Key", Py::String("F7"));
                    break;
                case SoKeyboardEvent::F8:
                    dict.setItem("Key", Py::String("F8"));
                    break;
                case SoKeyboardEvent::F9:
                    dict.setItem("Key", Py::String("F9"));
                    break;
                case SoKeyboardEvent::F10:
                    dict.setItem("Key", Py::String("F10"));
                    break;
                case SoKeyboardEvent::F11:
                    dict.setItem("Key", Py::String("F11"));
                    break;
                case SoKeyboardEvent::F12:
                    dict.setItem("Key", Py::String("F12"));
                    break;
                case SoKeyboardEvent::BACKSPACE:
                    dict.setItem("Key", Py::String("BACKSPACE"));
                    break;
                case SoKeyboardEvent::TAB:
                    dict.setItem("Key", Py::String("TAB"));
                    break;
                case SoKeyboardEvent::RETURN:
                    dict.setItem("Key", Py::String("RETURN"));
                    break;
                case SoKeyboardEvent::PAUSE:
                    dict.setItem("Key", Py::String("PAUSE"));
                    break;
                case SoKeyboardEvent::SCROLL_LOCK:
                    dict.setItem("Key", Py::String("SCROLL_LOCK"));
                    break;
                case SoKeyboardEvent::ESCAPE:
                    dict.setItem("Key", Py::String("ESCAPE"));
                    break;
                case SoKeyboardEvent::KEY_DELETE:
                    dict.setItem("Key", Py::String("DELETE"));
                    break;
                case SoKeyboardEvent::PRINT:
                    dict.setItem("Key", Py::String("PRINT"));
                    break;
                case SoKeyboardEvent::INSERT:
                    dict.setItem("Key", Py::String("INSERT"));
                    break;
                case SoKeyboardEvent::NUM_LOCK:
                    dict.setItem("Key", Py::String("NUM_LOCK"));
                    break;
                case SoKeyboardEvent::CAPS_LOCK:
                    dict.setItem("Key", Py::String("CAPS_LOCK"));
                    break;
                case SoKeyboardEvent::SHIFT_LOCK:
                    dict.setItem("Key", Py::String("SHIFT_LOCK"));
                    break;
                case SoKeyboardEvent::SPACE:
                    dict.setItem("Key", Py::String("SPACE"));
                    break;
                case SoKeyboardEvent::APOSTROPHE:
                    dict.setItem("Key", Py::String("APOSTROPHE"));
                    break;
                case SoKeyboardEvent::COMMA:
                    dict.setItem("Key", Py::String("COMMA"));
                    break;
                case SoKeyboardEvent::MINUS:
                    dict.setItem("Key", Py::String("MINUS"));
                    break;
                case SoKeyboardEvent::PERIOD:
                    dict.setItem("Key", Py::String("PERIOD"));
                    break;
                case SoKeyboardEvent::SLASH:
                    dict.setItem("Key", Py::String("SLASH"));
                    break;
                case SoKeyboardEvent::SEMICOLON:
                    dict.setItem("Key", Py::String("SEMICOLON"));
                    break;
                case SoKeyboardEvent::EQUAL:
                    dict.setItem("Key", Py::String("EQUAL"));
                    break;
                case SoKeyboardEvent::BRACKETLEFT:
                    dict.setItem("Key", Py::String("BRACKETLEFT"));
                    break;
                case SoKeyboardEvent::BACKSLASH:
                    dict.setItem("Key", Py::String("BACKSLASH"));
                    break;
                case SoKeyboardEvent::BRACKETRIGHT:
                    dict.setItem("Key", Py::String("BRACKETRIGHT"));
                    break;
                case SoKeyboardEvent::GRAVE:
                    dict.setItem("Key", Py::String("GRAVE"));
                    break;
                default:
                    dict.setItem("Key", Py::Char(ke->getPrintableCharacter()));
                    break;
            }
        }
        if (e->isOfType(SoMouseButtonEvent::getClassTypeId())) {
            const SoMouseButtonEvent* mbe = static_cast<const SoMouseButtonEvent*>(e);
            std::string button;
            switch (mbe->getButton()) {
                case SoMouseButtonEvent::BUTTON1:
                    button = "BUTTON1";
                    break;
                case SoMouseButtonEvent::BUTTON2:
                    button = "BUTTON2";
                    break;
                case SoMouseButtonEvent::BUTTON3:
                    button = "BUTTON3";
                    break;
                case SoMouseButtonEvent::BUTTON4:
                    button = "BUTTON4";
                    break;
                case SoMouseButtonEvent::BUTTON5:
                    button = "BUTTON5";
                    break;
                default:
                    button = "ANY";
                    break;
            }

            dict.setItem("Button", Py::String(button));
        }
        if (e->isOfType(SoSpaceballButtonEvent::getClassTypeId())) {
            const SoSpaceballButtonEvent* sbe = static_cast<const SoSpaceballButtonEvent*>(e);
            std::string button;
            switch (sbe->getButton()) {
                case SoSpaceballButtonEvent::BUTTON1:
                    button = "BUTTON1";
                    break;
                case SoSpaceballButtonEvent::BUTTON2:
                    button = "BUTTON2";
                    break;
                case SoSpaceballButtonEvent::BUTTON3:
                    button = "BUTTON3";
                    break;
                case SoSpaceballButtonEvent::BUTTON4:
                    button = "BUTTON4";
                    break;
                case SoSpaceballButtonEvent::BUTTON5:
                    button = "BUTTON5";
                    break;
                case SoSpaceballButtonEvent::BUTTON6:
                    button = "BUTTON6";
                    break;
                case SoSpaceballButtonEvent::BUTTON7:
                    button = "BUTTON7";
                    break;
                default:
                    button = "ANY";
                    break;
            }

            dict.setItem("Button", Py::String(button));
        }
        if (e->isOfType(SoMotion3Event::getClassTypeId())) {
            const SoMotion3Event* me = static_cast<const SoMotion3Event*>(e);
            const SbVec3f& m = me->getTranslation();
            const SbRotation& r = me->getRotation();
            Py::Tuple mov(3);
            mov.setItem(0, Py::Float(m[0]));
            mov.setItem(1, Py::Float(m[1]));
            mov.setItem(2, Py::Float(m[2]));
            dict.setItem("Translation", mov);
            Py::Tuple rot(4);
            rot.setItem(0, Py::Float(r.getValue()[0]));
            rot.setItem(1, Py::Float(r.getValue()[1]));
            rot.setItem(2, Py::Float(r.getValue()[2]));
            rot.setItem(3, Py::Float(r.getValue()[3]));
            dict.setItem("Rotation", rot);
        }

        // now run the method
        Py::Callable method(reinterpret_cast<PyObject*>(ud));
        Py::Tuple args(1);
        args.setItem(0, dict);
        method.apply(args);
    }
    catch (const Py::Exception& e) {
        Py::Object o = Py::type(e);
        if (o.isString()) {
            Py::String s(o);
            Base::Console().Log("%s\n", s.as_std_string().c_str());
        }
        else {
            Py::String s(o.repr());
            Base::Console().Log("%s\n", s.as_std_string().c_str());
        }
        // Prints message to console window if we are in interactive mode
        PyErr_Print();
    }
}

void View3DPy::eventCallbackSWIG(void * ud, SoEventCallback * n)
{
    const SoEvent* e = n->getEvent();
    std::string type = e->getTypeId().getName().getString();
    type += " *";

    swig_module_info *module = SWIG_GetModule(NULL);
    if (!module) {
        // No Python binding for Coin loaded
        return;
    }

    swig_type_info * swig_type = 0;
    swig_type = SWIG_TypeQuery(type.c_str());
    if (!swig_type) {
        // Cannot find type information for event type
        return;
    }
    
    try {
        PyObject *resultobj = NULL;
        resultobj = SWIG_Python_NewPointerObj((void*)e,swig_type,0);

        // now run the method
        Py::Object event(resultobj,true);
        Py::Callable method(reinterpret_cast<PyObject*>(ud));
        Py::Tuple args(1);
        args.setItem(0, event);
        method.apply(args);
    }
    catch (const Py::Exception& e) {
        Py::Object o = Py::type(e);
        if (o.isString()) {
            Py::String s(o);
            Base::Console().Log("%s\n", s.as_std_string().c_str());
        }
        else {
            Py::String s(o.repr());
            Base::Console().Log("%s\n", s.as_std_string().c_str());
        }
        // Prints message to console window if we are in interactive mode
        PyErr_Print();
    }
}

PYFUNCIMP_D(View3DPy,addEventCallback)
{
    char* eventtype;
    PyObject* method;
    if (!PyArg_ParseTuple(args, "sO", &eventtype, &method))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 
    try {
        if (PyCallable_Check(method) == 0) {
            PyErr_SetString(PyExc_TypeError, "object is not callable");
            return NULL;
        }
        SoType eventId = SoType::fromName(eventtype);
        if (eventId.isBad() || !eventId.isDerivedFrom(SoEvent::getClassTypeId())) {
            PyErr_Format(PyExc_Exception, "%s is not a valid event type", eventtype);
            return NULL;
        }

        _pcView->getViewer()->addEventCallback(eventId, View3DPy::eventCallback, method);
        Py_INCREF(method);
        return method;
    } catch (const Py::Exception&) {
        return NULL;
    }
}

PYFUNCIMP_D(View3DPy,removeEventCallback)
{
    char* eventtype;
    PyObject* method;
    if (!PyArg_ParseTuple(args, "sO", &eventtype, &method))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 
    try {
        if (PyCallable_Check(method) == 0) {
            PyErr_SetString(PyExc_TypeError, "object is not callable");
            return NULL;
        }
        SoType eventId = SoType::fromName(eventtype);
        if (eventId.isBad() || !eventId.isDerivedFrom(SoEvent::getClassTypeId())) {
            PyErr_Format(PyExc_Exception, "%s is not a valid event type", eventtype);
            return NULL;
        }

        _pcView->getViewer()->removeEventCallback(eventId, View3DPy::eventCallback, method);
        Py_Return;
    } catch (const Py::Exception&) {
        return NULL;
    }
}

PYFUNCIMP_D(View3DPy,setAnnotation)
{
    char *psAnnoName,*psBuffer;
    if (!PyArg_ParseTuple(args, "ss", &psAnnoName, &psBuffer))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception
    ViewProviderExtern* view = 0;
    try {
        view = new ViewProviderExtern();
        view->setModeByString(psAnnoName, psBuffer);
    }
    catch (const Base::Exception& e) {
        delete view;
        PyErr_SetString(PyExc_Exception, e.what());
        return NULL;
    }

    _pcView->getGuiDocument()->setAnnotationViewProvider(psAnnoName, view);
    Py_Return;
}

PYFUNCIMP_D(View3DPy,removeAnnotation)
{
    char *psAnnoName;
    if (!PyArg_ParseTuple(args, "s", &psAnnoName))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception
    ViewProvider* view = 0;
    view = _pcView->getGuiDocument()->getAnnotationViewProvider(psAnnoName);
    if (view) {
        _pcView->getGuiDocument()->removeAnnotationViewProvider(psAnnoName);
        Py_Return;
    }
    else {
        PyErr_Format(PyExc_KeyError, "No such annotation '%s'", psAnnoName);
        return NULL;
    }
}

PYFUNCIMP_D(View3DPy,getSceneGraph)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    swig_module_info *module = SWIG_GetModule(NULL);
    if (!module) {
        PyErr_SetString(PyExc_RuntimeError, "No Python binding for Coin loaded");
        return NULL;
    }

    swig_type_info * swig_type = 0;
    swig_type = SWIG_TypeQuery("SoSeparator *");
    if (!swig_type) {
        PyErr_SetString(PyExc_RuntimeError, "Cannot find type information for 'SoSeparator'");
        return NULL;
    }

    SoNode* scene = _pcView->getViewer()->getSceneGraph();
    scene->ref();
    
    PyObject *resultobj = NULL;
    resultobj = SWIG_Python_NewPointerObj((void*)scene,swig_type,1);
    return resultobj;
}

PYFUNCIMP_D(View3DPy,addEventCallbackSWIG)
{
    PyObject* proxy;
    PyObject* method;
    if (!PyArg_ParseTuple(args, "OO", &proxy, &method))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 
    
    swig_module_info *module = SWIG_GetModule(NULL);
    if (!module) {
        PyErr_SetString(PyExc_RuntimeError, "No Python binding for Coin loaded");
        return NULL;
    }

    swig_type_info * swig_type = 0;
    swig_type = SWIG_TypeQuery("SoType *");
    if (!swig_type) {
        PyErr_SetString(PyExc_RuntimeError, "Cannot find type information for 'SoType'");
        return NULL;
    }

    // return value of 0 is on success
    void* ptr = 0;
    if (SWIG_ConvertPtr(proxy, &ptr, swig_type, 0)) {
        PyErr_SetString(PyExc_RuntimeError, "Cannot convert into SoType");
        return NULL;
    }

    SoType* eventId = reinterpret_cast<SoType*>(ptr);
    if (eventId->isBad() || !eventId->isDerivedFrom(SoEvent::getClassTypeId())) {
        PyErr_Format(PyExc_Exception, "%s is not a valid event type", eventId->getName().getString());
        return NULL;
    }

    try {
        if (PyCallable_Check(method) == 0) {
            PyErr_SetString(PyExc_TypeError, "object is not callable");
            return NULL;
        }

        _pcView->getViewer()->addEventCallback(*eventId, View3DPy::eventCallbackSWIG, method);
        Py_INCREF(method);
        return method;
    } catch (const Py::Exception&) {
        return NULL;
    }
}

PYFUNCIMP_D(View3DPy,removeEventCallbackSWIG)
{
    PyObject* proxy;
    PyObject* method;
    if (!PyArg_ParseTuple(args, "OO", &proxy, &method))     // convert args: Python->C 
        return NULL;                       // NULL triggers exception 
    
    swig_module_info *module = SWIG_GetModule(NULL);
    if (!module) {
        PyErr_SetString(PyExc_RuntimeError, "No Python binding for Coin loaded");
        return NULL;
    }

    swig_type_info * swig_type = 0;
    swig_type = SWIG_TypeQuery("SoType *");
    if (!swig_type) {
        PyErr_SetString(PyExc_RuntimeError, "Cannot find type information for 'SoType'");
        return NULL;
    }

    // return value of 0 is on success
    void* ptr = 0;
    if (SWIG_ConvertPtr(proxy, &ptr, swig_type, 0)) {
        PyErr_SetString(PyExc_RuntimeError, "Cannot convert into SoType");
        return NULL;
    }

    SoType* eventId = reinterpret_cast<SoType*>(ptr);
    if (eventId->isBad() || !eventId->isDerivedFrom(SoEvent::getClassTypeId())) {
        PyErr_Format(PyExc_Exception, "%s is not a valid event type", eventId->getName().getString());
        return NULL;
    }

    try {
        if (PyCallable_Check(method) == 0) {
            PyErr_SetString(PyExc_TypeError, "object is not callable");
            return NULL;
        }

        _pcView->getViewer()->removeEventCallback(*eventId, View3DPy::eventCallbackSWIG, method);
        Py_INCREF(method);
        return method;
    } catch (const Py::Exception&) {
        return NULL;
    }
}

void View3DPy::cleanupSWIG()
{
    swig_module_info *swig_module = SWIG_GetModule(NULL);
    if (!swig_module) {
        // No Python binding for Coin loaded
        return;
    }

    swig_type_info * swig_type = 0;
    swig_type = SWIG_TypeQuery("SoBase *");
    if (!swig_type) {
        // Cannot find type information for event type
        return;
    }

    PyObject* module = PyImport_AddModule("__main__");
    if (module != NULL && PyModule_Check(module)) {
        PyObject* dict = PyModule_GetDict(module);
        if (!dict) return;

        Py_ssize_t pos;
        PyObject *key, *value;
        pos = 0;
        while (PyDict_Next(dict, &pos, &key, &value)) {
            if (value != Py_None && PyString_Check(key)) {
                //char *s = PyString_AsString(key);
                void* ptr = 0;
                if (SWIG_ConvertPtr(value, &ptr, 0, 0) == 0)
                    PyDict_SetItem(dict, key, Py_None);
            }
        }
    }
}

