/** \file $RCSfile$
 *  \brief The attribute module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

#include "PreCompiled.h"

#ifndef _PreComp_
#	include <TDF_Label.hxx>
#	include <TDF_ChildIterator.hxx>
#	include <TDF_Tool.hxx>
#	include <TCollection_AsciiString.hxx>
#	include <TDF_ListIteratorOfAttributeList.hxx>
#	include <Standard_GUID.hxx>
#endif

#include "Attribute.h"
#include "../Base/PyExportImp.h"




/** The OCC Attribute wrapper class
 *  This class wraps the functionality of the TDF_Attribute of OCC. 
 *  This base class can represent every Attribute!
 *  @see FCDocument,FCLabel
 */
class FCPyAttribute :public Base::FCPyObject
{
	/** always start with Py_Header */
	Py_Header;

public:
 
	//---------------------------------------------------------------------
	// construction / destruction +++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	/// Constructor 
	FCPyAttribute (const Handle(FCAttribute) &hAttribute, PyTypeObject *T = &Type);
	/// for Construction in python 
	static PyObject *PyMake(PyObject *, PyObject *);
	/// Destruction 
	~FCPyAttribute();

	//---------------------------------------------------------------------
	// python exports  ++++++++++++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	PyObject *_getattr(char *attr);				// __getattr__ function
	// getter setter
	int _setattr(char *attr, PyObject *value);	// __setattr__ function
	// methods
	PYFUNCDEF_D (FCPyAttribute,PyGetId);

protected:

	/// handle to the Attribute 
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#endif
	Handle(FCAttribute) _hAttribute;

};






//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& FCAttribute::GetID () 
{
  static Standard_GUID TDataStd_NameID("2a96b608-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_NameID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(FCAttribute) FCAttribute::Set(const TDF_Label& label,const TCollection_ExtendedString& string) 
{
  Handle(FCAttribute) N;
  if (!label.FindAttribute(FCAttribute::GetID(), N)) { 
    N = new FCAttribute ();   
    label.AddAttribute(N);
  }
  N->Set(string);    
  return N;  
}

FCAttribute::FCAttribute () {}

void FCAttribute::Set (const TCollection_ExtendedString& S) 
{
 
  Backup();
  myString = S;
  //TCollection_ExtendedString tmpS(S);
  //tmpS.RemoveAll(':');
  //myString = tmpS;
  //myEmpty = Standard_False;
}

TCollection_ExtendedString FCAttribute::Get () const {return myString;}

const Standard_GUID& FCAttribute::ID () const { return GetID(); }


Handle_TDF_Attribute FCAttribute::NewEmpty () const
{  
  return new FCAttribute(); 
}

void FCAttribute::Restore(const Handle(TDF_Attribute)& with) 
{
  myString = Handle(FCAttribute)::DownCast (with)->Get();
}


void FCAttribute::Paste (const Handle(TDF_Attribute)& into,
		           const Handle(TDF_RelocationTable)& RT) const
{
  Handle(FCAttribute)::DownCast (into)->Set (myString);
}


Standard_OStream& FCAttribute::Dump (Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  anOS << " Name=|"<<myString<<"|"<<endl;
  return anOS;
}


FCAttribute::~FCAttribute() {}
 


Standard_EXPORT Handle_Standard_Type& FCAttribute_Type_()
{

//  static Handle_Standard_Type aType1 = STANDARD_TYPE(FCAttribute);
//  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(FCAttribute);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(TDF_Attribute);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(TDF_Attribute);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(MMgt_TShared);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType4 = STANDARD_TYPE(Standard_Transient);
  if ( aType4.IsNull()) aType4 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {/*aType1,*/aType2,aType3,aType4,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("FCAttribute",
			                                 sizeof(FCAttribute),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(FCAttribute) Handle(FCAttribute)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(FCAttribute) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(FCAttribute))) {
       _anOtherObject = Handle(FCAttribute)((Handle(FCAttribute)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& FCAttribute::DynamicType() const 
{ 
  return STANDARD_TYPE(FCAttribute) ; 
}
Standard_Boolean FCAttribute::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(FCAttribute) == AType || TDF_Attribute::IsKind(AType)); 
}

Handle_FCAttribute::~Handle_FCAttribute() {}



//**************************************************************************
//**************************************************************************
// FCPyParametrGrp
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------
 
PyTypeObject FCPyAttribute::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"Attribute",		    /*tp_name*/
	sizeof(FCPyAttribute),  /*tp_basicsize*/
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
PyMethodDef FCPyAttribute::Methods[] = {
  {"GetId",            (PyCFunction) sPyGetId,           Py_NEWARGS},

  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject FCPyAttribute::Parents[] = {&FCPyObject::Type,&FCPyAttribute::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
FCPyAttribute::FCPyAttribute(const Handle(FCAttribute) &hAttribute, PyTypeObject *T ) 
	:FCPyObject(T),_hAttribute(hAttribute)
{
	//Console().Log("Create Param Group %p\n",this);
}

PyObject *FCPyAttribute::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new FCPyParameterGrp();			// Make new Python-able object
  return 0;
}

//--------------------------------------------------------------------------
//  FCPyParametrGrp destructor 
//--------------------------------------------------------------------------
FCPyAttribute::~FCPyAttribute()						// Everything handled in parent
{
	//Console().Log("Destroy ParameterGrp %p\n",this);
} 

//--------------------------------------------------------------------------
// FCPyParametrGrp Attributes
//--------------------------------------------------------------------------
PyObject *FCPyAttribute::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	/*
	try{
		// Access the number of attributes at this FCPyParametrGrp
		if (streq(attr, "AttributeCount"))						
			return Py_BuildValue("i", _cLabel.NbAttributes()); 
		else if (streq(attr, "ChildrenCount"))					
			return Py_BuildValue("i", _cLabel.NbChildren()); 
		else if (streq(attr, "Root"))						
			return _pcDocument->HasPyLabel( _cLabel.Root()); 
		else if (streq(attr, "Father"))						
			return _pcDocument->HasPyLabel( _cLabel.Father()); 
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
			if(_cLabel.FindAttribute(TDataStd_Name::GetID(),NameAttr))
				//return Py_BuildValue("u",NameAttr->Get().ToExtString()); 
#ifdef FC_OS_LINUX // u is unicode as ToExtString is!
				return Py_BuildValue("u",NameAttr->Get().ToExtString()); 
#else
				return Py_BuildValue("s",NameAttr->Get()); 
#endif				
			else
				return 0;
		}else*/
			_getattr_up(FCPyObject); 						// send to parent
/*	}catch(...){
		Console().Log("Exception in FCPyParametrGrp::_getattr()\n");
		return 0;
	}*/
		return 0;
} 

int FCPyAttribute::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ /*
	if (streq(attr, "Real"))						// settable new state
		TDataStd_Real::Set(_cLabel, PyFloat_AsDouble(value)); 
	else if (streq(attr, "Int"))						// settable new state
		TDataStd_Integer::Set(_cLabel, PyInt_AsLong(value)); 
	else if (streq(attr, "Name"))						// settable new state
		TDataStd_Name::Set(_cLabel, (short*)PyString_AsString(value)); 
		//TDataStd_Name::Set(_cLabel, (short*)PyUnicode_AsUnicode(value)); 
	else  
 */
		return FCPyObject::_setattr(attr, value);	// send up to parent
	return 0;
} 


//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------

PyObject *FCPyAttribute::PyGetId(PyObject *args)
{ 
	{
		PyErr_SetString(PyExc_IOError, "GetGroup failed");
		return 0L;
	}
} 



