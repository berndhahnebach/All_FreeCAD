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
# include <qlayout.h>
# include <qpopupmenu.h>
# include <qtabwidget.h>
# include <qvariant.h>
#endif

#include "ReportView.h"
#include "FileDialog.h"
#include "PythonEditor.h"

using namespace Gui;
using namespace Gui::DockWnd;

/**
 *  Constructs a ReportView which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
ReportView::ReportView( QWidget* parent,  const char* name, WFlags fl )
  : DockWindow( 0L, parent, name, fl )
{
  if ( !name )
    setName( "ReportOutput" );

  resize( 529, 162 );
  QGridLayout* tabLayout = new QGridLayout( this );
  tabLayout->setSpacing( 0 );
  tabLayout->setMargin( 0 );

  tab = new QTabWidget( this, "TabWidget" );
  tab->setProperty( "tabPosition", (int)QTabWidget::Bottom );
  tab->setProperty( "tabShape", (int)QTabWidget::Triangular );
  tabLayout->addWidget( tab, 0, 0 );


  mle = new ReportOutput( tab, "LogOutput" );
  tab->insertTab( mle, tr( "Output" ) );

  pyc = new PythonConsole(tab, "PythonConsole");
  tab->insertTab(pyc, "Python");
}

/**
 *  Destroys the object and frees any allocated resources
 */
ReportView::~ReportView()
{
  // no need to delete child widgets, Qt does it all for us
}

// ----------------------------------------------------------

ReportHighlighter::ReportHighlighter(QTextEdit* edit)
: QSyntaxHighlighter(edit), type(Message), lastPos(0), lastPar(0)
{
  txtCol = Qt::black;
  logCol = Qt::blue;
  warnCol = QColor(255, 170, 0);
  errCol = Qt::red;
}

ReportHighlighter::~ReportHighlighter()
{
}

int ReportHighlighter::highlightParagraph ( const QString & text, int endStateOfLastPara )
{
  int curPar = currentParagraph();
  if (curPar > lastPar)
  {
    lastPos = 0;
  }

  if (type == Message)
  {
    setFormat(lastPos, text.length()-lastPos, txtCol);
  }
  else if (type == Warning)
  {
    setFormat(lastPos, text.length()-lastPos, warnCol);
  }
  else if (type == Error)
  {
    setFormat(lastPos, text.length()-lastPos, errCol);
  }
  else if (type == LogText)
  {
    setFormat(lastPos, text.length()-lastPos, logCol);
  }

  lastPos = text.length()-1;
  lastPar = curPar;
  
  return 0;
}

void ReportHighlighter::setParagraphType(ReportHighlighter::Paragraph t)
{
  type = t;
}

void ReportHighlighter::setTextColor( const QColor& col )
{
  txtCol = col;
}

void ReportHighlighter::setLogColor( const QColor& col )
{
  logCol = col;
}

void ReportHighlighter::setWarningColor( const QColor& col )
{
  warnCol = col;
}

void ReportHighlighter::setErrorColor( const QColor& col )
{
  errCol = col;
}

// ----------------------------------------------------------

/**
 *  Constructs a ReportOutput which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
ReportOutput::ReportOutput(QWidget* parent, const char* name)
 : QTextEdit(parent, name), WindowParameter("OutputWindow"), 
   _err(true), _warn(true), _logg(false)
{
  reportHl = new ReportHighlighter(this);

  restoreFont();
  setReadOnly(true);
  clear();
  setHScrollBarMode(QScrollView::AlwaysOff);

  insert("FreeCAD output.\n");
  Base::Console().AttacheObserver(this);
  getWindowParameter()->Attach( this );

  getWindowParameter()->NotifyAll();
}

/**
 *  Destroys the object and frees any allocated resources
 */
ReportOutput::~ReportOutput()
{
  getWindowParameter()->Detach( this );
  Base::Console().DetacheObserver(this);
  delete reportHl;
}

void ReportOutput::restoreFont()
{
  QFont _font(  font() );
  _font.setFamily( "Courier" );
#ifdef FC_OS_LINUX
  _font.setPointSize( 15 );
#else
  _font.setPointSize( 10 );
#endif
//  _font.setBold( TRUE );
  setFont( _font ); 
}

void ReportOutput::Warning(const char * s)
{
  if ( _warn )
  {
    reportHl->setParagraphType(ReportHighlighter::Warning);
    append(s);
  }
}

