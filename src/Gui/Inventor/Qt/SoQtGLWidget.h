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

#ifndef SOQT_GLWIDGET_H
#define SOQT_GLWIDGET_H

#include <Inventor/SbBasic.h>
#include <Inventor/Qt/SoQtBasic.h>
#include <Inventor/Qt/SoQtComponent.h>

#ifdef __COIN_SOQT__
class QEvent;
#endif // __COIN_SOQT__
#ifdef __COIN_SOXT__
#include <GL/glx.h>
#endif // __COIN_SOXT__
#ifdef __COIN_SOGTK__
#include <gtk/gtk.h>
#endif // __COIN_SOGTK__
#ifdef __COIN_SOWIN__
#include <windows.h>
#endif // __COIN_SOWIN__

// *************************************************************************

enum GLModes {
  SO_GL_RGB      = 0x01, SO_GLX_RGB      = SO_GL_RGB,
  SO_GL_DOUBLE   = 0x02, SO_GLX_DOUBLE   = SO_GL_DOUBLE,
  SO_GL_ZBUFFER  = 0x04, SO_GLX_ZBUFFER  = SO_GL_ZBUFFER,
  SO_GL_OVERLAY  = 0x08, SO_GLX_OVERLAY  = SO_GL_OVERLAY,
  SO_GL_STEREO   = 0x10, SO_GLX_STEREO   = SO_GL_STEREO
};

// *************************************************************************

class SOQT_DLL_API SoQtGLWidget : public SoQtComponent {
  SOQT_OBJECT_ABSTRACT_HEADER(SoQtGLWidget, SoQtComponent);

public:

  void setBorder(const SbBool enable);
  SbBool isBorder(void) const;

  virtual void setDoubleBuffer(const SbBool enable);
  SbBool isDoubleBuffer(void) const;

  void setDrawToFrontBufferEnable(const SbBool enable);
  SbBool isDrawToFrontBufferEnable(void) const;
  
  void setQuadBufferStereo(const SbBool enable);
  SbBool isQuadBufferStereo(void) const;

  void setAccumulationBuffer(const SbBool enable);
  SbBool getAccumulationBuffer(void) const;

  void setStencilBuffer(const SbBool enable);
  SbBool getStencilBuffer(void) const;

  void setOverlayRender(const SbBool onoff);
  SbBool isOverlayRender(void) const;

  QWidget* getGLWidget(void) const;
  QWidget* getNormalWidget(void) const;
  QWidget* getOverlayWidget(void) const;

  SbBool hasOverlayGLArea(void) const;
  SbBool hasNormalGLArea(void) const;

  unsigned long getOverlayTransparentPixel(void);

  // OpenGL query functions.
  void getPointSizeLimits(SbVec2f & range, float & granularity);
  void getLineWidthLimits(SbVec2f & range, float & granularity);

protected:
  SoQtGLWidget(QWidget* const parent = NULL,
                  const char * const name = NULL,
                  const SbBool embed = TRUE,
                  const int glmodes = SO_GL_RGB,
                  const SbBool build = TRUE);
  ~SoQtGLWidget();

  virtual void processEvent(QEvent* event);

  QWidget* buildWidget(QWidget* parent);

  virtual void redraw(void) = 0;
  virtual void redrawOverlay(void);

  virtual void initGraphic(void);
  virtual void initOverlayGraphic(void);

  virtual void sizeChanged(const SbVec2s & size);
  virtual void widgetChanged(QWidget* w);

  void setGLSize(const SbVec2s size);
  SbVec2s getGLSize(void) const;
  float getGLAspectRatio(void) const;

  // old aliases
  void setGlxSize(const SbVec2s size) { this->setGLSize(size); }
  SbVec2s getGlxSize(void) const { return this->getGLSize(); }
  float getGlxAspectRatio(void) const { return this->getGLAspectRatio(); }

  void setStereoBuffer(SbBool flag);
  SbBool isStereoBuffer(void) const;

  SbBool isRGBMode(void);

  SbBool waitForExpose;
  SbBool drawToFrontBuffer;

  void glLockNormal(void);
  void glUnlockNormal(void);

  void glLockOverlay(void);
  void glUnlockOverlay(void);

  void glSwapBuffers(void);
  void glFlushBuffer(void);

  virtual SbBool glScheduleRedraw(void);

private:
  class SoQtGLWidgetP * pimpl;
#ifndef DOXYGEN_SKIP_THIS
  friend class SoGuiGLWidgetP;
  friend class SoQtGLWidgetP;
#endif // DOXYGEN_SKIP_THIS

  // FIXME: get rid of toolkit-specific stuff below. 20020613 mortene.

#ifdef __COIN_SOXT__
public:
  Window getNormalWindow(void);
  Window getOverlayWindow(void);

  int getOverlayColorMapSize(void);
  int getColorMapSize(void);

  virtual void setNormalVisual(XVisualInfo * visual);
  XVisualInfo * getNormalVisual(void);
  virtual void setOverlayVisual(XVisualInfo * visual);
  XVisualInfo * getOverlayVisual(void);

protected:
  int getDisplayListShareGroup(GLXContext context);
  Widget getGlxMgrWidget(void);
#endif // __COIN_SOXT__
#ifdef __COIN_SOWIN__
public:
  HWND getNormalWindow(void) const;
  HWND getOverlayWindow(void) const;
  HDC getNormalDC(void) const;
  HDC getOverlayDC(void) const;

  void setStealFocus(SbBool doStealFocus);

  virtual void setNormalVisual(PIXELFORMATDESCRIPTOR * vis);
  PIXELFORMATDESCRIPTOR * getNormalVisual(void);
  virtual void setOverlayVisual(PIXELFORMATDESCRIPTOR * vis);
  PIXELFORMATDESCRIPTOR * getOverlayVisual(void);
  virtual void setPixelFormat(int format);
  int getPixelFormat(void);

protected:
  static LRESULT CALLBACK glWidgetProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
  static LRESULT CALLBACK mgrWidgetProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
  int getDisplayListShareGroup(HGLRC ctx);
  HWND getManagerWidget(void);
  HWND parent;
  HWND toplevel;
#endif // __COIN_SOWIN__

#ifdef __COIN_SOGTK__
protected:
  virtual SbBool eventFilter(GtkWidget * object, GdkEvent * event);
#endif // __COIN_SOGTK__
};

// *************************************************************************

#endif // ! SOQT_GLWIDGET_H
