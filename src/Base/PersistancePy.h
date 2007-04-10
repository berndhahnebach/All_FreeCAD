
// This file is generated out of ... Every change you make here get lost at the next full rebuild!
#ifndef _PersistancePy_h_
#define _PersistancePy_h_

#include "BaseClassPy.h"

namespace Base
{

class Persistance;

//===========================================================================
// PersistancePy - Python wrapper
//===========================================================================

/** The python export class for Persistance
 */
class BaseExport PersistancePy :public Base::BaseClassPy
{
	/// always start with Py_Header
	Py_Header;

public:
	PersistancePy(Persistance *pcPersistanceObject, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);
	~PersistancePy();


	virtual PyObject *_repr(void);  				      // the representation
	PyObject *_getattr(char *attr);					      // __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function

  /** @name callbacks and implementer for the python object methodes */
  //@{
  //@}

	
  /** @name callbacks and implementer for the python object methodes */
  //@{
  /// geter for the  Content Attribute
  Py::String getContent(void);
  /// seter for the  Content Attribute
	void      setContent(Py::String arg);
  /// geter for the  MemSize Attribute
  Py::Int getMemSize(void);
  /// seter for the  MemSize Attribute
	void      setMemSize(Py::Int arg);
  /// getter methode for special Attributes (e.g. dynamic ones)
  PyObject *getCustomAttributes(const char* attr);
	/// setter for  special Attributes (e.g. dynamic ones)
  int setCustomAttributes(const char* attr, PyObject *obj);
  //@}

	/// geter for the object handled by this class
  Persistance *getPersistanceObject(void) const;
  
};
#define PARENTSPersistancePy &PersistancePy::Type,PARENTSBaseClassPy
}       //namespace Base
#endif	// _PersistancePy_h_


