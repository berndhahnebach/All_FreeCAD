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




inline EnvPrint(const char* sMsg)
{
	//cout << sMsg << endl;
}

/** Test if a Variable exist
  */
inline TestEnvExists(const char* sEnvName,bool &bFailure) 
{
	if (!getenv(sEnvName)){ 
          cerr<<"Environment variable "<<sEnvName<<" is not set!"<<endl; 
          bFailure = true;
        }  
}


inline std::string FindHomePath(const char* sCall)
{
	std::string Call(sCall), TempHomePath;
	std::string::size_type pos = Call.find_last_of(PATHSEP);
	TempHomePath.assign(Call,0,pos+1);
	return TempHomePath;
}

inline std::string GetFreeCADLib(void) 
{
	std::string TempLibPath = getenv("FREECADLIB");
	if(*(TempLibPath.rbegin()) != PATHSEP )
		TempLibPath += PATHSEP;
	EnvPrint(TempLibPath.c_str());
	return TempLibPath;
}


inline SetPluginDefaults(const char* sPath)
{
	std::string sTempString;

	sTempString += "CSF_StandardDefaults=";
	sTempString += sPath;
	putenv (sTempString.c_str());
	EnvPrint(sTempString.c_str());
	sTempString.clear();
	sTempString += "CSF_PluginDefaults=";
	sTempString += sPath;
	putenv (sTempString.c_str());
	EnvPrint(sTempString.c_str());
}


inline void SetPythonToFreeCADLib(const char* sLib)
{
	std::string sTempString2;
	if(std::string(sLib) != "")
	{
		sTempString2 += "PYTHONPATH=";
		sTempString2 += sLib;
		sTempString2 += "res";
		sTempString2 += PATHSEP;
		sTempString2 += "pylibs";
		sTempString2 += PATHSEP;
		putenv (sTempString2.c_str());
		EnvPrint(sTempString2.c_str());
		sTempString2.clear();
	}
}


inline SetCasCadeToFreeCADLib(const char* sLib)
{
	std::string sTempString3;
	if(std::string(sLib) != "")
	{
		/* CSF_MDTVFontDirectory */ 
		sTempString3 += "CSF_MDTVFontDirectory=";
		sTempString3 += sLib;
		sTempString3 += "res";
		sTempString3 += PATHSEP;
		sTempString3 += "OpenCascade";
		putenv (sTempString3.c_str());
		EnvPrint(sTempString3.c_str());
		sTempString3.clear();
		/* CSF_LANGUAGE */ 
		sTempString3 += "CSF_LANGUAGE=";
		sTempString3 += "us";
		/* CSF_SHMessage */ 
		sTempString3 += "CSF_SHMessage=";
		sTempString3 += sLib;
		sTempString3 += "res";
		sTempString3 += PATHSEP;
		sTempString3 += "OpenCascade";
		putenv (sTempString3.c_str());
		EnvPrint(sTempString3.c_str());
		sTempString3.clear();
		/* CSF_MDTVTexturesDirectory */  
		sTempString3 += "CSF_MDTVTexturesDirectory="; 
		sTempString3 += sLib; 
		sTempString3 += "res"; 
		sTempString3 += PATHSEP; 
		sTempString3 += "OpenCascade"; 
		putenv (sTempString3.c_str()); 
		EnvPrint(sTempString3.c_str()); 
		sTempString3.clear(); 
		/* CSF_XCAFDefaults */  
		sTempString3 += "CSF_XCAFDefaults="; 
		sTempString3 += sLib; 
		sTempString3 += "res"; 
		sTempString3 += PATHSEP; 
		sTempString3 += "OpenCascade"; 
		putenv (sTempString3.c_str()); 
		EnvPrint(sTempString3.c_str()); 
		sTempString3.clear(); 
		/* CSF_GraphicShr */  
		sTempString3 += "CSF_GraphicShr="; 
		sTempString3 += sLib; 
		sTempString3 += "dll"; 
		sTempString3 += PATHSEP; 
		sTempString3 += "TKOpenGl.dll"; 
		putenv (sTempString3.c_str()); 
		EnvPrint(sTempString3.c_str()); 
		sTempString3.clear(); 
		/* CSF_UnitsLexicon */  
		sTempString3 += "CSF_UnitsLexicon="; 
		sTempString3 += sLib; 
		sTempString3 += "res"; 
		sTempString3 += PATHSEP; 
		sTempString3 += "OpenCascade"; 
		sTempString3 += PATHSEP; 
		sTempString3 += "Lexi_Expr.dat"; 
		putenv (sTempString3.c_str()); 
		EnvPrint(sTempString3.c_str()); 
		sTempString3.clear(); 
		/* CSF_UnitsDefinition */  
		sTempString3 += "CSF_UnitsDefinition="; 
		sTempString3 += sLib; 
		sTempString3 += "res"; 
		sTempString3 += PATHSEP; 
		sTempString3 += "OpenCascade"; 
		sTempString3 += PATHSEP; 
		sTempString3 += "Units.dat"; 
		putenv (sTempString3.c_str()); 
		EnvPrint(sTempString3.c_str()); 
		sTempString3.clear(); 
		/* CSF_IGESDefaults */  
		sTempString3 += "CSF_IGESDefaults="; 
		sTempString3 += sLib; 
		sTempString3 += "res"; 
		sTempString3 += PATHSEP; 
		sTempString3 += "OpenCascade"; 
		putenv (sTempString3.c_str()); 
		EnvPrint(sTempString3.c_str()); 
		sTempString3.clear(); 
		/* CSF_STEPDefaults */  
		sTempString3 += "CSF_STEPDefaults="; 
		sTempString3 += sLib; 
		sTempString3 += "res"; 
		sTempString3 += PATHSEP; 
		sTempString3 += "OpenCascade"; 
		putenv (sTempString3.c_str()); 
		EnvPrint(sTempString3.c_str()); 
		sTempString3.clear(); 
	}
}
