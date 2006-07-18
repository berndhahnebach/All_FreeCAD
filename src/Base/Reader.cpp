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
# include <xercesc/sax/SAXParseException.hpp>
# include <xercesc/sax/SAXException.hpp>
# include <xercesc/sax2/XMLReaderFactory.hpp>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Reader.h"
#include "Exception.h"
#include "Persistance.h"
#include "gzstream.h"
#include "InputSource.h"

#include "zipios/zipios-config.h"
#include "zipios/zipfile.h"
#include "zipios/zipinputstream.h"
#include "zipios/zipoutputstream.h"
#include "zipios/meta-iostreams.h"

#include "XMLTools.h"

XERCES_CPP_NAMESPACE_USE

using namespace std;



// ---------------------------------------------------------------------------
//  Base::XMLReader: Constructors and Destructor
// ---------------------------------------------------------------------------

Base::XMLReader::XMLReader(const char* FileName, std::istream& str) : _File(FileName) 
{
  // create the parser
  parser = XMLReaderFactory::createXMLReader();
  //parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, false);
  //parser->setFeature(XMLUni::fgXercesSchema, false);
  //parser->setFeature(XMLUni::fgXercesSchemaFullChecking, false);
  //parser->setFeature(XMLUni::fgXercesIdentityConstraintChecking, false);
  //parser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, false);
  //parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
  //parser->setFeature(XMLUni::fgXercesDynamic, true);

  parser->setContentHandler(this);
  parser->setErrorHandler(this);

  try
  {
    StdInputSource file(str, _File.filePath().c_str());
    _valid = parser->parseFirst( file,token);
  }
  catch (const XMLException& toCatch) {
      char* message = XMLString::transcode(toCatch.getMessage());
      cout << "Exception message is: \n"
           << message << "\n";
      XMLString::release(&message);
  }
  catch (const SAXParseException& toCatch) {
      char* message = XMLString::transcode(toCatch.getMessage());
      cout << "Exception message is: \n"
           << message << "\n";
      XMLString::release(&message);
  }
#ifndef FC_DEBUG
  catch (...) {
      cout << "Unexpected Exception \n";
      Base::Exception("Unknown Exception in Base::XMLReader::XMLReader()");
  }
#endif
}

Base::XMLReader::~XMLReader()
{
  //  Delete the parser itself.  Must be done prior to calling Terminate, below.
  delete parser;
}

const char* Base::XMLReader::localName(void)
{
  return LocalName.c_str();
}

unsigned int Base::XMLReader::getAttributeCount(void) const
{
  return AttrMap.size();
}

long Base::XMLReader::getAttributeAsInteger(const char* AttrName) const
{
  AttrMapType::const_iterator pos = AttrMap.find(AttrName);

  if(pos != AttrMap.end())
    return atol(pos->second.c_str());
  else
    // wrong name, use hasAttribute if not shure!
    assert(0);

  return 0;
}

unsigned long Base::XMLReader::getAttributeAsUnsigned(const char* AttrName) const
{
  AttrMapType::const_iterator pos = AttrMap.find(AttrName);

  if(pos != AttrMap.end())
    return strtoul(pos->second.c_str(),0,10);
  else
    // wrong name, use hasAttribute if not shure!
    assert(0);

  return 0;
}

double Base::XMLReader::getAttributeAsFloat  (const char* AttrName) const
{
  AttrMapType::const_iterator pos = AttrMap.find(AttrName);

  if(pos != AttrMap.end())
    return atof(pos->second.c_str());
  else
    // wrong name, use hasAttribute if not shure!
    assert(0);

  return 0.0;

}

const char*  Base::XMLReader::getAttribute (const char* AttrName) const
{
  AttrMapType::const_iterator pos = AttrMap.find(AttrName);

  if(pos != AttrMap.end())
    return pos->second.c_str();
  else
    // wrong name, use hasAttribute if not shure!
    assert(0);

  return ""; 
}

bool Base::XMLReader::hasAttribute (const char* AttrName) const
{
  return AttrMap.find(AttrName) != AttrMap.end();
}



