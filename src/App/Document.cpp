/***************************************************************************
                          FCApplication.cpp  -  description
                             -------------------
    begin                : Tue Jan 2 2001
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/


#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <TDocStd_Application.hxx>
#	include <TDataStd_Real.hxx>
#	include <TDataStd_Name.hxx>
#	include <Handle_TDataStd_Name.hxx>
#	include <TDataStd_Integer.hxx>
#	include <Handle_TDataStd_Integer.hxx>
#endif


#include "Document.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"
#include "Application.h"
#include "Attribute.h"


//===========================================================================
// FCLabel - Warpper for the TDF_Label classes
//===========================================================================

//--------------------------------------------------------------------------
// Exported functions
//--------------------------------------------------------------------------

FCPyHandle<FCLabel> FCLabel::GetLabel(int iN)
{
	return FCPyHandle<FCLabel>(_pcDocument->HasPyLabel( _cLabel.FindChild(iN)));
}

/// Get all child labels
std::vector<FCPyHandle<FCLabel> > FCLabel::GetLabels(void)
{
	std::vector<FCPyHandle<FCLabel> > vhcLabels;

	for (TDF_ChildIterator it(_cLabel); it.More(); it.Next())
		vhcLabels.push_back( FCPyHandle<FCLabel>(_pcDocument->HasPyLabel(it.Value())));

	return vhcLabels;
}

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
FCLabel::FCLabel(const TDF_Label &cLabel,FCDocument *pcDocument, PyTypeObject *T) 
 : _cLabel(cLabel),_pcDocument(pcDocument),FCPyObject( T)
{
	GetConsole().Log("Create Label %p\n",this);
	
    //printf("Tag. %d\n",cLabel.Tag());
    //printf("Depth. %d\n",cLabel.Depth());

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
		// Access the number of attributes at this label
		if (streq(attr, "AttributeCount"))						
			return Py_BuildValue("i", _cLabel.NbAttributes()); 
		else if (streq(attr, "ChildrenCount"))					
			return Py_BuildValue("i", _cLabel.NbChildren()); 
		else if (streq(attr, "Root"))						
			return _pcDocument->HasPyLabel( _cLabel.Root()); 
		else if (streq(attr, "Father"))						
			return _pcDocument->HasPyLabel( _cLabel.Father()); 
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
#ifdef FC_OS_LINUX /* will "u" work? */ // u is unicode as ToExtString is!
				return Py_BuildValue("u",NameAttr->Get().ToExtString()); 
#else
				return Py_BuildValue("s",NameAttr->Get()); 
#endif				
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
// Python wrappers
//--------------------------------------------------------------------------

PyObject *FCLabel::PyGetLabel(PyObject *args)
{ 
	int Tag;
    if (!PyArg_ParseTuple(args, "i",&Tag ))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	FCPyHandle<FCLabel> hcL = GetLabel( Tag);
	hcL->_INCREF();
	return hcL.GetPyObject();
}
 
