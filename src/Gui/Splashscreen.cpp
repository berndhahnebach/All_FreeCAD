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

#include "../Version.h"
#ifndef _PreComp_
# include <qlabel.h>
# include <qlayout.h>
# include <qpixmap.h>
# include <qprogressbar.h>
# include <qpushbutton.h>
# include <qstylefactory.h>
# include <qthread.h>
# include <qvariant.h>
#endif

#include "Splashscreen.h"
#include <Base/Console.h>
#include <App/Application.h>
#include <Gui/BitmapFactory.h>


using namespace Gui;

namespace Gui {
/** Displays all messages at startup inside the splash screen.
 * \author Werner Mayer
 */
class SplashObserver : public Base::ConsoleObserver
{
public:
  SplashObserver(QSplashScreen* splasher=0, const char* name=0)
      : splash(splasher)
  {
    Base::Console().AttachObserver(this);
  }

  virtual ~SplashObserver()
  {
    Base::Console().DetachObserver(this);
  }

  void Warning(const char * s)
  {
    Log(s);
  }
  void Message(const char * s)
  {
    Log(s);
  }
  void Error  (const char * s)
  {
    Log(s);
  }
  void Log (const char * s)
  {
    QString msg(s);
    // remove leading blanks and carriage returns
    while (msg[0] == '\n' || msg[0] == ' ')
    {
      msg = msg.mid(1);
    }
    msg = QString("\n %1").arg(msg);
    splash->message( msg, Qt::AlignTop|Qt::AlignLeft, Qt::black );
//    qApp->processEvents();
    QWaitCondition().wait(50);
  }

private:
  QSplashScreen* splash;
};
} // namespace Gui

// ------------------------------------------------------------------------------

/**
 * Constructs a splash screen that will display the pixmap.
 */
SplashScreen::SplashScreen(  const QPixmap & pixmap , WFlags f )
    : QSplashScreen( pixmap, f), progBar(0L)
{
  // write the messages to splasher
  messages = new SplashObserver(this);

  // append also a progressbar for visual feedback
  progBar = new QProgressBar( this, "SplasherProgress" );
  progBar->setProperty( "progress", 0 );
  progBar->setStyle(QStyleFactory::create("motif"));
  progBar->setFixedSize(width()-6, 15);

  // make the splasher a bit higher
  resize(width(), height()+progBar->height());
  progBar->move(3, height()-(progBar->height()));
}

/** Destruction. */
SplashScreen::~SplashScreen()
{
  delete messages;
  delete progBar;
}

/** 
 * Draws the contents of the splash screen using painter \a painter. The default 
 * implementation draws the message passed by message().
 */
void SplashScreen::drawContents ( QPainter * painter )
{
  if (progBar)
    progBar->setProgress(progBar->progress() + 6);
  QSplashScreen::drawContents(painter);
}

// ------------------------------------------------------------------------------

/* TRANSLATOR Gui::AboutDialog */

/**
 *  Constructs a AboutDialog which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'WStyle_Customize|WStyle_NoBorder|WType_Modal'
 *
 *  The dialog will be modal.
 */
AboutDialog::AboutDialog( QWidget* parent, const char* name )
    : QDialog( parent, name, true,  QLabel::WStyle_Customize  |
               QLabel::WStyle_NoBorder   |
               QLabel::WType_Modal       ),
    image0( Gui::BitmapFactory().pixmap(App::Application::Config()["SplashPicture"].c_str()) )
{
  if ( !name )
    setName( "AboutDlg" );
  Form1Layout = new QGridLayout( this, 1, 1, 11, 6, "AboutDlgLayout");

  pixmapLabel1 = new QLabel( this, "pixmapLabel1" );
  pixmapLabel1->setPixmap( image0 );
  pixmapLabel1->setScaledContents( TRUE );

  Form1Layout->addWidget( pixmapLabel1, 0, 0 );

  layout1 = new QHBoxLayout( 0, 0, 6, "layout1");
  QSpacerItem* spacer = new QSpacerItem( 116, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
  layout1->addItem( spacer );

  pushButton1 = new QPushButton( this, "pushButton1" );
  pushButton1->setDefault( TRUE );
  layout1->addWidget( pushButton1 );
  QSpacerItem* spacer_2 = new QSpacerItem( 116, 31, QSizePolicy::Expanding, QSizePolicy::Minimum );
  layout1->addItem( spacer_2 );

  Form1Layout->addLayout( layout1, 2, 0 );

  textLabel1 = new QLabel( this, "textLabel1" );
  textLabel1->setAlignment( int( QLabel::AlignCenter ) );

  Form1Layout->addWidget( textLabel1, 1, 0 );
  languageChange();
  clearWState( WState_Polished );

  connect(pushButton1, SIGNAL(clicked()), this, SLOT(accept()));
}

/**
 *  Destroys the object and frees any allocated resources
 */
AboutDialog::~AboutDialog()
{
  // no need to delete child widgets, Qt does it all for us
}

/**
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void AboutDialog::languageChange()
{
  QString exeName = App::Application::Config()["ExeName"].c_str();
  QString banner  = App::Application::Config()["ConsoleBanner"].c_str();
  banner = banner.left( banner.find('\n') );

  pushButton1->setText( tr( "&Ok" ) );
  pushButton1->setAccel( QKeySequence( tr( "Alt+O" ) ) );
  QString SplasherText = QString(
                           "<!DOCTYPE HTML PUBLIC ""-//W3C//DTD HTML 4.0 Transitional//EN"">"
                           "<html>"
                           "<body bgcolor=""#ffffff"">"
                           "<p>"
                           "<table cellpadding=2 cellspacing=1 border=0  width=100% bgcolor=#E5E5E5 >"
                           "<tr>"
                           "<th bgcolor=#E5E5E5 width=33%>"
                           "<ul>"
                           "<a href=""FreeCAD"">%1 %2</a>\n"
                           "</ul>"
                           "</th>"
                           "</tr>"
                           "<tr>"
                           "<td>"
                           "<p>Version&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>%3.%4 </b></p>"
                           "<p>Build number&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;                             <b>%5</b></p>"
                           "<p>Build date&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <b>%6</b></p>"
                           "</td>"
                           "</tr>"
                           "<tr>"
                           "<td>"
                           "<ul>"
                           "</ul>"
                           "</td>"
                           "</tr>"
                           "</table>"
                           "</body></html>").arg(exeName).arg(banner).arg(FCVersionMajor).arg(FCVersionMinor).arg(FCVersionBuild).arg(FCVersionDisDa);
  textLabel1->setText( SplasherText );

}

#include "moc_Splashscreen.cpp"
