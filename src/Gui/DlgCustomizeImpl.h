/***************************************************************************
                          DlgCustomizeImpl.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize toolbars and button groups
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




#ifndef DLGCUSTOMIZE_H
#define DLGCUSTOMIZE_H

#include "DlgCustomize.h"
#include "Window.h"

class FCCommand;

class FCDlgCustomize : public FCDlgCustomizeBase, public FCWindow
{ 
  Q_OBJECT

  public:
    FCDlgCustomize( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~FCDlgCustomize();

  protected slots:
    void slotDescription(QString);
    void slotGroupSelected(const QString &);

  protected:
    // groups of commands
    FCmap<FCstring, FCvector<FCCommand*> > m_alCmdGroups;
};

#endif // DLGCUSTOMIZE_H