PyObject *FCLabel::PyHasChild(PyObject *args)
{ 
	if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 
	return Py_BuildValue("i",HasChild()?1:0);
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
// Exported functions
//--------------------------------------------------------------------------

// Save the Document under a new Name
void FCDocument::SaveAs (const char* Name)
{
	// creat a non const buffer
	PyBuf name(Name);

	Handle(FCApplicationOCC) hApp = GetApplication().GetOCCApp();
	if(hApp->SaveAs(_hDoc,(Standard_CString)name.str)==CDF_SS_Failure) 
		throw FCException("SaveAs failed");
}
// Save the document under the name its been opened
void FCDocument::Save (void)
{
	Handle(TDocStd_Application)::DownCast(_hDoc->Application())->Save(_hDoc);
}

bool FCDocument::Undo(void)					
{
  return _hDoc->Undo() != 0; 
}

bool FCDocument::Redo(void)
{
  return _hDoc->Redo() != 0; 
}

bool FCDocument::IsSaved() const
{
	return _hDoc->IsSaved() != 0;
}


/// Get the document name of a saved document
const short* FCDocument::GetName() const
{
  return _hDoc->GetName().ToExtString(); 
}

/// Get the path of a saved document
const short* FCDocument::GetPath() const
{
  return _hDoc->GetPath().ToExtString(); 
}

/// Get the Main Label of the document
FCPyHandle<FCLabel> FCDocument::Main() 
{
	if(!_pcMain){
		_pcMain = new FCLabel(_hDoc->Main(),this);
		_pcMain->_INCREF();
		mcLabelMap[_hDoc->Main()] = _pcMain;
	}
	return  FCPyHandle<FCLabel>(_pcMain);
}

/// Test if the document is empty
bool FCDocument::IsEmpty() const
{
  return _hDoc->IsEmpty() != 0; 
}

/// Returns False if the  document  contains notified modifications.
bool FCDocument::IsValid() const
{
  return _hDoc->IsValid() != 0;
}

/// Set a special Label as modified
void FCDocument::SetModified(FCLabel* L)
{
  _hDoc->SetModified(L->GetOCCLabel()); 
}

/// Remove all modifications. After this call The document becomesagain Valid.
void FCDocument::PurgeModified()
{
  _hDoc->PurgeModified(); 
}

/// New Command (Short cut for Commit and Open transaction)
void FCDocument::NewCommand() 
{
  _hDoc->NewCommand(); 
}

/// returns True if a Command transaction is open
bool FCDocument::HasOpenCommand() const
{
  return _hDoc->HasOpenCommand() != 0; 
}

/** Open a new command transaction.
 *  Raise a OCC Exception If a Command is already open.
 *  You may   check  it with the   previous method <HasOpenCommand>.
 */
void FCDocument::OpenCommand()
{
  _hDoc->OpenCommand(); 
}

/// Commit the Command transaction. Do nothing If there is no Command transaction open.
void FCDocument::CommitCommand()
{
  _hDoc->CommitCommand(); 
}

/// Abort the  Command  transaction. Do nothing If there is no Command transaction open.
void FCDocument::AbortCommand()
{
  _hDoc->AbortCommand(); 
}

/// The current limit on the number of undos
int FCDocument::GetUndoLimit() const
{
  return _hDoc->GetUndoLimit(); 
}

/** Set the limit on the number of Undo Deltas stored.
 *  0 will disable Undo on the document A negative value
 *  means no limit. Note that by default Undo is disabled.
 *  Enabling it will take effect with the next call to
 *  NewCommand. Of course this limit is the same for Redo.
 */
void FCDocument::SetUndoLimit(const int L)
{
  _hDoc->SetUndoLimit(L); 
}

/// Remove all stored Undos and Redos
void FCDocument::ClearUndos()
{
  _hDoc->ClearUndos(); 
}

/// Returns the  number  of stored Undos. If greater than 0 Undo will be effective.
int FCDocument::GetAvailableUndos() const
{
  return _hDoc->GetAvailableUndos(); 
}

/// Returns the number   of stored Redos. If greater than 0 Redo will be effective.
int FCDocument::GetAvailableRedos() const
{
  return _hDoc->GetAvailableRedos(); 
}

/// Recompute if the document was  not valid and propagate the reccorded modification.
void FCDocument::Recompute()
{
  _hDoc->Recompute(); 
}

/// Returns the storage string of the document.
const short* FCDocument::StorageFormat() const
{
  return _hDoc->StorageFormat().ToExtString(); 
}

/// Change the storage format of the document.
void FCDocument::ChangeStorageFormat(const short* sStorageFormat) 
{
  _hDoc->ChangeStorageFormat((Standard_ExtString)sStorageFormat); 
}


FCLabel *FCDocument::HasPyLabel(TDF_Label cLabel)
{
	FCLabel *pcL;
	std::map <TDF_Label,FCLabel*,LabelLess>::iterator It;
	
	// find a FCLabel if possible
	It = mcLabelMap.find(cLabel);

	// if not
	if(It == mcLabelMap.end())
	{
		// create a new label and append it to the map
		pcL = new FCLabel(cLabel,this);
		mcLabelMap[cLabel] = pcL;
		// taking care that python not delete the object
		pcL->_INCREF();
		return pcL;
	}
	// if yes return the found FCLabel
	return It->second;

}

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
  {"SetModified",  (PyCFunction) sPySetModified,  Py_NEWARGS},
  {"PurgeModified",(PyCFunction) sPyPurgeModified,Py_NEWARGS},
  {"NewCommand",   (PyCFunction) sPyNewCommand,   Py_NEWARGS},
  {"OpenCommand",  (PyCFunction) sPyOpenCommand,  Py_NEWARGS},
  {"CommitCommand",(PyCFunction) sPyCommitCommand,Py_NEWARGS},
  {"Recompute",    (PyCFunction) sPyRecompute,    Py_NEWARGS},

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
 : _hDoc(hDoc),FCPyObject( T),_pcMain(0)
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
	_hDoc.Nullify();
	GetConsole().Log("Destroy Document %p\n",this);
} 


