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

#ifndef SOGUIEXAMINERVIEWERP_H
#define SOGUIEXAMINERVIEWERP_H

#include <Inventor/SbLinear.h>
#include <Inventor/SbTime.h>

class SoQtExaminerViewer;

// ************************************************************************

// This class contains private data and methods used within the
// SoGuiExaminerViewer class.

class SoGuiExaminerViewerP
{
public:
  ~SoGuiExaminerViewerP();

  void genericConstructor(void);
  void genericDestructor(void);

  void setMotion3OnCamera(SbBool enable);
  SbBool getMotion3OnCamera(void) const;

  float rotXWheelMotion(float value, float old);
  float rotYWheelMotion(float value, float old);

  void reorientCamera(const SbRotation & rotation);
  void spin(const SbVec2f & mousepos);
  void pan(const SbVec2f & mousepos, const SbVec2f & prevpos);
  void zoom(const float diffvalue);
  void zoomByCursor(const SbVec2f & mousepos, const SbVec2f & prevpos);

  SbVec2f lastmouseposition;
  SbPlane panningplane;

  SbBool spinanimatingallowed;
  SbVec2f lastspinposition;
  int spinsamplecounter;
  SbRotation spinincrement;
  class SbSphereSheetProjector * spinprojector;

  SbRotation spinRotation;

  SbBool axiscrossEnabled;
  int axiscrossSize;

  void drawAxisCross(void);
  static void drawArrow(void);

  struct { // tracking mouse movement in a log
    short size;
    short historysize;
    SbVec2s * position;
    SbTime * time;
  } log;

  // The Microsoft Visual C++ v6.0 compiler needs a name on this class
  // to be able to generate a constructor (which it wants to have for
  // running the the SbVec2s constructors). So don't try to be clever
  // and make it anonymous.
  struct Pointer {
    SbVec2s now, then;
  } pointer;

  SbBool button1down;
  SbBool button3down;
  SbBool ctrldown, shiftdown;

  void clearLog(void);
  void addToLog(const SbVec2s pos, const SbTime time);

  SbTime prevRedrawTime;

  SbBool motion3OnCamera;

  enum ViewerMode {
    IDLE,
    INTERACT,
    ZOOMING,
    PANNING,
    DRAGGING,
    SPINNING,
    SEEK_WAIT_MODE,
    SEEK_MODE
  } mode;

  ViewerMode currentmode;
  void setMode(const ViewerMode mode);

  void setCursorRepresentation(int mode);

protected:
  SoGuiExaminerViewerP(SoQtExaminerViewer * publ);
  SoQtExaminerViewer * pub;
};

// ************************************************************************

#define PUBLIC(ptr) (ptr->pub)
#define PRIVATE(ptr) (ptr->pimpl)

// ************************************************************************

#endif // ! SOGUIEXAMINERVIEWERP_H
