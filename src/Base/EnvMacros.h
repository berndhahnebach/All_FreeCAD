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

#if defined (FC_OS_LINUX) || defined(FC_OS_CYGWIN)
#	include <unistd.h>
#	include <stdlib.h>
#	include <string>
#	include <vector>
#endif

#ifdef FC_OS_WIN32
#	include <direct.h>
# include <windows.h>
#endif

#define FC_VERBOSE

void EnvPrint(const char* sVar, const char* sVal)
{
#ifndef FC_VERBOSE
	std::cout << sVar << "=" << sVal  << std::endl;
#endif
}

void EnvPrint(const char* sMsg)
{
#ifndef FC_VERBOSE
	std::cout << sMsg << std::endl;
#endif
}

void SetEnvironment(const char* sVar, const char* sVal)
{
#if defined (FC_OS_LINUX) || defined(FC_OS_CYGWIN)
	setenv(sVar, sVal, 1);
//#elif defined (FC_OS_WIN32) // Seems not to work properly!
//	SetEnvironmentVariable(sVar,sVal);
#else
	std::string env(sVar);
	env += "=";
	env += sVal;
	_putenv (env.c_str());
	//SetEnvironmentVariable(sVar,sVal);
#endif
}

/** Test if a Variable exist
  */
void TestEnvExists(const char* sEnvName,bool &bFailure)
{
	if (!getenv(sEnvName)){
          std::cerr<<"Environment variable "<<sEnvName<<" is not set!"<<std::endl;
          bFailure = true;
        }
}

void SimplifyPath(std::string& sPath)
{
	// remove all unnecessary '/./' from sPath
	std::string sep; sep += PATHSEP;
	std::string pattern = sep + '.' + sep;
	std::string::size_type npos = sPath.find(pattern);
	while (npos != std::string::npos)
	{
		sPath.replace(npos, 3, sep);
		npos = sPath.find(pattern);
	}

	// remove all unnecessary '//' from sPath
	pattern = sep + sep;
	npos = sPath.find(pattern);
	while (npos != std::string::npos)
	{
		sPath.replace(npos, 2, sep);
		npos = sPath.find(pattern);
	}
}

#if defined (FC_OS_LINUX) || defined(FC_OS_CYGWIN)
std::string FindHomePathUnix(const char* sCall)
{
	std::string argv = sCall;
	std::string absPath;
	std::string homePath;
	std::string cwd;

	// get the current working directory
	char szDir[1024];
	if ( getcwd(szDir, sizeof(szDir)) == NULL)
		return homePath;

	cwd = szDir;

	// absolute path
	if ( argv[0] == PATHSEP )
	{
		absPath = argv;
#ifdef FC_DEBUG
		printf("Absolute path: %s\n", absPath.c_str());
#endif
	}
	// relative path
	else if ( argv.find(PATHSEP) != std::string::npos )
	{
		absPath = cwd + PATHSEP + argv;
#ifdef FC_DEBUG
		printf("Relative path: %s\n", argv.c_str());
		printf("Absolute path: %s\n", absPath.c_str());
#endif
	}
	// check PATH
	else
	{
#ifdef FC_DEBUG
		printf("Searching in PATH variable...");
#endif
		const char *pEnv = getenv( "PATH" );

		if ( pEnv )
		{
			std::string path = pEnv;
			std::vector<std::string> paths;

			// split into each component
			std::string::size_type start = 0;
			std::string::size_type npos = path.find(':', start);
			while ( npos != std::string::npos )
			{
				std::string tmp = path.substr(start, npos - start);
				paths.push_back( path.substr(start, npos - start) );
				start = npos + 1;
				npos = path.find(':', start);
			}

			// append also last component
			paths.push_back( path.substr(start) );

			for (std::vector<std::string>::const_iterator it = paths.begin(); it != paths.end(); ++it)
			{
				std::string test = *it + PATHSEP + argv;

				// no abs. path
				if ( test[0] != PATHSEP )
					test = cwd + PATHSEP + test;

				// does it exist?
				if ( _access(test.c_str(), 0) == 0 )
				{
					absPath = test;
#ifdef FC_DEBUG
					printf("found.\n");
					printf("Absolute path: %s\n", absPath.c_str());
#endif
					break;
				}
			}
		}
	}

  // neither an absolute path in the specified call nor a relative path nor a call in PATH (maybe called from within Python)
  if ( absPath.empty() )
  {
  	// get the current working directory
    absPath = cwd;
		std::string::size_type pos = absPath.find_last_of(PATHSEP);
		homePath.assign(absPath,0,pos);
    homePath += PATHSEP;

    EnvPrint(homePath.c_str());
  }
	// should be an absolute path now
	else if (absPath[0] == PATHSEP)
	{
		EnvPrint("FindHomePath -----------------");

		SimplifyPath( absPath );
		std::string::size_type pos = absPath.find_last_of(PATHSEP);
		homePath.assign(absPath,0,pos);
		pos = homePath.find_last_of(PATHSEP);
		homePath.assign(homePath,0,pos+1);

		EnvPrint(homePath.c_str());
	}
	else
	{
		printf("ERROR: no valid home path! (%s)\n", absPath.c_str());
		exit(0);
	}

	return homePath;
}

