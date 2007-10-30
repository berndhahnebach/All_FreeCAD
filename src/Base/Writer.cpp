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
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Writer.h"
#include "Persistance.h"
#include "Exception.h"

#include <algorithm>
#include <locale>

using namespace Base;
using namespace std;
using namespace zipios ;



// ---------------------------------------------------------------------------
//  Writer: Constructors and Destructor
// ---------------------------------------------------------------------------

Writer::Writer(void) 
: indent(0),forceXML(false)
{
  indBuf[0] = '\0';

}

Writer::~Writer()
{
}

void Writer::insertAsciiFile(const char* FileName)
{

}

void Writer::insertBinFile(const char* FileName)
{

}

void Writer::setForceXML(bool on)
{
  forceXML = on;
}

bool Writer::isForceXML(void)
{
  return forceXML;
}



std::string Writer::addFile(const char* Name,const Base::Persistance *Object)
{
  // always check isForceXML() before requesting a file!
  assert(isForceXML()==false);

  FileEntry temp;
  temp.FileName = getUniqueFileName(Name);
  temp.Object = Object;
  
  FileList.push_back(temp);

  FileNames.push_back( temp.FileName );

  // return the unique file name
  return temp.FileName;
}

string Writer::getUniqueFileName(const char *Name)
{
  //std::vector<std::string>::const_iterator pos;

  // name in use?
  std::vector<std::string>::const_iterator pos = find(FileNames.begin(),FileNames.end(),Name);

  if (pos == FileNames.end())
    // if not, name is OK
    return Name;
  else
  {
    // find highes sufix
    int nSuff = 0;  
    for(pos = FileNames.begin();pos != FileNames.end();++pos)
    {
      const string &rclObjName = *pos;
      if (rclObjName.substr(0, strlen(Name)) == Name)  // Prefix gleich
      {
        string clSuffix(rclObjName.substr(strlen(Name)));
        if (clSuffix.size() > 0){
          int nPos = clSuffix.find_first_not_of("0123456789");
          if(nPos==-1)
            nSuff = max<int>(nSuff, atol(clSuffix.c_str()));
        }
      }
    }
    char szName[200];
    snprintf(szName, 200, "%s%d", Name, nSuff + 1);

    return string(szName);
  }
}

const std::vector<std::string>& Writer::getFilenames() const
{
  return FileNames;
}


void Writer::incInd(void)
{
  // because of buffer 
  assert(indent < 255);

  indBuf[indent] = '\t';
  indBuf[indent+1] = '\0';

  indent++;
}

void Writer::decInd(void)
{
  // indention mismatch
  assert(indent > 0);
  indent--;
  indBuf[indent] = '\0';
}

ZipWriter::ZipWriter(const char* FileName) 
: ZipStream(FileName)
{
#ifdef _MSC_VER
  ZipStream.imbue(std::locale::empty());
#else
  //FIXME: Check whether this is correct
  ZipStream.imbue(std::locale::classic());
#endif
}

void ZipWriter::writeFiles(void)
{
  for ( std::vector<FileEntry>::const_iterator it = FileList.begin(); it != FileList.end(); ++it )
  {
    ZipStream.putNextEntry(it->FileName);
    it->Object->SaveDocFile( *this );
  }
}

ZipWriter::~ZipWriter()
{
  ZipStream.close();
}
