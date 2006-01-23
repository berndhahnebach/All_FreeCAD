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

#ifndef __InputSource_H__
#define __InputSource_H__


#ifndef _PreComp_
# include <xercesc/sax2/Attributes.hpp>
# include <string>
# include <map>
#endif

#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/InputSource.hpp>

XERCES_CPP_NAMESPACE_USE

namespace Base
{


class BaseExport StdInputStream : public BinInputStream
{
public :
    StdInputStream 
      (
        std::istream& Stream
        , MemoryManager* const manager = XMLPlatformUtils::fgMemoryManager
      );

    virtual ~StdInputStream();


    // -----------------------------------------------------------------------
    //  Implementation of the input stream interface
    // -----------------------------------------------------------------------
    virtual unsigned int curPos() const;

    virtual unsigned int readBytes
    (
                XMLByte* const      toFill
        , const unsigned int        maxToRead
    );


private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    StdInputStream(const StdInputStream&);
    StdInputStream& operator=(const StdInputStream&);   

    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSource
    //      The source file that we represent. The FileHandle type is defined
    //      per platform.
    // -----------------------------------------------------------------------
    std::istream            &stream;
    MemoryManager* const    fMemoryManager;
};



class BaseExport StdInputSource : public InputSource
{
public :
    StdInputSource
      (
        std::istream& Stream
        , MemoryManager* const manager = XMLPlatformUtils::fgMemoryManager
      );

     ~StdInputSource();
 
     virtual BinInputStream* makeStream() const;

private:
    StdInputSource(const StdInputSource&);
    StdInputSource& operator=(const StdInputSource&);

    std::istream            &stream;

};

}

#endif
