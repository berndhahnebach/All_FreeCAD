/***************************************************************************
                          FCApplication.cpp  -  description
                             -------------------
    begin                : Tue Jan 2 2001
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
#	include <TDocStd_Application.hxx>
#	include <TDataStd_Real.hxx>
#	include <TDataStd_Name.hxx>
#	include <Handle_TDataStd_Name.hxx>
#	include <TDataStd_Integer.hxx>
#	include <Handle_TDataStd_Integer.hxx>
#endif


#include "Document.h"
#include "Label.h"

#include "../Base/PyExport.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"
#include "Application.h"
#include "Attribute.h"
#include "DocType.h"
#include "DocTypeAttr.h"

using Base::Console;
using Base::streq;
using namespace App;


//===========================================================================
// FCDocumentPy - Python wrapper 
//===========================================================================

/** The Document python class 
 */
class AppExport FCDocumentPy :public Base::FCPyObject
{
	/// always start with Py_Header
	Py_Header;

public:
	FCDocumentPy(FCDocument *pcDoc, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);

	~FCDocumentPy();

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	virtual PyObject *_repr(void);  				// the representation
	PyObject *_getattr(char *attr);					// __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function
	PyObject *PyDocType(PyObject *args);		// Python wrapper
	static PyObject *sPyDocType(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PyDocType(args);};
	PyObject *PyUndo(PyObject *args);		// Python wrapper
	static PyObject *sPyUndo(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PyUndo(args);};
	PyObject *PyRedo(PyObject *args);		// Python wrapper
	static PyObject *sPyRedo(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PyRedo(args);};
	PyObject *PyClearUndos(PyObject *args);		// Python wrapper
	static PyObject *sPyClearUndos(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PyClearUndos(args);};
	PyObject *PySaveAs(PyObject *args);		// Python wrapper
	static PyObject *sPySaveAs(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PySaveAs(args);};
	PyObject *PySave(PyObject *args);		// Python wrapper
	static PyObject *sPySave(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PySave(args);};
	PyObject *PySetModified(PyObject *args);		// Python wrapper
	static PyObject *sPySetModified(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PySetModified(args);};
	PyObject *PyPurgeModified(PyObject *args);		// Python wrapper
	static PyObject *sPyPurgeModified(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PyPurgeModified(args);};
	PyObject *PyNewCommand(PyObject *args);		// Python wrapper
	static PyObject *sPyNewCommand(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PyNewCommand(args);};
	PyObject *PyOpenCommand(PyObject *args);		// Python wrapper
	static PyObject *sPyOpenCommand(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PyOpenCommand(args);};
	PyObject *PyCommitCommand(PyObject *args);		// Python wrapper
	static PyObject *sPyCommitCommand(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PyCommitCommand(args);};
	PyObject *PyAbortCommand(PyObject *args);		// Python wrapper
	static PyObject *sPyAbortCommand(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PyAbortCommand(args);};
	PyObject *PyRecompute(PyObject *args);		// Python wrapper
	static PyObject *sPyRecompute(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocumentPy*)self)->PyRecompute(args);};



private:
	FCDocument *_pcDoc;

};


//===========================================================================
// FCDocument - Wrapper for the TDocStd_Document class
//===========================================================================

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
FCDocument::FCDocument(const Handle_TDocStd_Document &hDoc)
 : _hDoc(hDoc)
{
	_pcDocPy = new FCDocumentPy(this);
	Console().Log("Create Document: %p Py: %p\n",this,_pcDocPy);

}

FCDocument::~FCDocument()
{
	_hDoc.Nullify();
	Console().Log("Destroy Document %p\n",this);
}




void FCDocument::InitType(App::DocType *pcDocType)
{
	// attach the type object

	FCDocTypeAttr::Set(Main(),pcDocType);

	
	//Main()->GetOCCLabel().AddAttribute(new FCDocTypeAttr(pcDocType));

	// set up document
	pcDocType->Init(this);

}


DocType *FCDocument::GetDocType(void)
{
	Handle_FCDocTypeAttr hAttr;

	Main().FindAttribute(FCDocTypeAttr::GetID(),hAttr);

	return hAttr->Get();

	
	/*	Handle(FCDocTypeAttr) TSR;
	if (!Main()->GetOCCLabel().FindAttribute(FCDocTypeAttr::GetID(), TSR )) 
		return 0;

	return TSR->Get();*/
}

//--------------------------------------------------------------------------
// Exported functions
//--------------------------------------------------------------------------

