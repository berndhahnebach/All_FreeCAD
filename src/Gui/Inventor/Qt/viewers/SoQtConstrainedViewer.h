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

#ifndef SOQT_CONSTRAINEDVIEWER_H
#define SOQT_CONSTRAINEDVIEWER_H

#include <Inventor/Qt/viewers/SoQtFullViewer.h>

// ************************************************************************

class SOQT_DLL_API SoQtConstrainedViewer : public SoQtFullViewer {
  SOQT_OBJECT_ABSTRACT_HEADER(SoQtConstrainedViewer, SoQtFullViewer);

public:
  void setUpDirection(const SbVec3f & dir);
  const SbVec3f & getUpDirection(void) const;

  virtual void setCamera(SoCamera * newcam);
  virtual void saveHomePosition(void);
  virtual void resetToHomePosition(void);

protected:
  SoQtConstrainedViewer(QWidget* parent,
                           const char * const name,
                           SbBool embed,
                           SoQtFullViewer::BuildFlag flag, 
                           SoQtViewer::Type type, 
                           SbBool build);

  ~SoQtConstrainedViewer();

  virtual void tiltCamera(float delta);
  virtual void bottomWheelMotion(float value);
  virtual void leftWheelMotion(float value);

  virtual void changeCameraValues(SoCamera * camera);
  void findUpDirection(SbVec2s mouselocation);
  void checkForCameraUpConstrain(void);

private:
  class SoQtConstrainedViewerP * pimpl;
};

// ************************************************************************

#endif // ! SOQT_CONSTRAINEDVIEWER_H
