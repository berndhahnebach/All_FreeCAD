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

#include "ReportView.h"
#include "FileDialog.h"
#include "PythonConsole.h"
#include "BitmapFactory.h"

using namespace Gui;
using namespace Gui::DockWnd;

/* TRANSLATOR Gui::DockWnd::ReportView */

/**
 *  Constructs a ReportView which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
ReportView::ReportView( QWidget* parent )
  : QWidget(parent)
{
  setObjectName( "ReportOutput" );

  resize( 529, 162 );
  QGridLayout* tabLayout = new QGridLayout( this );
  tabLayout->setSpacing( 0 );
  tabLayout->setMargin( 0 );

  tabWidget = new QTabWidget( this );
  tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
  tabWidget->setTabPosition(QTabWidget::South);
  tabWidget->setTabShape(QTabWidget::Rounded);
  tabLayout->addWidget( tabWidget, 0, 0 );

  tabOutput = new ReportOutput();
  int output = tabWidget->addTab(tabOutput, trUtf8("Output"));
  tabWidget->setTabIcon(output, BitmapFactory().pixmap("MacroEditor"));

  tabPython = new PythonConsole();
  int python = tabWidget->addTab(tabPython, trUtf8("Python console"));
  tabWidget->setTabIcon(python, BitmapFactory().pixmap("python_small"));
  tabWidget->setCurrentIndex(0);

  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("General")->GetGroup("AutoloadTab");
  std::string txt = hGrp->GetASCII("currentText");
  for (int i=0; i<tabWidget->count(); i++)
  {
    if ( tabWidget->tabText(i) == txt.c_str() )
    {
      tabWidget->setCurrentPage( i );
      break;
    }
  }
}

/**
 *  Destroys the object and frees any allocated resources
 */
ReportView::~ReportView()
{
  // no need to delete child widgets, Qt does it all for us
}

void ReportView::changeEvent(QEvent *e)
{
  QWidget::changeEvent(e);
  if (e->type() == QEvent::LanguageChange) {
    tabWidget->setTabText( tabWidget->indexOf(tabOutput), trUtf8( "Output" ) );
    tabWidget->setTabText( tabWidget->indexOf(tabPython), trUtf8("Python console") );
  }
}

// ----------------------------------------------------------

ReportHighlighter::ReportHighlighter(QTextEdit* edit)
  : QSyntaxHighlighter(edit), type(Message)
{
  txtCol = Qt::black;
  logCol = Qt::blue;
  warnCol = QColor(255, 170, 0);
  errCol = Qt::red;
}

ReportHighlighter::~ReportHighlighter()
{
}

