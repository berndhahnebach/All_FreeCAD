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

/*!
  \class SoQtKeyboard SoQtKeyboard.h Inventor/Qt/devices/SoQtKeyboard.h
  \brief The SoQtKeyboard class is the keyboard input device abstraction.
  \ingroup devices

  The SoQtKeyboard class is the glue between native keyboard
  handling and keyboard interaction with the Inventor scenegraph.

  All components derived from the SoQtRenderArea have got an
  SoQtKeyboard device attached by default.
*/

// *************************************************************************

// includes the FreeCAD configuration
#include "../../../Config.h"

#include <Inventor/Qt/devices/SoQtKeyboard.h>
#include <Inventor/Qt/devices/SoGuiKeyboardP.h>
#include <Inventor/events/SoKeyboardEvent.h>

// *************************************************************************

SOQT_OBJECT_SOURCE(SoQtKeyboard);

// *************************************************************************

/*!
  \enum SoQtKeyboard::Events
  Enumeration over supported event types.
*/

/*!
  \var SoQtKeyboard::Events SoQtKeyboard::KEY_PRESS
  Maskbit for a keyboard button press event.
*/

/*!
  \var SoQtKeyboard::Events SoQtKeyboard::KEY_RELEASE
  Maskbit for a keyboard button release event.
*/

/*!
  \var SoQtKeyboard::Events SoQtKeyboard::ALL_EVENTS
  Combined bitmask for all possible events.
*/

/*!
  \fn SoQtKeyboard::SoQtKeyboard(int mask)

  Constructor. The \a mask specifies which keyboard-related events to
  handle. Others will just be ignored.
*/

/*!
  \fn SoQtKeyboard::~SoQtKeyboard()

  Destructor.
*/

// *************************************************************************

#ifndef DOXYGEN_SKIP_THIS

SoGuiKeyboardP::SoGuiKeyboardP(void)
{
  this->kbdevent = new SoKeyboardEvent;
}

SoGuiKeyboardP::~SoGuiKeyboardP()
{
  delete this->kbdevent;
}

#endif // !DOXYGEN_SKIP_THIS

// *************************************************************************
