/** \file Application.cpp
 *  \brief
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile
 *  time! So every external header (libs and system) should included in
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
// standard
#	include <iostream>
#	include <stdio.h>
#	include <assert.h>
#	include <fcntl.h>
#	include <ctype.h>
#	include <typeinfo>
// STL
#	include <vector>
#	include <map>
#	include <string>
#	include <list>
#	include <set>
#	include <algorithm>
#	include <stack>
#	include <queue>
#	include <string>
// Python
#	if defined (_POSIX_C_SOURCE)
#		undef  _POSIX_C_SOURCE
#	endif // (re-)defined in pyconfig.h
#	include <Python.h>
#	include <limits.h>
#	include <algorithm>
// Qt
#	include <qapplication.h>
#	include <qaction.h>
#	include <qbuttongroup.h>
#	include <qcombobox.h>
#	include <qcursor.h>
#	include <qlabel.h>
#	include <qmenubar.h>
#	include <qmessagebox.h>
#	include <qpopupmenu.h>
#	include <qprocess.h>
#	include <qsplashscreen.h>
#	include <qstatusbar.h>
#	include <qstyle.h>
#	include <qstylefactory.h>
#	include <qsyntaxhighlighter.h>
#	include <qtabbar.h>
#	include <qtabwidget.h>
#	include <qtextbrowser.h>
#	include <qthread.h>
#	include <qtimer.h>
#	include <qtoolbar.h>
#	include <qvbox.h>
#	include <qworkspace.h>
#endif


// FreeCAD Base header
#include "../Base/Console.h"
#include "../Base/Interpreter.h"
#include "../Base/Parameter.h"
#include "../Base/Exception.h"
#include "../Base/EnvMacros.h"
#include "../Base/Factory.h"
#include "../App/Application.h"

#include "Application.h"
#include "Document.h"
#include "View.h"
//#include "CommandStd.h"
//#include "CommandView.h"
#include "../Base/Documentation.h"

#include "Icons/developers.h"
#include "Icons/FCIcon.xpm"
#include "Splashscreen.h"
#include "WidgetFactory.h"
#include "PrefWidgets.h"
#include "Command.h"
#include "Tree.h"
#include "PropertyView.h"

#include "CommandLine.h"
#include "DlgDocTemplatesImp.h"
#include "DlgTipOfTheDayImp.h"
#include "DlgUndoRedo.h"
#include "ButtonGroup.h"
#include "HtmlView.h"
#include "ReportView.h"
#include "Macro.h"
#include "Themes.h"

#include "Inventor/Qt/SoQt.h"

#include "Language/LanguageFactory.h"

#include "GuiInitScript.h"

using Base::Console;
using Base::Interpreter;
using namespace Gui::DockWnd;


static ApplicationWindow* stApp;
//static QWorkspace* stWs;


ApplicationWindow* ApplicationWindow::Instance = 0L;

// Pimpl class
struct ApplicationWindowP
{
  ApplicationWindowP()
    : toolbars(0L), cmdbars(0L), viewbar(0L), _pcActiveDocument(0L),
    _bIsClosing(false), _bControlButton(false)
  {
	  // create the macro manager
	  _pcMacroMngr = new FCMacroManager();
  }

  ~ApplicationWindowP()
  {
    viewbar = 0L;
    delete toolbars;
    delete cmdbars;
    delete _pcWidgetMgr;
    delete _pcMacroMngr;
  }

  QPopupMenu* toolbars;
  QPopupMenu* cmdbars;
  FCPopupMenu* viewbar;
  std::map<int, QWidget*> mCheckBars;
	/// list of all handled documents
 	std::list<FCGuiDocument*>         lpcDocuments;
	/// list of windows
	std::map <std::string,FCWindow*> mpcDocWindows;
	/// Active document
	FCGuiDocument*   _pcActiveDocument;
	FCCustomWidgetManager*		 _pcWidgetMgr;
	FCMacroManager*  _pcMacroMngr;
	QLabel *         _pclSizeLabel, *_pclActionLabel;
	CStackBar*        _pcStackBar;
	/// workbench python dictionary
	PyObject*		 _pcWorkbenchDictionary;
	QString			 _cActiveWorkbenchName;
	QTimer *		 _pcActivityTimer; 
	/// List of all registered views
	std::list<FCBaseView*>					_LpcViews;
	bool _bIsClosing;
  // store it if the CTRL button is pressed or released
  bool _bControlButton;
	/// Handels all commands 
	FCCommandManager _cCommandManager;
};

ApplicationWindow::ApplicationWindow()
    : QextMdiMainFrm( 0, "Main window", WDestructiveClose )
{
	std::string language = GetApplication().GetUserParameter
		().GetGroup("BaseApp")->GetGroup("Window")->GetGroup
		("Language")->GetASCII("Language", "English");
	Gui::LanguageFactory().SetLanguage(language.c_str());
	GetWidgetFactorySupplier();

	// seting up Python binding
	(void) Py_InitModule("FreeCADGui", ApplicationWindow::Methods);

	// init the Inventor subsystem
	SoQt::init(this);

  d = new ApplicationWindowP;

	setCaption( "Qt-FreeCAD" );
	setIcon(QPixmap(FCIcon));

	d->_cActiveWorkbenchName="<none>";

	// global access 
	Instance = this;

	stApp = this;

	// instanciate the workbench dictionary
	d->_pcWorkbenchDictionary = PyDict_New();

    // attach the console observer
	Base::Console().AttacheObserver( new GuiConsoleObserver(this) );


	// setting up the Bitmap manager
//	QString tmpWb = _cActiveWorkbenchName;
//	_cBmpFactory.GetPixmap("Function");
/*
  QDir dir(GetApplication().GetHomePath()); 
  QString root = dir.path();
	GetDocumentationManager().AddProvider(new FCDocProviderDirectory("FCDoc:/"          ,(root + "/Doc/Online\\"   ).latin1()));
	GetDocumentationManager().AddProvider(new FCDocProviderDirectory("FCDoc:/Framework/",(root + "/Doc/FrameWork\\").latin1()));

	std::string test =  GetDocumentationManager().Retrive("FCDoc:/index", Html );

	test =  GetDocumentationManager().Retrive("FCDoc:/Framework/index", Html );
*/

  // animation
  FCAnimation::Instance()->reparent(statusBar(), QPoint());
  statusBar()->addWidget(FCAnimation::Instance(),0,true);
	// labels and progressbar
	statusBar()->addWidget(FCProgressBar::Instance(),0,true);
	//FCProgressBar::Instance().setFixedWidth(200);
	//_pclActionLabel = new QLabel("Ready", statusBar(), "Action");
	//_pclActionLabel->setFixedWidth(120);
	//statusBar()->addWidget(_pclActionLabel,0,true);
	d->_pclSizeLabel = new QLabel("Dimension", statusBar(), "Dimension");
	d->_pclSizeLabel->setFixedWidth(120);
	statusBar()->addWidget(d->_pclSizeLabel,0,true);

	// update gui timer
	d->_pcActivityTimer = new QTimer( this );
    connect( d->_pcActivityTimer, SIGNAL(timeout()),this, SLOT(UpdateCmdActivity()) );
    d->_pcActivityTimer->start( 4000, TRUE );                 // 4 seconds single-shot (wait until the gui is up)


	// Command Line +++++++++++++++++++++++++++++++++++++++++++++++++++
	GetCmdLine().SetParent(statusBar());
	statusBar()->addWidget(&FCCommandLine::Instance(), 0, true);
  statusBar()->message( tr("Ready"), 2001 );

	// Cmd Button Group +++++++++++++++++++++++++++++++++++++++++++++++
	d->_pcStackBar = new CStackBar(this,"Cmd_Group");
	d->_pcWidgetMgr = new FCCustomWidgetManager(GetCommandManager(), d->_pcStackBar);
	d->_pcWidgetMgr->addDockWindow( "Command bar",d->_pcStackBar, NULL, KDockWidget::DockRight, 83);

	// Html View ++++++++++++++++++++++++++++++++++++++++++++++++++++++
	FCParameterGrp::handle hURLGrp = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Windows/HelpViewer");
	QString home = QString(hURLGrp->GetASCII("LineEditURL", "index.php@OnlineDocumentation.html").c_str());
	FCHtmlView* pcHtmlView = new FCHtmlView(home, this, "HelpViewer");
	d->_pcWidgetMgr->addDockWindow("Help bar", pcHtmlView,"Command bar", KDockWidget::DockBottom, 40);


	// Tree Bar  ++++++++++++++++++++++++++++++++++++++++++++++++++++++	
	FCTree* pcTree = new FCTree(0,0,"Raw_tree");
	pcTree->setMinimumWidth(210);
	d->_pcWidgetMgr->addDockWindow("Tree bar", pcTree,0, KDockWidget::DockLeft, 0);

	// PropertyView  ++++++++++++++++++++++++++++++++++++++++++++++++++++++	
	FCPropertyView* pcPropView = new FCPropertyView(0,0,"PropertyView");
	pcPropView->setMinimumWidth(210);
	d->_pcWidgetMgr->addDockWindow("Property View", pcPropView,"Tree bar", KDockWidget::DockBottom, 60);

	// Report View
	Gui::DockWnd::CReportView* pcOutput = new Gui::DockWnd::CReportView(this,"CReportView");
	d->_pcWidgetMgr->addDockWindow("Report View", pcOutput, 0, KDockWidget::DockBottom, 90);

 	CreateStandardOperations();

	
	
	// misc stuff
  LoadWindowSettings();
  //  resize( 800, 600 );
	//setBackgroundPixmap(QPixmap((const char*)FCBackground));
	//setUsesBigPixmaps (true);

  FCParameterGrp::handle hGrp = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Macro");
  if (hGrp->HasGroup("Macros"))
  {
    hGrp = hGrp->GetGroup("Macros");
    std::vector<FCHandle<FCParameterGrp> > macros = hGrp->GetGroups();
    for (std::vector<FCHandle<FCParameterGrp> >::iterator it = macros.begin(); it!=macros.end(); ++it )
    {
      FCScriptCommand* pScript = new FCScriptCommand((*it)->GetGroupName());
      pScript->SetScriptName((*it)->GetASCII("Script").c_str());
      pScript->SetMenuText((*it)->GetASCII("Menu").c_str());
      pScript->SetToolTipText((*it)->GetASCII("Tooltip").c_str());
      pScript->SetWhatsThis((*it)->GetASCII("WhatsThis").c_str());
      pScript->SetStatusTip((*it)->GetASCII("Statustip").c_str());
      if ((*it)->GetASCII("Pixmap", "nix") != "nix") pScript->SetPixmap((*it)->GetASCII("Pixmap").c_str());
        pScript->SetAccel((*it)->GetInt("Accel"));
      d->_cCommandManager.AddCommand(pScript);
    }
  }

  // load recent file list
  hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp");
  if (hGrp->HasGroup("Recent files"))
  {
    hGrp = hGrp->GetGroup("Recent files");
    FCCommand* pCmd = d->_cCommandManager.GetCommandByName("Std_MRU");
    if (pCmd)
    {
      ((FCCmdMRU*)pCmd)->SetMaxItems(hGrp->GetInt("RecentFiles", 4));
      std::vector<std::string> MRU = hGrp->GetASCIIs("MRU");
      for (std::vector<std::string>::iterator it = MRU.begin(); it!=MRU.end();++it)
      {
        ((FCCmdMRU*)pCmd)->AddItem(it->c_str());
      }
    }
  }
}

