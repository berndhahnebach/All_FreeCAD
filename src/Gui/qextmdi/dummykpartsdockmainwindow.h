//----------------------------------------------------------------------------
//    filename             : dummykpartsdockmainwindow.h
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

#ifndef DUMMYKPARTSDOCKMAINWINDOW_H
#define DUMMYKPARTSDOCKMAINWINDOW_H

#include "qextmdidefines.h"
#include "kdockwidget.h"

namespace KParts {

/**
  * @short The base class of QextMdiMainFrm in case of QextMDI without KDE2 support
  *
  */
class DLL_IMP_EXP_QEXTMDICLASS DockMainWindow : public KDockMainWindow
{
   Q_OBJECT;
public:
   DockMainWindow( QWidget* parentWidget = 0L, const char* name = "", WFlags flags = WType_TopLevel) : KDockMainWindow( parentWidget, name, flags) {};
   ~DockMainWindow() {};
};

}; // end of namespace KParts

#endif   // DUMMYKPARTSDOCKMAINWINDOW_H
