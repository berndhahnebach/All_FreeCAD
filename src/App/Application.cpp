/***************************************************************************
                          FCApplication.cpp  -  description
                             -------------------
    begin                : Tue Jan 2 2001
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/



#include "PreCompiled.h"

#ifndef _PreComp_
#	include <TColStd_SequenceOfExtendedString.hxx>
#	include <TCollection_ExtendedString.hxx>
#endif


#include "Application.h"
#include "Document.h"
#include "Function.h"
#include "../Base/Interpreter.h"
#include "../Base/Exception.h"
#include "../Base/Parameter.h"

// static members
#include "../Version.h"
/// Major version nummber
const unsigned int FCApplication::VersionMajor = FCVersionMajor;
/// Minor version nummber
const unsigned int FCApplication::VersionMinor = FCVersionMinor;
/// Build nummber
const unsigned int FCApplication::VersionBuild = FCVersionBuild;
/// Build date
const char *       FCApplication::VersionDisDa = FCVersionDisDa;
/// Build date
const char *       FCApplication::VersionDate  = __DATE__;
/// Build date
const char *       FCApplication::VersionTime  = __TIME__;


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

FCApplicationOCC::FCApplicationOCC() 
{
	// Instanciate a TOcafFunction_BoxDriver and add it to the TFunction_DriverTable
//	TFunction_DriverTable::Get()->AddDriver(FCFunction::GetID(), 
//											new FCFunction());

}

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

FCApplication::FCApplication(FCParameterManager *pcSysParamMngr, FCParameterManager *pcUserParamMngr,std::map<std::string,std::string> &mConfig)
	:_pcSysParamMngr(pcSysParamMngr),
	 _pcUserParamMngr(pcUserParamMngr),
	 _mConfig(mConfig),
	 _pActiveDoc(0)
{
	_hApp = new FCApplicationOCC;
	mpcPramManager["User parameter"] = pcUserParamMngr;
	mpcPramManager["System parameter"] = pcSysParamMngr;

	// instanciate the workbench dictionary
	_pcTemplateDictionary = PyDict_New();

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
	PyObject* pcTemplate;

	if(Name)
	{
		// net buffer because of char* <-> const char*
		PyBuf NameBuf(Name);

		// get the python template object from the dictionary
		pcTemplate = PyDict_GetItemString(_pcTemplateDictionary, NameBuf.str);

		// test if the template is valid
		if(!pcTemplate)
			return NULL;
	}

	_hApp->NewDocument("FreeCad-Std",hDoc);
	//_hApp->NewDocument("MDTV-Standard",hDoc);
	//_hApp->NewDocument("Standard",hDoc);
	pDoc = new FCDocument(hDoc);

	// add the document to the internal list
	pDoc->_INCREF();
	_DocVector.push_back(pDoc);
	_pActiveDoc = pDoc;

	// runing the start of the workbench object
	if(Name)
	{
		GetInterpreter().RunMethodVoid(pcTemplate, "Start");
	}

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

FCParameterManager & FCApplication::GetSystemParameter(void) 
{
	return *_pcSysParamMngr;
}

FCParameterManager & FCApplication::GetUserParameter(void) 
{
	return *_pcUserParamMngr;
}

FCParameterManager & FCApplication::GetParameterSet(const char* sName)
{
	return *mpcPramManager[sName];
}

const std::map<std::string,FCParameterManager *> & FCApplication::GetParameterSetList(void)
{
	return mpcPramManager;
}

std::vector<std::string> FCApplication::GetAllTemplates(void)
{
	PyObject *key, *value;
	int pos = 0;
	std::vector<std::string> cTemp;
     

	// insert all items
	while (PyDict_Next(_pcTemplateDictionary, &pos, &key, &value)) {
		/* do something interesting with the values... */
		cTemp.push_back(PyString_AsString(key));
	}

	return cTemp;
}

FCHandle<FCParameterGrp>  FCApplication::GetParameterGroupByPath(const char* sName)
{
	std::string cName = sName,cTemp;

	std::string::size_type pos = cName.find(':');

	// is there a path seperator ?
	if(pos == std::string::npos)
	{
		throw FCException("FCApplication::GetParameterGroupByPath() no parameter set name specified");
	} 
	// assigning the parameter set name
    cTemp.assign(cName,0,pos);
	cName.erase(0,pos+1);

	// test if name is valid
	std::map<std::string,FCParameterManager *>::iterator It = mpcPramManager.find(cTemp.c_str());
	if (It == mpcPramManager.end())
		throw FCException("FCApplication::GetParameterGroupByPath() unknown parameter set name specified");

	return It->second->GetGroup(cName.c_str());
}


//**************************************************************************
// Python stuff

// FCApplication Methods						// Methods structure
PyMethodDef FCApplication::Methods[] = {
	{"DocNew",         (PyCFunction) FCApplication::sNew,            1},
	{"DocOpen",        (PyCFunction) FCApplication::sOpen,           1},
	{"DocSave"  ,      (PyCFunction) FCApplication::sSave,           1},
	{"DocSaveAs",      (PyCFunction) FCApplication::sSaveAs,         1},
	{"DocGet",         (PyCFunction) FCApplication::sGet,            1},
	{"ParamGet",       (PyCFunction) FCApplication::sGetParam,       1},
	{"Version",        (PyCFunction) FCApplication::sGetVersion,     1},
	{"ConfigGet",      (PyCFunction) FCApplication::sGetConfig,      1},
	{"ConfigSet",      (PyCFunction) FCApplication::sSetConfig,      1},
	{"TemplateAdd",    (PyCFunction) FCApplication::sTemplateAdd,    1},
	{"TemplateDelete", (PyCFunction) FCApplication::sTemplateDelete ,1},
	{"TemplateGet",    (PyCFunction) FCApplication::sTemplateGet    ,1},

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
		strstream strm;

		strm << E << endl;
		//strm.freeze();
		PyErr_SetString(PyExc_IOError, strm.str());
		return 0L;
	}

}

