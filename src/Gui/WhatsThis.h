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


#ifndef GUI_WHATSTHIS_H
#define GUI_WHATSTHIS_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#ifndef __Qt3All__
# include "Qt3All.h"
#endif

#include "Command.h"

class Q3TextBrowser;

namespace Gui {

/**
 * The WhatsThis class provides a simple description of any widget, i.e. answering the 
 * question "What's this?". The description is shown in the help viewer.
 * @see Gui::HelpView
 * \author Werner Mayer
 */
class WhatsThis : public Q3WhatsThis
{
public:
  WhatsThis( QWidget *);
  WhatsThis( QWidget *, const QString& url);
  virtual ~WhatsThis();

  void setUrl( const QString& url );
  void setText( const QString& txt );
  virtual QString text( const QPoint & );

  static void setHelpView( Q3TextBrowser* );

private:
  QWidget* _widget; /**< Corresponding widget to object. */
  QString _url; /**< URL of the Html document. */
  QString _txt; /**< Description of the widget. */

  static Q3TextBrowser* _helpViewer; /**< global HelpViewer to display help. */
};

// --------------------------------------------------------------------

/**
 * @author Werner Mayer
 */
class StdCmdDescription : public Command
{
public:
  StdCmdDescription();
  ~StdCmdDescription();

  static bool inDescriptionMode();
  static void enterDescriptionMode();
  static void leaveDescriptionMode();

  static void setHelpView( Q3TextBrowser* );
  static void setSource( const QString& );

protected:
  Action* createAction(void);
  void activated(int iMsg);

private:
  static bool _descrMode;
  static Q3TextBrowser* _helpViewer; /**< global HelpViewer to display help. */
};

} // namespace Gui

#endif // GUI_WHATSTHIS_H
