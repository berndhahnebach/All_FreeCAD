#! python
# -*- coding: utf-8 -*-
# (c) 2006 Juergen Riegel 

import template
import generateBase.generateModel_Module
import generateBase.generateTools

class TemplateClassPyExport (template.ModelTemplate):
	def Generate(self):
		print "TemplateClassPyExport",self.path + self.export.Name
		file = open(self.path + self.export.Name + "Imp.cpp",'w')
		generateBase.generateTools.replace(self.TemplateImplement,locals(),file)
		file = open(self.path + self.export.Name + ".cpp",'w')
		generateBase.generateTools.replace(self.TemplateModule,locals(),file)
		file = open(self.path + self.export.Name + ".h",'w')
		generateBase.generateTools.replace(self.TemplateHeader,locals(),file)
		#file.write( generateBase.generateTools.replace(self.Template,locals()))

	TemplateHeader = """
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

/** The DocTypeStd python class
 */
class BaseExport PersistancePy :public Base::BaseClassPy
{
	/// always start with Py_Header
	Py_Header;

public:
	PersistancePy(Persistance *pcDocumentObject, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);
	~PersistancePy();

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	virtual PyObject *_repr(void);  				// the representation
	PyObject *_getattr(char *attr);					// __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function
	PYFUNCDEF_D(PersistancePy,getMemSize)

  

	//---------------------------------------------------------------------
	// helpers for python exports goes here +++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------
  Persistance *getPersistanceObject(void) const;
  
};
} //namespace Base
#endif	
	
"""	

	TemplateModule = """

	
"""	

	# here the template for the user part of the implementation. This gets NOT overrite when allready exist
	TemplateImplement = """
// 
#include "PreCompiled.h"

#include "@self.export.Name@.h"
using namespace @self.export.Name@;

// TODO This methode implement the function of the Feature
int @self.export.Name@::execute(void)
{
   return 0;
}

"""