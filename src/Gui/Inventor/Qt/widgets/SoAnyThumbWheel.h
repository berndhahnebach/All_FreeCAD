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

#ifndef SOANY_THUMBWHEEL_H
#define SOANY_THUMBWHEEL_H

// ************************************************************************

class SoAnyThumbWheel {
public:
  enum State              { DISABLED, ENABLED };
  enum Alignment          { VERTICAL, HORIZONTAL };
  enum BoundaryHandling   { MODULATE, ACCUMULATE, CLAMP };
  enum Movement           { UNIFORM, AUTHENTIC };
  enum GraphicsByteOrder  { ABGR, RGBA, ARGB, BGRA };

  SoAnyThumbWheel(void);
  ~SoAnyThumbWheel(void);

  void setSize(const int diameter, const int width);
  void getSize(int & diameter, int & width) const;

  void setColor(const float red, const float green, const float blue);
  void getColor(float & red, float & green, float & blue) const;
  void setColorFactors(const float light, const float front, const float normal, const float shade);
  void getColorFactors(float & light, float & front, float & normal, float & shade) const;

  int getNumBitmaps(void) const;
  void drawBitmap(const int number, void * bitmap, Alignment alignment) const;
  float calculateValue(const float origValue, const int origPosition, const int deltaPosition) const;
  int getBitmapForValue(const float value, const State state) const;

  void setGraphicsByteOrder(const GraphicsByteOrder byteorder);
  GraphicsByteOrder getGraphicsByteOrder(void) const;

  void setMovement(const Movement movement);
  Movement getMovement(void) const;

  void setBoundaryHandling(const BoundaryHandling handling);
  BoundaryHandling getBoundaryHandling(void) const;

private:
  unsigned int swapWord(unsigned int) const;

  int diameter, width;
  // float disabledred, disabledgreen, disabledblue; // not implemented
  float red, green, blue;
  float light, front, normal, shade;

  GraphicsByteOrder  byteorder;
  BoundaryHandling   boundaryhandling;
  Movement           movement;

  enum Tables { SIN, COS, RAD, NUMTABLES };

  mutable float * tables [ NUMTABLES ];
  mutable int dirtyTables;
  mutable int dirtyVariables;
  mutable float squarelength, squarespacing, shadelength, unistep, numsquares;

  void drawDisabledWheel(const int number, void * bitmap, Alignment alignment) const;
  void drawEnabledWheel(const int number, void * bitmap, Alignment alignment) const;

  void validate(void) const;

}; // class SoAnyThumbWheel

// ************************************************************************

#endif // ! SOANY_THUMBWHEEL_H
