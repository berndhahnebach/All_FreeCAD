/***************************************************************************
                          ReportView.cpp  -  description
                             -------------------
    begin                : 2003/08/16 10:47:44
    copyright            : (C) 2003 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file ReportView.cpp
 *  \brief The output window of FreeCAD
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
 


#include "PreCompiled.h"
#ifndef _PreComp_
#	include <qscrollview.h>
#	include <qtabwidget.h>
#	include <qtextbrowser.h>
#	include <qtimer.h>
#endif

#include "ReportView.h"
#include "Widgets.h"
#include "PythonEditor.h"

/*
 *  Constructs a FCReportOutput which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
FCReportView::FCReportView( QWidget* parent,  const char* name, WFlags fl )
    : FCDockWindow( parent, name, fl )
{
  if ( !name )
  	setName( "FCReportOutput" );

  resize( 529, 162 );
  QGridLayout* tabLayout = new QGridLayout( this );
  tabLayout->setSpacing( 0 );
  tabLayout->setMargin( 0 );

  tab = new QTabWidget( this, "TabWidget" );
  tab->setProperty( "tabPosition", (int)QTabWidget::Bottom );
  tab->setProperty( "tabShape", (int)QTabWidget::Triangular );
  tabLayout->addWidget( tab, 0, 0 );


  mle = new FCReportOutput( tab, "LogOutput" );
  tab->insertTab( mle, tr( "Output" ) );

	pyc = new PythonConsole(tab, "PythonConsole");
	tab->insertTab(pyc, "Python");
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCReportView::~FCReportView()
{
    // no need to delete child widgets, Qt does it all for us
}

// ----------------------------------------------------------

class FCReportOutputPrivate
{
  public:
    std::vector<QString> alOriginal;
};

FCReportOutput::FCReportOutput(QWidget* parent, const char* name)
 : QTextBrowser(parent, name)
{
#if QT_VERSION >= 300
  setTextFormat(LogText);
#endif

  d = new FCReportOutputPrivate;

  QFont _font(  font() );
  _font.setFamily( "Courier" );
  _font.setPointSize( 10 );
  _font.setBold( TRUE );
  setFont( _font ); 
  onClear();
  setHScrollBarMode(QScrollView::AlwaysOff);

  setText("<qt>FreeCAD output.</qt>");
  GetConsole().AttacheObserver(this);
}

FCReportOutput::~FCReportOutput()
{
  GetConsole().DetacheObserver(this);
  delete d; d = NULL;
}

void FCReportOutput::appendLog(const char * s, const char * color)
{
  d->alOriginal.push_back(s);
  QString qs;

  if (color)
  {
#if QT_VERSION < 300
    if (strchr(s, '\n') != NULL)
      qs = QString("<font color=\"%1\">%2</font>\n<br/>").arg(color).arg(s);
    else
      qs = QString("<font color=\"%1\">%2</font>").arg(color).arg(s);
#else
      qs = QString("<font color=%1>%2</font>").arg(color).arg(s);;
#endif
  }
  else
  {
#if QT_VERSION < 300
    if (strchr(s, '\n') != NULL)
      qs = QString("%1<br/>").arg(s);
    else
#endif
      qs = QString("%1").arg(s);
  }
  
  append(qs);
  setContentsPos(0, contentsHeight());
}

void FCReportOutput::Warning(const char * s)
{
  appendLog(s, "orange");
}

void FCReportOutput::Message(const char * s)
{
  appendLog(s);
}

void FCReportOutput::Error  (const char * s)
{
  appendLog(s, "red");
}

void FCReportOutput::Log (const char * s)
{
  // ignore this
}

bool FCReportOutput::event( QEvent* ev )
{
  bool ret = QWidget::event( ev ); 
  if ( ev->type() == QEvent::ApplicationFontChange ) 
  {
	  QFont _font(  font() );
	  _font.setFamily( "Courier" );
	  _font.setPointSize( 10 );
	  _font.setBold( TRUE );
	  setFont( _font ); 
  }
  
  return ret;
}

void FCReportOutput::viewportMousePressEvent (QMouseEvent * e)
{
  if (e->button() == RightButton)
  {
    QPopupMenu menu;
    menu.insertItem(tr("Copy") , this, SLOT(copy()));
    menu.insertItem(tr("Clear"), this, SLOT(onClear()));
    menu.insertSeparator();
    menu.insertItem(tr("Select All"), this, SLOT(selectAll()));
    menu.insertSeparator();
    menu.insertItem(tr("Save As..."), this, SLOT(onSaveAs()));
    menu.exec(e->globalPos());
  }
  else
    QTextBrowser::viewportMousePressEvent(e);
}

void FCReportOutput::onClear()
{
//  setText("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n");
  setText("");
}

void FCReportOutput::onSaveAs()
{
//  QString fn = FCFileDialog::getSaveFileName(QString::null,"RTF Files (*.rtf);;Plain Text Files (*.txt *.log)",
//                                             this, QString("Save Report Output"));
  QString fn = FCFileDialog::getSaveFileName(QString::null,"Plain Text Files (*.txt *.log)", this, QObject::tr("Save Report Output"));
  if (!fn.isEmpty())
  {
    int dot = fn.find('.');
    if (dot != -1)
    {
      QString ext = fn.right(fn.length() - dot - 1).lower();
      if (ext == QString("rtf"))
      {
        QFile f(fn);
        if (f.open(IO_WriteOnly))
        {
          QTextStream t (&f);
          t << text();
          f.close();
        }
      }
      else if (ext == QString("txt") || ext == QString("log"))
      {
        QFile f(fn);
        if (f.open(IO_WriteOnly))
        {
          QTextStream t (&f);
          for (std::vector<QString>::iterator it = d->alOriginal.begin(); it != d->alOriginal.end(); ++it)
          {
            t << *it;
          }
          f.close();
        }
      }
    }
  }
}

#include "moc_ReportView.cpp"
