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

#ifndef SOQT_MOUSE_H
#define SOQT_MOUSE_H

#include <Inventor/Qt/devices/SoQtDevice.h>

class SoMouseButtonEvent;
class SoLocation2Event;

// *************************************************************************

class SOQT_DLL_API SoQtMouse : public SoQtDevice {
  SOQT_OBJECT_HEADER(SoQtMouse, SoQtDevice);

public:
  enum Events {
    BUTTON_PRESS    = 0x01,
    BUTTON_RELEASE  = 0x02,
    POINTER_MOTION  = 0x04,
    BUTTON_MOTION   = 0x08,

    ALL_EVENTS = BUTTON_PRESS | BUTTON_RELEASE | POINTER_MOTION | BUTTON_MOTION
  };

  SoQtMouse(int eventmask = ALL_EVENTS);
  virtual ~SoQtMouse(void);

  virtual void enable(QWidget* widget, SoQtEventHandler * handler, void * closure);
  virtual void disable(QWidget* widget, SoQtEventHandler * handler, void * closure);

  virtual const SoEvent * translateEvent(QEvent* event);

private:
  class SoQtMouseP * pimpl;
#ifndef DOXYGEN_SKIP_THIS
  friend class SoQtMouseP;
  friend class SoGuiMouseP;
#endif // DOXYGEN_SKIP_THIS
};

// *************************************************************************

#endif // ! SOQT_MOUSE_H