std::string FindPyHomePathUnix(const char* sCall)
{
	std::string argv = sCall;
	std::string absPath;
	std::string homePath;
	std::string cwd;

	// get the current working directory
	char szDir[1024];
	if ( getcwd(szDir, sizeof(szDir)) == NULL)
		return homePath;

	cwd = szDir;

	absPath = cwd + PATHSEP + argv;

	// check PATH if module is not in cwd
	if ( _access(absPath.c_str(), 0) != 0 )
	{
		const char *pEnv = getenv( "PYTHONPATH" );

		if ( pEnv )
		{
			std::string path = pEnv;
			std::vector<std::string> paths;

			// split into each component
			std::string::size_type start = 0;
			std::string::size_type npos = path.find(':', start);
			while ( npos != std::string::npos )
			{
				std::string tmp = path.substr(start, npos - start);
				paths.push_back( path.substr(start, npos - start) );
				start = npos + 1;
				npos = path.find(':', start);
			}

			// append also last component
			paths.push_back( path.substr(start) );

			for (std::vector<std::string>::const_iterator it = paths.begin(); it != paths.end(); ++it)
			{
				std::string test = *it + PATHSEP + argv;

				// no abs. path
				if ( test[0] != PATHSEP )
					test = cwd + PATHSEP + test;

				// does it exist?
				if ( _access(test.c_str(), 0) == 0 )
				{
					absPath = test;
					break;
				}
			}
		}
	}

	// should be an absolute path now
	if (absPath[0] == PATHSEP)
	{
		EnvPrint("FindHomePath -----------------");

		SimplifyPath( absPath );
		std::string::size_type pos = absPath.find_last_of(PATHSEP);
		homePath.assign(absPath,0,pos);
		pos = homePath.find_last_of(PATHSEP);
		homePath.assign(homePath,0,pos+1);

		EnvPrint(homePath.c_str());
	}

	return homePath;
}
#endif

#ifdef FC_OS_WIN32
std::string FindHomePathWin32(HANDLE hModule)
{
	char  szFileName [MAX_PATH] ;
	GetModuleFileName((HMODULE)hModule,
		               szFileName,
					   MAX_PATH-1);

	EnvPrint("FindHomePath -----------------");

	std::string Call(szFileName), TempHomePath;
	std::string::size_type pos = Call.find_last_of(PATHSEP);
	TempHomePath.assign(Call,0,pos);
	pos = TempHomePath.find_last_of(PATHSEP);
	TempHomePath.assign(TempHomePath,0,pos+1);

  // switch to posix style
  //for(std::string::iterator i=TempHomePath.begin();i!=TempHomePath.end();++i)
  //  if(*i == '\\')
  //    *i = '/';

	EnvPrint(TempHomePath.c_str());
//	return FindHomePath(szFileName);
	return TempHomePath;

}
#endif


std::string GetFreeCADLib(const char* sHomePath)
{
	EnvPrint("GetFreeCADLib ----------------");
	const char* env = getenv("FREECADLIB");
	std::string sEnv = env ? env : "";

	// Make sure that 'env' is _not_ empty, otherwise we run into trouble on platforms using gcc
	if ( !sEnv.empty() )
	{
		std::string TempLibPath = sEnv;
		if(*(TempLibPath.rbegin()) != PATHSEP )
			TempLibPath += PATHSEP;
		EnvPrint(TempLibPath.c_str());
		return TempLibPath;
	}else{
		EnvPrint("Failed using FREECADLIB env variable, search for Setup constellation (./LibPack)");
		std::string cStrLibPack(sHomePath);
		cStrLibPack += "LibPack";
		if(_chdir(cStrLibPack.c_str()) != -1)
		{
			std::string str(sHomePath);
			str += PATHSEP;
			str += "bin";
			// switch back to bin
			_chdir(str.c_str());
			EnvPrint("Found Setup constellation");
			return cStrLibPack+PATHSEP;
		}
		EnvPrint("No LibPack found, using standard environment (if there any)");
		return std::string();
	}
}


void SetPluginDefaults(const char* sPath)
{
	EnvPrint("OCC Plugins ------------------");

	std::string sTempString;

	sTempString += sPath;
	SetEnvironment ( "CSF_StandardDefaults", sTempString.c_str());
	EnvPrint( "CSF_StandardDefaults", sTempString.c_str());
	sTempString.clear();
	sTempString += sPath;
	SetEnvironment ("CSF_PluginDefaults", sTempString.c_str());
	EnvPrint("CSF_PluginDefaults", sTempString.c_str());
}


void SetPythonToFreeCADLib(const char* sLib)
{
	EnvPrint("Python Vars ------------------");

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

    SetEnvironment ("PYTHONHOME", "");
		EnvPrint("PYTHONHOME", "");
 
		sTempString2 += sLib;
		sTempString2 += "res";
		sTempString2 += PATHSEP;
		sTempString2 += "pylibs";
		//sTempString2 += PATHSEP;
		SetEnvironment ("TCLLIBPATH", sTempString2.c_str());
		EnvPrint("TCLLIBPATH", sTempString2.c_str());
		sTempString2.clear();
	}
}


void SetCasCadeToFreeCADLib(const char* sLib)
{
	EnvPrint("OCC Vars ---------------------");

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
#elif defined(FC_OS_LINUX) || defined(FC_OS_CYGWIN)
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

void PrintPath(void)
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
