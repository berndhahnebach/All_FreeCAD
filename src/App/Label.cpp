/***************************************************************************
 *   (c) J�rgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/

#include "PreCompiled.h"

#ifndef _PreComp_
#	include <TDocStd_Application.hxx>
#	include <TDataStd_Real.hxx>
#	include <TDataStd_Name.hxx>
#	include <Handle_TDataStd_Name.hxx>
#	include <TDataStd_Integer.hxx>
#	include <Handle_TDataStd_Integer.hxx>
#	include <TDF_ChildIterator.hxx>
#endif


#include "Document.h"
#include "Label.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"
#include "Application.h"
#include "Attribute.h"
#include "Function.h"

using Base::Console;
using Base::streq;

using namespace App;

/*

//===========================================================================
// FCLabel - Warpper for the TDF_Label classes
//===========================================================================

FCLabel::FCLabel(const TDF_Label &cLabel,FCDocument *pcDocument) 
 : _cLabel(cLabel),_pcDocument(pcDocument)
{
	_pcPyObject = new LabelPy(this);

	Console().Log("Create Label %p Py: %p Tag: %d Depth: %d\n",this,_pcPyObject,cLabel.Tag(),cLabel.Depth());
	
    //printf("Tag. %d\n",cLabel.Tag());
    //printf("Depth. %d\n",cLabel.Depth());

}


//--------------------------------------------------------------------------
//  FCLabel destructor 
//--------------------------------------------------------------------------
FCLabel::~FCLabel()						// Everything handled in parent
{
	Console().Log("Destroy Label %p\n",this);
} 




//--------------------------------------------------------------------------
// Exported functions
//--------------------------------------------------------------------------

FCPyHandle<FCLabel> FCLabel::GetLabel(int iN)
{
	return FCPyHandle<FCLabel>(_pcDocument->HasLabel( _cLabel.FindChild(iN)));
}

/// Gets a child label by name (Name Attribute), creats if not exist
FCPyHandle<FCLabel> FCLabel::GetLabel(const char* sName)
{

	// create a new label and put a name on it
	TDF_Label cL = _cLabel.NewChild();
	TDataStd_Name::Set(cL, TCollection_ExtendedString((Standard_CString)sName));
	return FCPyHandle<FCLabel>(_pcDocument->HasLabel( cL ));	


//	return FCPyHandle<FCLabel>(_pcDocument->HasPyLabel( _cLabel.FindChild(iN)));
}


FCFeature *FCLabel::GetFeature(void)
{
	Handle_TFunction_Function hcFunction;

	if(_cLabel.FindAttribute(TFunction_Function::GetID(), hcFunction))
	{
		//return hcFunction->GetFeature();
	}

	return NULL;

}


void FCLabel::SetFeature(FCFeature *pcFeature)
{
	Handle_TFunction_Function hcFunction;

	if(_cLabel.FindAttribute(TFunction_Function::GetID(), hcFunction))
	{
		//return hcFunction->GetFeature();
	}


}


/// checks if the label is there
bool FCLabel::HasLabel(int iN)
{
	return _cLabel.FindChild(iN,false).IsNull() == 0;
	//return false;
}


/// checks if the label is there by name (Name Attribute)
bool FCLabel::HasLabel(const char* sName)
{
	TDF_Label cDummy;

	return _FindLabelByName(sName,cDummy);

}

/// checks if the label is there by name (Name Attribute)
bool FCLabel::_FindLabelByName(const char* sName, TDF_Label &rcLabel)
{
	// scann all child labels to find the right name
	for (TDF_ChildIterator it(_cLabel); it.More(); it.Next())
	{
		Handle(TDataStd_Name) NameAttr;
		if(it.Value().FindAttribute(TDataStd_Name::GetID(),NameAttr))
			if( TCollection_ExtendedString((Standard_CString) sName) == NameAttr->Get())//.ToExtString())
			{
				rcLabel = it.Value();
				return true;
			}
	}

	return false;

}



/// Get all child labels
std::vector<FCPyHandle<FCLabel> > FCLabel::GetLabels(void)
{
	std::vector<FCPyHandle<FCLabel> > vhcLabels;

	for (TDF_ChildIterator it(_cLabel); it.More(); it.Next())
		vhcLabels.push_back( FCPyHandle<FCLabel>(_pcDocument->HasLabel(it.Value())));

	return vhcLabels;
}



FCPyObject *FCLabel::GetPyObject(void)
{
	return _pcPyObject;
}
*/

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject LabelPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"LabelPy",				/*tp_name*/
	sizeof(LabelPy),			/*tp_basicsize*/
	0,						/*tp_itemsize*/
	/* methods */
	PyDestructor,	  		/*tp_dealloc*/
	0,			 			/*tp_print*/
	__getattr, 				/*tp_getattr*/
	__setattr, 				/*tp_setattr*/
	0,						/*tp_compare*/
	__repr,					/*tp_repr*/
	0,						/*tp_as_number*/
	0,						/*tp_as_sequence*/
	0,						/*tp_as_mapping*/
	0,						/*tp_hash*/
	0,						/*tp_call */
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef LabelPy::Methods[] = {
  {"GetLabel",         (PyCFunction) sPyGetLabel,         Py_NEWARGS},
//  {"GetName",          (PyCFunction) sPyGetName,          Py_NEWARGS},
  {"HasChildren",      (PyCFunction) sPyHasChildren,      Py_NEWARGS},
  {"AttributeCount",   (PyCFunction) sPyAttributeCount,   Py_NEWARGS},
  {"ChildrenCount",    (PyCFunction) sPyChildrenCount,    Py_NEWARGS},
//  {"GetRoot",          (PyCFunction) sPyGetRoot,          Py_NEWARGS},
//  {"GetFather",        (PyCFunction) sPyGetFather,        Py_NEWARGS},

  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject LabelPy::Parents[] = {&LabelPy::Type, NULL};

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
LabelPy::LabelPy(TDF_Label cLabel, PyTypeObject *T)
 : FCPyObject( T), _cLabel(cLabel)
{
	//Console().Log("Create LabelPy: %p \n",this);
}

PyObject *LabelPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new LabelPy();			// Make new Python-able object
  return 0;
}

