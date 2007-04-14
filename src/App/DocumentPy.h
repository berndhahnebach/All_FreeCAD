
// This file is generated out of ... Every change you make here get lost at the next full rebuild!
#ifndef APP_DOCUMENTPY_H
#define APP_DOCUMENTPY_H

#include <App/PropertyContainerPy.h>

namespace App
{

class Document;

//===========================================================================
// DocumentPy - Python wrapper
//===========================================================================

/** The python export class for Document
 */
class AppExport DocumentPy : public App::PropertyContainerPy
{
	/// always start with Py_Header
	Py_Header;

public:
	DocumentPy(Document *pcDocumentObject, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);
	~DocumentPy();


	virtual PyObject *_repr(void);  				      // the representation
	PyObject *_getattr(char *attr);					      // __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function

	/** @name callbacks and implementer for the python object methods */
	//@{
	/// callback for the save() method
	static PyObject * staticCallback_save (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the save() method
	PyObject*  save(PyObject *args);
	/// callback for the restore() method
	static PyObject * staticCallback_restore (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the restore() method
	PyObject*  restore(PyObject *args);
	/// callback for the openTransaction() method
	static PyObject * staticCallback_openTransaction (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the openTransaction() method
	PyObject*  openTransaction(PyObject *args);
	/// callback for the abortTransaction() method
	static PyObject * staticCallback_abortTransaction (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the abortTransaction() method
	PyObject*  abortTransaction(PyObject *args);
	/// callback for the commitTransaction() method
	static PyObject * staticCallback_commitTransaction (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the commitTransaction() method
	PyObject*  commitTransaction(PyObject *args);
	/// callback for the addObject() method
	static PyObject * staticCallback_addObject (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the addObject() method
	PyObject*  addObject(PyObject *args);
	/// callback for the removeObject() method
	static PyObject * staticCallback_removeObject (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the removeObject() method
	PyObject*  removeObject(PyObject *args);
	/// callback for the undo() method
	static PyObject * staticCallback_undo (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the undo() method
	PyObject*  undo(PyObject *args);
	/// callback for the redo() method
	static PyObject * staticCallback_redo (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the redo() method
	PyObject*  redo(PyObject *args);
	/// callback for the clearUndos() method
	static PyObject * staticCallback_clearUndos (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the clearUndos() method
	PyObject*  clearUndos(PyObject *args);
	/// callback for the recompute() method
	static PyObject * staticCallback_recompute (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the recompute() method
	PyObject*  recompute(PyObject *args);
	/// callback for the getObject() method
	static PyObject * staticCallback_getObject (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the getObject() method
	PyObject*  getObject(PyObject *args);
	//@}

	
	/** @name callbacks and implementer for the python object methods */
	//@{
	/// getter for the ActiveObject attribute
	Py::Object getActiveObject(void) const;
	/// setter for the ActiveObject attribute
	void setActiveObject(Py::Object arg);
	/// getter for the Objects attribute
	Py::List getObjects(void) const;
	/// setter for the Objects attribute
	void setObjects(Py::List arg);
	/// getter for the UndoMode attribute
	Py::Int getUndoMode(void) const;
	/// setter for the UndoMode attribute
	void setUndoMode(Py::Int arg);
	/// getter for the UndoRedoMemSize attribute
	Py::Int getUndoRedoMemSize(void) const;
	/// setter for the UndoRedoMemSize attribute
	void setUndoRedoMemSize(Py::Int arg);
	/// getter for the UndoCount attribute
	Py::Int getUndoCount(void) const;
	/// setter for the UndoCount attribute
	void setUndoCount(Py::Int arg);
	/// getter for the RedoCount attribute
	Py::Int getRedoCount(void) const;
	/// setter for the RedoCount attribute
	void setRedoCount(Py::Int arg);
	/// getter for the UndoNames attribute
	Py::List getUndoNames(void) const;
	/// setter for the UndoNames attribute
	void setUndoNames(Py::List arg);
	/// getter for the RedoNames attribute
	Py::List getRedoNames(void) const;
	/// setter for the RedoNames attribute
	void setRedoNames(Py::List arg);
	/// getter methode for special attributes (e.g. dynamic ones)
	PyObject *getCustomAttributes(const char* attr) const;
	/// setter for special attributes (e.g. dynamic ones)
	int setCustomAttributes(const char* attr, PyObject *obj);
	//@}

	/// getter for the object handled by this class
	Document *getDocumentObject(void) const;
};

#define PARENTSDocumentPy &DocumentPy::Type,PARENTSPropertyContainerPy

}  //namespace App

#endif  // APP_DOCUMENTPY_H


