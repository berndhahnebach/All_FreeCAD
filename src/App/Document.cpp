/***************************************************************************
                          FCApplication.cpp  -  description
                             -------------------
    begin                : Tue Jan 2 2001
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/



#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <TDocStd_Application.hxx>
#	include <TDataStd_Real.hxx>
#endif

#include "Document.h"
#include "../base/Console.h"

//===========================================================================
// FCLabel - Warpper for the TDF_Label classes
//===========================================================================

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject FCLabel::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"FCLabel",				/*tp_name*/
	sizeof(FCLabel),			/*tp_basicsize*/
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
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef FCLabel::Methods[] = {
  {"GetLabel",         (PyCFunction) sPyGetLabel,         Py_NEWARGS},

  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject FCLabel::Parents[] = {&FCLabel::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
FCLabel::FCLabel(const TDF_Label &cLabel, PyTypeObject *T) 
 : _cLabel(cLabel),FCPyObject( T)
{
	GetConsole().Log("Create Label %p\n",this);
}

PyObject *FCLabel::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new FCLabel();			// Make new Python-able object
  return 0;
}

//--------------------------------------------------------------------------
//  FCLabel destructor 
//--------------------------------------------------------------------------
FCLabel::~FCLabel()						// Everything handled in parent
{
	GetConsole().Log("Destroy Label %p\n",this);
} 

//--------------------------------------------------------------------------
// FCLabel Attributes
//--------------------------------------------------------------------------
PyObject *FCLabel::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	try{
		if (streq(attr, "AttributeCount"))						// accessable new state
			return Py_BuildValue("i", _cLabel.NbAttributes()); 
		else if (streq(attr, "ChildrenCount"))						// accessable new state
			return Py_BuildValue("i", _cLabel.NbChildren()); 
		else if (streq(attr, "Root"))						// accessable new state
			return new FCLabel( _cLabel.Root()); 
		else if (streq(attr, "Father"))						// accessable new state
			return new FCLabel( _cLabel.Father()); 
		else if (streq(attr, "Real")){
			Handle(TDataStd_Real) RealAttr;
			if(_cLabel.FindAttribute(TDataStd_Real::GetID(),RealAttr))
				return Py_BuildValue("d",RealAttr->Get()); 
			else
				return 0;
		}else if (streq(attr, "Int")){
			Handle(TDataStd_Integer) IntAttr;
			if(_cLabel.FindAttribute(TDataStd_Integer::GetID(),IntAttr))
				return Py_BuildValue("d",IntAttr->Get()); 
			else
				return 0;
		}else if (streq(attr, "Name")){
			Handle(TDataStd_Name) NameAttr;
			if(_cLabel.FindAttribute(TDataStd_Name::GetID(),NameAttr))
				//return Py_BuildValue("u",NameAttr->Get().ToExtString()); 
				return Py_BuildValue("s",NameAttr->Get()); 
			else
				return 0;
		}else
			_getattr_up(FCPyObject); 						// send to parent
	}catch(...){
		GetConsole().Log("Exception in FCLabel::_getattr()\n");
		return 0;
	}
} 

int FCLabel::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (streq(attr, "Real"))						// settable new state
		TDataStd_Real::Set(_cLabel, PyFloat_AsDouble(value)); 
	else if (streq(attr, "Int"))						// settable new state
		TDataStd_Integer::Set(_cLabel, PyInt_AsLong(value)); 
	else if (streq(attr, "Name"))						// settable new state
		TDataStd_Name::Set(_cLabel, (short*)PyString_AsString(value)); 
		//TDataStd_Name::Set(_cLabel, (short*)PyUnicode_AsUnicode(value)); 
	else  
 
		return FCPyObject::_setattr(attr, value);	// send up to parent
	return 0;
} 

//--------------------------------------------------------------------------
// Exported functions
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PyObject *FCLabel::PyGetLabel(PyObject *args)
{ 
	int Tag;
    if (!PyArg_ParseTuple(args, "i",&Tag ))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	return new FCLabel( _cLabel.FindChild(Tag) );
} 

