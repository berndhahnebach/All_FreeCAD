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

#endif //__FC_PLAT_QT_H__
