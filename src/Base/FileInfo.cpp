/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2005                        *
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
# include <assert.h>
# include <stdio.h>
# if defined (FC_OS_LINUX) || defined(FC_OS_CYGWIN)
# include <unistd.h>
# include <sys/stat.h>
# elif defined (FC_OS_WIN32)
# include <direct.h>
# include <io.h>
# include <windows.h>
# endif
#endif


#include "FileInfo.h"
#include <sys/types.h>
#include <sys/stat.h>
//#include <windows.h>
#include <stdio.h>


using namespace Base;
using namespace std;


FileInfo::FileInfo (const char* FileName)
{
  setFile(FileName);
}

const char *FileInfo::getTempPath(void)
{
  static string tempPath;

  if(tempPath == "")
  {
#ifdef FC_OS_WIN32
    char buf[MAX_PATH + 2];
    GetTempPath(MAX_PATH + 1,buf);
    tempPath = buf;
#else
  const char* tmp = getenv("TEMP");
  if ( !tmp ) tmp = getenv("TMP");
  if ( tmp )
    tempPath = tmp;
#endif
  }

  return tempPath.c_str();
}

string FileInfo::getTempFileName(void)
{
#ifdef FC_OS_WIN32
  char buf[MAX_PATH + 2];
  GetTempFileName(getTempPath(),"FCTempFile",0,buf);
  return string(buf);
#else
  const char* tmp = getenv("TEMP");
  if ( !tmp ) tmp = getenv("TMP");
  std::string buf;
  if ( tmp ) { 
    buf = tmp;
    buf += "/";
  }
  buf += "FCT_tmpfile.tmp";
  return buf;
#endif
}

void FileInfo::makeTemp(const char* Template)
{
  char tmp[256];
  strcpy(tmp,Template);

#if defined (__GNUC__)
  mktemp(tmp);
#else
  _mktemp(tmp);
#endif

  setFile(tmp);
}

void FileInfo::setFile(const char* name)
{
  string result;
  const char *It=name;

  while(*It != '\0')
  {
    switch(*It)
    {
    case '\\':
      result += "/";
      break;
//    case ' ':
//      result += "\\ ";
//      break;
//    case '\'':
//      result += "\\\'";
//      break;
    default:
      result += *It;
    }
    It++;
  }

  FileName = result;
}

string FileInfo::filePath () const
{
  return FileName;
}

string FileInfo::fileName () const
{
  return FileName.substr(FileName.find_last_of('/')+1);
}

std::string FileInfo::dirPath () const
{
  return FileName.substr(0,FileName.find_last_of('/'));
}

string FileInfo::fileNamePure () const
{
  string temp = fileName();
  unsigned int pos = temp.find_last_of('.');
  
  if(pos != string::npos)
    return temp.substr(0,pos);
  else 
    return temp;
}

string FileInfo::extension ( bool complete ) const
{
  // complete not implemented
  assert(complete==false);
  return FileName.substr(FileName.find_last_of('.')+1);
}

bool FileInfo::hasExtension ( const char* Ext) const
{
#if defined (__GNUC__)
  return strcasecmp(Ext,extension().c_str()) == 0;
#else
  return _stricmp(Ext,extension().c_str()) == 0;
#endif
}

bool FileInfo::exists () const
{
#if defined (__GNUC__)
  return access(FileName.c_str(),0) == 0;
#else
  return _access(FileName.c_str(),0) == 0;
#endif
}

bool FileInfo::isReadable () const
{
#if defined (__GNUC__)
  return access(FileName.c_str(),4) == 0;
#else
  return _access(FileName.c_str(),4) == 0;
#endif
}

bool FileInfo::isWritable () const
{
#if defined (__GNUC__)
  return access(FileName.c_str(),2) == 0;
#else
  return _access(FileName.c_str(),2) == 0;
#endif
}

bool FileInfo::isFile () const
{
  if ( exists() )
  {
    FILE* file = fopen(FileName.c_str(), "r");
    if ( file )
    {
      // okay, it's an existing file
      fclose(file);
      return true;
    }
    else
      // must be a directory
      return false;
  }

  // TODO: Check for valid file name
  return true;
}

bool FileInfo::isDir () const
{
  if ( exists() )
  {
    // if we can chdir then it must be a directory
    char cwd[1000];
#if defined (__GNUC__)
    if ( getcwd(cwd,1000) != 0 && chdir(FileName.c_str()) == 0 )
    {
      chdir(cwd);
      return true;
#else
    if ( _getcwd(cwd,1000) != 0 && _chdir(FileName.c_str()) == 0 )
    {
      _chdir(cwd);
      return true;
#endif
    }
    else
      return false;
  }

  // TODO: Check for valid path name
  return true;
}

unsigned int FileInfo::size () const
{
  // not implemented
  assert(0);
  return 0;
}

bool FileInfo::deleteFile(void)
{
  #if defined (_MSC_VER)
    return DeleteFile(FileName.c_str())>0;
  #elif defined(__GNUC__)
    error "DeleteFile() not implemented for this platform!"
  #else
  # error "DeleteFile() not implemented for this platform!"
  #endif
}
bool FileInfo::createDirectory( const char* directory ) const
{
#if defined (_MSC_VER)
  return _mkdir( directory ) == 0;
#elif defined(__GNUC__)
  return mkdir( directory, 0777 ) == 0;
#else
# error "FileInfo::createDirectory() not implemented for this platform!"
#endif
}

