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

#ifndef _PreComp_
# include <qcombobox.h>
# include <qlineedit.h>
# include <qvalidator.h>
#endif

namespace Gui {

/** Validates input of the command line
 * \author Werner Mayer
 */
class ConsoleValidator : public QValidator
{
public:
  ConsoleValidator ( QWidget * parent, const char * name = 0 );
  virtual ~ConsoleValidator ();

  virtual State validate ( QString &, int & ) const;
  virtual void fixup ( QString & ) const;
};

/** The command line class
 * \author Werner Mayer
 */
class GuiExport CommandLineBase : public QComboBox, public FCWindowParameter
{
  Q_OBJECT

protected:
  // Singleton
  CommandLineBase(void);
  virtual ~CommandLineBase(void);

  static CommandLineBase *_pcSingleton;

  std::vector<std::string> _astrRunCmds;

protected:
  void loadHistory();
  void saveHistory();

  void keyPressEvent    ( QKeyEvent         * e );
  void contextMenuEvent ( QContextMenuEvent * e );
  void wheelEvent       ( QWheelEvent       * e );
  void dropEvent        ( QDropEvent        * e );
  void dragEnterEvent   ( QDragEnterEvent   * e );

public:
  bool eventFilter       ( QObject* o, QEvent* e );
  void show();
  void reparent(QWidget* parent);

  static void Destruct(void);
  static CommandLineBase &Instance(void);

protected slots:
  void onClearHistory();
  void onShowHistory();
  void onLaunchCommand();
};

inline CommandLineBase &CommandLine(void)
{
  return CommandLineBase::Instance();
}

} // namespace Gui

#endif // __COMMAND_LINE_H__
