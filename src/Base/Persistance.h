/***************************************************************************
 *   Copyright (c) Riegel         <juergen.riegel@web.de>                  *
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


#ifndef __Persistance_H__
#define __Persistance_H__

// Std. configurations


#ifndef _PreComp_
# include <string>
# include <map>
#endif

#include "Base.h"
#include "zipios/zipinputstream.h"

namespace Base
{
  typedef std::istream Reader;
  class Writer;
  class XMLReader;

/// Persistance class and root of the type system
class BaseExport Persistance : public BaseClass
{

  TYPESYSTEM_HEADER();

public: 
  /// This method is used to save properties or very small amounts of data to an XML document.
  virtual void Save (Writer &writer){
    assert(0);
  } 
  /// This method is used to restore properties from an XML document.
  virtual void Restore(XMLReader &reader){
    assert(0);
  } 
  /// This method is used to save large amounts of data to a binary file.
  virtual void SaveDocFile (Writer &writer){
    assert(0);
  } 
  /// This method is used to restore large amounts of data from a binary file.
  virtual void RestoreDocFile(Reader &reader){
    assert(0);
  } 

};





} //namespace Base

#endif // __Persistance_H__

