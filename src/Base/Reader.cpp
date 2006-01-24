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
#include "Reader.h"
#include "Exception.h"
#include "gzstream.h"
#include "InputSource.h"

#include "XMLTools.h"

using namespace Base;
using namespace std;



// ---------------------------------------------------------------------------
//  Reader: Constructors and Destructor
// ---------------------------------------------------------------------------

Reader::Reader(const char* FileName, std::istream& str) : _File(FileName) 
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
#if 1
    StdInputSource file(str, _File.filePath().c_str());
    _valid = parser->parseFirst( file,token);
#else
    std::string file(FileName);
    _valid = parser->parseFirst( (char*)file.c_str(),token);
#endif
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
      Base::Exception("Unknown Exception in Reader::Reader()");
  }
#endif
}

Reader::~Reader()
{
  //  Delete the parser itself.  Must be done prior to calling Terminate, below.
  delete parser;
}

const char* Reader::localName(void)
{
  return LocalName.c_str();
}

unsigned int Reader::getAttributeCount(void)
{
  return AttrMap.size();
}

long Reader::getAttributeAsInteger(const char* AttrName)
{
  AttrMapType::const_iterator pos = AttrMap.find(AttrName);

  if(pos != AttrMap.end())
    return atol(pos->second.c_str());
  else
    // wrong name, use hasAttribute if not shure!
    assert(0);

  return 0;
}

double Reader::getAttributeAsFloat  (const char* AttrName)
{
  AttrMapType::const_iterator pos = AttrMap.find(AttrName);

  if(pos != AttrMap.end())
    return atof(pos->second.c_str());
  else
    // wrong name, use hasAttribute if not shure!
    assert(0);

  return 0.0;

}

const char*  Reader::getAttribute (const char* AttrName)
{
  AttrMapType::const_iterator pos = AttrMap.find(AttrName);

  if(pos != AttrMap.end())
    return pos->second.c_str();
  else
    // wrong name, use hasAttribute if not shure!
    assert(0);

  return ""; 
}

bool Reader::hasAttribute (const char* AttrName)
{
  return AttrMap.find(AttrName) != AttrMap.end();
}



void Reader::read(void)
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
  }
  catch (const SAXParseException& toCatch) {
      char* message = XMLString::transcode(toCatch.getMessage());
      cout << "Exception message is: \n"
           << message << "\n";
      XMLString::release(&message);
  }
  catch (...) {
      cout << "Unexpected Exception \n" ;
  }

}

void Reader::readElement(const char* ElementName)
{
  read();
  while( (ReadType != StartElement && ReadType != StartEndElement) || (ElementName && LocalName != ElementName))
    read();
}

void Reader::readEndElement(const char* ElementName)
{
  read();
  while(ReadType != EndElement || (ElementName && LocalName != ElementName))
    read();
}

void Reader::readCharacters(void)
{
}



// ---------------------------------------------------------------------------
//  Reader: Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
void Reader::startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs)
{
  LocalName = StrX(localname).c_str();

  // saving attributes
  for (unsigned int i = 0; i < attrs.getLength(); i++) {
    AttrMap[StrX(attrs.getQName(i)).c_str()] = StrX(attrs.getValue(i)).c_str();
  }

  ReadType = StartElement;
}

void Reader::endElement  (const XMLCh* const uri, const XMLCh *const localname, const XMLCh *const qname)
{
  LocalName = StrX(localname).c_str();

  if(ReadType == StartElement)
    ReadType = StartEndElement;
  else
    ReadType = EndElement;
}


void Reader::characters(const   XMLCh* const    chars, const unsigned int    length)
{
  Characters = StrX(chars).c_str();
  ReadType = Chars;
  CharacterCount += length;
}

void Reader::ignorableWhitespace( const   XMLCh* const chars, const unsigned int length)
{
    //fSpaceCount += length;
}

void Reader::resetDocument()
{
    //fAttrCount = 0;
    //fCharacterCount = 0;
    //fElementCount = 0;
    //fSpaceCount = 0;
}


// ---------------------------------------------------------------------------
//  Reader: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
void Reader::error(const SAXParseException& e)
{
   cerr << "\nError at file " << StrX(e.getSystemId())
	 << ", line " << e.getLineNumber()
	 << ", char " << e.getColumnNumber()
       << "\n  Message: " << StrX(e.getMessage()) <<  endl;
}

void Reader::fatalError(const SAXParseException& e)
{
   cerr << "\nFatal Error at file " << StrX(e.getSystemId())
	 << ", line " << e.getLineNumber()
	 << ", char " << e.getColumnNumber()
       << "\n  Message: " << StrX(e.getMessage()) <<  endl;
}

void Reader::warning(const SAXParseException& e)
{
   cerr << "\nWarning at file " << StrX(e.getSystemId())
	 << ", line " << e.getLineNumber()
	 << ", char " << e.getColumnNumber()
       << "\n  Message: " << StrX(e.getMessage()) <<  endl;         
}

void Reader::resetErrors()
{
}
