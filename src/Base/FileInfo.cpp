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


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <string>
#endif


#include "FileInfo.h"

using namespace Base;





FileInfo::FileInfo (const char* FileName)
{
  setFile(FileName);
}

FileInfo::makeTemp(const char* Template)
{
  char tmp[256];
  strcpy(tmp,Template);

  mktemp(tmp);

  setFile(tmp);
}
void FileInfo::setFile(const char* name)
{
  std::string result;
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


std::string FileInfo::filePath () const
{
  return FileName;
}

std::string FileInfo::fileName () const
{
  return FileName.substr(FileName.find_last_of('/')+1);
}

std::string FileInfo::extension ( bool complete ) const
{
  // complete not implemented
  assert(complete==false);
  return FileName.substr(FileName.find_last_of('.')+1);
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
  return exists();
}

bool FileInfo::isDir () const
{
  // not implemented
  assert(0);
  return false;

}

unsigned int FileInfo::size () const
{
  // not implemented
  assert(0);
  return 0;
}