/*
PyObject *FCLabel::PyIsDifferent(PyObject *args)
{ 
	PyObject* pcObject;
    if (!PyArg_ParseTuple(args, "O!", &Type, &pcObject))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	
	//_cLabel.IsDifferent( ((FCLabel*)pcObject)->GetLabel() );

	return Py_BuildValue("i", _cLabel.IsDifferent( ((FCLabel*)pcObject)->GetLabel() ) );
	//Undo(); 
	//Py_Return; 
} 

*/
//===========================================================================
// FCDocument - Wrapper for the TDocStd_Document class
//===========================================================================

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject FCDocument::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"FCDocument",				/*tp_name*/
	sizeof(FCDocument),			/*tp_basicsize*/
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
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef FCDocument::Methods[] = {
  {"Undo",         (PyCFunction) sPyUndo,         Py_NEWARGS},
  {"Redo",         (PyCFunction) sPyRedo,         Py_NEWARGS},
  {"ClearUndos",   (PyCFunction) sPyClearUndos,   Py_NEWARGS},
  {"SaveAs",       (PyCFunction) sPySaveAs,       Py_NEWARGS},
  {"Save",         (PyCFunction) sPySave,         Py_NEWARGS},

  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject FCDocument::Parents[] = {&FCPyObject::Type, NULL};     

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
FCDocument::FCDocument(const Handle_TDocStd_Document &hDoc, PyTypeObject *T) 
 : _hDoc(hDoc),FCPyObject( T)
{
	GetConsole().Log("Create Document %p\n",this);
}

PyObject *FCDocument::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new FCDocument(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
FCDocument::~FCDocument()						// Everything handled in parent
{
	GetConsole().Log("Destroy Document %p\n",this);
} 

//--------------------------------------------------------------------------
// FCDocument Attributes
//--------------------------------------------------------------------------
PyObject *FCDocument::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	try{
		if (streq(attr, "UndoLimit"))						// accessable new state
			return Py_BuildValue("i", _hDoc->GetUndoLimit()); 
		else if (streq(attr, "AvailableUndos"))						// accessable new state
			return Py_BuildValue("i", _hDoc->GetAvailableUndos()); 
		else if (streq(attr, "AvailableRedos"))						// accessable new state
			return Py_BuildValue("i", _hDoc->GetAvailableRedos()); 
		else if (streq(attr, "Name"))						// accessable new state
			return Py_BuildValue("s", _hDoc->GetName()); 
		else if (streq(attr, "Path"))						// accessable new state
			return Py_BuildValue("´s", _hDoc->GetPath()); 
		else if (streq(attr, "Main"))						// accessable new state
			return new FCLabel( _hDoc->Main()); 
		else
			_getattr_up(FCPyObject); 						// send to parent
	}catch(...){
		return 0;
	}
} 

int FCDocument::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (streq(attr, "UndoLimit")){						// settable new state
		_hDoc->SetUndoLimit(PyInt_AsLong(value)); 
		return 1;
	}else  
		return FCPyObject::_setattr(attr, value); 	// send up to parent
} 

//--------------------------------------------------------------------------
// Exported functions
//--------------------------------------------------------------------------

void FCDocument::Undo(void)					// C++ method
{
  _hDoc->Undo(); 
}
void FCDocument::Redo(void)					// C++ method
{
  _hDoc->Redo(); 
}
void FCDocument::ClearUndos(void)			// C++ method
{
  _hDoc->ClearUndos(); 
}
// Save the Document under a new Name
void FCDocument::SaveAs (const char* Name)
{
	char path[512];
	strcpy(path,Name);
	Handle(TDocStd_Application)::DownCast(_hDoc->Application())->SaveAs(_hDoc,path);
}
// Save the document under the name its been opened
void FCDocument::Save (void)
{
	Handle(TDocStd_Application)::DownCast(_hDoc->Application())->Save(_hDoc);
}

//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PyObject *FCDocument::PyUndo(PyObject *args)
{ 
	Undo(); 
	Py_Return; 
} 

PyObject *FCDocument::PyRedo(PyObject *args)
{ 
	Redo(); 
	Py_Return; 
} 
PyObject *FCDocument::PyClearUndos(PyObject *args)
{ 
	ClearUndos(); 
	Py_Return; 
} 

PyObject *FCDocument::PySaveAs(PyObject *args)
{
	char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	SaveAs(pstr);	
	Py_Return; 
} 

PyObject *FCDocument::PySave(PyObject *args)
{ 
	Save(); 
	Py_Return; 
} 
