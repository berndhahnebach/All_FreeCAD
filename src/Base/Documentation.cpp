/** \file Documentation.cpp
 *  \brief This module handels the retraivel and editing of build in documentation 
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
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
 

/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#	if defined (_POSIX_C_SOURCE)
#		undef  _POSIX_C_SOURCE
#	endif // (re-)defined in pyconfig.h
#	include <Python.h>
#	include <iostream>
#	include <fstream>
#	include <fcntl.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Documentation.h"



using namespace std;
//using namespace xalanc_1_5;


//**************************************************************************
//**************************************************************************
// FCDocumentationManager
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction


FCDocumentationProvider::FCDocumentationProvider(const char* sRootPath)
:_cRootPath(sRootPath)
{
	// last char of root path has to be always the path seperator '/' !!!
	assert((*(_cRootPath.rbegin())) == '/');
}
	

FCDocumentationProvider::~FCDocumentationProvider(void)
{

}

//**************************************************************************
//**************************************************************************
// FCDocProviderDirectory
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction


FCDocProviderDirectory::FCDocProviderDirectory(const char* sRootPath,const char* sDirPath )
	:FCDocumentationProvider(sRootPath),
	 _cDirPath(sDirPath)
{
	// empty dir string not allowed
	assert(_cDirPath.begin() != _cDirPath.end());
#ifdef FC_OS_WIN32
	// sDirPath string has to end with path seperator
	assert(_cDirPath[_cDirPath.size()-1] == '\\');
#else
	// sDirPath string has to end with path seperator
	assert(_cDirPath[_cDirPath.size()-1] == '/');
#endif
}
	

FCDocProviderDirectory::~FCDocProviderDirectory(void)
{

}

//**************************************************************************
// Exported methods



string FCDocProviderDirectory::Retrive(const char * PathExtension, TDocType& rtWhichType)
{
	ifstream cInputFile;

	string cBasePath = _cDirPath + PathExtension; 

// setting backslash for windows .....
#ifdef FC_OS_WIN32
	for( string::size_type i = 0;i< cBasePath.size();i++)
	{
		if(cBasePath[i]=='/') 
		{
			cBasePath[i]='\\'; 
		}
	}
#endif

	// test on std documentation

	int r = open( (cBasePath + ".FCDoc").c_str() ,O_RDONLY );
//	cInputFile.open((cBasePath + ".FCDoc").c_str(),ios::in);
//	if(cInputFile){
	if(r != -1){
		close(r);
		cBasePath += ".FCDoc";
		rtWhichType = Doc;
	}else{
		r = open( (cBasePath + ".FCDocScript").c_str() ,O_RDONLY );
		if(r != -1){
			close(r);
			cBasePath += ".FCDocScript";
			rtWhichType = DocScript;
		}else{
			r = open( (cBasePath + ".html").c_str() ,O_RDONLY );
			if(r != -1){
				close(r);
				cBasePath += ".html";
				rtWhichType = Html;
			}else{
				return string();
			}
		}
	}

	cInputFile.open(cBasePath.c_str(),ios::in);
	string cFileContent,cTemp;
	while(! cInputFile.eof()) {
		getline(cInputFile,cTemp);
		cFileContent += cTemp + "\n";
	}

	return cFileContent;

}


void FCDocProviderDirectory::Save(const char * PathExtension,const char* sDoc, TDocType tWhichType )
{
	ofstream cOutputFile;

	string cBasePath = _cDirPath + PathExtension; 

// setting backslash for windows .....
#ifdef FC_OS_WIN32
	for( string::size_type i = 0;i< cBasePath.size();i++)
		if(cBasePath[i]=='/') cBasePath[i]='\\'; 
#endif

	// std documentation

	if(tWhichType == DocScript)
		cOutputFile.open((cBasePath + ".FCDocScript").c_str());
	else if(tWhichType == Doc)
		cOutputFile.open((cBasePath + ".FCDoc").c_str());
	else
		// save with not allowed doc type!
		assert(0);

	return;
}


//**************************************************************************
//**************************************************************************
// FCDocumentationManager
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


string FCDocumentationManager::Retrive(const char * URL, TDocType tTypeRequest )
{
	string cOutput;
	TDocType tWhichType;

	for(list<FCDocumentationProvider*>::iterator It= mpcProviderList.begin();It!=mpcProviderList.end();It++)
	{
		if( strncmp(URL,(*It)->GetRootPath(),strlen((*It)->GetRootPath())) == 0)		
		{
			cOutput = (*It)->Retrive(&URL[strlen((*It)->GetRootPath())],tWhichType);

			if(cOutput != "")
				break;
		}
	}

	// if empty or already the right type?
	if(cOutput == "" || tTypeRequest==tWhichType ) return cOutput;

	// call a script and transform to Doc
	if(tWhichType == DocScript)
		_CallScript(cOutput);

	tWhichType = Doc;

	// translate in the right doc type 
	switch(tTypeRequest)
	{
	case TextPlain:// is a plain text coded document. 
		_Translate(cOutput,"PlainText.xsl");
		break;
	case TextAnsi: // is a text coded document with ANSI higlighting. 
		_Translate(cOutput,"AnsiText.xsl");
		break;
	case Html:     // is a html coded document. 
		_Translate(cOutput,"Html.xsl");
		break;
	case Tech:     // is a LaTex coded document. 
		_Translate(cOutput,"Tech.xsl");
		break;
	default:
		// unknown or unsuported translation
		return string();	
	}

	return cOutput;
	
}

void FCDocumentationManager::Save(const char * URL,const char* sDoc, TDocType tWhichType )
{

}


void FCDocumentationManager::AddProvider(FCDocumentationProvider* pcProvider)
{
	// insert the new provider at the end
	mpcProviderList.push_back( pcProvider );
}

FCDocumentationManager::FCDocumentationManager()
{
	
//	XMLPlatformUtils::Initialize();
//	XalanTransformer::initialize();
}

FCDocumentationManager::~FCDocumentationManager()
{
}


FCDocumentationManager & FCDocumentationManager::Instance(void)
{
	// not initialized?
	if(!_pcSingelton)
	{
		_pcSingelton = new FCDocumentationManager();
		(void) Py_InitModule("FreeCAD", FCDocumentationManager::Methods);
	}
	return *_pcSingelton;
}

FCDocumentationManager *FCDocumentationManager::_pcSingelton;

//**************************************************************************
// private helper methods

void FCDocumentationManager::_Translate(std::string &rcDoc, const char* sTransXSL)
{

/*	XalanTransformer cXalanTransformer;

	char* Temp = (char*) malloc(rcDoc.size()+2);
	strcpy(Temp,rcDoc.c_str());

	istringstream Input(Temp,rcDoc.size()+2);

	XSLTInputSource xmlIn(Input);
	XSLTInputSource xslIn(sTransXSL);
	XSLTResultTarget xmlOut("foo-out.xml");

	int theResult = cXalanTransformer.transform(xmlIn,xslIn,xmlOut);
*/
}

