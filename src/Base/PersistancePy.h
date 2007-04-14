
// This file is generated out of ... Every change you make here get lost at the next full rebuild!
#ifndef BASE_PERSISTANCEPY_H
#define BASE_PERSISTANCEPY_H

#include <Base/BaseClassPy.h>

namespace Base
{

class Persistance;

//===========================================================================
// PersistancePy - Python wrapper
//===========================================================================

/** The python export class for Persistance
 */
class BaseExport PersistancePy : public Base::BaseClassPy
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

	/** @name callbacks and implementer for the python object methods */
	//@{
	//@}

	
	/** @name callbacks and implementer for the python object methods */
	//@{
	/// getter for the Content attribute
	Py::String getContent(void) const;
	/// setter for the Content attribute
	void setContent(Py::String arg);
	/// getter for the MemSize attribute
	Py::Int getMemSize(void) const;
	/// setter for the MemSize attribute
	void setMemSize(Py::Int arg);
	/// getter methode for special attributes (e.g. dynamic ones)
	PyObject *getCustomAttributes(const char* attr) const;
	/// setter for special attributes (e.g. dynamic ones)
	int setCustomAttributes(const char* attr, PyObject *obj);
	//@}

	/// getter for the object handled by this class
	Persistance *getPersistanceObject(void) const;
};

#define PARENTSPersistancePy &PersistancePy::Type,PARENTSBaseClassPy

}  //namespace Base

#endif  // BASE_PERSISTANCEPY_H