void ReportHighlighter::highlightBlock ( const QString & text )
{
  if (type == Message)
  {
    setFormat(0, text.length(), txtCol);
  }
  else if (type == Warning)
  {
    setFormat(0, text.length(), warnCol);
  }
  else if (type == Error)
  {
    setFormat(0, text.length(), errCol);
  }
  else if (type == Qt::LogText)
  {
    setFormat(0, text.length(), logCol);
  }
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

/* TRANSLATOR Gui::DockWnd::ReportOutput */

/**
 *  Constructs a ReportOutput which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
ReportOutput::ReportOutput(QWidget* parent)
 : QTextEdit(parent), WindowParameter("OutputWindow")
{
  bLog = false;
  reportHl = new ReportHighlighter(this);

  restoreFont();
  setReadOnly(true);
  clear();
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  Base::Console().AttachObserver(this);
  getWindowParameter()->Attach( this );

  getWindowParameter()->NotifyAll();

  // scroll to bottom at startup to make sure that last appended text is visible
  ensureCursorVisible();
}

/**
 *  Destroys the object and frees any allocated resources
 */
ReportOutput::~ReportOutput()
{
  getWindowParameter()->Detach( this );
  Base::Console().DetachObserver(this);
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
  setFont( _font ); 
}

void ReportOutput::Warning(const char * s)
{
  reportHl->setParagraphType(ReportHighlighter::Warning);

  QTextCursor cursor(this->document());
  cursor.beginEditBlock();
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(s);
  cursor.endEditBlock();
  ensureCursorVisible();
}

void ReportOutput::Message(const char * s)
{
  reportHl->setParagraphType(ReportHighlighter::Message);

  QTextCursor cursor(this->document());
  cursor.beginEditBlock();
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(s);
  cursor.endEditBlock();
  ensureCursorVisible();
}

void ReportOutput::Error  (const char * s)
{
  reportHl->setParagraphType(ReportHighlighter::Error);

  QTextCursor cursor(this->document());
  cursor.beginEditBlock();
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(s);
  cursor.endEditBlock();
  ensureCursorVisible();
}

void ReportOutput::Log (const char * s)
{
  reportHl->setParagraphType(ReportHighlighter::LogText);

  QTextCursor cursor(this->document());
  cursor.beginEditBlock();
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(s);
  cursor.endEditBlock();
  ensureCursorVisible();
}

void ReportOutput::contextMenuEvent ( QContextMenuEvent * e )
{
  QMenu* menu = createStandardContextMenu();
  QAction* first = menu->actions().front();

  QMenu* submenu = new QMenu( menu );
  QAction* logAct = submenu->addAction(tr("Logging"), this, SLOT(onToggleLogging()));
  logAct->setCheckable(true);
  logAct->setChecked(bLog);

  QAction* wrnAct = submenu->addAction(tr("Warning"), this, SLOT(onToggleWarning()));
  wrnAct->setCheckable(true);
  wrnAct->setChecked(bWrn);

  QAction* errAct = submenu->addAction(tr("Error"), this, SLOT(onToggleError()));
  errAct->setCheckable(true);
  errAct->setChecked(bErr);

  submenu->setTitle(tr("Options"));
  menu->insertMenu(first, submenu);
  menu->insertSeparator(first);

  menu->addAction(tr("Clear"), this, SLOT(clear()));
  menu->addSeparator();
  menu->addAction(tr("Save As..."), this, SLOT(onSaveAs()));

  menu->exec(e->globalPos());
  delete menu;
}

void ReportOutput::onSaveAs()
{
  QString fn = QFileDialog::getSaveFileName(this, tr("Save Report Output"), QString(), tr("Plain Text Files (*.txt *.log)"));
  if (!fn.isEmpty())
  {
    QFileInfo fi(fn);
    if (fi.extension().isEmpty())
      fn += ".log";
    QFile f(fn);
    if (f.open(QIODevice::WriteOnly))
    {
      QTextStream t (&f);
      t << text();
      f.close();
    }
  }
}

bool ReportOutput::isError() const
{
  return bErr;
}

bool ReportOutput::isWarning() const
{
  return bWrn;
}

bool ReportOutput::isLogging() const
{
  return bLog;
}

void ReportOutput::onToggleError()
{
  bErr = bErr ? false : true;
  getWindowParameter()->SetBool( "checkError", bErr );
}

void ReportOutput::onToggleWarning()
{
  bWrn = bWrn ? false : true;
  getWindowParameter()->SetBool( "checkWarning", bWrn );
}

void ReportOutput::onToggleLogging()
{
  bLog = bLog ? false : true;
  getWindowParameter()->SetBool( "checkLogging", bLog );
}

void ReportOutput::OnChange(Base::Subject<const char*> &rCaller, const char * sReason)
{
  ParameterGrp& rclGrp = ((ParameterGrp&)rCaller);
  if (strcmp(sReason, "checkLogging") == 0)
  {
    bLog = rclGrp.GetBool( sReason, bLog );
  }
  else if (strcmp(sReason, "checkWarning") == 0)
  {
    bWrn = rclGrp.GetBool( sReason, bWrn );
  }
  else if (strcmp(sReason, "checkError") == 0)
  {
    bErr = rclGrp.GetBool( sReason, bErr );
  }
  else if (strcmp(sReason, "colorText") == 0)
  {
    unsigned long col = rclGrp.GetUnsigned( sReason );
    reportHl->setTextColor( QColor( (col >> 24) & 0xff,(col >> 16) & 0xff,(col >> 8) & 0xff) );
  }
  else if (strcmp(sReason, "colorLogging") == 0)
  {
    unsigned long col = rclGrp.GetUnsigned( sReason );
    reportHl->setLogColor( QColor( (col >> 24) & 0xff,(col >> 16) & 0xff,(col >> 8) & 0xff) );
  }
  else if (strcmp(sReason, "colorWarning") == 0)
  {
    unsigned long col = rclGrp.GetUnsigned( sReason );
    reportHl->setWarningColor( QColor( (col >> 24) & 0xff,(col >> 16) & 0xff,(col >> 8) & 0xff) );
  }
  else if (strcmp(sReason, "colorError") == 0)
  {
    unsigned long col = rclGrp.GetUnsigned( sReason );
    reportHl->setErrorColor( QColor( (col >> 24) & 0xff,(col >> 16) & 0xff,(col >> 8) & 0xff) );
  }
}

#include "moc_ReportView.cpp"
