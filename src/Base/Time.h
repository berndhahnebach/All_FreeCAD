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


#ifndef __Time_H__
#define __Time_H__

// Std. configurations


#ifndef _PreComp_
# include <stdio.h>
# include <sys/timeb.h>
# include <time.h>
#endif

# include <sys/timeb.h>
# include <time.h>

namespace Base
{
  //typedef struct _object PyObject;


/// BaseClass class and root of the type system
class BaseExport Time
{

public:
  /// Construction
  Time();
  /// Destruction
  virtual ~Time();

  /// sets the object to the actual system time
  void setToActual(void);

  void operator =  (const Time &time); 
  bool operator == (const Time &time) const;
  bool operator != (const Time &time) const;

  bool operator <  (const Time &time) const;
  bool operator <= (const Time &time) const;
  bool operator >= (const Time &time) const;
  bool operator >  (const Time &time) const;

protected:
#if defined (_MSC_VER)
  struct _timeb timebuffer;
#elif defined(__GNUC__)
  struct timeb timebuffer;
#endif
};


inline bool
Time::operator != (const Time &time) const
{
  return (timebuffer.time != time.timebuffer.time || timebuffer.millitm != time.timebuffer.millitm);
}

inline void
Time::operator = (const Time &time) 
{
  timebuffer = time.timebuffer;
}

inline bool
Time::operator == (const Time &time) const
{
  return (timebuffer.time == time.timebuffer.time && timebuffer.millitm == time.timebuffer.millitm);
}

inline bool
Time::operator <  (const Time &time) const
{
  return (timebuffer.time < time.timebuffer.time && timebuffer.millitm < time.timebuffer.millitm);
}

inline bool
Time::operator <= (const Time &time) const
{
  return (timebuffer.time <= time.timebuffer.time && timebuffer.millitm <= time.timebuffer.millitm);
}

inline bool
Time::operator >= (const Time &time) const
{
  return (timebuffer.time >= time.timebuffer.time && timebuffer.millitm >= time.timebuffer.millitm);
}

inline bool
Time::operator >  (const Time &time) const
{
  return (timebuffer.time > time.timebuffer.time && timebuffer.millitm > time.timebuffer.millitm);
}



} //namespace Base

#endif // __Time_H__

