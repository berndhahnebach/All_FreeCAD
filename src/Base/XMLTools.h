/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de)                             *
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


#ifndef __XMLTools_H__
#define __XMLTools_H__

// Std. configurations


// Include files
#include <iostream>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/XMLString.hpp>


XERCES_CPP_NAMESPACE_BEGIN
	class DOMNode;
	class DOMElement;
	class DOMDocument;
XERCES_CPP_NAMESPACE_END

using namespace Base;


//**************************************************************************
//**************************************************************************
// StrX
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class StrX
{
public :
    StrX(const XMLCh* const toTranscode);
    ~StrX();

    /// Getter method
    const char* c_str() const;

private :
    //  This is the local code page form of the string.
    char*   fLocalForm;
};

inline std::ostream& operator<<(std::ostream& target, const StrX& toDump)
{
    target << toDump.c_str();
    return target;
}

inline StrX::StrX(const XMLCh* const toTranscode)
{
    // Call the private transcoding method
    fLocalForm = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toTranscode);
}

inline StrX::~StrX()
{
    //delete [] fLocalForm; // dont work on VC7.1
  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&fLocalForm);
}


// -----------------------------------------------------------------------
//  Getter methods
// -----------------------------------------------------------------------
inline const char* StrX::c_str() const
{
    return fLocalForm;
}


//**************************************************************************
//**************************************************************************
// XStr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class XStr
{
public :
    ///  Constructors and Destructor
    XStr(const char* const toTranscode);
    /// 
    ~XStr();


    ///  Getter method
    const XMLCh* unicodeForm() const;

private :
    /// This is the Unicode XMLCh format of the string.
    XMLCh*   fUnicodeForm;
};


inline XStr::XStr(const char* const toTranscode)
{
    // Call the private transcoding method
    fUnicodeForm = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toTranscode);
}

inline XStr::~XStr()
{
    //delete [] fUnicodeForm;
  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&fUnicodeForm);
}


// -----------------------------------------------------------------------
//  Getter methods
// -----------------------------------------------------------------------
inline const XMLCh* XStr::unicodeForm() const
{
    return fUnicodeForm;
}

#endif 
