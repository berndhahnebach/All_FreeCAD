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

#ifndef SO@GUI@_INPUTFOCUS_H
#define SO@GUI@_INPUTFOCUS_H

#include <Inventor/@Gui@/devices/So@Gui@Device.h>

// *************************************************************************

class SO@GUI@_DLL_API So@Gui@InputFocus : public So@Gui@Device {
  SO@GUI@_OBJECT_HEADER(So@Gui@InputFocus, So@Gui@Device);

public:
  enum Events {
    ENTER_WINDOW = 1 << 0,
    LEAVE_WINDOW = 1 << 1,
    ALL_EVENTS   = ENTER_WINDOW | LEAVE_WINDOW
  };

  So@Gui@InputFocus(int mask = ALL_EVENTS);
  virtual ~So@Gui@InputFocus();

  virtual void enable(@WIDGET@ widget, So@Gui@EventHandler * handler, void * closure);
  virtual void disable(@WIDGET@ widget, So@Gui@EventHandler * handler, void * closure);

  virtual const SoEvent * translateEvent(@EVENT@ event);

private:
  class So@Gui@InputFocusP * pimpl;
#ifndef DOXYGEN_SKIP_THIS
  friend class SoGuiInputFocusP;
  friend class So@Gui@InputFocusP;
#endif // DOXYGEN_SKIP_THIS
};

#define SO_@GUI@_ALL_FOCUS_EVENTS So@Gui@InputFocus::ALL_EVENTS;

// *************************************************************************

#endif // ! SO@GUI@_INPUTFOCUS_H
