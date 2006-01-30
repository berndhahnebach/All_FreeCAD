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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <xercesc/sax2/Attributes.hpp>
# include <xercesc/sax/SAXParseException.hpp>
# include <xercesc/sax/SAXException.hpp>
# include <xercesc/sax2/XMLReaderFactory.hpp>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Writer.h"
#include "Exception.h"

using namespace Base;
using namespace std;
using namespace zipios ;



// ---------------------------------------------------------------------------
//  Writer: Constructors and Destructor
// ---------------------------------------------------------------------------

Writer::Writer(const char* FileName) 
: ZipOutputStream(FileName),indent(0)
{
}

Writer::~Writer()
{
}

void Writer::addFile(const char* Name, Base::Persistance *Object)
{
  FileEntry temp;
  temp.FileName = Name;
  temp.Object = Object;
  
  FileList.push_back(temp);

}

const char* Writer::ind(void)
{
  static char buf[256];
  short i;
  for(i=0;i<indent;i++)
    buf[i] = '\t';
  buf[i] = '\0';

  return buf;
}

void Writer::incInd(void)
{
  // because of buffer in ::ind()
  assert(indent < 255);
  indent++;
}

void Writer::decInd(void)
{
  indent--;
}