void FCDocumentationManager::_CallScript(std::string &rcScript)
{

}


//**************************************************************************
// Python stuff

// FCConsole Methods						// Methods structure
PyMethodDef FCDocumentationManager::Methods[] = {
	{"HelpAddPath",         (PyCFunction) FCDocumentationManager::sPyAddPath,         1},
	{"HelpGet",             (PyCFunction) FCDocumentationManager::sPyGet,             1},

  {NULL, NULL}		/* Sentinel */
};


PyObject *FCDocumentationManager::sPyAddPath(PyObject *self,			// static python wrapper
								             PyObject *args, 
								             PyObject *kwd)
{
    char *Path;
    char *Dir;
    if (!PyArg_ParseTuple(args, "ss",&Path, &Dir))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	Instance().AddProvider(new FCDocProviderDirectory(Path,Dir));				 // process massage 

	Py_INCREF(Py_None);
	return Py_None;                              // None: no errors 
}

PyObject *FCDocumentationManager::sPyGet(PyObject *self,			// static python wrapper
								         PyObject *args, 
								         PyObject *kwd)
{
    char *Path;
    if (!PyArg_ParseTuple(args, "s",&Path))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	Instance().Retrive(Path,TextPlain);				 // process massage 

	Py_INCREF(Py_None);
	return Py_None;                              // None: no errors 
}
