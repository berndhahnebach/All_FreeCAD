//----------------------------------------------------------------------------
//    filename             : dummykmainwindow.h
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

#ifndef DUMMYKMAINWINDOW_H
#define DUMMYKMAINWINDOW_H

#include <qmainwindow.h>

#include "qextmdidefines.h"

/**
  * @short The base class of QextMdiMainFrm in case of QextMDI without KDE2 support
  *
  */
class DLL_IMP_EXP_QEXTMDICLASS KMainWindow : public QMainWindow
{
   Q_OBJECT
public:
   KMainWindow( QWidget* parentWidget = 0L, const char* name = "", WFlags flags = WType_TopLevel) : QMainWindow( parentWidget, name, flags) {};
   ~KMainWindow() {};
};

#endif   // #define DUMMYKMAINWINDOW_H
