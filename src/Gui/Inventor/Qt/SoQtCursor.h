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

#ifndef SOQT_CURSOR_H
#define SOQT_CURSOR_H

#include <Inventor/SbLinear.h>
#include <Inventor/Qt/SoQtBasic.h>

class SOQT_DLL_API SoQtCursor {
public:
  struct CustomCursor {
    SbVec2s dim;
    SbVec2s hotspot;
    unsigned char * bitmap;
    unsigned char * mask;
  };


  // FIXME: add more default shapes. 20011119 pederb.
  enum Shape {
    CUSTOM_BITMAP = -1,
    DEFAULT = 0,
    BUSY,
    CROSSHAIR,
    UPARROW
  };
  
  SoQtCursor(void);
  SoQtCursor(const Shape shape);
  SoQtCursor(const CustomCursor * cc);
  ~SoQtCursor();

  Shape getShape(void) const;
  void setShape(const Shape shape);

  const CustomCursor & getCustomCursor(void) const;

  static const SoQtCursor & getZoomCursor(void);
  static const SoQtCursor & getPanCursor(void);
  static const SoQtCursor & getRotateCursor(void);
  static const SoQtCursor & getBlankCursor(void);
  
private:
  void commonConstructor(const Shape shape, const CustomCursor * cc);

  Shape shape;
  CustomCursor * cc;
};

#endif // ! SOQT_CURSOR_H
