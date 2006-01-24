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
# include <string>
# include <stdio.h>
# ifdef FC_OS_LINUX
# include <unistd.h>
# include <sys/stat.h>
# endif
#endif


#include "FileInfo.h"

using namespace Base;
using namespace std;


FileInfo::FileInfo (const char* FileName)
{
  setFile(FileName);
}

void FileInfo::makeTemp(const char* Template)
{
  char tmp[256];
  strcpy(tmp,Template);

  mktemp(tmp);

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
#ifdef FC_OS_LINUX
  return strcasecmp(Ext,extension().c_str()) == 0;
#else
  return stricmp(Ext,extension().c_str()) == 0;
#endif
}

bool FileInfo::exists () const
{
  return access(FileName.c_str(),0) == 0;
}

bool FileInfo::isReadable () const
{
  return access(FileName.c_str(),4) == 0;
}

bool FileInfo::isWritable () const
{
  return access(FileName.c_str(),2) == 0;
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
    if ( getcwd(cwd,1000) != 0 && chdir(FileName.c_str()) == 0 )
    {
      chdir(cwd);
      return true;
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