//--------------------------------------------------------------------------
//  LabelPy destructor 
//--------------------------------------------------------------------------
LabelPy::~LabelPy()						// Everything handled in parent
{
	//Console().Log("Destroy LabelPy: %p \n",this);
} 

//--------------------------------------------------------------------------
// LabelPy Attributes
//--------------------------------------------------------------------------
PyObject *LabelPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	PY_TRY{
		// Access the number of attributes at this label
		if (Base::streq(attr, "AttributeCount"))						
			return Py_BuildValue("i", _cLabel.NbAttributes()); 
		else if (streq(attr, "ChildrenCount"))					
			return Py_BuildValue("i", _cLabel.NbChildren()); 
		else if (streq(attr, "Root"))						
			return new LabelPy(_cLabel.Root()); 
		else if (streq(attr, "Father"))						
			return new LabelPy(_cLabel.Father()); 
		else if (streq(attr, "Real")){
			Handle(TDataStd_Real) RealAttr;
			if(_cLabel.FindAttribute(TDataStd_Real::GetID(),RealAttr))
				return Py_BuildValue("d",RealAttr->Get()); 
			else
				return 0;
		}else if (streq(attr, "Int")){
			Handle(TDataStd_Integer) IntAttr;
			if(_cLabel.FindAttribute(TDataStd_Integer::GetID(),IntAttr))
				return Py_BuildValue("d",IntAttr->Get()); 
			else
				return 0;
		}else if (streq(attr, "Name")){
			Handle(TDataStd_Name) NameAttr;
      if(_cLabel.FindAttribute(TDataStd_Name::GetID(),NameAttr)){
				//return Py_BuildValue("u",NameAttr->Get().ToExtString()); 
#ifdef FC_OS_LINUX /* will "u" work? */ // u is unicode as ToExtString is!
				return Py_BuildValue("u",NameAttr->Get().ToExtString()); 
#else
				return Py_BuildValue("s",NameAttr->Get()); 
#endif				
      }else{
				return 0;
      }
    }else
			_getattr_up(FCPyObject); 						// send to parent

  }PY_CATCH;
		
  return 0;

} 

int LabelPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (streq(attr, "Real"))						// settable new state
		TDataStd_Real::Set(_cLabel, PyFloat_AsDouble(value)); 
	else if (streq(attr, "Int"))						// settable new state
		TDataStd_Integer::Set(_cLabel, PyInt_AsLong(value)); 
	else if (streq(attr, "Name"))						// settable new state
		TDataStd_Name::Set(_cLabel, (short*)PyString_AsString(value)); 
		//TDataStd_Name::Set(_cLabel, (short*)PyUnicode_AsUnicode(value)); 
	else  
 
		return FCPyObject::_setattr(attr, value);	// send up to parent
	return 0;
} 


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PyObject *LabelPy::PyGetLabel(PyObject *args)
{ 
	int Tag;
    if (!PyArg_ParseTuple(args, "i",&Tag ))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	return new LabelPy(_cLabel.FindChild( Tag));
}
 
PyObject *LabelPy::PyHasChildren(PyObject *args)
{ 
	if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 
	return Py_BuildValue("i",_cLabel.HasChild()?1:0);
} 

PyObject *LabelPy::PyAttributeCount(PyObject *args)
{ 
	if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 
	return Py_BuildValue("i",_cLabel.NbAttributes());
} 

PyObject *LabelPy::PyChildrenCount(PyObject *args)
{ 
	if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 
	return Py_BuildValue("i",_cLabel.NbChildren());
}
 
/// checks if the label is there by name (Name Attribute)
bool LabelPy::_FindLabelByName(const char* sName, TDF_Label &rcLabel)
{
	// scann all child labels to find the right name
	for (TDF_ChildIterator it(_cLabel); it.More(); it.Next())
	{
		Handle(TDataStd_Name) NameAttr;
		if(it.Value().FindAttribute(TDataStd_Name::GetID(),NameAttr))
			if( TCollection_ExtendedString((Standard_CString) sName) == NameAttr->Get())//.ToExtString())
			{
				rcLabel = it.Value();
				return true;
			}
	}

	return false;

}



/*
PyObject *LabelPy::PyGetRoot(PyObject *args)
{ 
	if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 
	return new LabelPy(_cLabel.Root());
} 

PyObject *LabelPy::PyGetFather(PyObject *args)
{ 
	if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 
	return new LabelPy(_cLabel.Father());; 
} 

PyObject *LabelPy::PyGetName(PyObject *args)
{ 
	char *pstr;
	if (!PyArg_ParseTuple(args, "s",&pstr))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 
	Handle(TDataStd_Name) NameAttr;
	if(_pcLabel->_cLabel.FindAttribute(TDataStd_Name::GetID(),NameAttr))
		//return Py_BuildValue("u",NameAttr->Get().ToExtString()); 
#ifdef FC_OS_LINUX // will "u" work?  // u is unicode as ToExtString is!
		return Py_BuildValue("u",NameAttr->Get().ToExtString()); 
#else
		return Py_BuildValue("s",NameAttr->Get()); 
#endif				
	else
		return 0;
} 
*/

/*
PyObject *LabelPy::PyIsDifferent(PyObject *args)
{ 
	PyObject* pcObject;
    if (!PyArg_ParseTuple(args, "O!", &Type, &pcObject))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	
	//_cLabel.IsDifferent( ((LabelPy*)pcObject)->GetLabel() );

	return Py_BuildValue("i", _cLabel.IsDifferent( ((LabelPy*)pcObject)->GetLabel() ) );
	//Undo(); 
	//Py_Return; 
} 

*/