// Save the Document under a new Name
void FCDocument::SaveAs (const char* Name)
{
	// creat a non const buffer
	Base::PyBuf name(Name);

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

/*
/// Get the Main Label of the document
FCPyHandle<FCLabel> FCDocument::Main() 
{
	if(!_hcMain){
		_hcMain = new FCLabel(_hDoc->Main(),this);
		//_hcMain->IncRef();
		mcLabelMap[_hDoc->Main()] = _pcMain;
	}
	return  FCPyHandle<FCLabel>(_pcMain);
}
*/
/// Get the Main Label of the document
TDF_Label FCDocument::Main() 
{
	return  _hDoc->Main();
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

/*
/// Set a special Label as modified
void FCDocument::SetModified(FCLabel* L)
{
  _hDoc->SetModified(L->GetOCCLabel()); 
}
*/
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

/*
FCLabel *FCDocument::HasLabel(TDF_Label cLabel)
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
		pcL->IncRef();
		return pcL;
	}
	// if yes return the found FCLabel
	return It->second;

}

*/

Base::FCPyObject * FCDocument::GetPyObject(void)
{
	return _pcDocPy;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject FCDocumentPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"FCDocumentPy",				/*tp_name*/
	sizeof(FCDocumentPy),			/*tp_basicsize*/
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
PyMethodDef FCDocumentPy::Methods[] = {
  {"DocType",      (PyCFunction) sPyDocType,         Py_NEWARGS},
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
PyParentObject FCDocumentPy::Parents[] = {&FCPyObject::Type, NULL};     

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
FCDocumentPy::FCDocumentPy(FCDocument *pcDoc, PyTypeObject *T)
 : FCPyObject( T), _pcDoc(pcDoc)
{
	Base::Console().Log("Create FCDocumentPy (%d)\n",this);

}

PyObject *FCDocumentPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new FCDocumentPy(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}

//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
FCDocumentPy::~FCDocumentPy()						// Everything handled in parent
{
	Console().Log("Destroy PyDocument: %p \n",this);

} 


//--------------------------------------------------------------------------
// FCDocumentPy representation
//--------------------------------------------------------------------------
PyObject *FCDocumentPy::_repr(void)
{
	return Py_BuildValue("s", "FreeCAD Document");
}
//--------------------------------------------------------------------------
// FCDocumentPy Attributes
//--------------------------------------------------------------------------
PyObject *FCDocumentPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	try{
		if (streq(attr, "UndoLimit"))						
			return Py_BuildValue("i", _pcDoc->GetUndoLimit()); 
		else if (streq(attr, "AvailableUndos"))				
			return Py_BuildValue("i", _pcDoc->GetAvailableUndos()); 
		else if (streq(attr, "AvailableRedos"))				
			return Py_BuildValue("i", _pcDoc->GetAvailableRedos()); 
		else if (streq(attr, "Name"))						
			return Py_BuildValue("u", _pcDoc->GetName()); 
		else if (streq(attr, "Path"))						
			return Py_BuildValue("u", _pcDoc->GetPath()); 
		else if (streq(attr, "Main")){
			//_pcDoc->Main()->IncRef();
			return new FCLabelPy(_pcDoc->_hDoc->Main()); 
		}
		else if (streq(attr, "IsEmpty"))					
			return Py_BuildValue("u", _pcDoc->IsEmpty()?1:0); 
		else if (streq(attr, "IsValid"))					
			return Py_BuildValue("u", _pcDoc->IsValid()?1:0); 
		else if (streq(attr, "HasOpenCommand"))				
			return Py_BuildValue("u", _pcDoc->HasOpenCommand()?1:0);
		else if (streq(attr, "StorageFormat"))						
			return Py_BuildValue("u", _pcDoc->StorageFormat()); 
		else
			_getattr_up(FCPyObject); 						
	}catch(...){
		Py_Error(PyExc_Exception,"Error in get Attribute");
	}
} 

int FCDocumentPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (streq(attr, "UndoLimit")){						// settable new state
		_pcDoc->SetUndoLimit(PyInt_AsLong(value)); 
		return 1;
	}else if (streq(attr, "StorageFormat")){						// settable new state
		_pcDoc->ChangeStorageFormat(const_cast<const short*>((short*)PyUnicode_AS_UNICODE(value))); 
		return 1;
	}else  
		return FCPyObject::_setattr(attr, value); 	// send up to parent
} 



//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PyObject *FCDocumentPy::PyDocType(PyObject *args)
{ 
	return _pcDoc->GetDocType()->GetPyObject();
} 

PyObject *FCDocumentPy::PyUndo(PyObject *args)
{ 
	_pcDoc->Undo(); 
	Py_Return; 
} 

PyObject *FCDocumentPy::PyRedo(PyObject *args)
{ 
	_pcDoc->Redo(); 
	Py_Return; 
} 
PyObject *FCDocumentPy::PyClearUndos(PyObject *args)
{ 
	_pcDoc->ClearUndos(); 
	Py_Return; 
} 

PyObject *FCDocumentPy::PySaveAs(PyObject *args)
{
	char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	try {
		_pcDoc->SaveAs(pstr);
	}	
	catch(...) {
		PyErr_SetString(PyExc_IOError, "Save Failed");
		return 0L;
	}

	Py_Return; 
} 

PyObject *FCDocumentPy::PySave(PyObject *args)
{ 
	_pcDoc->Save(); 
	Py_Return; 
} 


PyObject *FCDocumentPy::PySetModified(PyObject *args)
{ 
	assert(0);
	_pcDoc->Save(); 
	Py_Return; 
} 
	
PyObject *FCDocumentPy::PyPurgeModified(PyObject *args)
{ 
	_pcDoc->PurgeModified(); 
	Py_Return; 
} 
	
PyObject *FCDocumentPy::PyNewCommand(PyObject *args)
{ 
	_pcDoc->NewCommand(); 
	Py_Return; 
} 
		
PyObject *FCDocumentPy::PyOpenCommand(PyObject *args)
{ 
	_pcDoc->OpenCommand(); 
	Py_Return; 
} 
	
PyObject *FCDocumentPy::PyCommitCommand(PyObject *args)
{ 
	_pcDoc->CommitCommand(); 
	Py_Return; 
} 
	
PyObject *FCDocumentPy::PyAbortCommand(PyObject *args)
{ 
	_pcDoc->AbortCommand(); 
	Py_Return; 
} 
	
PyObject *FCDocumentPy::PyRecompute(PyObject *args)
{ 
	_pcDoc->Recompute(); 
	Py_Return; 
} 
		

