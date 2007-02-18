/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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

#ifndef __DocumentPy_H__
#define __DocumentPy_H__

#include "../Base/PyObjectBase.h"


namespace App {
/** The Document python class
 */
class AppExport DocumentPy :public Base::PyObjectBase
{
	/// always start with Py_Header
	Py_Header;

protected:
	~DocumentPy();

public:
	DocumentPy(Document *pcDoc, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	virtual PyObject *_repr(void);  				// the representation
	PyObject *_getattr(char *attr);					// __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function
//	PyObject *PyDocType(PyObject *args);		// Python wrapper
//	static PyObject *sPyDocType(PyObject *self, PyObject *args, PyObject *kwd){return ((DocumentPy*)self)->PyDocType(args);};
	PYFUNCDEF_D(DocumentPy,save)
//	PYFUNCDEF_D(DocumentPy,SaveAs)
//	PYFUNCDEF_D(DocumentPy,SetModified)
//	PYFUNCDEF_D(DocumentPy,PurgeModified)

//	UndoRedo stuff
	PYFUNCDEF_D(DocumentPy,setUndoMode)
	PYFUNCDEF_D(DocumentPy,setTransactionMode)
	PYFUNCDEF_D(DocumentPy,openTransaction)
	PYFUNCDEF_D(DocumentPy,commitTransaction)
	PYFUNCDEF_D(DocumentPy,abortTransaction)
	PYFUNCDEF_D(DocumentPy,undo)
	PYFUNCDEF_D(DocumentPy,redo)
	PYFUNCDEF_D(DocumentPy,clearUndos)
	PYFUNCDEF_D(DocumentPy,getUndoMemSize)
	PYFUNCDEF_D(DocumentPy,getAvailableUndoNames)
	PYFUNCDEF_D(DocumentPy,getAvailableRedoNames)
	PYFUNCDEF_D(DocumentPy,getAvailableUndos)
	PYFUNCDEF_D(DocumentPy,getAvailableRedos)


//	Transaction stuff
	//PYFUNCDEF_D(DocumentPy,beginTransaction)
	//PYFUNCDEF_D(DocumentPy,rollbackTransaction)
	//PYFUNCDEF_D(DocumentPy,endTransaction)

  PYFUNCDEF_D(DocumentPy,recompute)
  PYFUNCDEF_D(DocumentPy,update)

//	PYFUNCDEF_D(DocumentPy,Dump)

	PYFUNCDEF_D(DocumentPy,activeObject)
	PYFUNCDEF_D(DocumentPy,addObject)
	PYFUNCDEF_D(DocumentPy,getObject)
	PYFUNCDEF_D(DocumentPy,removeObject)
	PYFUNCDEF_D(DocumentPy,listObjects)
	PYFUNCDEF_D(DocumentPy,getName)
	PYFUNCDEF_D(DocumentPy,getMemSize)

   // deprecated methods
	PYFUNCDEF_D(DocumentPy,getActiveObject)

private:
	Document *_pcDoc;

};

} // namespace App

#endif
