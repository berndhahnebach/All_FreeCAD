/***************************************************************************
                          CommandLine.h  -  description
                             -------------------
    begin                : 2002/08/06 19:52:20
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief The command line module
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




#ifndef __COMMAND_LINE_H__
#define __COMMAND_LINE_H__
 

#include <qlineedit.h>
#include <list>
#include <string>

/** The command line class
 */
class GuiExport FCCommandLine : public QLineEdit
{
  Q_OBJECT

private:
	// Singleton
	FCCommandLine(void);
	~FCCommandLine(void);

	static FCCommandLine *_pcSingleton;

protected:
  void keyPressEvent ( QKeyEvent * e );
  void mousePressEvent ( QMouseEvent * e );
  void wheelEvent ( QWheelEvent * e );
  void PopupCmdList ();
  void ScrollUp();
  void ScrollDown();
  QPopupMenu* CreatePopupMenu();

public:
  void SetParent(QWidget* parent);
	static void Destruct(void);
	static FCCommandLine &Instance(void);
	friend FCCommandLine &GetCmdLine(void); 

protected slots:
  void SetCmdText( QListBoxItem * item);
  void slotCut();
  void slotCopy();
  void slotPaste();
  void slotSeleclAll();
  void slotClear();
  void slotClearConsole();

private:
  FClist<FCstring> _alCmdList;
  FClist<FCstring>::iterator _TIterator;
  bool bOpen;
};

inline FCCommandLine &GetCmdLine(void)
{
  return FCCommandLine::Instance();
}

/** A special list box used by the command line class
 */
class GuiExport FCCommandListBox : public QListBox
{
  Q_OBJECT

public:
  FCCommandListBox( QWidget * parent=0, const char * name=0, WFlags f=0);

protected:
  void keyPressEvent ( QKeyEvent * e );

protected slots:
  void Close();
};

#endif // __COMMAND_LINE_H__
