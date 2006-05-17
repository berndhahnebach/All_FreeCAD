/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2005     *
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


#include "../Base/PyExportImp.h"


namespace Gui {
/** The Document python class 
 */
class GuiExport DocumentPy :public Base::PyObjectBase
{
	/// always start with Py_Header
	Py_Header;

protected:
	~DocumentPy();

public:
  DocumentPy(Gui::Document *pcDoc, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	virtual PyObject *_repr(void);  				// the representation
	PyObject *_getattr(char *attr);					// __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function
//	PyObject *PyDocType(PyObject *args);		// Python wrapper
//	static PyObject *sPyDocType(PyObject *self, PyObject *args, PyObject *kwd){return ((DocumentPy*)self)->PyDocType(args);};
	PYFUNCDEF_D(DocumentPy,hide)
	PYFUNCDEF_D(DocumentPy,show)
	PYFUNCDEF_D(DocumentPy,setPos)
	PYFUNCDEF_D(DocumentPy,addAnnotation)
	PYFUNCDEF_D(DocumentPy,update)
 
private:
  Gui::Document *_pcDoc;

};

} // namespace App

