/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef APP_DOCUMENTOBJECTGROUPPY_H
#define APP_DOCUMENTOBJECTGROUPPY_H

#include "FeaturePy.h"


namespace App
{
class DocumentObjectGroup;

class AppExport DocumentObjectGroupPy : public DocumentObjectPy
{
  Py_Header;

protected:
  virtual ~DocumentObjectGroupPy();

public:
	DocumentObjectGroupPy(DocumentObjectGroup *pcGroup, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

  virtual PyObject *_repr(void);  				// the representation
  PyObject *_getattr(char *attr);					// __getattr__ function
  int _setattr(char *attr, PyObject *value);		// __setattr__ function
	PYFUNCDEF_D(DocumentObjectGroupPy,addObject)
	PYFUNCDEF_D(DocumentObjectGroupPy,removeObject)
	PYFUNCDEF_D(DocumentObjectGroupPy,getObject)
	PYFUNCDEF_D(DocumentObjectGroupPy,hasObject)

	//---------------------------------------------------------------------
	// helpers for python exports goes here +++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------
  DocumentObjectGroup *getDocumentObjectGroup(void) const;
};

} //namespace App


#endif // APP_DOCUMENTOBJECTGROUPPY_H
