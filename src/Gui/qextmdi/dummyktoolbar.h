//----------------------------------------------------------------------------
//    filename             : dummyktoolbar.h
//----------------------------------------------------------------------------
//    Project              : Qt MDI extension
//
//    begin                : 10/2000       Falk Brettschneider
//
//    copyright            : (C) 1999-2000 by Falk Brettschneider
//
//    email                :  gigafalk@yahoo.com (Falk Brettschneider)
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Library General Public License as
//    published by the Free Software Foundation; either version 2 of the
//    License, or (at your option) any later version.
//
//----------------------------------------------------------------------------

#ifndef DUMMYKTOOLBAR_H
#define DUMMYKTOOLBAR_H

#include <qtoolbar.h>
#include <qmainwindow.h>

#include "qextmdidefines.h"

/**
  * @short The base class of QextMdiTaskBar in case of QextMDI without KDE2 support
  *
  */
class DLL_IMP_EXP_QEXTMDICLASS KToolBar : public QToolBar
{
   Q_OBJECT
public:
   KToolBar( QMainWindow* parent, const char *name, bool /*honor_style = FALSE*/, bool /*readConfig = TRUE*/)
#if QT_VERSION < 300
      : QToolBar( name, parent, QMainWindow::Bottom) {};
#else
      : QToolBar( name, parent, QMainWindow::DockBottom) {};
#endif
   ~KToolBar() {};
};

#endif  // DUMMYKTOOLBAR_H
