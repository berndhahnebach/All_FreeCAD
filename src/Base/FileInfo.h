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


#ifndef _FILEINFO_H_
#define _FILEINFO_H_

#include <string>


namespace Base
{

 
/** File name unification
  * This class handles everything related to file names
  */
class BaseExport FileInfo
{
	public:
		/// Constrction
		FileInfo (const char* FileName="");
    /// gecerates a unique file name
    makeTemp(const char* Template);
    /// set a new file name
    void setFile(const char* name);
    /// set a new file name
    void setFile(const std::string &name){setFile(name.c_str());}
    /// exists the file?
    bool exists () const;
    /// Returns the file name, including the path (which may be absolute or relative).
    std::string filePath () const;
    /// Returns the name of the file, excluding the path, including the extension.
    std::string fileName () const;
    /** Returns the file's extension name.
      * If complete is TRUE (the default), extension() returns the string of all characters in the file name after (but not including) the first '.' character.
      * If complete is FALSE, extension() returns the string of all characters in the file name after (but not including) the last '.' character.
      * Example:
      *@code
      *  FileInfo fi( "/tmp/archive.tar.gz" );
      *  std::string ext = fi.extension(true);  // ext = "tar.gz"
      *  ext = fi.extension(false);   // ext = "gz"
      *  ext = fi.extension();   // ext = "gz"
      *@endcode
      */
      std::string extension ( bool complete = false ) const;
    /// checks if the file is readable
    bool isReadable () const;
    bool isWritable () const;
    bool isFile () const;
    bool isDir () const;
    unsigned int size () const;

  protected:
    std::string FileName;
};

// --------------------------------------------------------------------



} //namespace Base


#endif

