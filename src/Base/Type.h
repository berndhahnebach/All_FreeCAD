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


#ifndef __Type_H__
#define __Type_H__

// Std. configurations

#include "PyExport.h"

#ifndef _PreComp_
# include <string>
# include <map>
# include <vector>
#endif

namespace Base
{

struct TypeData;


/// Type system class
class BaseExport Type
{
public:
  /// Construction
  Type(const Type& type);
  Type(void);
  /// Destruction
  virtual ~Type();

  static Type fromName(const char *name);
  const char *getName(void) const;
  const Type getParent(void) const;
  bool isDerivedFrom(const Type type) const;

  static int getAllDerivedFrom(const Type type, std::vector<Type>& List);

  static int getNumTypes(void);

  static const Type createType(const Type parent, const char *name);

  unsigned int getKey(void) const;
  bool isBad(void) const;

  void operator =  (const Type type); 
  bool operator == (const Type type) const;
  bool operator != (const Type type) const;

  bool operator <  (const Type type) const;
  bool operator <= (const Type type) const;
  bool operator >= (const Type type) const;
  bool operator >  (const Type type) const;

  static Type badType(void);
  static void init(void);

 
private:


  unsigned int index;


  static std::map<std::string,unsigned int> typemap;
  static std::vector<TypeData*>     typedata;


};


inline unsigned int
Type::getKey(void) const
{
  return this->index;
}

inline bool
Type::operator != (const Type type) const
{
  return (this->getKey() != type.getKey());
}

inline void
Type::operator = (const Type type) 
{
  this->index = type.getKey();
}

inline bool
Type::operator == (const Type type) const
{
  return (this->getKey() == type.getKey());
}

inline bool
Type::operator <  (const Type type) const
{
  return (this->getKey() < type.getKey());
}

inline bool
Type::operator <= (const Type type) const
{
  return (this->getKey() <= type.getKey());
}

inline bool
Type::operator >= (const Type type) const
{
  return (this->getKey() >= type.getKey());
}

inline bool
Type::operator >  (const Type type) const
{
  return (this->getKey() > type.getKey());
}

inline bool
Type::isBad(void) const
{
  return (this->index == 0);
}




} //namespace Base

#endif // __FILETEMPLATE_H__

