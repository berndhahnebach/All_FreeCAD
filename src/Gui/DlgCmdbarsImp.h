/***************************************************************************
                          DlgCmdbarsImp.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize command bars
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/




#ifndef DLGCMDBARS_IMP_H
#define DLGCMDBARS_IMP_H

#include "DlgToolbarsImp.h"

class FCCommand;
class FCToolBar;

/**
 * This class implements the creation of user defined command bars.
 */
class FCDlgCustomCmdbarsImp : public FCDlgCustomToolbarsBase
{ 
  Q_OBJECT

  public:
    FCDlgCustomCmdbarsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCDlgCustomCmdbarsImp();

  protected:
    void apply();
    void cancel();
		void onUpdate();

    /// creates new toolbar
    void onCreateToolbar();
    /// deletes toolbar
    void onDeleteToolbar();
};

#endif
