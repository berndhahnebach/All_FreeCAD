/***************************************************************************
                          FCApplication.cpp  -  description
                             -------------------
    begin                : Tue Jan 2 2001
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/



#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#	pragma warning( disable : 4251 )
#else
#	include <TColStd_SequenceOfExtendedString.hxx>
#	include <TCollection_ExtendedString.hxx>
#endif


#include "Application.h"
#include "Document.h"
#include "../Base/Interpreter.h"
#include "../Base/Exception.h"
#include "../Base/Parameter.h"


Standard_CString FCApplicationOCC::ResourcesName()
{
	//return Standard_CString ("Resources");
	return Standard_CString ("Standard");	
}

void FCApplicationOCC::Formats(TColStd_SequenceOfExtendedString& Formats)
{
	Formats.Append(TCollection_ExtendedString ("FreeCad-Std"));
	Formats.Append(TCollection_ExtendedString ("MDTV-Standard"));
}

void FCApplicationOCC::InitViewer(const Handle(TDocStd_Document)& aDoc) const
{
	// Im not doing it here!
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

FCApplication::FCApplication(FCParameterManager *pcParamMngr)
	:_pcParamMngr(pcParamMngr)
{
	_hApp = new FCApplicationOCC;
	// seting up Python binding 
	(void) Py_InitModule("FreeCAD", FCApplication::Methods);

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

	_hApp->NewDocument("FreeCad-Std",hDoc);
	//_hApp->NewDocument("MDTV-Standard",hDoc);
	//_hApp->NewDocument("Standard",hDoc);
	pDoc = new FCDocument(hDoc);

	// Load module
	if(!GetInterpreter().LoadModule(Name))
	{
		delete pDoc ;  
		throw FCException("No doc module");
	}
	pDoc->_INCREF();
	_DocVector.push_back(pDoc);
	_pActiveDoc = pDoc;

	// trigger Observers (open windows and so on)
	NotifyDocNew(pDoc);

	return pDoc;
}

FCDocument* FCApplication::Open(const char * Name)
{
	Handle_TDocStd_Document hDoc;
	FCDocument*				pDoc;

	// create new (empty) document 
	//pDoc = New("Standard");

	// load
	// TCollection_ExtendedString aName = (Standard_CString) Name;
	switch(_hApp->Open(TCollection_ExtendedString((Standard_CString) Name),hDoc) )
	{
		case CDF_RS_OK:
			break;
		case CDF_RS_UnknownDocument:
			throw FCException("Unknown Document");
		case CDF_RS_AlreadyRetrieved:
			throw FCException("Already Retrieved");
		case CDF_RS_AlreadyRetrievedAndModified:
			throw FCException("AlreadyRetrievedAndModified");
		case CDF_RS_NoDriver:
			throw FCException("NoDriver");
		case CDF_RS_NoVersion:
			throw FCException("NoVersion");
		case CDF_RS_NoModel:
			throw FCException("NoModel");
		case CDF_RS_TypeNotFoundInSchema:
			throw FCException("TypeNotFoundInSchema");
		case CDF_RS_UnrecognizedFileFormat:
			throw FCException("UnrecognizedFileFormat");
		case CDF_RS_PermissionDenied:
			throw FCException("PermissionDenied");
		default:
			throw FCException("Unknown open error");
	}
	
	// Creating a FreeCAD Document
	pDoc = new FCDocument(hDoc);
	pDoc->_INCREF();
	_DocVector.push_back(pDoc);
	_pActiveDoc = pDoc;


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
	FCDocument*	pDoc = Active();

	pDoc->SaveAs(Name);
	
	return pDoc;
}

FCDocument* FCApplication::Active(void)
{
	
	return _pActiveDoc;
}

void FCApplication::SetActive(FCDocument* pDoc)
{
	_pActiveDoc = pDoc;
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
	{"ParamGet",       (PyCFunction) FCApplication::sGetParam,  1},

  {NULL, NULL}		/* Sentinel */
};

PYFUNCIMP_S(FCApplication,sOpen)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

         
	try {
		// return new document		
		return GetApplication().Open(pstr); 	
	}	
	catch(FCException e) {
		PyErr_SetString(PyExc_IOError, e.what());
		return 0L;
	}
	catch(Standard_Failure e)
	{
		Handle(Standard_Failure) E = Standard_Failure::Caught();
		FCstrstream strm;

		strm << E << endl;
		//strm.freeze();
		PyErr_SetString(PyExc_IOError, strm.str());
		return 0L;
	}

}

PYFUNCIMP_S(FCApplication,sNew)
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


PYFUNCIMP_S(FCApplication,sSave)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	//Instance().Message("%s",pstr);				 // process massage 
	return Py_None;                              // None: no errors 
}


PYFUNCIMP_S(FCApplication,sSaveAs)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	//Instance().Message("%s",pstr);				 // process massage 
	return Py_None;                              // None: no errors 
}

PYFUNCIMP_S(FCApplication,sGet)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	//Instance().Message("%s",pstr);				 // process massage 
	return Py_None;                              // None: no errors 
}

PYFUNCIMP_S(FCApplication,sGetParam)
{
    char *pstr=0;
    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	if(pstr) // if parameter give deticated group
		return new FCPyParametrGrp(GetApplication().GetParameter().GetGroup("BaseApp")->GetGroup(pstr)); 
	else
		return new FCPyParametrGrp(GetApplication().GetParameter().GetGroup("BaseApp"));  
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
/*
FCApplication & FCApplication::Instance(void)
{
	// not initialized?
	assert(_pcSingelton);
	if(!_pcSingelton)
	{
		_pcSingelton = new FCApplication();
		(void) Py_InitModule("FreeCAD", FCApplication::Methods);
	}
	return *_pcSingelton;
}
*/
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
	for(FCset<FCApplicationObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->OnDocNew(pcDoc);   // send doc to the listener
}
void FCApplication::NotifyDocDelete(FCDocument* pcDoc)
{
	for(FCset<FCApplicationObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->OnDocDelete(pcDoc);   // send doc to the listener
}