PYFUNCIMP_S(FCApplication,sNew)
{
    char *pstr = 0;
    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	try{
		FCDocument*	pDoc = GetApplication().New(pstr);
		if (pDoc)
			return pDoc;
		else
		{
			PyErr_SetString(PyExc_IOError, "Unknown Template");
			return NULL;
		}
	}catch(...){
		PyErr_SetString(PyExc_IOError, "Unknown Error in DocNew()");
		return NULL;
	}
}


PYFUNCIMP_S(FCApplication,sSave)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	//Instance().Message("%s",pstr);				 // process massage 
	Py_INCREF(Py_None);
	return Py_None;                              // None: no errors 
}


PYFUNCIMP_S(FCApplication,sSaveAs)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	//Instance().Message("%s",pstr);				 // process massage 
	Py_INCREF(Py_None);
	return Py_None;                              // None: no errors 
}

PYFUNCIMP_S(FCApplication,sGet)
{
    char *pstr=0;
    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	if(pstr == 0)
		return GetApplication().Active();

	Py_INCREF(Py_None);
	return Py_None;                              // None: no errors 
}

PYFUNCIMP_S(FCApplication,sGetParam)
{
    char *pstr=0;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	try{
		return GetPyObject(GetApplication().GetParameterGroupByPath(pstr)); 
	}catch(...)
	{
		PyErr_SetString(PyExc_IOError, "GetParam faild!\nusage:\n   GetParam\"(SetName:GroupName_1/GroupName_2/.../GroupName_n)");
		return NULL;
	}
}


PYFUNCIMP_S(FCApplication,sGetConfig)
{
	char *pstr=0;

    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	if(pstr) // if parameter give deticated group
		return Py_BuildValue("s",GetApplication()._mConfig[pstr].c_str()); 
	else
	{
		PyObject *pDict = PyDict_New();
		for(std::map<std::string,std::string>::iterator It= GetApplication()._mConfig.begin();It!=GetApplication()._mConfig.end();It++)
		{
			PyBuf Buf(It->second.c_str()),Buf2(It->first.c_str());
			PyDict_SetItemString(pDict,Buf2.str,PyString_FromString(Buf.str));
		}
		return pDict;
		
	}
}

PYFUNCIMP_S(FCApplication,sSetConfig)
{
	char *pstr,*pstr2;

    if (!PyArg_ParseTuple(args, "ss", &pstr,&pstr2))  // convert args: Python->C 
        return NULL; // NULL triggers exception 

	GetApplication()._mConfig[pstr] = pstr2;

	Py_INCREF(Py_None);
	return Py_None;
}

PYFUNCIMP_S(FCApplication,sGetVersion)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL; // NULL triggers exception 

	PyObject* pList = PyList_New(6); 
	PyObject *pItem;
	pItem = PyInt_FromLong(FCApplication::VersionMajor);
	PyList_SetItem(pList, 0, pItem);
	pItem = PyInt_FromLong(FCApplication::VersionMinor);
	PyList_SetItem(pList, 1, pItem);
	pItem = PyInt_FromLong(FCApplication::VersionBuild);
	PyList_SetItem(pList, 2, pItem);
	pItem = PyString_FromString(FCApplication::VersionDisDa);
	PyList_SetItem(pList, 3, pItem);
	pItem = PyString_FromString(FCApplication::VersionTime);
	PyList_SetItem(pList, 4, pItem);
	pItem = PyString_FromString(FCApplication::VersionDate);
	PyList_SetItem(pList, 5, pItem);

	return pList;
}

PYFUNCIMP_S(FCApplication,sTemplateAdd)
{
	char*       psKey;
	PyObject*   pcObject;
	if (!PyArg_ParseTuple(args, "sO", &psKey,&pcObject))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	Py_INCREF(pcObject);

	PyDict_SetItemString(GetApplication()._pcTemplateDictionary,psKey,pcObject);

	Py_INCREF(Py_None);
	return Py_None;
} 

PYFUNCIMP_S(FCApplication,sTemplateDelete)
{
	char*       psKey;
	if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	PyDict_DelItemString(GetApplication()._pcTemplateDictionary,psKey);

	Py_INCREF(Py_None);
    return Py_None;
} 
/*
PYFUNCIMP_S(FCApplication,sWorkbenchActivate)
{
	char*       psKey;
	if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	Instance->ActivateWorkbench(psKey);

  Py_INCREF(Py_None);
    return Py_None;
}
*/
PYFUNCIMP_S(FCApplication,sTemplateGet)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	return GetApplication()._pcTemplateDictionary;
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
	for(std::set<FCApplicationObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->OnDocNew(pcDoc);   // send doc to the listener
}
void FCApplication::NotifyDocDelete(FCDocument* pcDoc)
{
	for(std::set<FCApplicationObserver * >::iterator Iter=_aclObservers.begin();Iter!=_aclObservers.end();Iter++)
        (*Iter)->OnDocDelete(pcDoc);   // send doc to the listener
}
