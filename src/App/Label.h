/** \file $RCSfile$
 *  \brief The label module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/
 
#ifndef __Label_h__
#define __Label_h__



#include "../Base/PyExport.h"
//#include "../Base/Observer.h"
//#include <TDocStd_Document.hxx>
//#include <TDF_Label.hxx>
//#include <TDF_ChildIterator.hxx>

//#include <map>
//#include <vector>

#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#	pragma warning( disable : 4290 )  // not implemented throw specification
#	pragma warning( disable : 4275 )  
#endif



class FCDocument;

/** The OCC Label wrapper class
 *  This class wrapps the functionality of the TDFSdt_Label of OCC. 
 *  Its used for building up hirachy in a OCC document by representing
 *  Nodes and Leavs
 *  @see FCDocument
 */
class AppExport FCLabel :public FCPyObject
{
	/** always start with Py_Header */
	Py_Header;

public:


	//---------------------------------------------------------------------
	// construction / destruction +++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	/// Constructer 
	FCLabel(const TDF_Label &hLabel,FCDocument *pcDocument, PyTypeObject *T = &Type);
	/// for Construction in python 
	static PyObject *PyMake(PyObject *, PyObject *);
	/// Destruction 
	~FCLabel();


	//---------------------------------------------------------------------
	// exported functions goes here +++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	/// Gets a child label, creats if not exist
	FCPyHandle<FCLabel> GetLabel(int);

	/// Gets a child label by name (Name Attribute), creats if not exist
	FCPyHandle<FCLabel> GetLabel(const char*);

	/// checks if the label is there
	bool HasLabel(int);

	/// checks if the label is there by name (Name Attribute)
	bool HasLabel(const char*);

	/// Get all child labels
	std::vector<FCPyHandle<FCLabel> > GetLabels(void);

	/// return if the label has at least one child
	bool HasChildren(void){return _cLabel.HasChild()!=0;}

	/// Gets the OCC Label
	TDF_Label GetOCCLabel(void){return _cLabel;}

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	PyObject *_getattr(char *attr);				// __getattr__ function
	// getter setter
	int _setattr(char *attr, PyObject *value);	// __setattr__ function
	// methods
	PyObject *PyGetLabel(PyObject *args);	// Python wrapper
	static PyObject *sPyGetLabel(PyObject *self, PyObject *args, PyObject *kwd){return ((FCLabel*)self)->PyGetLabel(args);};
	PYFUNCDEF_D (FCLabel,PyHasChildren);
	PYFUNCDEF_D (FCLabel,PyGetName);
	PYFUNCDEF_D (FCLabel,PyAttributeCount);
	PYFUNCDEF_D (FCLabel,PyChildrenCount);
	PYFUNCDEF_D (FCLabel,PyGetRoot);
	PYFUNCDEF_D (FCLabel,PyGetFather);


protected:

	/// The OCC Label
	TDF_Label _cLabel;
	/// Pointer to the FCDocument where the label comes from 
	FCDocument *_pcDocument;

};



#endif // __Label_h__
