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


#include "PreCompiled.h"

#ifndef _PreComp_

# include <qapplication.h>
# include <qtextbrowser.h>
#endif

#include "WhatsThis.h"

#include "Action.h"


using namespace Gui;

QTextBrowser* WhatsThis::_helpViewer = 0L;

/**
 * Constructs a dynamic "What's this?" object for \a widget. The object is deleted when 
 * the \a widget is destroyed.
 *
 * When the widget is queried by the user the text() function of this QWhatsThis will be 
 * called to provide the appropriate text, rather than using the text assigned by add().
 */
WhatsThis::WhatsThis( QWidget * widget)
  : QWhatsThis( widget ), _widget( widget ), _url( QString::null ), _txt( QString::null )
{
}

/**
 * Constructs a dynamic "What's this?" object for \a widget. The object is deleted when 
 * the \a widget is destroyed.
 *
 * When the widget is queried by the user the text() function of this QWhatsThis will be 
 * called to provide the appropriate text, rather than using the text assigned by add().
 */
WhatsThis::WhatsThis( QWidget * widget, const QString& url)
 : QWhatsThis( widget ), _widget( widget ), _url( url ), _txt( QString::null )
{
}

/**
 * Destroys the object and frees any allocated resources.
 */
WhatsThis::~WhatsThis()
{
}

/**
 * Sets the URL of Html document to \a url.
 * The document is shown in the text browser specified by \ref setHelpView().
 */
void WhatsThis::setUrl( const QString& url )
{
  _url = url;
}

/**
 * Sets the description to the widget specified in the constructor
 * to \a txt. The description is shown in a bubble help.
 */
void WhatsThis::setText( const QString& txt )
{
  _txt = txt;
}

/** 
 * Shows the specified URL inside the text browser 
 * and returns the specified text with @ref setText(). 
 */
QString WhatsThis::text( const QPoint & )
{
  _helpViewer->setSource( _url );
  return _txt;
}

/**
 * Shows the help text in the specified browser.  
 */
void WhatsThis::setHelpView( QTextBrowser* browser )
{
  _helpViewer = browser;
}


// --------------------------------------------------------------------


/* TRANSLATOR Gui::StdCmdDescription */


bool StdCmdDescription::_descrMode = false;

QTextBrowser* StdCmdDescription::_helpViewer = 0L;


StdCmdDescription::StdCmdDescription()
  :CppCommand("Std_DescriptionMode", Cmd_Toggle)
{
  sGroup        = QT_TR_NOOP("Help");
  sMenuText     = QT_TR_NOOP("Des&cription");
  sToolTipText  = QT_TR_NOOP("Long description of commands");
  sWhatsThis    = QT_TR_NOOP("Long description of commands");
  sStatusTip    = QT_TR_NOOP("Long description of commands");
  iAccel        = Qt::Key_F1;
}

StdCmdDescription::~StdCmdDescription()
{
}

QAction * StdCmdDescription::createAction(void)
{
  QAction *pcAction;

  pcAction = new Action(this,qApp->mainWidget(),sName,true);
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setMenuText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  pcAction->setAccel(iAccel);

  return pcAction;
}

void StdCmdDescription::activated(int iMsg)
{
}

void StdCmdDescription::toggled ( bool b )
{
  if ( b )
    enterDescriptionMode();
  else
    leaveDescriptionMode();
}

bool StdCmdDescription::inDescriptionMode()
{
  return _descrMode;
}

/**
 * Shows the help text in the specified browser.  
 */
void StdCmdDescription::setHelpView( QTextBrowser* browser )
{
  _helpViewer = browser;
}

void StdCmdDescription::setSource( const QString& src )
{
  if ( _helpViewer && !src.isEmpty() )
  {
    QString url = "index.php@";
    url.append( src );
    _helpViewer->setSource( url );
  }
}

void StdCmdDescription::enterDescriptionMode()
{
  _descrMode = true;
  QApplication::setOverrideCursor( Qt::WhatsThisCursor, false );
}

void StdCmdDescription::leaveDescriptionMode()
{
  _descrMode = false;
  QApplication::restoreOverrideCursor();
}

