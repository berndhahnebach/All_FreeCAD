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

#ifdef FC_OS_WIN32
#	include <direct.h>
#	include <windows.h>
#endif
#ifdef FC_OS_LINUX
#	include <unistd.h>
#	include <stdlib.h>
#endif



inline void EnvPrint(const char* sVar, const char* sVal)
{
	std::cout << sVar << "=" << sVal  << std::endl;
}

inline void EnvPrint(const char* sMsg)
{
	std::cout << sMsg << std::endl;
}

inline void SetEnvironment(const char* sVar, const char* sVal)
{
#ifdef FC_OS_LINUX
	setenv(sVar, sVal, 1);
#else
	std::string env(sVar);
	env += "=";
	env += sVal;
	putenv (env.c_str());
#endif
}

/** Test if a Variable exist
  */
inline void TestEnvExists(const char* sEnvName,bool &bFailure)
{
	if (!getenv(sEnvName)){
          std::cerr<<"Environment variable "<<sEnvName<<" is not set!"<<std::endl;
          bFailure = true;
        }
}

inline void SimplifyPath(std::string& sPath)
{
	// remove all unnecessary './' from sPath
	std::string sep; sep += PATHSEP;
	std::string pattern = sep + '.' + sep;
	std::string::size_type npos = sPath.find(pattern);
	while (npos != std::string::npos)
	{
		sPath.replace(npos, 3, sep);
		npos = sPath.find(pattern);
	}
}

#ifdef FC_OS_LINUX

inline std::string FindHomePathUnix(const char* sCall)
{
	std::string Call(sCall), TempHomePath;

	// no absolute path
	if (Call[0] != PATHSEP)
	{
		// relative path
		if (Call[0] == '.')
		{
			// get the current working directory
			char szDir[1024];
			if ( getcwd(szDir, sizeof(szDir)) )
			{
				Call = szDir;
				Call += PATHSEP;
				Call += sCall;
			}
		}
		// check the PATH environment variable
		else
		{
			if ( getenv("PATH") )
			{
				std::string sPATH = getenv("PATH");

				bool notfound = true, success = false;
				std::string tmp;
				std::string path;
				std::string::size_type npos = sPATH.find(':');
				while ( notfound )
				{
					if ( npos != std::string::npos )
					{
						path = sPATH.substr(0, npos);
						sPATH = sPATH.substr(npos+1);
						npos = sPATH.find(':');
					}
					else
					{
						// if this also fails break anyway
						notfound = false;
						path = sPATH;
					}

					tmp = path + PATHSEP + sCall;

					// does file exist?
					if ( access(tmp.c_str(), 0) == 0 )
					{
						Call = tmp;
						notfound = false;
						success = true;
						break;
					}
				}

				// path found?
				if ( success )
					std::cout << "Path found in " << path << std::endl;
				else
					std::cerr << "Sorry, could't find the right path!" << std::endl;
			}
		}
	}

	if (Call[0] == PATHSEP)
	{
		SimplifyPath(Call);
		EnvPrint("FindHomePath--------------------");
		EnvPrint(Call.c_str());
		std::string::size_type pos = Call.find_last_of(PATHSEP);
		TempHomePath.assign(Call,0,pos);
		pos = TempHomePath.find_last_of(PATHSEP);
		TempHomePath.assign(TempHomePath,0,pos+1);
		EnvPrint(TempHomePath.c_str());
	}

	return TempHomePath;
}
#endif

#ifdef FC_OS_WIN32
inline std::string FindHomePathWin32(HANDLE hModule)
{
	char  szFileName [MAX_PATH] ;
	GetModuleFileName((HMODULE)hModule,
		               szFileName,
					   MAX_PATH-1);

	EnvPrint("FindHomePath--------------------");
	EnvPrint(szFileName);

	std::string Call(szFileName), TempHomePath;
	std::string::size_type pos = Call.find_last_of(PATHSEP);
	TempHomePath.assign(Call,0,pos);
	pos = TempHomePath.find_last_of(PATHSEP);
	TempHomePath.assign(TempHomePath,0,pos+1);

	EnvPrint(TempHomePath.c_str());
//	return FindHomePath(szFileName);
	return TempHomePath;

}
#endif


inline std::string GetFreeCADLib(const char* sHomePath)
{
	EnvPrint("GetFreeCADLib--------------------");
	EnvPrint(sHomePath);

	if(getenv("FREECADLIB") )
	{
		std::string TempLibPath = getenv("FREECADLIB");
		if(*(TempLibPath.rbegin()) != PATHSEP )
			TempLibPath += PATHSEP;
		EnvPrint(TempLibPath.c_str());
		return TempLibPath;
	}else{
		EnvPrint("Failed using FREECADLIB env variable, search for Setup constellation (./LibPack)");
		std::string cStrLibPack(sHomePath);
		cStrLibPack += "LibPack";
		if(chdir(cStrLibPack.c_str()) != -1)
		{
			std::string str(sHomePath);
			str += PATHSEP;
			str += "bin";
			// switch back to bin
			chdir(str.c_str());
			EnvPrint("Found Setup constellation");
			return cStrLibPack+PATHSEP;
		}
		EnvPrint("No LibPack found, using standard environment (if there any)");
		return std::string();
	}
}


inline void SetPluginDefaults(const char* sPath)
{
	std::string sTempString;

	sTempString += sPath;
	SetEnvironment ( "CSF_StandardDefaults", sTempString.c_str());
	EnvPrint( "CSF_StandardDefaults", sTempString.c_str());
	sTempString.clear();
	sTempString += sPath;
	SetEnvironment ("CSF_PluginDefaults", sTempString.c_str());
	EnvPrint("CSF_PluginDefaults", sTempString.c_str());
}


