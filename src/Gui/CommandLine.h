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
#include "window.h"
#include <qwindowsstyle.h>
#include <qvalidator.h>

/** Validate input
 */
class FCConsoleValidator : public QValidator
{
  public:
    FCConsoleValidator ( QWidget * parent, const char * name = 0 );
    virtual ~FCConsoleValidator ();

    virtual State validate ( QString &, int & ) const;
    virtual void fixup ( QString & ) const;
};

/** The windows style for the command line
 */
class FCWindowsStyle : public QWindowsStyle
{
  public:
    // avoid drawing the button
    QRect comboButtonRect ( int x, int y, int w, int h )
    { 
      return QWindowsStyle::comboButtonRect(x, y, w, h);
    }

    void drawComboButton ( QPainter * p, int x, int y, int w, int h, 
                           const QColorGroup & g, bool sunken = FALSE, 
                           bool editable = FALSE, bool enabled = TRUE, 
                           const QBrush * fill = 0 )
    {
      qDrawWinPanel(p, x, y, w, h, g, TRUE, 
              	    fill?fill:(enabled?&g.brush( QColorGroup::Base ): &g.brush( QColorGroup::Base )));

      drawArrow( p, QStyle::DownArrow, sunken, x+w-2-16+ 2, y+2+ 2, 16- 4, h-4- 4, g, enabled,
        	       fill ? fill : &g.brush( QColorGroup::Base ) );
    }
};

/** The command line class
 */
class GuiExport FCCommandLine : public QComboBox, public FCWindow
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
  void enterEvent ( QEvent * );
  void leaveEvent ( QEvent * );
  QPopupMenu* CreatePopupMenu();
  void ReadCmdList();
  void SaveCmdList();

public:
  void SetParent(QWidget* parent);
	static void Destruct(void);
	static FCCommandLine &Instance(void);
	friend FCCommandLine &GetCmdLine(void); 

protected slots:
  void slotClearConsole();
  void slotOpenConsole();
  void slotLaunchCommand();
};

inline FCCommandLine &GetCmdLine(void)
{
  return FCCommandLine::Instance();
}

//
///** The command line class
// */
//class GuiExport FCCommandLine : public QLineEdit, public FCWindow
//{
//  Q_OBJECT
//
//private:
//	// Singleton
//	FCCommandLine(void);
//	~FCCommandLine(void);
//
//	static FCCommandLine *_pcSingleton;
//
//protected:
//  void keyPressEvent ( QKeyEvent * e );
//  void mousePressEvent ( QMouseEvent * e );
//  void wheelEvent ( QWheelEvent * e );
//  void PopupCmdList ();
//  void ScrollUp();
//  void ScrollDown();
//  QPopupMenu* CreatePopupMenu();
//  void ReadCmdList();
//  void SaveCmdList();
//
//public:
//  void SetParent(QWidget* parent);
//	static void Destruct(void);
//	static FCCommandLine &Instance(void);
//	friend FCCommandLine &GetCmdLine(void); 
//
//protected slots:
//  void SetCmdText( QListBoxItem * item);
//  void slotCut();
//  void slotCopy();
//  void slotPaste();
//  void slotSeleclAll();
//  void slotClear();
//  void slotClearConsole();
//  void slotOpenConsole();
//  void slotLaunchCommand();
//
//private:
//  FClist<FCstring> _alCmdList;
//  FClist<FCstring>::iterator _TIterator;
//  bool bOpen;
//};
//
//inline FCCommandLine &GetCmdLine(void)
//{
//  return FCCommandLine::Instance();
//}
//
///** A special list box used by the command line class
// */
//class GuiExport FCCommandListBox : public QListBox
//{
//  Q_OBJECT
//
//public:
//  FCCommandListBox( QWidget * parent=0, const char * name=0, WFlags f=0);
//
//protected:
//  void keyPressEvent ( QKeyEvent * e );
//
//protected slots:
//  void Close();
//};

#endif // __COMMAND_LINE_H__