void ReportOutput::Message(const char * s)
{
  reportHl->setParagraphType(ReportHighlighter::Message);
  append(s);
}

void ReportOutput::Error  (const char * s)
{
  if ( _err )
  {
    reportHl->setParagraphType(ReportHighlighter::Error);
    append(s);
  }
}

void ReportOutput::Log (const char * s)
{
  if ( _logg )
  {
    reportHl->setParagraphType(ReportHighlighter::LogText);
    append(s);
  }
}

bool ReportOutput::event( QEvent* ev )
{
  bool ret = QWidget::event( ev ); 
  if ( ev->type() == QEvent::ApplicationFontChange ) 
  {
    restoreFont();
  }

  return ret;
}

QPopupMenu * ReportOutput::createPopupMenu ( const QPoint & pos )
{
  QPopupMenu* menu = QTextEdit::createPopupMenu(pos);
  
  QPopupMenu* sub = new QPopupMenu( menu );
  int id;
  id = sub->insertItem( tr("Logging"), this, SLOT( onToggleLogging() ) );
  sub->setItemChecked( id, _logg );
  id = sub->insertItem( tr("Warning"), this, SLOT( onToggleWarning() ) );
  sub->setItemChecked( id, _warn );
  id = sub->insertItem( tr("Error"  ), this, SLOT( onToggleError  () ) );
  sub->setItemChecked( id, _err );
  menu->insertItem( tr("Options"), sub, -1, 0 );
  menu->insertSeparator( 1 );

  menu->insertItem(tr("Clear"), this, SLOT(clear()));
  menu->insertSeparator();
  menu->insertItem(tr("Save As..."), this, SLOT(onSaveAs()));
  return menu;
}

void ReportOutput::onSaveAs()
{
  QString fn = FileDialog::getSaveFileName(QString::null,"Plain Text Files (*.txt *.log)", 
                                           this, QObject::tr("Save Report Output"));
  if (!fn.isEmpty())
  {
    int dot = fn.find('.');
    if (dot != -1)
    {
      QFile f(fn);
      if (f.open(IO_WriteOnly))
      {
        QTextStream t (&f);
        t << text();
        f.close();
      }
    }
  }
}

bool ReportOutput::isError() const
{
  return _err;
}

bool ReportOutput::isWarning() const
{
  return _warn;
}

bool ReportOutput::isLogging() const
{
  return _logg;
}

void ReportOutput::onToggleError()
{
  _err = _err ? false : true;
  getWindowParameter()->SetBool( "checkError", _err );
}

void ReportOutput::onToggleWarning()
{
  _warn = _warn ? false : true;
  getWindowParameter()->SetBool( "checkWarning", _warn );
}

void ReportOutput::onToggleLogging()
{
  _logg = _logg ? false : true;
  getWindowParameter()->SetBool( "checkLogging", _logg );
}

void ReportOutput::OnChange(FCSubject<const char*> &rCaller, const char * sReason)
{
  FCParameterGrp& rclGrp = ((FCParameterGrp&)rCaller);
  if (strcmp(sReason, "checkLogging") == 0)
  {
    _logg = rclGrp.GetBool( sReason, _logg );
  }
  else if (strcmp(sReason, "checkWarning") == 0)
  {
    _warn = rclGrp.GetBool( sReason, _warn );
  }
  else if (strcmp(sReason, "checkError") == 0)
  {
    _err = rclGrp.GetBool( sReason, _err );
  }
  else if (strcmp(sReason, "colorText") == 0)
  {
    long col = rclGrp.GetInt( sReason );
    reportHl->setTextColor( QColor(col & 0xff,(col >> 8) & 0xff,(col >> 16) & 0xff) );
  }
  else if (strcmp(sReason, "colorLogging") == 0)
  {
    long col = rclGrp.GetInt( sReason );
    reportHl->setLogColor( QColor(col & 0xff,(col >> 8) & 0xff,(col >> 16) & 0xff) );
  }
  else if (strcmp(sReason, "colorWarning") == 0)
  {
    long col = rclGrp.GetInt( sReason );
    reportHl->setWarningColor( QColor(col & 0xff,(col >> 8) & 0xff,(col >> 16) & 0xff) );
  }
  else if (strcmp(sReason, "colorError") == 0)
  {
    long col = rclGrp.GetInt( sReason );
    reportHl->setErrorColor( QColor(col & 0xff,(col >> 8) & 0xff,(col >> 16) & 0xff) );
  }
}

#include "moc_ReportView.cpp"
