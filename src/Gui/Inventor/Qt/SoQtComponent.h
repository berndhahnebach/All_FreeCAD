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

#ifndef SOQT_COMPONENT_H
#define SOQT_COMPONENT_H

#include <Inventor/SbLinear.h>
#include <Inventor/Qt/SoQtObject.h>

#ifdef __COIN_SOQT__
class QWidget;
#endif // __COIN_SOQT__
#ifdef __COIN_SOXT__
#include <X11/Intrinsic.h>
#endif // __COIN_SOXT__
#ifdef __COIN_SOGTK__
#include <gtk/gtk.h>
#endif // __COIN_SOGTK__
#ifdef __COIN_SOWIN__
#include <wtypes.h>
#endif // __COIN_SOWIN__


class SoQtComponent;
class SoQtCursor;

typedef void SoQtComponentCB(void * user, SoQtComponent * component);
typedef void SoQtComponentVisibilityCB(void * user, SbBool visible);

// *************************************************************************

class SOQT_DLL_API SoQtComponent : public SoQtObject {
  SOQT_OBJECT_ABSTRACT_HEADER(SoQtComponent, SoQtObject);

public:
  virtual ~SoQtComponent();

  virtual void show(void);
  virtual void hide(void);

  virtual void setComponentCursor(const SoQtCursor & cursor);
  static void setWidgetCursor(QWidget* w, const SoQtCursor & cursor);

  SbBool isFullScreen(void) const;
  SbBool setFullScreen(const SbBool onoff);

  SbBool isVisible(void);
  SbBool isTopLevelShell(void) const;

  QWidget* getWidget(void) const;
  QWidget* getBaseWidget(void) const;
  QWidget* getShellWidget(void) const;
  QWidget* getParentWidget(void) const;

  void setSize(const SbVec2s size);
  SbVec2s getSize(void) const;

  void setTitle(const char * const title);
  const char * getTitle(void) const;
  void setIconTitle(const char * const title);
  const char * getIconTitle(void) const;

  const char * getWidgetName(void) const;
  const char * getClassName(void) const;

  void setWindowCloseCallback(SoQtComponentCB * const func,
                              void * const user = NULL);
  static SoQtComponent * getComponent(QWidget* widget);

  static void initClasses(void);

protected:
  SoQtComponent(QWidget* const parent = NULL,
                   const char * const name = NULL,
                   const SbBool embed = TRUE);

  virtual void afterRealizeHook(void);

  void setClassName(const char * const name);
  void setBaseWidget(QWidget* widget);

  void registerWidget(QWidget* widget);
  void unregisterWidget(QWidget* widget);

  virtual const char * getDefaultWidgetName(void) const;
  virtual const char * getDefaultTitle(void) const;
  virtual const char * getDefaultIconTitle(void) const;

  virtual void sizeChanged(const SbVec2s & size);

  void addVisibilityChangeCallback(SoQtComponentVisibilityCB * const func,
                                   void * const user = NULL);
  void removeVisibilityChangeCallback(SoQtComponentVisibilityCB * const func,
                                      void * const user = NULL);

private:
  class SoQtComponentP * pimpl;
#ifndef DOXYGEN_SKIP_THIS
  friend class SoGuiComponentP;
  friend class SoQtComponentP;
#endif // DOXYGEN_SKIP_THIS

  // FIXME: get rid of remaining toolkit specifics below. 20020611 mortene.

#ifdef __COIN_SOWIN__
public:
  HWND setFocusProxy(HWND widget);
  HWND getFocusProxy(void);

protected:
  HWND buildFormWidget(HWND parent);
  virtual void windowCloseAction(void);
  void setResize(SbBool set);
  SbBool getResize(void);
#endif // __COIN_SOWIN__

#ifdef __COIN_SOXT__
public:
  Display * getDisplay(void);
  void fitSize(const SbVec2s size);
  // FIXME: I guess these should really be part of the common
  // API. 20011012 mortene.
  void addWindowCloseCallback(SoXtComponentCB * callback, void * closure = NULL);
  void removeWindowCloseCallback(SoXtComponentCB * callback, void * closure = NULL);

protected:
  // FIXME: I guess this should perhaps be part of the common API?
  // 20011012 mortene.
  virtual void windowCloseAction(void);
  void invokeVisibilityChangeCallbacks(const SbBool enable) const;
  void invokeWindowCloseCallbacks(void) const;

  // FIXME: get rid of these two? 20011012 mortene.
  static char * getlabel(unsigned int what);
  SbBool firstRealize;

  // FIXME: should this really be protected? Same goes for the other
  // toolkits' eventHandler() functions. They also have nameclashes
  // with the static So*GLWidget::eventHandler() function. 20011024 mortene.
  virtual Boolean sysEventHandler(Widget widget, XEvent * event);

private:
  // FIXME: get rid of this? 20011012 mortene.
  static void event_handler(Widget, XtPointer, XEvent *, Boolean *);
#endif // __COIN_SOXT__

#ifdef __COIN_SOGTK__
protected:
  virtual SbBool eventFilter(GtkWidget * object, GdkEvent * event);
private:
  static gint eventHandler(GtkWidget * object, GdkEvent * event, gpointer closure);
#endif // __COIN_SOGTK__
};

// *************************************************************************

#endif // ! SOQT_COMPONENT_H
