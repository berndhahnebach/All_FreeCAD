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

// @configure_input@

#ifndef SOANY_H
#define SOANY_H

#include <Inventor/SbPList.h>
#include <Inventor/Qt/SoQt.h>

// ************************************************************************

class SoAny {
public:
  // use the singleton design pattern for access
  static SoAny * si(void);

  const char * getenv(const char * e);

  void * getSharedGLContext(void * display, void * screen);
  void registerGLContext(void * context, void * display, void * screen);
  void unregisterGLContext(void * context);
  int getSharedCacheContextId(void * context);

  SoQt::FatalErrorCB * setFatalErrorHandler(SoQt::FatalErrorCB * cb,
                                               void * userdata);
  SbBool invokeFatalErrorHandler(SbString errmsg, SoQt::FatalErrors errcode);


  typedef void InternalFatalErrorCB(void * userdata);
  void addInternalFatalErrorHandler(SoAny::InternalFatalErrorCB * cb,
                                    void * ud);

private:
  SoAny(void);
  ~SoAny();
  static void cleanup_si(void);

  // (use SbPList instances to be able to compile properly on SGI /
  // TGS Inventor aswell as Coin)
  SbPList cclist;
  SbPList internalfehandlers, internalfedata;

  SoQt::FatalErrorCB * fatalcb;
  void * userdata;
}; // class SoAny

// ************************************************************************

#endif // ! SOANY_H