bool Base::XMLReader::read(void)
{
  ReadType = None;

  try
  {
      parser->parseNext(token);
  }
  catch (const XMLException& toCatch) {
      char* message = XMLString::transcode(toCatch.getMessage());
      cout << "Exception message is: \n"
           << message << "\n";
      XMLString::release(&message);
      return false;
  }
  catch (const SAXParseException& toCatch) {
      char* message = XMLString::transcode(toCatch.getMessage());
      cout << "Exception message is: \n"
           << message << "\n";
      XMLString::release(&message);
      return false;
  }
  catch (...) {
      cout << "Unexpected Exception \n" ;
      return false;
  }

  return true;
}

void Base::XMLReader::readElement(const char* ElementName)
{
  bool ok;
  do {
    ok = read(); if (!ok) break;
  } while( (ReadType != StartElement && ReadType != StartEndElement) || (ElementName && LocalName != ElementName));
}

void Base::XMLReader::readEndElement(const char* ElementName)
{
  bool ok;
  do {
    ok = read(); if (!ok) break;
  } while(ReadType != EndElement || (ElementName && LocalName != ElementName));
}

void Base::XMLReader::readCharacters(void)
{
}


void Base::XMLReader::readFiles(zipios::ZipInputStream &zipstream)
{
  for ( std::vector<FileEntry>::const_iterator it = FileList.begin(); it != FileList.end(); ++it )
  {
    zipios::ConstEntryPointer entry = zipstream.getNextEntry();
    if ( entry->isValid() && entry->getName() == it->FileName )
    {
      it->Object->RestoreDocFile( zipstream );
    }else
      throw Exception("Base::XMLReader::readFiles(): Files in ZIP not in the right order!");
  }
}

const char *Base::XMLReader::addFile(const char* Name, Base::Persistance *Object)
{
  FileEntry temp;
  temp.FileName = Name;
  temp.Object = Object;
  
  FileList.push_back(temp);

  FileNames.push_back( temp.FileName );

  return Name;
}

// ---------------------------------------------------------------------------
//  Base::XMLReader: Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
void Base::XMLReader::startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs)
{
  LocalName = StrX(localname).c_str();

  // saving attributes
  for (unsigned int i = 0; i < attrs.getLength(); i++) {
    AttrMap[StrX(attrs.getQName(i)).c_str()] = StrX(attrs.getValue(i)).c_str();
  }

  ReadType = StartElement;
}

void Base::XMLReader::endElement  (const XMLCh* const uri, const XMLCh *const localname, const XMLCh *const qname)
{
  LocalName = StrX(localname).c_str();

  if(ReadType == StartElement)
    ReadType = StartEndElement;
  else
    ReadType = EndElement;
}


void Base::XMLReader::characters(const   XMLCh* const    chars, const unsigned int    length)
{
  Characters = StrX(chars).c_str();
  ReadType = Chars;
  CharacterCount += length;
}

void Base::XMLReader::ignorableWhitespace( const   XMLCh* const chars, const unsigned int length)
{
    //fSpaceCount += length;
}

void Base::XMLReader::resetDocument()
{
    //fAttrCount = 0;
    //fCharacterCount = 0;
    //fElementCount = 0;
    //fSpaceCount = 0;
}


// ---------------------------------------------------------------------------
//  Base::XMLReader: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
void Base::XMLReader::error(const SAXParseException& e)
{
   cerr << "\nError at file " << StrX(e.getSystemId())
	 << ", line " << e.getLineNumber()
	 << ", char " << e.getColumnNumber()
       << "\n  Message: " << StrX(e.getMessage()) <<  endl;
}

void Base::XMLReader::fatalError(const SAXParseException& e)
{
   cerr << "\nFatal Error at file " << StrX(e.getSystemId())
	 << ", line " << e.getLineNumber()
	 << ", char " << e.getColumnNumber()
       << "\n  Message: " << StrX(e.getMessage()) <<  endl;
}

void Base::XMLReader::warning(const SAXParseException& e)
{
   cerr << "\nWarning at file " << StrX(e.getSystemId())
	 << ", line " << e.getLineNumber()
	 << ", char " << e.getColumnNumber()
       << "\n  Message: " << StrX(e.getMessage()) <<  endl;         
}

void Base::XMLReader::resetErrors()
{
}
