/** \file $RCSfile$
 *  \brief he windows base class, father of all windows
 *  \author $Author$
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
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/
 

#include "PreCompiled.h"
#include <qtabwidget.h>


#ifndef _PreComp_
#endif


#include "../Base/Console.h"
#include "Window.h"
#include "Application.h"
#include "../App/Application.h"



//**************************************************************************
//**************************************************************************
// FCWindowParameter
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

FCWindowParameter::FCWindowParameter(const char *name)
{
	FCHandle<FCParameterGrp> h;

	// not allowed to use a FCWindow without a name, see the constructor of a FCDockWindow or a other QT Widget
	assert(name);
	//printf("Instanceate:%s\n",name);

	// geting the group for the window
	h = GetApplication().GetSystemParameter().GetGroup("BaseApp");
	h = h->GetGroup("Windows");
	//h = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Windows/");

	_handle = h->GetGroup(name);



}

FCWindowParameter::~FCWindowParameter()
{

}

void FCWindowParameter::OnParameterChanged(void)
{
	GetConsole().Warning("FCWindowParameter::OnParameterChanged(): Parameter has changed and window (%s) has not overriden this function!",_handle->GetGroupName());
}


FCParameterGrp::handle  FCWindowParameter::GetWindowParameter(void)
{
	return _handle;
}

FCParameterGrp::handle  FCWindowParameter::GetParameter(void)
{
	return GetApplication().GetUserParameter().GetGroup("BaseApp");
}

ApplicationWindow* FCWindowParameter::GetAppWnd(void)
{
	return ApplicationWindow::Instance;
}

//**************************************************************************
//**************************************************************************
// FCWindow
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

FCDockWindow::FCDockWindow(QWidget *parent, const char *name, WFlags f)
	:QWidget(parent,name,f),
	FCWindowParameter(name)
{
}


FCDockWindow::~FCDockWindow()
{
}

//**************************************************************************
//**************************************************************************
// FCViewContainer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction
/*
FCViewContainer::FCViewContainer(QWidget* parent, const char* name, int wflags )
    :QextMdiChildView( parent, name, wflags ),
	 FCWindowParameter(name)
{
		
}

FCViewContainer::~FCViewContainer()
{
  
}
*/

/* 
 *  Constructs a FCReportOutput which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
FCReportOutput::FCReportOutput( QWidget* parent,  const char* name, WFlags fl )
    : FCDockWindow( parent, name, fl )
{
  if ( !name )
  	setName( "FCReportOutput" );

  resize( 529, 162 ); 
  tabLayout = new QGridLayout( this ); 
  tabLayout->setSpacing( 0 );
  tabLayout->setMargin( 0 );

  tab = new QTabWidget( this, "TabWidget" );
  tab->setProperty( "tabPosition", (int)QTabWidget::Bottom );
  tab->setProperty( "tabShape", (int)QTabWidget::Triangular );


  mle = new FCLogOutput( tab, "LogOutput" );
  tab->insertTab( mle, tr( "Output" ) );
  tabLayout->addWidget( tab, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCReportOutput::~FCReportOutput()
{
    // no need to delete child widgets, Qt does it all for us
}

// ----------------------------------------------------------

FCLogOutput::FCLogOutput(QWidget* parent, const char* name)
 : QTextBrowser(parent, name)
{
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

FCLogOutput::~FCLogOutput()
{
  GetConsole().DetacheObserver(this);
}

void FCLogOutput::appendLog(const char * s, const char * color)
{
  alOriginal.push_back(s);
  QString qs;

  if (color)
  {
    if (strchr(s, '\n') != NULL)
      qs = tr("<font color=\"%1\">%2</font>\n<br/>").arg(color).arg(s);
    else
      qs = tr("<font color=\"%1\">%2</font>").arg(color).arg(s);
  }
  else
  {
    if (strchr(s, '\n') != NULL)
      qs = tr("%1<br/>").arg(s);
    else
      qs = tr("%1").arg(s);
  }
  
  append(qs);
  setContentsPos(0, contentsHeight());
}

void FCLogOutput::Warning(const char * s)
{
  appendLog(s, "orange");
}

void FCLogOutput::Message(const char * s)
{
  appendLog(s);
}

void FCLogOutput::Error  (const char * s)
{
  appendLog(s, "red");
}

void FCLogOutput::Log (const char * s)
{
  // ignore this
}

bool FCLogOutput::event( QEvent* ev )
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

void FCLogOutput::viewportMousePressEvent (QMouseEvent * e)
{
  if (e->button() == RightButton)
  {
    QPopupMenu menu;
    menu.insertItem("Copy" , this, SLOT(copy()));
    menu.insertItem("Clear", this, SLOT(onClear()));
    menu.insertSeparator();
    menu.insertItem("Select All", this, SLOT(selectAll()));
    menu.insertSeparator();
    menu.insertItem("Save As...", this, SLOT(onSaveAs()));
    menu.exec(e->globalPos());
  }
  else
    QTextBrowser::viewportMousePressEvent(e);
}

void FCLogOutput::onClear()
{
//  setText("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n");
  setText("");
}

void FCLogOutput::onSaveAs()
{
//  QString fn = FCFileDialog::getSaveFileName(QString::null,"RTF Files (*.rtf);;Plain Text Files (*.txt *.log)",
//                                             this, QString("Save Report Output"));
  QString fn = FCFileDialog::getSaveFileName(QString::null,"Plain Text Files (*.txt *.log)", this, QString("Save Report Output"));
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
          for (std::vector<QString>::iterator it = alOriginal.begin(); it != alOriginal.end(); ++it)
          {
            t << *it;
          }
          f.close();
        }
      }
    }
  }
}

#include "moc_Window.cpp"
