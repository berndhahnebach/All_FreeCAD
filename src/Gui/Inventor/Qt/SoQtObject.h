/**************************************************************************\
 *
 *  This file is part of the Coin GUI toolkit libraries.
 *  Copyright (C) 1998-2002 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation.  See the
 *  file LICENSE.LGPL at the root directory of this source distribution
 *  for more details.
 *
 *  If you want to use this library with software that is incompatible
 *  licensewise with the LGPL, and / or you would like to take
 *  advantage of the additional benefits with regard to our support
 *  services, please contact Systems in Motion about acquiring a Coin
 *  Professional Edition License.  See <URL:http://www.coin3d.org> for
 *  more information.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  <URL:http://www.sim.no>, <mailto:support@sim.no>
 *
\**************************************************************************/

#ifndef SOQTOBJECT_H
#define SOQTOBJECT_H

#include <assert.h>

#include <Inventor/SbBasic.h>
#include <Inventor/SbString.h>
#include <Inventor/SoType.h>

#include <Inventor/Qt/SoQtBasic.h>

// *************************************************************************

class SOQT_DLL_API SoQtObject {
  static SoType classTypeId;

public:
  static void initClass(void);
  static SoType getClassTypeId(void);
  virtual SoType getTypeId(void) const = 0;
  SbBool isOfType(SoType type) const;

  static void init(void);

}; // SoQtObject

// *************************************************************************

// The getTypeId() method should be abstract for abstract objects, but doing
// that would cause custom components derived from abstract components to
// have to include the typed object header / source, which could be a
// problem if the custom component wasn't written for Coin in the first
// place.

#define SOQT_OBJECT_ABSTRACT_HEADER(classname, parentname) \
public: \
  static void initClass(void); \
  static SoType getClassTypeId(void); \
  virtual SoType getTypeId(void) const /* = 0 (see comment above) */; \
private: \
  typedef parentname inherited; \
  static SoType classTypeId

#define SOQT_OBJECT_HEADER(classname, parentname) \
public: \
  static void initClass(void); \
  static SoType getClassTypeId(void); \
  virtual SoType getTypeId(void) const; \
  static void * createInstance(void); \
private: \
  typedef parentname inherited; \
  static SoType classTypeId

#define SOQT_OBJECT_ABSTRACT_SOURCE(classname) \
void classname::initClass(void) { \
  assert(classname::classTypeId == SoType::badType()); \
  classname::classTypeId = \
    SoType::createType(inherited::getClassTypeId(), \
                        SO__QUOTE(classname)); \
} \
SoType classname::getClassTypeId(void) { \
  return classname::classTypeId; \
} \
SoType classname::getTypeId(void) const { \
  return classname::classTypeId; \
} \
SoType classname::classTypeId

#define SOQT_OBJECT_SOURCE(classname) \
void classname::initClass(void) { \
  assert(classname::classTypeId == SoType::badType()); \
  classname::classTypeId = \
    SoType::createType(inherited::getClassTypeId(), \
                        SO__QUOTE(classname), \
                        classname::createInstance); \
} \
SoType classname::getClassTypeId(void) { \
  return classname::classTypeId; \
} \
SoType classname::getTypeId(void) const { \
  return classname::classTypeId; \
} \
void * classname::createInstance(void) { \
  assert(classname::classTypeId != SoType::badType()); \
  return (void *) new classname; \
} \
SoType classname::classTypeId

// *************************************************************************

#endif // ! SOQTOBJECT_H
