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

#ifndef SOQT_INPUTFOCUS_H
#define SOQT_INPUTFOCUS_H

#include <Inventor/Qt/devices/SoQtDevice.h>

// *************************************************************************

class SOQT_DLL_API SoQtInputFocus : public SoQtDevice {
  SOQT_OBJECT_HEADER(SoQtInputFocus, SoQtDevice);

public:
  enum Events {
    ENTER_WINDOW = 1 << 0,
    LEAVE_WINDOW = 1 << 1,
    ALL_EVENTS   = ENTER_WINDOW | LEAVE_WINDOW
  };

  SoQtInputFocus(int mask = ALL_EVENTS);
  virtual ~SoQtInputFocus();

  virtual void enable(QWidget* widget, SoQtEventHandler * handler, void * closure);
  virtual void disable(QWidget* widget, SoQtEventHandler * handler, void * closure);

  virtual const SoEvent * translateEvent(QEvent* event);

private:
  class SoQtInputFocusP * pimpl;
#ifndef DOXYGEN_SKIP_THIS
  friend class SoGuiInputFocusP;
  friend class SoQtInputFocusP;
#endif // DOXYGEN_SKIP_THIS
};

#define SO_QT_ALL_FOCUS_EVENTS SoQtInputFocus::ALL_EVENTS;

// *************************************************************************

#endif // ! SOQT_INPUTFOCUS_H
