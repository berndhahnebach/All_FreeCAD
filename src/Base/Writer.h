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


#ifndef _PreComp_
# include <string>
# include <map>
#endif

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
    /// opens the file and read the first element
  Writer(const char* FileName);
  ~Writer();

  void addFile(const char* Name, Base::Persistance *Object);

  const char* ind(void);
  void incInd(void);
  void decInd(void);


private:
  struct FileEntry {
    std::string FileName;
    Base::Persistance *Object;
  };
  std::vector<FileEntry> FileList;

  short indent;
};

}


#endif
