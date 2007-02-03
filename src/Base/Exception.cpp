/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
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


#include "PreCompiled.h"


#include "Exception.h"
#include "Console.h"

using namespace Base;


TYPESYSTEM_SOURCE(Base::Exception,Base::BaseClass);


Exception::Exception(void) : _handled(false)
{
  _sErrMsg = "FreeCAD Exception";
}

Exception::Exception(const Exception &inst)
 : _sErrMsg(inst._sErrMsg), _handled(inst._handled)
{
}


Exception::Exception(const char * sMessage)
 : _sErrMsg(sMessage), _handled(false)
{
}

Exception::Exception(const std::string& sMessage)
 : _sErrMsg(sMessage), _handled(false)
{
}

Exception &Exception::operator=(const Exception &inst)
{
  _sErrMsg = inst._sErrMsg;
  _handled = inst._handled;
  return *this;
}

const char* Exception::what(void) const throw()
{
	return _sErrMsg.c_str();
}

void Exception::ReportException (void) const
{
	Console().Error("\nException (%s): %s \n",Console().Time(),what());
}

// ---------------------------------------------------------

AbortException::AbortException(const char * sMessage)
  : Exception( sMessage )
{
}

AbortException::AbortException()
{
  _sErrMsg = "Aborted operation";
}

AbortException::AbortException(const AbortException &inst)
 : Exception(inst)
{
}

const char* AbortException::what() const throw()
{
  return Exception::what();
}

// ---------------------------------------------------------

FileException::FileException(const char * sMessage, const char * sFileName)
  : Exception( sMessage ),file(sFileName)
{
}

FileException::FileException(const char * sMessage, const FileInfo& File)
  : Exception( sMessage ),file(File)
{
}

FileException::FileException()
  : Exception( "Unknown file exeption happened" )
{
}

FileException::FileException(const FileException &inst)
  : Exception( inst._sErrMsg.c_str() ),file(inst.file)
{
}


const char* FileException::what() const throw()
{
  return Exception::what();
}

// ---------------------------------------------------------

MemoryException::MemoryException()
{
  _sErrMsg = "Not enough memory available";
}

MemoryException::MemoryException(const MemoryException &inst)
 : Exception(inst)
{
}

#if defined (__GNUC__)
const char* MemoryException::what() const throw()
{
  // call what() of Exception, not of std::bad_alloc
  return Exception::what();
}
#endif