//--------------------------------------------------------------------------
// FCDocument representation
//--------------------------------------------------------------------------
PyObject *FCDocument::_repr(void)
{
	return Py_BuildValue("s", "FreeCAD Document");
}
//--------------------------------------------------------------------------
// FCDocument Attributes
//--------------------------------------------------------------------------
PyObject *FCDocument::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	try{
		if (streq(attr, "UndoLimit"))						
			return Py_BuildValue("i", GetUndoLimit()); 
		else if (streq(attr, "AvailableUndos"))				
			return Py_BuildValue("i", GetAvailableUndos()); 
		else if (streq(attr, "AvailableRedos"))				
			return Py_BuildValue("i", GetAvailableRedos()); 
		else if (streq(attr, "Name"))						
			return Py_BuildValue("u", GetName()); 
		else if (streq(attr, "Path"))						
			return Py_BuildValue("u", GetPath()); 
		else if (streq(attr, "Main")){
			Main()->_INCREF();
			return Main().GetPyObject(); }
		else if (streq(attr, "IsEmpty"))					
			return Py_BuildValue("u", IsEmpty()?1:0); 
		else if (streq(attr, "IsValid"))					
			return Py_BuildValue("u", IsValid()?1:0); 
		else if (streq(attr, "HasOpenCommand"))				
			return Py_BuildValue("u", HasOpenCommand()?1:0);
		else if (streq(attr, "StorageFormat"))						
			return Py_BuildValue("u", StorageFormat()); 
		else
			_getattr_up(FCPyObject); 						
	}catch(...){
		Py_Error(PyExc_Exception,"Error in get Attribute");
	}
} 

int FCDocument::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (streq(attr, "UndoLimit")){						// settable new state
		SetUndoLimit(PyInt_AsLong(value)); 
		return 1;
	}else if (streq(attr, "StorageFormat")){						// settable new state
		ChangeStorageFormat(const_cast<const short*>((short*)PyUnicode_AS_UNICODE(value))); 
		return 1;
	}else  
		return FCPyObject::_setattr(attr, value); 	// send up to parent
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
	try {
		SaveAs(pstr);
	}	
	catch(...) {
		PyErr_SetString(PyExc_IOError, "Save Failed");
		return 0L;
	}

	Py_Return; 
} 

PyObject *FCDocument::PySave(PyObject *args)
{ 
	Save(); 
	Py_Return; 
} 


PyObject *FCDocument::PySetModified(PyObject *args)
{ 
	assert(0);
	Save(); 
	Py_Return; 
} 
	
PyObject *FCDocument::PyPurgeModified(PyObject *args)
{ 
	PurgeModified(); 
	Py_Return; 
} 
	
PyObject *FCDocument::PyNewCommand(PyObject *args)
{ 
	NewCommand(); 
	Py_Return; 
} 
		
PyObject *FCDocument::PyOpenCommand(PyObject *args)
{ 
	OpenCommand(); 
	Py_Return; 
} 
	
PyObject *FCDocument::PyCommitCommand(PyObject *args)
{ 
	CommitCommand(); 
	Py_Return; 
} 
	
PyObject *FCDocument::PyAbortCommand(PyObject *args)
{ 
	AbortCommand(); 
	Py_Return; 
} 
	
PyObject *FCDocument::PyRecompute(PyObject *args)
{ 
	Recompute(); 
	Py_Return; 
} 
		
