/***************************************************************************
                          DlgGeneralImp.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief 
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




#ifndef DLG_GENERAL_IMP_H
#define DLG_GENERAL_IMP_H

#include "DlgGeneral.h"
#include "PropertyPage.h"

/**
 * This class implements the settings for the application
 * like style, size of pixmaps, size of recent file list and so on
 * @see class ApplicationWindow
 */
class FCDlgGeneral : public FCDlgGeneralBase, public FCPreferencePage
{
  Q_OBJECT

  public:
    FCDlgGeneral( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCDlgGeneral();

  private slots:
    /** Looks in the system parameters for the entry "BigPixmaps".
     * If it is true the tool buttons in the main window are set to use big pixmaps, 
     * and small pixmaps if "BigPixmaps" is false.
     * @see ApplicationWindow, QMainWindow
     */
    void onBigPixmaps();
    /** Sets the application's style
     * @see QStyle, QApplication
     */
    void onSetStyle();
    /** Sets the size of the recent file list (MRU) in the
     * system parameters.
     * @see FCCmdMRU
     */
    void onSetMRUSize();
};

#endif // DLG_GENERAL_IMP_H