ApplicationWindow::~ApplicationWindow()
{
  std::vector<FCCommand*> macros = d->_cCommandManager.GetModuleCommands("Macro");
  if (macros.size() > 0)
  {
    FCParameterGrp::handle hGrp = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Macro")->GetGroup("Macros");
    for (std::vector<FCCommand*>::iterator it = macros.begin(); it!=macros.end(); ++it )
    {
      FCScriptCommand* pScript = (FCScriptCommand*)(*it);
      FCParameterGrp::handle hMacro = hGrp->GetGroup(pScript->GetName());
      hMacro->SetASCII("Script", pScript->GetScriptName());
      hMacro->SetASCII("Menu", pScript->GetMenuText());
      hMacro->SetASCII("Tooltip", pScript->GetToolTipText());
      hMacro->SetASCII("WhatsThis", pScript->GetWhatsThis());
      hMacro->SetASCII("Statustip", pScript->GetStatusTip());
      hMacro->SetASCII("Pixmap", pScript->GetPixmap());
      hMacro->SetInt("Accel", pScript->GetAccel());
    }
  }

  // save recent file list
  FCCommand* pCmd = d->_cCommandManager.GetCommandByName("Std_MRU");
  if (pCmd)
  {
    char szBuf[200];
    int i=0;
    FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("Recent files");
    hGrp->Clear();
    hGrp->GetInt("RecentFiles", ((FCCmdMRU*)pCmd)->GetMaxItems());

    std::vector<std::string> MRU = ((FCCmdMRU*)pCmd)->GetItems();
    if (MRU.size() > 0)
    {
      for (std::vector<std::string>::iterator it = MRU.begin(); it!=MRU.end();++it,i++)
      {
        sprintf(szBuf, "MRU%d", i);
        hGrp->SetASCII(szBuf, it->c_str());
      }
    }
  }

	SaveWindowSettings();
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating std commands
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



void ApplicationWindow::CreateStandardOperations()
{
	// register the application Standard commands from CommandStd.cpp
	CreateStdCommands();
	CreateViewStdCommands();
	CreateTestCommands();

  setMenuForSDIModeSysButtons( menuBar());
}

void ApplicationWindow::Polish()
{
  d->viewbar  = d->_pcWidgetMgr->getPopupMenu("View");
  d->viewbar->setCanModify(true);
  d->toolbars = new QPopupMenu(d->viewbar, "Toolbars");
  d->cmdbars  = new QPopupMenu(d->viewbar, "Cmdbars");

  connect(d->viewbar,  SIGNAL(aboutToShow (   )), this, SLOT(OnShowView(     )));
  connect(d->viewbar,  SIGNAL(activated ( int )), this, SLOT(OnShowView( int )));
  connect(d->toolbars, SIGNAL(activated ( int )), this, SLOT(OnShowView( int )));
  connect(d->cmdbars,  SIGNAL(activated ( int )), this, SLOT(OnShowView( int )));
}

void ApplicationWindow::OnShowView()
{
  FCPopupMenu* menu = d->viewbar;
  menu->update(GetCommandManager());
  menu->setCheckable(true);
  d->mCheckBars.clear();

  // toolbars
  {
    QPopupMenu* m = d->toolbars;
    m->clear();
    std::vector<FCToolBar*> tb = d->_pcWidgetMgr->getToolBars();
    for (std::vector<FCToolBar*>::iterator it = tb.begin(); it!=tb.end(); ++it)
    {
      int id = m->insertItem(tr((*it)->name()));
      d->mCheckBars[id] = *it;
      if ((*it)->isVisible())
		    m->setItemChecked(id, true);
    }

    m->insertSeparator();
    int id = m->insertItem(tr("Taskbar"));
    d->mCheckBars[id] = m_pTaskBar;
    if (m_pTaskBar->isVisible())
		  m->setItemChecked(id, true);

    menu->insertItem(tr("Toolbars"), m);
  }

  // command bars
  {
    QPopupMenu* m = d->cmdbars;
    m->clear();
    std::vector<FCToolBar*> tb = d->_pcWidgetMgr->getCmdBars();
    for (std::vector<FCToolBar*>::iterator it = tb.begin(); it!=tb.end(); ++it)
    {
      int id = m->insertItem(tr((*it)->name()));
      d->mCheckBars[id] = *it;
      if (d->_pcStackBar->isPageVisible(*it))
		    m->setItemChecked(id, true);
    }

    menu->insertItem(tr("Commandbars"), m);
  }
  menu->insertSeparator();

  // dock windows
  std::vector<FCDockWindow*> windows = d->_pcWidgetMgr->getDockWindows();
  for (std::vector<FCDockWindow*>::iterator it = windows.begin(); it!=windows.end(); ++it)
  {
    KDockWidget* w = manager()->getDockWidgetFromName((*it)->name());
    if (w)
    {
      int id = menu->insertItem(tr(w->tabPageLabel()));
      d->mCheckBars[id] = w;
      if (w->isVisible())
		    menu->setItemChecked(id, true);
    }
  }

  // status bar
  menu->insertSeparator();
  QWidget* w = statusBar();
  int id = menu->insertItem(tr("Status bar"));
  d->mCheckBars[id] = w;
  if (w->isVisible())
		menu->setItemChecked(id, true);
}

void ApplicationWindow::OnShowView(int id)
{
  if (d->mCheckBars.find(id)==d->mCheckBars.end())
    return; // not a dock window
  if (!d->mCheckBars[id])
    return; // no valid dock window

  QPopupMenu* menu = (QPopupMenu*)sender();

  if (menu->isItemChecked(id))
  {
    if (d->mCheckBars[id]->inherits("KDockWidget"))
      ((KDockWidget*)d->mCheckBars[id])->changeHideShowState();
    else if (d->_pcStackBar->hasView(d->mCheckBars[id]))
      d->_pcStackBar->hidePage(d->mCheckBars[id]);
    else
      d->mCheckBars[id]->hide();
  }
  else
  {
    if (d->mCheckBars[id]->inherits("KDockWidget"))
      ((KDockWidget*)d->mCheckBars[id])->changeHideShowState();
    else if (d->_pcStackBar->hasView(d->mCheckBars[id]))
      d->_pcStackBar->showPage(d->mCheckBars[id]);
    else
      d->mCheckBars[id]->show();
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// document observers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ApplicationWindow::OnDocNew(FCDocument* pcDoc)
{
	d->lpcDocuments.push_back( new FCGuiDocument(pcDoc,this) );
}

void ApplicationWindow::OnDocDelete(FCDocument* pcDoc)
{
	FCGuiDocument* pcGDoc;

	for(std::list<FCGuiDocument*>::iterator It = d->lpcDocuments.begin();It != d->lpcDocuments.end();It++)
	{
		if( ((*It)->GetDocument()) == pcDoc)
		{
			pcGDoc = *It;
			d->lpcDocuments.erase(It);
			delete pcGDoc;
		}
	}
	
}

void ApplicationWindow::OnLastWindowClosed(FCGuiDocument* pcDoc)
{
	if(!d->_bIsClosing)
	{
		// GuiDocument has closed the last window and get destructed
		d->lpcDocuments.remove(pcDoc);
		//lpcDocuments.erase(pcDoc);
		delete pcDoc;
		
		// last document closed?
		if(d->lpcDocuments.size() == 0 )
			// reset active document
			SetActiveDocument(0);
	}
}

// set text to the pane
void ApplicationWindow::SetPaneText(int i, QString text)
{
  if (i==1)
    d->_pclActionLabel->setText(text);
  else if (i==2)
    d->_pclSizeLabel->setText(text);
}



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// view handling
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



/// send Messages to the active view
bool ApplicationWindow::SendMsgToActiveView(const char* pMsg)
{
	FCView* pView = GetActiveView();

	if(pView){
		return pView->OnMsg(pMsg);
	}else
		return false;
}

bool ApplicationWindow::SendHasMsgToActiveView(const char* pMsg)
{
	FCView* pView = GetActiveView();

	if(pView){
		return pView->OnHasMsg(pMsg);
	}else
		return false;
}


FCView* ApplicationWindow::GetActiveView(void)
{
	FCView * pView = reinterpret_cast <FCView *> ( activeWindow() );
	return pView;
}


/// Geter for the Active View
FCGuiDocument* ApplicationWindow::GetActiveDocument(void)
{
	return d->_pcActiveDocument;
	/*
	FCView* pView = GetActiveView();

	if(pView)
		return pView->GetGuiDocument();
	else
		return 0l;*/
}

void ApplicationWindow::SetActiveDocument(FCGuiDocument* pcDocument)
{
	d->_pcActiveDocument=pcDocument;

	Console().Log("Activate Document (%p) \n",d->_pcActiveDocument);

	// notify all views attached to the application (not views belong to a special document)
	for(std::list<FCBaseView*>::iterator It=d->_LpcViews.begin();It!=d->_LpcViews.end();It++)
		(*It)->SetDocument(pcDocument);

}

void ApplicationWindow::AttachView(FCBaseView* pcView)
{
	d->_LpcViews.push_back(pcView);
}


void ApplicationWindow::DetachView(FCBaseView* pcView)
{
	d->_LpcViews.remove(pcView);
}

void ApplicationWindow::Update(void)
{
	// update all documents
	for(std::list<FCGuiDocument*>::iterator It = d->lpcDocuments.begin();It != d->lpcDocuments.end();It++)
	{
		(*It)->Update();
	}
	// update all the independed views
	for(std::list<FCBaseView*>::iterator It2 = d->_LpcViews.begin();It2 != d->_LpcViews.end();It2++)
	{
		(*It2)->Update();
	}

}

/// get calld if a view gets activated, this manage the whole activation scheme
void ApplicationWindow::ViewActivated(FCView* pcView)
{

	Console().Log("Activate View (%p) Type=\"%s\" \n",pcView,pcView->GetName());
	// set the new active document
	if(pcView->IsPassiv())
		SetActiveDocument(0);
	else
		SetActiveDocument(pcView->GetGuiDocument());
}


void ApplicationWindow::UpdateActive(void)
{
	GetActiveDocument()->Update();
}


void ApplicationWindow::OnUndo()
{
	puts("ApplicationWindow::slotUndo()");
}

void ApplicationWindow::OnRedo()
{
	puts("ApplicationWindow::slotRedo()");

}



/** just additionally fits the system menu button position to the menu position */
void ApplicationWindow::resizeEvent ( QResizeEvent *e)
{
   QextMdiMainFrm::resizeEvent( e);
   setSysButtonsAtMenuPosition();
}

void ApplicationWindow::closeEvent ( QCloseEvent * e )
{

	if(d->lpcDocuments.size() == 0)
	{
		e->accept();
	}else{
		// ask all documents if closable
		for (std::list<FCGuiDocument*>::iterator It = d->lpcDocuments.begin();It!=d->lpcDocuments.end();It++)
		{
			(*It)->CanClose ( e );
//			if(! e->isAccepted() ) break;
			if(! e->isAccepted() ) return;
		}
	}

	// ask all passiv views if closable
	for (std::list<FCBaseView*>::iterator It2 = d->_LpcViews.begin();It2!=d->_LpcViews.end();It2++)
	{
		if((*It2)->CanClose() )
			e->accept();
		else 
			e->ignore();

//		if(! e->isAccepted() ) break;
		if(! e->isAccepted() ) return;
	}

	if( e->isAccepted() )
	{
		d->_bIsClosing = true;

		std::list<FCGuiDocument*>::iterator It;

		// close all views belonging to a document
		for (It = d->lpcDocuments.begin();It!=d->lpcDocuments.end();It++)
		{
			(*It)->CloseAllViews();
		}

		//detache the passiv views
		//SetActiveDocument(0);
		std::list<FCBaseView*>::iterator It2 = d->_LpcViews.begin();
		while (It2!=d->_LpcViews.end())
		{
			(*It2)->Close();
			It2 = d->_LpcViews.begin();
		}

		// remove all documents
		for (It = d->lpcDocuments.begin();It!=d->lpcDocuments.end();It++)
		{
			delete(*It);
		}

		d->_pcActivityTimer->stop();

    ActivateWorkbench("<none>");
		QextMdiMainFrm::closeEvent(e);
	}
}

bool ApplicationWindow::eventFilter( QObject* o, QEvent *e )
{
  // show menu with all available toolbars
  if (isDockMenuEnabled () && e->type() == QEvent::MouseButtonPress && o == this && ((QMouseEvent*)e)->button() == RightButton)
  {
    QPoint p = ((QMouseEvent*)e)->globalPos();
    QPopupMenu menu;
    menu.setCheckable(true);

    std::map<int, QToolBar*> toolb;

    std::vector<FCToolBar*> aclToolBars = d->_pcWidgetMgr->getToolBars();
	  for (std::vector<FCToolBar*>::iterator It = aclToolBars.begin(); It != aclToolBars.end(); ++It)
    {
      int id = menu.insertItem(tr((*It)->name()));
      QToolBar* tb = *It;
      toolb[id] = tb;
      if (tb->isVisible())
		    menu.setItemChecked(id, true);
    }
    if (m_pTaskBar)
    {
      int id = menu.insertItem(tr("Taskbar"));
      toolb[id] = m_pTaskBar;
      if (m_pTaskBar->isVisible())
		    menu.setItemChecked(id, true);
    }

    menu.insertSeparator();
    int lineUp1 = menu.insertItem( tr( "Line Up Toolbars (compact)" ) );
    int lineUp2 = menu.insertItem( tr( "Line Up Toolbars (normal)" ) );

    int id = menu.exec(p);
    if (id == lineUp1) 
    {
    	lineUpToolBars(false);
    } 
    else if (id == lineUp2) 
    {
    	lineUpToolBars(true);
    } 
    else if (toolb.find(id) != toolb.end()) 
    {
      QToolBar* tb = toolb[id];
      if (menu.isItemChecked(id))
      {
        tb->hide();
      }
      else 
      {
        tb->show();
    	}
    }
    
    return true;
  }

  return QextMdiMainFrm::eventFilter(o, e);
}

bool ApplicationWindow::focusNextPrevChild( bool next )
{
  if (d->_bControlButton)
  {
    if (next)
      activateNextWin();
    else
      activatePrevWin();

    return (activeWindow() != NULL);
  }

  return QextMdiMainFrm::focusNextPrevChild(next);
}

void ApplicationWindow::keyPressEvent ( QKeyEvent * e )
{
  d->_bControlButton = (e->state() &  ControlButton);
  QextMdiMainFrm::keyPressEvent(e);
}

void ApplicationWindow::keyReleaseEvent ( QKeyEvent * e )
{
  d->_bControlButton = (e->state() &  ControlButton);
  QextMdiMainFrm::keyReleaseEvent (e);
}

/**
 *  Activate the named workbench by calling the methodes in the 
 *  python workbench object. If the workbench is allready active
 *  nothing get called!
 */
void ApplicationWindow::ActivateWorkbench(const char* name)
{
	// net buffer because of char* <-> const char*
	Base::PyBuf Name(name);

	// close old workbench
	if(d->_cActiveWorkbenchName != "")
	{
		Base::PyBuf OldName ( d->_cActiveWorkbenchName.latin1());
		PyObject* pcOldWorkbench = PyDict_GetItemString(d->_pcWorkbenchDictionary, OldName.str);
		assert(pcOldWorkbench);
		Interpreter().RunMethodVoid(pcOldWorkbench, "Stop");
	}
	// get the python workbench object from the dictionary
	PyObject* pcWorkbench = PyDict_GetItemString(d->_pcWorkbenchDictionary, Name.str);

  try{
	  // test if the workbench in
	  assert(pcWorkbench);
    if (!pcWorkbench)
    {
      QString exc = tr("Workbench '%1' does not exist").arg(name);
      throw FCException(exc.latin1());
    }

    // rename with new workbench before(!!!) calling "Start"
	  d->_cActiveWorkbenchName = name;

	  // running the start of the workbench object
	  Interpreter().RunMethodVoid(pcWorkbench, "Start");
    d->_pcWidgetMgr->update(name);

	  // update the Std_Workbench command and its action object
    FCCommand* pCmd = d->_cCommandManager.GetCommandByName("Std_Workbench");
    if (pCmd)
    {
      ((FCCmdWorkbench*)pCmd)->UpdateAction(name);
    }

	  show();
  }
  catch (const FCException& rclE)
  {
    Console().Error("%s\n", rclE.what());
  }
}

void ApplicationWindow::UpdateWorkbenchEntrys(void)
{
	PyObject *key, *value;
	int pos = 0;
     
  FCCommand* pCmd = d->_cCommandManager.GetCommandByName("Std_Workbench");
  if (pCmd)
  {
  	// remove all items from the command
    ((FCCmdWorkbench*)pCmd)->Clear();

  	// insert all items
    while (PyDict_Next(d->_pcWorkbenchDictionary, &pos, &key, &value)) {
		  /* do something interesting with the values... */
      ((FCCmdWorkbench*)pCmd)->AddItem(PyString_AsString(key));
	  }
  }
}

std::vector<std::string> ApplicationWindow::GetWorkbenches(void)
{
	PyObject *key, *value;
	int pos = 0;
  std::vector<std::string> wb;
	// insert all items
	while (PyDict_Next(d->_pcWorkbenchDictionary, &pos, &key, &value)) {
		/* do something interesting with the values... */
		wb.push_back(PyString_AsString(key));
	}
  return wb;
}

void ApplicationWindow::AppendRecentFile(const char* file)
{
  FCCommand* pCmd = d->_cCommandManager.GetCommandByName("Std_MRU");
  if (pCmd)
  {
    ((FCCmdMRU*)pCmd)->AddItem(file);
  }
}

void ApplicationWindow::UpdateCmdActivity()
{
	static QTime cLastCall;

	if(cLastCall.elapsed() > 250 && isVisible () )
	{
		//puts("testActive");
		d->_cCommandManager.TestActive();
		// remember last call
		cLastCall.start();
	}

	d->_pcActivityTimer->start( 300, TRUE );	
}

void ApplicationWindow::LoadWindowSettings()
{
  LoadDockWndSettings();

  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
  int w = hGrp->GetInt("Width", 800);
  int h = hGrp->GetInt("Height", 600);
  int x = hGrp->GetInt("PosX", pos().x());
  int y = hGrp->GetInt("PosY", pos().y());
  bool max = hGrp->GetBool("Maximized", false);
  resize( w, h );
  move(x, y);
  if (max) showMaximized();
	//setBackgroundPixmap(QPixmap((const char*)FCBackground));

	UpdatePixmapsSize();
	UpdateStyle();
}

void ApplicationWindow::UpdatePixmapsSize(void)
{
  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
  bool bigPixmaps = hGrp->GetBool("BigPixmaps", false);
  if (bigPixmaps != usesBigPixmaps())
		setUsesBigPixmaps (bigPixmaps);
}

void ApplicationWindow::UpdateStyle(void)
{
  QStyle& curStyle = QApplication::style();
  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp");
	hGrp = hGrp->GetGroup("WindowSettings")->GetGroup ("WindowStyle");
	std::vector<std::string> aStyles = hGrp->GetASCIIs("Item");
	long pos = hGrp->GetInt("currentItem");
	if (long(aStyles.size()) > pos)
	{
		QString styleName = aStyles[pos].c_str();

		if (strcmp(styleName.latin1(), curStyle.name()) != 0)
		{
#if QT_VERSION > 300
		  QStyle* newStyle = QStyleFactory::create(styleName);

			if (newStyle != NULL)
			{
				QApplication::setStyle(newStyle);
			}
#endif
		}
	}
}

void ApplicationWindow::SaveWindowSettings()
{
  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
  if (isMaximized())
  {
    hGrp->SetBool("Maximized", true);
  }
  else
  {
    hGrp->SetInt("Width", width());
    hGrp->SetInt("Height", height());
    hGrp->SetInt("PosX", pos().x());
    hGrp->SetInt("PosY", pos().y());
    hGrp->SetBool("Maximized", false);
  }

  SaveDockWndSettings();
}

void ApplicationWindow::LoadDockWndSettings()
{
  // open file
  std::string FileName(GetApplication().GetHomePath());
  FileName += "FreeCAD.xml";
  QFile* datafile = new QFile(FileName.c_str());
  if (!datafile->open(IO_ReadOnly)) 
  {
    // error opening file
    bool bMute = GuiConsoleObserver::bMute;
    GuiConsoleObserver::bMute = true;
    Console().Warning((tr("Error: Cannot open file '%1' "
                             "(Maybe you're running FreeCAD the first time)\n").arg(FileName.c_str())));
    GuiConsoleObserver::bMute = bMute;
    datafile->close();
    delete (datafile);
    return;
  }

  // open dom document
  QDomDocument doc("DockWindows");
  if (!doc.setContent(datafile)) 
  {
    Console().Warning("Error:  is not a valid file\n");
    datafile->close();
    delete (datafile);
    return;
  }

  datafile->close();
  delete (datafile);

  // check the doc type and stuff
  if (doc.doctype().name() != "DockWindows") 
  {
    // wrong file type
    Console().Warning("Error: is not a valid file\n");
    return;
  }

  QDomElement root = doc.documentElement();
  if (root.attribute("application") != QString("FreeCAD")) 
  {
    // right file type, wrong application
    Console().Warning("Error: wrong file\n");
    return;
  }

  readDockConfig(root);
}

void ApplicationWindow::SaveDockWndSettings()
{
  // save dock window settings
  QDomDocument doc("DockWindows");

  // create the root element
  QDomElement root = doc.createElement(doc.doctype().name());
  root.setAttribute("version", "0.1");
  root.setAttribute("application", "FreeCAD");

  writeDockConfig(root);
  doc.appendChild(root);

  // save into file
  std::string FileName(GetApplication().GetHomePath());
  FileName += "FreeCAD.xml";
  QFile* datafile = new QFile (FileName.c_str());
  if (!datafile->open(IO_WriteOnly)) 
  {
    // error opening file
    Console().Warning("Error: Cannot open file\n");
    datafile->close();
    delete (datafile);
    return;
  }

  // write it out
  QTextStream textstream(datafile);
  doc.save(textstream, 0);
  datafile->close();
  delete (datafile);
}

void ApplicationWindow::setPalette(const QPalette& pal)
{
  QextMdiMainFrm::setPalette(pal);
  setAreaPal(pal);
}

void ApplicationWindow::setAreaPal(const QPalette& pal)
{
  m_pMdi->setMdiCaptionActiveBackColor(pal.disabled().background());
  m_pMdi->setMdiCaptionActiveForeColor(pal.active().foreground());
  m_pMdi->setMdiCaptionInactiveBackColor(pal.active().mid());
  m_pMdi->setMdiCaptionInactiveForeColor(pal.disabled().light());
}

bool ApplicationWindow::IsClosing(void)
{
  return d->_bIsClosing;
}

FCCustomWidgetManager* ApplicationWindow::GetCustomWidgetManager(void) 
{ 
  return d->_pcWidgetMgr; 
}

QString ApplicationWindow::GetActiveWorkbench(void)
{
  return d->_cActiveWorkbenchName;
}

FCMacroManager *ApplicationWindow::GetMacroMngr(void)
{
  return d->_pcMacroMngr;
}

FCCommandManager &ApplicationWindow::GetCommandManager(void)
{
  return d->_cCommandManager;
}

//**************************************************************************
// Init, Destruct and singelton

QApplication* ApplicationWindow::_pcQApp = NULL ;

QSplashScreen *ApplicationWindow::_splash = NULL;


void ApplicationWindow::InitApplication(void)
{
//	std::map<std::string,std::string> &Config = GetApplication().Config();


	new Base::ScriptProducer( "FreeCADGuiInit", FreeCADGuiInit );

}

void ApplicationWindow::RunApplication(void)
{
	// A new QApplication
	Console().Log("Creating GUI Application...\n");
	// if application not yet created by the splasher
	int argc = FCApplication::GetARGC();
	if (!_pcQApp)  _pcQApp = new QApplication (argc, FCApplication::GetARGV());

	StartSplasher();
	ApplicationWindow * mw = new ApplicationWindow();
	_pcQApp->setMainWidget(mw);

	// runing the Gui init script
	Interpreter().Launch(Base::ScriptFactory().ProduceScript("FreeCADGuiInit"));
	// show the main window
	Console().Log("Showing GUI Application...\n");
	mw->Polish();
	mw->show();
	StopSplasher();
	ShowTipOfTheDay();

	_pcQApp->connect( _pcQApp, SIGNAL(lastWindowClosed()), _pcQApp, SLOT(quit()) );

	// run the Application event loop
	Console().Log("Running event loop...\n");
	_pcQApp->exec();
	Console().Log("event loop left\n");
}

void ApplicationWindow::StartSplasher(void)
{
	// startup splasher
	// when running in verbose mode no splasher
	if ( ! (FCApplication::Config()["Verbose"] == "Strict") && (FCApplication::Config()["RunMode"] == "Gui") )
	{
		FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
		if (hGrp->GetBool("AllowSplasher", true))
		{
			QPixmap pixmap(( const char** ) splash_screen );
			_splash = new FCSplashScreen( pixmap );
			_splash->show();
		}
	}
}

void ApplicationWindow::StopSplasher(void)
{
	if ( _splash )
	{
		_splash->finish( Instance );
		delete _splash;
		_splash = 0L;
	}
}

void ApplicationWindow::ShowTipOfTheDay( bool force )
{
	// tip of the day?
	FCParameterGrp::handle
  hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
  bool tips = hGrp->GetBool("Tipoftheday", true);
	if ( tips || force)
	{
		Gui::Dialog::CDlgTipOfTheDayImp dlg(Instance, "Tipofday");
		dlg.exec();
	}
}

void ApplicationWindow::Destruct(void)
{
	Console().Log("Destruct GuiApplication\n");

	delete _pcQApp;

}




//**************************************************************************
// Python stuff

// FCApplication Methods						// Methods structure
PyMethodDef ApplicationWindow::Methods[] = {
	{"MenuAppendItems",       (PyCFunction) ApplicationWindow::sMenuAppendItems,         1},
	{"MenuRemoveItems",       (PyCFunction) ApplicationWindow::sMenuRemoveItems,         1},
	{"MenuDelete",            (PyCFunction) ApplicationWindow::sMenuDelete,              1},
	{"ToolbarAppendItems",    (PyCFunction) ApplicationWindow::sToolbarAppendItems,      1},
	{"ToolbarRemoveItems",    (PyCFunction) ApplicationWindow::sToolbarRemoveItems,      1},
	{"ToolbarDelete",         (PyCFunction) ApplicationWindow::sToolbarDelete,           1},
	{"CommandbarAppendItems", (PyCFunction) ApplicationWindow::sCommandbarAppendItems,   1},
	{"CommandbarRemoveItems", (PyCFunction) ApplicationWindow::sCommandbarRemoveItems,   1},
	{"CommandbarDelete",      (PyCFunction) ApplicationWindow::sCommandbarDelete,        1},
	{"WorkbenchAdd",          (PyCFunction) ApplicationWindow::sWorkbenchAdd,            1},
	{"WorkbenchDelete",       (PyCFunction) ApplicationWindow::sWorkbenchDelete,         1},
	{"WorkbenchActivate",     (PyCFunction) ApplicationWindow::sWorkbenchActivate,       1},
	{"WorkbenchGet",          (PyCFunction) ApplicationWindow::sWorkbenchGet,            1},
	{"UpdateGui",             (PyCFunction) ApplicationWindow::sUpdateGui,               1},
	{"CreateDialog",          (PyCFunction) ApplicationWindow::sCreateDialog,            1},
	{"CommandAdd",            (PyCFunction) ApplicationWindow::sCommandAdd,              1},
	{"RunCommand",            (PyCFunction) ApplicationWindow::sRunCommand,              1},
	{"SendMsgToActiveView",   (PyCFunction) ApplicationWindow::sSendActiveView,          1},

	{NULL, NULL}		/* Sentinel */
};

PYFUNCIMP_S(ApplicationWindow,sSendActiveView)
{
	char *psCommandStr;
	if (!PyArg_ParseTuple(args, "s",&psCommandStr))     // convert args: Python->C 
		return NULL;                                      // NULL triggers exception 

	Instance->SendMsgToActiveView(psCommandStr);
	
	Py_INCREF(Py_None);
	return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sUpdateGui)
{
	if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
		return NULL;                                      // NULL triggers exception 

	qApp->processEvents();
	
	Py_INCREF(Py_None);
	return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sCreateDialog)
{
  char* fn = 0;
	if (!PyArg_ParseTuple(args, "s", &fn))     // convert args: Python->C 
		return NULL;                                      // NULL triggers exception 

	PyObject* pPyResource=0L;
	try{
		pPyResource = new FCPyResource();
		((FCPyResource*)pPyResource)->load(fn);
	} catch (const FCException& e)
	{
    PyErr_SetString(PyExc_AssertionError, e.what());
	}

	return pPyResource;
} 

PYFUNCIMP_S(ApplicationWindow,sMenuAppendItems)
{
  PyObject* pObject;
  char* psMenuName;
  char* parent = 0;
  int bModify=1;
  int bRemovable=1;
  // convert args: Python->C 
  if (!PyArg_ParseTuple(args, "sO|iis", &psMenuName, &pObject, &bModify,
                                        &bRemovable, &parent ))     
    return NULL;                             // NULL triggers exception 
  if (!PyList_Check(pObject))
  {
    PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
    return NULL;                             // NULL triggers exception 
  }

  std::vector<std::string> aclItems;
  int nSize = PyList_Size(pObject);
  for (int i=0; i<nSize;++i)
  {
    PyObject* item = PyList_GetItem(pObject, i);
    if (!PyString_Check(item))
      continue;

    char* pItem = PyString_AsString(item);

    aclItems.push_back(pItem);
  }

  try{
    Instance->d->_pcWidgetMgr->addPopupMenu(psMenuName, aclItems, parent);
    Instance->d->_pcWidgetMgr->getPopupMenu(psMenuName)->setCanModify(bModify!=0);
    Instance->d->_pcWidgetMgr->getPopupMenu(psMenuName)->setRemovable(bRemovable!=0);
	}catch(const FCException& e) {
		PyErr_SetString(PyExc_AssertionError, e.what());		
		return NULL;
	}catch(...){
		PyErr_SetString(PyExc_RuntimeError, "unknown error");
		return NULL;
	}

  Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sMenuRemoveItems)
{
  PyObject* pObject;
  char* psMenuName;
  if (!PyArg_ParseTuple(args, "sO", &psMenuName, 
                                     &pObject))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 
  if (!PyList_Check(pObject))
  {
    PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
    return NULL;                             // NULL triggers exception 
  }

  std::vector<std::string> aclItems;
  int nSize = PyList_Size(pObject);
  for (int i=0; i<nSize;++i)
  {
    PyObject* item = PyList_GetItem(pObject, i);
    if (!PyString_Check(item))
      continue;

    char* pItem = PyString_AsString(item);

    aclItems.push_back(pItem);
  }

  Instance->d->_pcWidgetMgr->removeMenuItems(psMenuName, aclItems);

  Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sMenuDelete)
{
    char *psMenuName;
    if (!PyArg_ParseTuple(args, "s", &psMenuName))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	Instance->GetCustomWidgetManager()->delPopupMenu(psMenuName);

	Py_INCREF(Py_None);
  return Py_None;
}

PYFUNCIMP_S(ApplicationWindow,sToolbarAppendItems)
{
  PyObject* pObject;
  char* psToolbarName;
  int bModify=1;
  int bVisible=1;
  int bRemovable=1;
  // convert args: Python->C 
  if (!PyArg_ParseTuple(args, "sO|iii", &psToolbarName, &pObject, &bModify,
                                        &bRemovable, &bVisible ))     
    return NULL;                             // NULL triggers exception 
  if (!PyList_Check(pObject))
  {
    PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
		return NULL;                                      // NULL triggers exception 
  }

  std::vector<std::string> aclItems;
  int nSize = PyList_Size(pObject);
  for (int i=0; i<nSize;++i)
  {
    PyObject* item = PyList_GetItem(pObject, i);
    if (!PyString_Check(item))
      continue;

    char* pItem = PyString_AsString(item);

    aclItems.push_back(pItem);
  }

	try{
    Instance->d->_pcWidgetMgr->addToolBar(psToolbarName, aclItems);
    if (!bVisible)
      Instance->d->_pcWidgetMgr->getToolBar(psToolbarName)->hide();
    Instance->d->_pcWidgetMgr->getToolBar(psToolbarName)->setCanModify(bModify!=0);
    Instance->d->_pcWidgetMgr->getToolBar(psToolbarName)->setRemovable(bRemovable!=0);
	}catch(const FCException& e) {
		PyErr_SetString(PyExc_AssertionError, e.what());		
		return NULL;
	}catch(...){
		PyErr_SetString(PyExc_RuntimeError, "unknown error");
		return NULL;
	}

	Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sToolbarRemoveItems)
{
  PyObject* pObject;
  char* psToolbarName;
  if (!PyArg_ParseTuple(args, "sO", &psToolbarName, 
                                     &pObject))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 
  if (!PyList_Check(pObject))
  {
    PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
		return NULL;                             // NULL triggers exception 
  }

  std::vector<std::string> aclItems;
  int nSize = PyList_Size(pObject);
  for (int i=0; i<nSize;++i)
  {
    PyObject* item = PyList_GetItem(pObject, i);
    if (!PyString_Check(item))
      continue;

    char* pItem = PyString_AsString(item);

    aclItems.push_back(pItem);
  }

  Instance->d->_pcWidgetMgr->removeToolBarItems(psToolbarName, aclItems);
    
	Py_INCREF(Py_None);
	return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sToolbarDelete)
{
    char *psToolbarName;
    if (!PyArg_ParseTuple(args, "s", &psToolbarName))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	Instance->GetCustomWidgetManager()->delToolBar(psToolbarName);

	Py_INCREF(Py_None);
    return Py_None;
}

PYFUNCIMP_S(ApplicationWindow,sCommandbarAppendItems)
{
  PyObject* pObject;
  char* psToolbarName;
  int bModify=1;
  int bRemovable=1;
  // convert args: Python->C 
  if (!PyArg_ParseTuple(args, "sO|ii", &psToolbarName, &pObject,
                                      &bModify, &bRemovable))
    return NULL;                             // NULL triggers exception 
  if (!PyList_Check(pObject))
  {
    PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
		return NULL;                                      // NULL triggers exception 
  }

  std::vector<std::string> aclItems;
  int nSize = PyList_Size(pObject);
  for (int i=0; i<nSize;++i)
  {
    PyObject* item = PyList_GetItem(pObject, i);
    if (!PyString_Check(item))
      continue;

    char* pItem = PyString_AsString(item);

    aclItems.push_back(pItem);
  }

	try{
    Instance->d->_pcWidgetMgr->addCmdBar(psToolbarName, aclItems);
    Instance->d->_pcWidgetMgr->getCmdBar(psToolbarName)->setCanModify(bModify!=0);
    Instance->d->_pcWidgetMgr->getCmdBar(psToolbarName)->setRemovable(bRemovable!=0);
	}catch(const FCException& e) {
		PyErr_SetString(PyExc_AssertionError, e.what());		
		return NULL;
	}catch(...){
		PyErr_SetString(PyExc_RuntimeError, "unknown error");
		return NULL;
	}

	Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sCommandbarRemoveItems)
{
  PyObject* pObject;
  char* psToolbarName;
  if (!PyArg_ParseTuple(args, "sO", &psToolbarName, 
                                     &pObject))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 
  if (!PyList_Check(pObject))
  {
    PyErr_SetString(PyExc_AssertionError, "Expected a list as second argument");
		return NULL;                             // NULL triggers exception 
  }

  std::vector<std::string> aclItems;
  int nSize = PyList_Size(pObject);
  for (int i=0; i<nSize;++i)
  {
    PyObject* item = PyList_GetItem(pObject, i);
    if (!PyString_Check(item))
      continue;

    char* pItem = PyString_AsString(item);

    aclItems.push_back(pItem);
  }

  Instance->d->_pcWidgetMgr->removeCmdBarItems(psToolbarName, aclItems);

	Py_INCREF(Py_None);
	return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sCommandbarDelete)
{
    char *psToolbarName;
    if (!PyArg_ParseTuple(args, "s", &psToolbarName))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	Instance->GetCustomWidgetManager()->delCmdBar(psToolbarName);
    
	Py_INCREF(Py_None);
	return Py_None;
}


PYFUNCIMP_S(ApplicationWindow,sWorkbenchAdd)
{
	char*       psKey;
	PyObject*   pcObject;
	if (!PyArg_ParseTuple(args, "sO", &psKey,&pcObject))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	//Py_INCREF(pcObject);

	PyDict_SetItemString(Instance->d->_pcWorkbenchDictionary,psKey,pcObject);

	Instance->UpdateWorkbenchEntrys();

	Py_INCREF(Py_None);
	return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sWorkbenchDelete)
{
	char*       psKey;
	if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	PyDict_DelItemString(Instance->d->_pcWorkbenchDictionary,psKey);

  Instance->UpdateWorkbenchEntrys();

	Py_INCREF(Py_None);
  return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sWorkbenchActivate)
{
	char*       psKey;
	if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	Instance->ActivateWorkbench(psKey);

	Py_INCREF(Py_None);
    return Py_None;
}

PYFUNCIMP_S(ApplicationWindow,sWorkbenchGet)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	return Instance->d->_pcWorkbenchDictionary;
}

PYFUNCIMP_S(ApplicationWindow,sCommandAdd)
{
	char*       pName;
	PyObject*   pcCmdObj;
	if (!PyArg_ParseTuple(args, "sO", &pName,&pcCmdObj))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	//Py_INCREF(pcObject);

	ApplicationWindow::Instance->GetCommandManager().AddCommand(new FCPythonCommand(pName,pcCmdObj));

	Py_INCREF(Py_None);
	return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sRunCommand)
{
	char*       pName;
	if (!PyArg_ParseTuple(args, "s", &pName))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

  ApplicationWindow::Instance->GetCommandManager().RunCommandByName(pName);

	Py_INCREF(Py_None);
	return Py_None;
} 

//**************************************************************************
//**************************************************************************
// FCAppConsoleObserver
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool GuiConsoleObserver::bMute = false;

GuiConsoleObserver::GuiConsoleObserver(ApplicationWindow *pcAppWnd)
	:_pcAppWnd(pcAppWnd){}

/// get calles when a Warning is issued
void GuiConsoleObserver::Warning(const char *m)
{
	if(!bMute){
		QMessageBox::warning( _pcAppWnd, "Warning",m);
		_pcAppWnd->statusBar()->message( m, 2001 );
	}
}

/// get calles when a Message is issued
void GuiConsoleObserver::Message(const char * m)
{
	if(!bMute)
		_pcAppWnd->statusBar()->message( m, 2001 );
}

/// get calles when a Error is issued
void GuiConsoleObserver::Error  (const char *m)
{
	if(!bMute)
	{
		QMessageBox::critical( _pcAppWnd, "Exception happens",m);
		_pcAppWnd->statusBar()->message( m, 2001 );
	}
}

/// get calles when a Log Message is issued
void GuiConsoleObserver::Log    (const char *)
{
}

//**************************************************************************
//**************************************************************************
// FCAppConsoleObserver
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#include "moc_Application.cpp"
