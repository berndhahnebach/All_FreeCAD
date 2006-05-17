/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de)          *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef __Writer_H__
#define __Writer_H__


#include <string>
#include <vector>

#include "zipios/zipios-config.h"
#include "zipios/zipfile.h"
#include "zipios/zipinputstream.h"
#include "zipios/zipoutputstream.h"
#include "zipios/meta-iostreams.h"

#include "FileInfo.h"



namespace Base
{

class Persistance;


/** The Writer class 
 * This is a important helper class for the store and retrivel system
 * of objects in FreeCAD. 
 * \see App::Persistance
 * \author Juergen Riegel
 */
class BaseExport Writer: public zipios::ZipOutputStream
{

public:
  /// opens the file and write the first file
  Writer(const char* FileName);
  ~Writer();

  /// switch the writer in XML only mode (no files allowed)
  void setForceXML(bool on);
  /// check on state
  bool isForceXML(void);

  /// insert a file as CDATA section in the XML file
  void insertAsciiFile(const char* FileName);
  /// insert a binary file BASE64 coded as CDATA section in the XML file
  void insertBinFile(const char* FileName);

  /** @name additional file writing */
  //@{
  /// add a write request of a Persitant object
  std::string addFile(const char* Name, const Base::Persistance *Object);
  /// process the requested file writes
  void writeFiles(void);
  /// get all registered file names
  const std::vector<std::string>& getFilenames() const;
  //@{

  /** @name prety formating for XML */
  //@{
  /// get the actual indetion
  const char* ind(void){return indBuf;}
  /// increase indetion by one tab
  void incInd(void);
  /// decreas indetion by one tab
  void decInd(void);
  //@{


private:
  std::string getUniqueFileName(const char *Name);
  struct FileEntry {
    std::string FileName;
    const Base::Persistance *Object;
  };
  std::vector<FileEntry> FileList;
  std::vector<std::string> FileNames;

  short indent;
  char indBuf[256];

  bool forceXML;
};


/** The Reader class 
 * This is a important helper class for the restore of objects in FreeCAD. 
 * @see App::Persistance
 * @author Werner Mayer
 */
/*
class BaseExport Reader: public zipios::ZipInputStream
{
private:
  struct FileEntry {
    std::string FileName;
    std::string FeatName;
  };

public:
  typedef std::vector<FileEntry>::const_iterator  ConstIterator;

  ConstIterator begin (void) const { return FileList.begin(); }
  ConstIterator end (void) const { return FileList.end(); }

  /// opens the file and read the first file
  Reader(const char* FileName);
  ~Reader();

  void addFile(const char* FileName, const char* FeatName);

private:
  std::vector<FileEntry> FileList;
};
*/
}


#endif