inline void SetPythonToFreeCADLib(const char* sLib)
{
	if (sLib == NULL) return;
	std::string sTempString2;
	if(std::string(sLib) != "")
	{
		sTempString2 += sLib;
		sTempString2 += "res";
		sTempString2 += PATHSEP;
		sTempString2 += "pylibs";
		sTempString2 += PATHSEP;
		SetEnvironment ("PYTHONPATH", sTempString2.c_str());
		EnvPrint("PYTHONPATH", sTempString2.c_str());
		sTempString2.clear();

		sTempString2 += sLib;
		sTempString2 += "res";
		sTempString2 += PATHSEP;
		sTempString2 += "tcl8.4";
		//sTempString2 += PATHSEP;
		SetEnvironment ("TCL_LIBRARY", sTempString2.c_str());
		EnvPrint("TCL_LIBRARY", sTempString2.c_str());
		sTempString2.clear();

		sTempString2 += sLib;
		sTempString2 += "res";
		//sTempString2 += PATHSEP;
		//sTempString2 += "pylibs";
		//sTempString2 += PATHSEP;
		SetEnvironment ("TCLLIBPATH", sTempString2.c_str());
		EnvPrint("TCLLIBPATH", sTempString2.c_str());
		sTempString2.clear();
	}
}


inline void SetCasCadeToFreeCADLib(const char* sLib)
{
	if (sLib == NULL) return;
	std::string sTempString3;
	if(std::string(sLib) != "")
	{
		/* CSF_MDTVFontDirectory */
		sTempString3 += sLib;
		sTempString3 += "res";
		sTempString3 += PATHSEP;
		sTempString3 += "OpenCascade";
		SetEnvironment ("CSF_MDTVFontDirectory", sTempString3.c_str());
		EnvPrint("CSF_MDTVFontDirectory", sTempString3.c_str());
		sTempString3.clear();
		/* CSF_LANGUAGE */
		sTempString3 += "CSF_LANGUAGE=";
		sTempString3 += "us";
		sTempString3.clear();
		/* CSF_SHMessage */
		sTempString3 += sLib;
		sTempString3 += "res";
		sTempString3 += PATHSEP;
		sTempString3 += "OpenCascade";
		SetEnvironment ("CSF_SHMessage", sTempString3.c_str());
		EnvPrint("CSF_SHMessage", sTempString3.c_str());
		sTempString3.clear();
		/* CSF_MDTVTexturesDirectory */
		sTempString3 += sLib;
		sTempString3 += "res";
		sTempString3 += PATHSEP;
		sTempString3 += "OpenCascade";
		SetEnvironment ("CSF_MDTVTexturesDirectory", sTempString3.c_str());
		EnvPrint("CSF_MDTVTexturesDirectory", sTempString3.c_str());
		sTempString3.clear();
		/* CSF_XCAFDefaults */
		sTempString3 += sLib;
		sTempString3 += "res";
		sTempString3 += PATHSEP;
		sTempString3 += "OpenCascade";
		SetEnvironment ("CSF_XCAFDefaults", sTempString3.c_str());
		EnvPrint("CSF_XCAFDefaults", sTempString3.c_str());
		sTempString3.clear();
		/* CSF_GraphicShr */
#ifdef FC_OS_WIN32
		sTempString3 += sLib;
		sTempString3 += "dll";
		sTempString3 += PATHSEP;
		sTempString3 += "TKOpenGl.dll";
#elif defined(FC_OS_LINUX)
		sTempString3 += sLib;
		sTempString3 += "lib";
		sTempString3 += PATHSEP;
		sTempString3 += "libTKOpenGl.so";
#endif
		SetEnvironment ("CSF_GraphicShr", sTempString3.c_str());
		EnvPrint("CSF_GraphicShr", sTempString3.c_str());
		sTempString3.clear();
		/* CSF_UnitsLexicon */
		sTempString3 += sLib;
		sTempString3 += "res";
		sTempString3 += PATHSEP;
		sTempString3 += "OpenCascade";
		sTempString3 += PATHSEP;
		sTempString3 += "Lexi_Expr.dat";
		SetEnvironment ("CSF_UnitsLexicon", sTempString3.c_str());
		EnvPrint("CSF_UnitsLexicon", sTempString3.c_str());
		sTempString3.clear();
		/* CSF_UnitsDefinition */
		sTempString3 += sLib;
		sTempString3 += "res";
		sTempString3 += PATHSEP;
		sTempString3 += "OpenCascade";
		sTempString3 += PATHSEP;
		sTempString3 += "Units.dat";
		SetEnvironment ("CSF_UnitsDefinition", sTempString3.c_str());
		EnvPrint("CSF_UnitsDefinition", sTempString3.c_str());
		sTempString3.clear();
		/* CSF_IGESDefaults */
		sTempString3 += sLib;
		sTempString3 += "res";
		sTempString3 += PATHSEP;
		sTempString3 += "OpenCascade";
		SetEnvironment ("CSF_IGESDefaults", sTempString3.c_str());
		EnvPrint("CSF_IGESDefaults", sTempString3.c_str());
		sTempString3.clear();
		/* CSF_STEPDefaults */
		sTempString3 += sLib;
		sTempString3 += "res";
		sTempString3 += PATHSEP;
		sTempString3 += "OpenCascade";
		SetEnvironment ("CSF_STEPDefaults", sTempString3.c_str());
		EnvPrint("CSF_STEPDefaults", sTempString3.c_str());
		sTempString3.clear();
	}
}


inline void PrintPath(void)
{
	char * p = getenv("PATH");
	if ( p != NULL )
	{
		EnvPrint(p);
	}
	else
	{
		EnvPrint("No Path set");
	}
}
