/***************************************************************************
                          PlatQt.h  -  description
                             -------------------
    begin                : 2002/12/20 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file PlatQt.h
 *  \brief Qt binding stuff for scintilla
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
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
 

#ifndef __FC_PLAT_QT_H__
#define __FC_PLAT_QT_H__

#include "View.h"

/**
 *  Listbox for the editor based on scintilla
 */
class ListBoxX;
class FCScintListBox : public QListBox
{
	Q_OBJECT

  public:
	  FCScintListBox(ListBoxX* listbox, QWidget *parent);
	  virtual ~FCScintListBox();

  private slots:
	  void slotDoubleClicked(QListBoxItem* item);

  private:
	  ListBoxX* m_pXListbox;
};

class FCScintEditor;
class FCScintillaDoc;
class FCScintEditView : public FCFloatingView
{
  public:
	  FCScintEditView(FCScintillaDoc* pcDoc, QWidget* parent, const char* name);
	  ~FCScintEditView();

  protected:
    FCScintEditor* view;
};

class FCScintillaView : public FCFloatingChildView
{
  public:
	  FCScintillaView( FCFloatingView* pcView, QWidget* parent, const char* name);
	  ~FCScintillaView();

  protected:
    virtual void closeEvent(QCloseEvent *e);
};

class FCScintillaDoc : public FCFloatingDoc
{
  Q_OBJECT

  public:
    FCScintillaDoc();
    ~FCScintillaDoc();
    void CreateView(const QString& s);
};

#endif //__FC_PLAT_QT_H__
