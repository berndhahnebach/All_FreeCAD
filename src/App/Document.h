#ifndef __Document_h__
#define __Document_h__

#include "../Base/Export.h"

#include "../base/PyExport.h"
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>

class AppExport FCLabel :public FCPyObject
{
	Py_Header;			// always start with Py_Header


public:
	FCLabel(const TDF_Label &hLabel, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);

	~FCLabel();


	// exported functions goes here +++++++++++++++++++++++++++++++++++++++
	TDF_Label GetLabel(void){return _cLabel;}	// Geter for the handled OCC class
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	PyObject *_getattr(char *attr);				// __getattr__ function
	// geter setter
	int _setattr(char *attr, PyObject *value);	// __setattr__ function
	// methods
	PyObject *PyGetLabel(PyObject *args);	// Python wrapper
	static PyObject *sPyGetLabel(PyObject *self, PyObject *args, PyObject *kwd){return ((FCLabel*)self)->PyGetLabel(args);};

protected:
#	pragma warning( disable : 4251 )
	TDF_Label _cLabel;
#	pragma warning( default : 4251 )

};


class AppExport FCDocument :public FCPyObject
{
	Py_Header;			// always start with Py_Header

protected:
#	pragma warning( disable : 4251 )
	Handle_TDocStd_Document _hDoc;
#	pragma warning( default : 4251 )

public:
	FCDocument(const Handle_TDocStd_Document &hDoc, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);

	~FCDocument();


	// exported functions goes here +++++++++++++++++++++++++++++++++++++++
	void Undo(void);				// Undo the last commited command
	void Redo(void);				// Redo the last undo
	void ClearUndos(void);			// Clear undo list 
	void SaveAs (const char* Name); // Save the Document under a new Name
	void Save (void);				// Save the document under the name its been opened
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	PyObject *_getattr(char *attr);					// __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function
	PyObject *PyUndo(PyObject *args);		// Python wrapper
	static PyObject *sPyUndo(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PyUndo(args);};
	PyObject *PyRedo(PyObject *args);		// Python wrapper
	static PyObject *sPyRedo(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PyRedo(args);};
	PyObject *PyClearUndos(PyObject *args);		// Python wrapper
	static PyObject *sPyClearUndos(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PyClearUndos(args);};
	PyObject *PySaveAs(PyObject *args);		// Python wrapper
	static PyObject *sPySaveAs(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PySaveAs(args);};
	PyObject *PySave(PyObject *args);		// Python wrapper
	static PyObject *sPySave(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PySave(args);};

};

#endif // __Document_h__