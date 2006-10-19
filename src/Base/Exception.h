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
#ifndef BASE_EXCEPTION_H
#define BASE_EXCEPTION_H

#include <exception>
#include <string>
#include "Base.h"

namespace Base
{

class BaseExport Exception : public BaseClass
{
  TYPESYSTEM_HEADER();

public:
  Exception(const char * sMessage);
  Exception(void);
  Exception(const Exception &inst);
  virtual ~Exception() throw() {}

  Exception &operator=(const Exception &inst);

  /** Exception handling
   * This is useful if an exception should be handled directly where it occurs e.g. by showing an error 
   * message and the receiving instance of the forwarded exception should not report a message again.
   */
  //@{
  /// Returns true if this exception was already handled, false otherwise.
  bool isHandled() const
  { return _handled; }
  /// Sets the exception as handled.
  void setHandled()
  { _handled = true; }
  //@}

  virtual const char* what(void) const throw();
  void ReportException (void) const;
  inline void setMessage(const char * sMessage);
  
protected:
#ifdef _MSC_VER
# pragma warning( disable : 4251 )
#endif
  std::string _sErrMsg;
  bool _handled;
};


/**
 * The AbortException is thrown if a pending operation was aborted.
 * @author Werner Mayer
 */
class BaseExport AbortException : public Exception
{
public:
  /// Construction
  AbortException(const char * sMessage);
  /// Construction
  AbortException();
  /// Construction
  AbortException(const AbortException &inst);
  /// Destruction
  virtual ~AbortException() throw() {}
  /// Description of the exception
  virtual const char* what() const throw();
};

/**
 * The MemoryException is thrown if not enough memory can be allocated.
 * @author Werner Mayer
 */
#if defined (__GNUC__)
// It seems that the calling instance of our new handler expects a bad_alloc exception
class BaseExport MemoryException : public Exception, virtual public std::bad_alloc
#else
class BaseExport MemoryException : public Exception
#endif
{
public:
  /// Construction
  MemoryException();
  /// Construction
  MemoryException(const MemoryException &inst);
  /// Destruction
  virtual ~MemoryException() throw() {}
#if defined (__GNUC__)
  /// Description of the exception
  virtual const char* what() const throw();
#endif
};


inline void Exception::setMessage(const char * sMessage)
{
  _sErrMsg = sMessage;
}


} //namespace Base

#endif // BASE_EXCEPTION_H

