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

#ifndef SOQTPLANEVIEWER_H
#define SOQTPLANEVIEWER_H

#include <Inventor/Qt/viewers/SoQtFullViewer.h>

// ************************************************************************

class SOQT_DLL_API SoQtPlaneViewer : public SoQtFullViewer {
  SOQT_OBJECT_HEADER(SoQtPlaneViewer, SoQtFullViewer);

public:
  SoQtPlaneViewer(QWidget* parent = NULL,
                     const char * const name = NULL, 
                     SbBool embed = TRUE, 
                     SoQtFullViewer::BuildFlag flag = BUILD_ALL, 
                     SoQtViewer::Type type = BROWSER);
  ~SoQtPlaneViewer();

  virtual void setViewing(SbBool enable);
  virtual void setCamera(SoCamera * camera);
  virtual void setCursorEnabled(SbBool enable);

protected:
  SoQtPlaneViewer(QWidget* parent,
                     const char * const name, 
                     SbBool embed, 
                     SoQtFullViewer::BuildFlag flag, 
                     SoQtViewer::Type type, 
                     SbBool build);

  QWidget* buildWidget(QWidget* parent);

  virtual const char * getDefaultWidgetName(void) const;
  virtual const char * getDefaultTitle(void) const;
  virtual const char * getDefaultIconTitle(void) const;

  virtual SbBool processSoEvent(const SoEvent * const event);
  virtual void setSeekMode(SbBool enable);
  virtual void actualRedraw(void);

  virtual void bottomWheelStart(void);
  virtual void bottomWheelMotion(float value);
  virtual void bottomWheelFinish(void);

  virtual void leftWheelStart(void);
  virtual void leftWheelMotion(float value);
  virtual void leftWheelFinish(void);

  virtual void rightWheelStart(void);
  virtual void rightWheelMotion(float value);
  virtual void rightWheelFinish(void);

  virtual void createViewerButtons(QWidget* parent, SbPList * buttons);

  virtual void afterRealizeHook(void);

private:
  class SoQtPlaneViewerP * pimpl;
#ifndef DOXYGEN_SKIP_THIS
  friend class SoGuiPlaneViewerP;
  friend class SoQtPlaneViewerP;
#endif // DOXYGEN_SKIP_THIS

//// FIXME!: merge the remaining So*PlaneViewer definitions. 20020111 mortene.

#ifdef __COIN_SOWIN__
protected:
  virtual LRESULT onCommand(HWND window,UINT message,
                            WPARAM wparam, LPARAM lparam);
  virtual void buildViewerButtonsEx(HWND parent, int x, int y, int size);  
#endif // !__COIN_SOWIN__
};

#endif // !SOQTPLANEVIEWER_H
