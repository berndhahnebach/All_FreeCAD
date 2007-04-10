
// This file is generated out of ... Every change you make here get lost at the next full rebuild!
#ifndef _DocumentPy_h_
#define _DocumentPy_h_

#include "PropertyContainerPy.h"

namespace App
{

class Document;

//===========================================================================
// DocumentPy - Python wrapper
//===========================================================================

/** The python export class for Document
 */
class AppExport DocumentPy :public App::PropertyContainerPy
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

  /** @name callbacks and implementer for the python object methodes */
  //@{
  /// callback for the save() methode
  static PyObject * staticCallback_save (PyObject *self, PyObject *args, PyObject *kwd);
  /// implementer for the save() methode
	PyObject*  save(PyObject *args);
  /// callback for the restore() methode
  static PyObject * staticCallback_restore (PyObject *self, PyObject *args, PyObject *kwd);
  /// implementer for the restore() methode
	PyObject*  restore(PyObject *args);
  /// callback for the openTransaction() methode
  static PyObject * staticCallback_openTransaction (PyObject *self, PyObject *args, PyObject *kwd);
  /// implementer for the openTransaction() methode
	PyObject*  openTransaction(PyObject *args);
  /// callback for the abortTransaction() methode
  static PyObject * staticCallback_abortTransaction (PyObject *self, PyObject *args, PyObject *kwd);
  /// implementer for the abortTransaction() methode
	PyObject*  abortTransaction(PyObject *args);
  /// callback for the commitTransaction() methode
  static PyObject * staticCallback_commitTransaction (PyObject *self, PyObject *args, PyObject *kwd);
  /// implementer for the commitTransaction() methode
	PyObject*  commitTransaction(PyObject *args);
  /// callback for the addObject() methode
  static PyObject * staticCallback_addObject (PyObject *self, PyObject *args, PyObject *kwd);
  /// implementer for the addObject() methode
	PyObject*  addObject(PyObject *args);
  /// callback for the removeObject() methode
  static PyObject * staticCallback_removeObject (PyObject *self, PyObject *args, PyObject *kwd);
  /// implementer for the removeObject() methode
	PyObject*  removeObject(PyObject *args);
  /// callback for the undo() methode
  static PyObject * staticCallback_undo (PyObject *self, PyObject *args, PyObject *kwd);
  /// implementer for the undo() methode
	PyObject*  undo(PyObject *args);
  /// callback for the redo() methode
  static PyObject * staticCallback_redo (PyObject *self, PyObject *args, PyObject *kwd);
  /// implementer for the redo() methode
	PyObject*  redo(PyObject *args);
  /// callback for the clearUndos() methode
  static PyObject * staticCallback_clearUndos (PyObject *self, PyObject *args, PyObject *kwd);
  /// implementer for the clearUndos() methode
	PyObject*  clearUndos(PyObject *args);
  /// callback for the recompute() methode
  static PyObject * staticCallback_recompute (PyObject *self, PyObject *args, PyObject *kwd);
  /// implementer for the recompute() methode
	PyObject*  recompute(PyObject *args);
  /// callback for the getObject() methode
  static PyObject * staticCallback_getObject (PyObject *self, PyObject *args, PyObject *kwd);
  /// implementer for the getObject() methode
	PyObject*  getObject(PyObject *args);
  //@}

	
  /** @name callbacks and implementer for the python object methodes */
  //@{
  /// geter for the  ActiveObject Attribute
  Py::Object getActiveObject(void);
  /// seter for the  ActiveObject Attribute
	void      setActiveObject(Py::Object arg);
  /// geter for the  Objects Attribute
  Py::List getObjects(void);
  /// seter for the  Objects Attribute
	void      setObjects(Py::List arg);
  /// geter for the  UndoMode Attribute
  Py::Int getUndoMode(void);
  /// seter for the  UndoMode Attribute
	void      setUndoMode(Py::Int arg);
  /// geter for the  UndoRedoMemSize Attribute
  Py::Int getUndoRedoMemSize(void);
  /// seter for the  UndoRedoMemSize Attribute
	void      setUndoRedoMemSize(Py::Int arg);
  /// geter for the  UndoCount Attribute
  Py::Int getUndoCount(void);
  /// seter for the  UndoCount Attribute
	void      setUndoCount(Py::Int arg);
  /// geter for the  RedoCount Attribute
  Py::Int getRedoCount(void);
  /// seter for the  RedoCount Attribute
	void      setRedoCount(Py::Int arg);
  /// geter for the  UndoNames Attribute
  Py::List getUndoNames(void);
  /// seter for the  UndoNames Attribute
	void      setUndoNames(Py::List arg);
  /// geter for the  RedoNames Attribute
  Py::List getRedoNames(void);
  /// seter for the  RedoNames Attribute
	void      setRedoNames(Py::List arg);
  /// getter methode for special Attributes (e.g. dynamic ones)
  PyObject *getCustomAttributes(const char* attr);
	/// setter for  special Attributes (e.g. dynamic ones)
  int setCustomAttributes(const char* attr, PyObject *obj);
  //@}

	/// geter for the object handled by this class
  Document *getDocumentObject(void) const;
  
};
#define PARENTSDocumentPy &DocumentPy::Type,PARENTSPropertyContainerPy
}       //namespace App
#endif	// _DocumentPy_h_


