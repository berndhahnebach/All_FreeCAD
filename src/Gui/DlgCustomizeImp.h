/***************************************************************************
                          DlgCustomizeImp.h  -  description
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

#include "Window.h"

class FCCommand;
class FCToolBar;
class QTabWidget;

class FCPropertyPage
{
  protected:
    FCPropertyPage();

  public:
    virtual ~FCPropertyPage() {}
    bool isModified();
    void setModified(bool b);
    void slotApply();
    void slotCancel();

  protected:
    virtual void apply();
    virtual void cancel();

  private:
    bool bChanged;
};


/**
 * Dialog which contains all available commands and separate
 * them in several categories (file, edit, view,...).
 * You can extend the existing toolbars or buttongroups with 
 * several commands just by drag and drop.
 */
class FCDlgCustomize : public QDialog
{ 
    Q_OBJECT

  public:
    FCDlgCustomize( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~FCDlgCustomize();
    void insertTab (QWidget* w, QString name);

  protected slots:
    /// click the OK button
    void slotOK();
    /// click the Apply button
    void slotApply();
    /// click the Cancel button
    void slotCancel();

  protected:
    QPushButton* buttonHelp;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QTabWidget* tabWidget;
    QGridLayout* FCDlgCustomizeBaseLayout;
    QHBoxLayout* Layout;
    std::vector<QWidget*> tabPages;
};

#endif // DLGCUSTOMIZE_H
