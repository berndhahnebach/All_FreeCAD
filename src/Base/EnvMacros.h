/** \file EnvMacros.h
 *  \brief Macros to handle environment checking and seting
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  We use macros here because every DLL has to implement the functions because of undefined loading time...
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2003                        *   
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





/** Test if a Variable exist
  */
#define TEST_ENVVAR_EXISTS(envvar,bFailure) \
	if (!getenv(envvar)){ \
          cerr<<"Environment variable "<<envvar<<" is not set!"<<endl; \
          bFailure = true;\
        }  

#define FIND_HOME_PATH(_Call,_Into)\
	std::string Call(_Call), TempHomePath;\
	std::string::size_type pos = Call.find_last_of(PATHSEP);\
	TempHomePath.assign(Call,0,pos+1);\
	_Into = TempHomePath.c_str();


#define GET_FREECADLIB(_Into) \
  std::string TempLibPath = getenv("FREECADLIB") ? getenv("FREECADLIB") : "";\
	if(*(TempLibPath.rbegin()) != PATHSEP )\
		TempLibPath += PATHSEP;\
	cout << "FREECADLIB=" << TempLibPath << endl;\
	_Into = TempLibPath;

#define SET_PLUGINDEFAULTS(_Path)\
	std::string sTempString;\
	sTempString += "CSF_StandardDefaults=";\
	sTempString += _Path;\
	putenv (sTempString.c_str());\
	cout << sTempString << endl;\
	sTempString.clear();\
	sTempString += "CSF_PluginDefaults=";\
	sTempString += _Path;\
	putenv (sTempString.c_str());\
	cout << sTempString << endl;\
	sTempString.clear();

#define SET_PYTHON_TO_FREECADLIB(_Lib)\
	std::string sTempString2;\
	if(_Lib != "")\
	{\
		sTempString2 += "PYTHONPATH=";\
		sTempString2 += _Lib;\
		sTempString2 += "res";\
		sTempString2 += PATHSEP;\
		sTempString2 += "pylibs23";\
		sTempString2 += PATHSEP;\
		putenv (sTempString2.c_str());\
		cout << sTempString2 << endl;\
		sTempString2.clear();\
	}


#define SET_CASCADE_TO_FREECADLIB(_Lib)\
	std::string sTempString3;\
	if(_Lib != "")\
	{\
		/* CSF_MDTVFontDirectory */ \
		sTempString3 += "CSF_MDTVFontDirectory=";\
		sTempString3 += _Lib;\
		sTempString3 += "res";\
		sTempString3 += PATHSEP;\
		sTempString3 += "OpenCascade51";\
		putenv (sTempString3.c_str());\
		cout << sTempString3 << endl;\
		sTempString3.clear();\
		/* CSF_LANGUAGE */ \
		sTempString3 += "CSF_LANGUAGE=";\
		sTempString3 += "us";\
		/* CSF_SHMessage */ \
		sTempString3 += "CSF_SHMessage=";\
		sTempString3 += _Lib;\
		sTempString3 += "res";\
		sTempString3 += PATHSEP;\
		sTempString3 += "OpenCascade51";\
		putenv (sTempString3.c_str());\
		cout << sTempString3 << endl;\
		sTempString3.clear();\
		/* CSF_MDTVTexturesDirectory */ \
		sTempString3 += "CSF_MDTVTexturesDirectory=";\
		sTempString3 += _Lib;\
		sTempString3 += "res";\
		sTempString3 += PATHSEP;\
		sTempString3 += "OpenCascade51";\
		putenv (sTempString3.c_str());\
		cout << sTempString3 << endl;\
		sTempString3.clear();\
		/* CSF_XCAFDefaults */ \
		sTempString3 += "CSF_XCAFDefaults=";\
		sTempString3 += _Lib;\
		sTempString3 += "res";\
		sTempString3 += PATHSEP;\
		sTempString3 += "OpenCascade51";\
		putenv (sTempString3.c_str());\
		cout << sTempString3 << endl;\
		sTempString3.clear();\
		/* CSF_GraphicShr */ \
		sTempString3 += "CSF_GraphicShr=";\
		sTempString3 += _Lib;\
		sTempString3 += "dll";\
		sTempString3 += PATHSEP;\
		sTempString3 += "TKOpenGl.dll";\
		putenv (sTempString3.c_str());\
		cout << sTempString3 << endl;\
		sTempString3.clear();\
		/* CSF_UnitsLexicon */ \
		sTempString3 += "CSF_UnitsLexicon=";\
		sTempString3 += _Lib;\
		sTempString3 += "res";\
		sTempString3 += PATHSEP;\
		sTempString3 += "OpenCascade51";\
		sTempString3 += PATHSEP;\
		sTempString3 += "Lexi_Expr.dat";\
		putenv (sTempString3.c_str());\
		cout << sTempString3 << endl;\
		sTempString3.clear();\
		/* CSF_UnitsDefinition */ \
		sTempString3 += "CSF_UnitsDefinition=";\
		sTempString3 += _Lib;\
		sTempString3 += "res";\
		sTempString3 += PATHSEP;\
		sTempString3 += "OpenCascade51";\
		sTempString3 += PATHSEP;\
		sTempString3 += "Units.dat";\
		putenv (sTempString3.c_str());\
		cout << sTempString3 << endl;\
		sTempString3.clear();\
		/* CSF_IGESDefaults */ \
		sTempString3 += "CSF_IGESDefaults=";\
		sTempString3 += _Lib;\
		sTempString3 += "res";\
		sTempString3 += PATHSEP;\
		sTempString3 += "OpenCascade51";\
		putenv (sTempString3.c_str());\
		cout << sTempString3 << endl;\
		sTempString3.clear();\
		/* CSF_STEPDefaults */ \
		sTempString3 += "CSF_STEPDefaults=";\
		sTempString3 += _Lib;\
		sTempString3 += "res";\
		sTempString3 += PATHSEP;\
		sTempString3 += "OpenCascade51";\
		putenv (sTempString3.c_str());\
		cout << sTempString3 << endl;\
		sTempString3.clear();\
	}
