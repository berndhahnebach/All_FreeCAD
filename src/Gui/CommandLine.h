/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           * 
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef __COMMAND_LINE_H__
#define __COMMAND_LINE_H__
 

#include "Window.h"

#ifdef FC_OS_LINUX
#	include <qvalidator.h>
#	include <qlineedit.h>
#endif

/** Validate input of the command line
 */
class FCConsoleValidator : public QValidator
{
  public:
    FCConsoleValidator ( QWidget * parent, const char * name = 0 );
    virtual ~FCConsoleValidator ();

    virtual State validate ( QString &, int & ) const;
    virtual void fixup ( QString & ) const;
};

/** The command line class
 */
class GuiExport FCCommandLine : public QComboBox, public FCWindowParameter
{
  Q_OBJECT

private:
	// Singleton
	FCCommandLine(void);
	~FCCommandLine(void);

	static FCCommandLine *_pcSingleton;

  std::vector<std::string> _astrRunCmds;

protected:
  QPopupMenu* CreatePopupMenu();
  /** import old commands from the preferences */
  void ReadCmdList();
  /** export new commands into the preferences */
  void SaveCmdList();

  void keyPressEvent ( QKeyEvent * e );
  void mousePressEvent ( QMouseEvent * e );
  void wheelEvent ( QWheelEvent * e );
  void dropEvent      ( QDropEvent      * e );
  void dragEnterEvent ( QDragEnterEvent * e );

public:
  bool eventFilter       ( QObject* o, QEvent* e );
  void show();
  void SetParent(QWidget* parent);
	static void Destruct(void);
	static FCCommandLine &Instance(void);
  /// gloabal function to get command line object
	friend FCCommandLine &GetCmdLine(void); 

protected slots:
  /** clear all new commands */
  void slotClearConsole();
  /** open the console */
  void slotOpenConsole();
  /** launch the command */
  void slotLaunchCommand();
};

inline FCCommandLine &GetCmdLine(void)
{
  return FCCommandLine::Instance();
}

#endif // __COMMAND_LINE_H__
