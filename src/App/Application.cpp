/***************************************************************************
                          FCApplication.cpp  -  description
                             -------------------
    begin                : Tue Jan 2 2001
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/



#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <TColStd_SequenceOfExtendedString.hxx>
#	include <TCollection_ExtendedString.hxx>
#endif


#include "Application.h"
#include "Document.h"
#include "../Base/Interpreter.h"
#include "../Base/Exception.h"


Standard_CString FCApplicationOCC::ResourcesName()
{
	//return Standard_CString ("Resources");
	return Standard_CString ("Standard");
}

void FCApplicationOCC::Formats(TColStd_SequenceOfExtendedString& Formats)
{
	//Formats.Append(TCollection_ExtendedString ("FreeCad-Part"));
	Formats.Append(TCollection_ExtendedString ("MDTV-Standard"));

}

void FCApplicationOCC::InitViewer(const Handle(TDocStd_Document)& aDoc) const
{
}


FCApplicationOCC::~FCApplicationOCC() {}


// OCAF specific stuff


Standard_EXPORT Handle_Standard_Type& FCApplicationOCC_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TDocStd_Application);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TDocStd_Application);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(CDF_Application);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(CDF_Application);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(CDM_Application);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(CDM_Application);
  static Handle_Standard_Type aType4 = STANDARD_TYPE(Standard_Transient);
  if ( aType4.IsNull()) aType4 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,aType4,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("FCApplicationOCC",
			                                 sizeof(FCApplicationOCC),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle_FCApplicationOCC Handle_FCApplicationOCC::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle_FCApplicationOCC _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(FCApplicationOCC))) {
       _anOtherObject = Handle_FCApplicationOCC((Handle_FCApplicationOCC&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& FCApplicationOCC::DynamicType() const
{
  return STANDARD_TYPE(FCApplicationOCC) ;
}
Standard_Boolean FCApplicationOCC::IsKind(const Handle(Standard_Type)& AType) const
{
  return (STANDARD_TYPE(FCApplicationOCC) == AType || FCApplicationOCC::IsKind(AType));
}
Handle_FCApplicationOCC::~Handle_FCApplicationOCC() {}


//==========================================================================
// FCApplication
//==========================================================================



//**************************************************************************
// Construction and destruction

FCApplication::FCApplication(void)
{
	_hApp = new FCApplicationOCC;
}

FCApplication::~FCApplication()
{
}


//**************************************************************************
// Interface

FCDocument* FCApplication::New(const char * Name)
{
	Handle_TDocStd_Document hDoc;
	FCDocument*				pDoc;

	_hApp->NewDocument("FreeCad-Part",hDoc);
	//_hApp->NewDocument("MDTV-Standard",hDoc);
	//_hApp->NewDocument("Standard",hDoc);
	pDoc = new FCDocument(hDoc);

	// Load module
	if(!GetInterpreter().LoadModule(Name))
	{
		delete pDoc ;  
		throw FCException("No doc module");
	}

	_DocVector.push_back(pDoc);
	// trigger Observers (open windows and so on)
	NotifyDocNew(pDoc);

	return pDoc;
}

FCDocument* FCApplication::Open(const char * Name)
{
	Handle_TDocStd_Document hDoc;
	FCDocument*				pDoc;

	// create new (empty) document 
	_hApp->NewDocument("Standard",hDoc);

	// load
	// TCollection_ExtendedString aName = (Standard_CString) Name;
	_hApp->Open(TCollection_ExtendedString((Standard_CString) Name),hDoc);
	
	// Creating a FreeCAD Document
	pDoc = new FCDocument(hDoc);
	
	_DocVector.push_back(pDoc);


	// trigger Observers (open windows and so on)
	NotifyDocNew(pDoc);

	return pDoc;
}

FCDocument* FCApplication::Save(void)
{
	FCDocument*	pDoc = Active();

	pDoc->Save();
	
	return pDoc;
}

FCDocument* FCApplication::SaveAs(const char * Name)
{
	assert(0);
	return NULL;
}

FCDocument* FCApplication::Active(void)
{
	FCDocument*	pDoc = 0;
	
	return pDoc;
}


//**************************************************************************
// Python stuff

// FCApplication Methods						// Methods structure
PyMethodDef FCApplication::Methods[] = {
	{"DocNew",         (PyCFunction) FCApplication::sNew,       1},
	{"DocOpen",        (PyCFunction) FCApplication::sOpen,      1},
	{"DocSave"  ,      (PyCFunction) FCApplication::sSave,      1},
	{"DocSaveAs",      (PyCFunction) FCApplication::sSaveAs,    1},
	{"DocGet",         (PyCFunction) FCApplication::sGet,       1},

  {NULL, NULL}		/* Sentinel */
};

PYFUNCIMP(FCApplication,sOpen)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	//Instance().Message("%s",pstr);				 // process massage 
	return Py_None;                              // None: no errors 
}


PYFUNCIMP(FCApplication,sNew)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	try{
		return GetApplication().New(pstr);			 // process massage 
	}catch(...){
		return NULL;
	}
}


PYFUNCIMP(FCApplication,sSave)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	//Instance().Message("%s",pstr);				 // process massage 
	return Py_None;                              // None: no errors 
}


PYFUNCIMP(FCApplication,sSaveAs)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	//Instance().Message("%s",pstr);				 // process massage 
	return Py_None;                              // None: no errors 
}

PYFUNCIMP(FCApplication,sGet)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	//Instance().Message("%s",pstr);				 // process massage 
	return Py_None;                              // None: no errors 
}





//**************************************************************************
// Singelton stuff

FCApplication * FCApplication::_pcSingelton = 0;


void FCApplication::Destruct(void)
{
	// not initialized or doubel destruct!
	assert(_pcSingelton);
	delete _pcSingelton;
}

FCApplication & FCApplication::Instance(void)
{
	// not initialized?
	if(!_pcSingelton)
	{
		_pcSingelton = new FCApplication();
		(void) Py_InitModule("FreeCAD", FCApplication::Methods);
	}
	return *_pcSingelton;
}

//**************************************************************************
// Observer stuff


void FCApplication::AttacheObserver(FCApplicationObserver *pcObserver)
{
	// double insert !!
	assert(_aclObservers.find(pcObserver) == _aclObservers.end() );

	_aclObservers.insert(pcObserver);
}

void FCApplication::DetacheObserver(FCApplicationObserver *pcObserver)
{
	_aclObservers.erase(pcObserver);
}


void FCApplication::NotifyDocNew(FCDocument* pcDoc)
{
	for(stlport::set<FCApplicationObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->OnDocNew(pcDoc);   // send doc to the listener
}
void FCApplication::NotifyDocDelete(FCDocument* pcDoc)
{
	for(stlport::set<FCApplicationObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->OnDocDelete(pcDoc);   // send doc to the listener
}
