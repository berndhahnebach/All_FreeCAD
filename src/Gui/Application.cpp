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
#	include <algorithm>
#	include <qapplication.h>
#	include <qvbox.h>
#	include <qworkspace.h>
#	include <qstatusbar.h>
#	include <qmenubar.h>
#	include <qmessagebox.h>
#	include <qtoolbar.h>
#	include <qpopupmenu.h>
#	include <qcombobox.h>
#	include <qstatusbar.h>
#endif


#include "../Base/Exception.h"
#include "../Base/Interpreter.h"
#include "../App/Application.h"

#if QT_VER != QT_VERSION
//#	error "QT Version missmatch, please set the right version in src/Config.h line 92"
#endif


#include "Application.h"
#include "Document.h"
#include "View.h"
//#include "CommandStd.h"
//#include "CommandView.h"
#include "../Base/Documentation.h"

#include "Splashscreen.h"
#include "WidgetFactory.h"
#include "PrefWidgets.h"
#include "Tree.h"
#include "Tools.h"
#include "PropertyView.h"

#include "CommandLine.h"
#include "DlgDocTemplatesImp.h"
#include "DlgUndoRedo.h"
#include "ButtonGroup.h"
#include "HtmlView.h"
#include "ReportView.h"
#include "Macro.h"
#include "Themes.h"

#include "Inventor/Qt/SoQt.h"

#include "Icons/images.cpp"
#include "Icons/FCIcon.xpm"
#include "Icons/x.xpm"
//#include "Icons/FCBackground.xpm"

//#include "FreeCADAbout.h"
  
//#include "Icons/Background3.xpm"

static ApplicationWindow* stApp;
static QWorkspace* stWs;


ApplicationWindow* ApplicationWindow::Instance = 0L;
FCAutoWaitCursor* FCAutoWaitCursor::_pclSingleton = NULL;

// Pimpl class
struct ApplicationWindowP
{
  ApplicationWindowP()
    : toolbars(0L), cmdbars(0L), viewbar(0L)
  {
  }

  ~ApplicationWindowP()
  {
    viewbar = 0L;
    delete toolbars;
    delete cmdbars;
  }

  QPopupMenu* toolbars;
  QPopupMenu* cmdbars;
  FCPopupMenu* viewbar;
};

ApplicationWindow::ApplicationWindow()
    : QextMdiMainFrm( 0, "Main window", WDestructiveClose ),
      _pcActiveDocument(NULL),
	  _bIsClosing(false),
    _bControlButton(false)
{
	// seting up Python binding 
	(void) Py_InitModule("FreeCADGui", ApplicationWindow::Methods);

	// init the Inventor subsystem
	SoQt::init(this);

  d = new ApplicationWindowP;

	setCaption( "Qt-FreeCAD" );

	_cActiveWorkbenchName="<none>";

	// start thread which observes the application and 
	// sets/unsets the waiting cursor if necessary
	FCAutoWaitCursor* waitCursor = &FCAutoWaitCursor::Instance();
	connect(this, SIGNAL(timeEvent()), waitCursor, SLOT(timeEvent()));
	startTimer(waitCursor->GetInterval() / 2);

	// global access 
	Instance = this;

	stApp = this;

	// instanciate the workbench dictionary
	_pcWorkbenchDictionary = PyDict_New();

    // attach the console observer
	GetConsole().AttacheObserver( new FCGuiConsoleObserver(this) );

	// create the macro manager
	_pcMacroMngr = new FCMacroManager();

	// setting up the Bitmap manager
//	QString tmpWb = _cActiveWorkbenchName;
	_cActiveWorkbenchName = "Standard";
	_cBmpFactory.AddPath("../../FreeCADIcons");
	_cBmpFactory.AddPath("../Icons");
//	_cBmpFactory.GetPixmap("Function");

  QDir dir(GetApplication().GetHomePath()); dir.cdUp();
  QString root = dir.path();
	GetDocumentationManager().AddProvider(new FCDocProviderDirectory("FCDoc:/"          ,(root + "/Doc/Online\\"   ).latin1()));
	GetDocumentationManager().AddProvider(new FCDocProviderDirectory("FCDoc:/Framework/",(root + "/Doc/FrameWork\\").latin1()));

	std::string test =  GetDocumentationManager().Retrive("FCDoc:/index", Html );

	test =  GetDocumentationManager().Retrive("FCDoc:/Framework/index", Html );


  // animation
  FCAnimation::Instance()->reparent(statusBar(), QPoint());
  statusBar()->addWidget(FCAnimation::Instance(),0,true);
	// labels and progressbar
	_pclProgress = new FCProgressBar(statusBar(), "Sequencer");
	//_pclProgress->setFixedWidth(200);
	statusBar()->addWidget(_pclProgress,0,true);
	//_pclActionLabel = new QLabel("Ready", statusBar(), "Action");
	//_pclActionLabel->setFixedWidth(120);
	//statusBar()->addWidget(_pclActionLabel,0,true);
	_pclSizeLabel = new QLabel("Dimension", statusBar(), "Dimension");
	_pclSizeLabel->setFixedWidth(120);
	statusBar()->addWidget(_pclSizeLabel,0,true);

	// update gui timer
	_pcActivityTimer = new QTimer( this );
    connect( _pcActivityTimer, SIGNAL(timeout()),this, SLOT(UpdateCmdActivity()) );
    _pcActivityTimer->start( 4000, TRUE );                 // 4 seconds single-shot (wait until the gui is up)


	// Command Line +++++++++++++++++++++++++++++++++++++++++++++++++++
	GetCmdLine().SetParent(statusBar());
	statusBar()->addWidget(&FCCommandLine::Instance(), 0, true);
    statusBar()->message( tr("Ready"), 2001 );

	// Cmd Button Group +++++++++++++++++++++++++++++++++++++++++++++++
	_pcStackBar = new FCStackBar(this,"Cmd_Group");
	_pcWidgetMgr = new FCCustomWidgetManager(GetCommandManager(), _pcStackBar);
	_pcWidgetMgr->addDockWindow( "Command bar",_pcStackBar, NULL, KDockWidget::DockRight, 83);

	// Html View ++++++++++++++++++++++++++++++++++++++++++++++++++++++
	FCParameterGrp::handle hURLGrp = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Windows/HelpViewer");
	QString home = QString(hURLGrp->GetASCII("LineEditURL", "index.html").c_str());
	FCHtmlView* pcHtmlView = new FCHtmlView(home, this, "HelpViewer");
	_pcWidgetMgr->addDockWindow("Help bar", pcHtmlView,"Command bar", KDockWidget::DockBottom, 40);


	// Tree Bar  ++++++++++++++++++++++++++++++++++++++++++++++++++++++	
	FCTree* pcTree = new FCTree(0,0,"Raw_tree");
	pcTree->setMinimumWidth(210);
	_pcWidgetMgr->addDockWindow("Tree bar", pcTree,0, KDockWidget::DockLeft, 0);

	// PropertyView  ++++++++++++++++++++++++++++++++++++++++++++++++++++++	
	FCPropertyView* pcPropView = new FCPropertyView(0,0,"PropertyView");
	pcPropView->setMinimumWidth(210);
	_pcWidgetMgr->addDockWindow("Property View", pcPropView,"Tree bar", KDockWidget::DockBottom, 60);

  // Report View
  FCReportView* pcOutput = new FCReportView(this,"ReportView");
  _pcWidgetMgr->addDockWindow("Report View", pcOutput, 0, KDockWidget::DockBottom, 90);

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
      _cCommandManager.AddCommand(pScript);
    }
  }

  // load recent file list
  hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp");
  if (hGrp->HasGroup("Recent files"))
  {
    hGrp = hGrp->GetGroup("Recent files");
    FCCommand* pCmd = _cCommandManager.GetCommandByName("Std_MRU");
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
  delete _pcWidgetMgr;
  delete _pcMacroMngr;
  std::vector<FCCommand*> macros = _cCommandManager.GetModuleCommands("Macro");
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
  FCCommand* pCmd = _cCommandManager.GetCommandByName("Std_MRU");
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


	// populate a tool bar with some actions

//	bool bInit = _pcWidgetMgr->init(GetActiveWorkbench().latin1());
	// default toolbars -----------------------------------------------------------------------
	//
	// populate toolbars with all default actions
//	QToolBar *pcStdToolBar =  GetCustomWidgetManager()->getToolBar("file operations");
	//_pcStdToolBar->setLabel( "File" );
/*
	if (!bInit)
	{
		std::vector<std::string> defToolbar;
		defToolbar.push_back("Std_New");
		defToolbar.push_back("Std_Open");
		defToolbar.push_back("Std_Save");
		defToolbar.push_back("Std_Print");
		defToolbar.push_back("Separator");
		defToolbar.push_back("Std_Cut");
		defToolbar.push_back("Std_Copy");
		defToolbar.push_back("Std_Paste");
		defToolbar.push_back("Separator");
		defToolbar.push_back("Std_Undo");
		defToolbar.push_back("Std_Redo");
		defToolbar.push_back("Separator");
		defToolbar.push_back("Std_Workbench");
		defToolbar.push_back("Std_WhatsThis");
		_pcWidgetMgr->addToolBar("file operations", defToolbar);

		defToolbar.clear();
		defToolbar.push_back("Std_DlgMacroRecord");
		defToolbar.push_back("Std_DlgMacroStop");
		defToolbar.push_back("Std_DlgMacroExecute");
		_pcWidgetMgr->addToolBar("Macro recording", defToolbar);
		// hide
		_pcWidgetMgr->getToolBar("Macro recording")->hide();

	}*/
	
	// default menu bar -----------------------------------------------------------------------
	//
	// populate menus with all default actions
/*	if (!bInit)
	{
		std::vector<std::string> defaultMenus;
		defaultMenus.push_back("Std_New");
		defaultMenus.push_back("Std_Open");
		defaultMenus.push_back("Std_Save");
		defaultMenus.push_back("Std_SaveAs");
		defaultMenus.push_back("Separator");
		defaultMenus.push_back("Std_Print");
		defaultMenus.push_back("Separator");
		defaultMenus.push_back("Std_MRU");
		defaultMenus.push_back("Separator");
		defaultMenus.push_back("Std_Quit");
		_pcWidgetMgr->addPopupMenu("File", defaultMenus);

		defaultMenus.clear();
		defaultMenus.push_back("Std_Cut");
		defaultMenus.push_back("Std_Copy");
		defaultMenus.push_back("Std_Paste");
		defaultMenus.push_back("Separator");
		defaultMenus.push_back("Std_Undo");
		defaultMenus.push_back("Std_Redo");
		defaultMenus.push_back("Separator");
		defaultMenus.push_back("Std_DlgPreferences");
    _pcWidgetMgr->addPopupMenu("Edit", defaultMenus);

		defaultMenus.clear();
		defaultMenus.push_back("Std_ViewCreateInventor");
		defaultMenus.push_back("Std_ViewCreateOCC");
		defaultMenus.push_back("Separator");
    _pcWidgetMgr->addPopupMenu("View", defaultMenus);
  
		defaultMenus.clear();
		defaultMenus.push_back("Std_CommandLine");
		defaultMenus.push_back("Std_DlgParameter");
#ifdef FC_USE_OCAFBROWSER
		defaultMenus.push_back("Std_OCAFBrowser");
#endif
		defaultMenus.push_back("Separator");
		defaultMenus.push_back("Std_DlgMacroRecord");
		defaultMenus.push_back("Std_DlgMacroStop");
		defaultMenus.push_back("Std_DlgMacroExecute");
		defaultMenus.push_back("Separator");
		defaultMenus.push_back("Std_ViewCreateInventor");
		defaultMenus.push_back("Std_ViewCreateOCC");
		defaultMenus.push_back("Separator");
		defaultMenus.push_back("Std_DlgCustomize");
		defaultMenus.push_back("Std_DlgPreferences");
		_pcWidgetMgr->addPopupMenu("Tools", defaultMenus);

		defaultMenus.clear();
		defaultMenus.push_back("Std_TileHoricontal");
		defaultMenus.push_back("Std_TileVertical");
		defaultMenus.push_back("Std_TilePragmatic");
		defaultMenus.push_back("Separator");
		defaultMenus.push_back("Std_MDIToplevel");
		defaultMenus.push_back("Std_MDITabed");
		_pcWidgetMgr->addPopupMenu("Windows", defaultMenus);
  
		defaultMenus.clear();
		defaultMenus.push_back("Std_OnlineHelp");
		defaultMenus.push_back("Std_About");
		defaultMenus.push_back("Std_WhatsThis");
		_pcWidgetMgr->addPopupMenu("?", defaultMenus);
	}*/

	setMenuForSDIModeSysButtons( menuBar());
	_cActiveWorkbenchName = "<none>";
}

void ApplicationWindow::Polish()
{
  d->viewbar  = _pcWidgetMgr->getPopupMenu("View");
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
  mCheckBars.clear();

  // toolbars
  {
    QPopupMenu* m = d->toolbars;
    m->clear();
    std::vector<FCToolBar*> tb = _pcWidgetMgr->getToolBars();
    for (std::vector<FCToolBar*>::iterator it = tb.begin(); it!=tb.end(); ++it)
    {
      int id = m->insertItem((*it)->name());
      mCheckBars[id] = *it;
      if ((*it)->isVisible())
		    m->setItemChecked(id, true);
    }

    m->insertSeparator();
    int id = m->insertItem("Taskbar");
    mCheckBars[id] = m_pTaskBar;
    if (m_pTaskBar->isVisible())
		  m->setItemChecked(id, true);

    menu->insertItem("Toolbars", m);
  }

  // command bars
  {
    QPopupMenu* m = d->cmdbars;
    m->clear();
    std::vector<FCToolBar*> tb = _pcWidgetMgr->getCmdBars();
    for (std::vector<FCToolBar*>::iterator it = tb.begin(); it!=tb.end(); ++it)
    {
      int id = m->insertItem((*it)->name());
      mCheckBars[id] = *it;
      if (_pcStackBar->isPageVisible(*it))
		    m->setItemChecked(id, true);
    }

    menu->insertItem("Commandbars", m);
  }
  menu->insertSeparator();

  // dock windows
  std::vector<FCDockWindow*> windows = _pcWidgetMgr->getDockWindows();
  for (std::vector<FCDockWindow*>::iterator it = windows.begin(); it!=windows.end(); ++it)
  {
    KDockWidget* w = manager()->getDockWidgetFromName((*it)->name());
    if (w)
    {
      int id = menu->insertItem(w->tabPageLabel());
      mCheckBars[id] = w;
      if (w->isVisible())
		    menu->setItemChecked(id, true);
    }
  }

  // status bar
  menu->insertSeparator();
  QWidget* w = statusBar();
  int id = menu->insertItem("Status bar");
  mCheckBars[id] = w;
  if (w->isVisible())
		menu->setItemChecked(id, true);
}

void ApplicationWindow::OnShowView(int id)
{
  if (mCheckBars.find(id)==mCheckBars.end())
    return; // not a dock window
  if (!mCheckBars[id])
    return; // no valid dock window

  QPopupMenu* menu = (QPopupMenu*)sender();

  if (menu->isItemChecked(id))
  {
    if (mCheckBars[id]->inherits("KDockWidget"))
      ((KDockWidget*)mCheckBars[id])->changeHideShowState();
    else if (_pcStackBar->hasView(mCheckBars[id]))
      _pcStackBar->hidePage(mCheckBars[id]);
    else
      mCheckBars[id]->hide();
  }
  else
  {
    if (mCheckBars[id]->inherits("KDockWidget"))
      ((KDockWidget*)mCheckBars[id])->changeHideShowState();
    else if (_pcStackBar->hasView(mCheckBars[id]))
      _pcStackBar->showPage(mCheckBars[id]);
    else
      mCheckBars[id]->show();
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// document observers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ApplicationWindow::OnDocNew(FCDocument* pcDoc)
{
	lpcDocuments.push_back( new FCGuiDocument(pcDoc,this) );
}

void ApplicationWindow::OnDocDelete(FCDocument* pcDoc)
{
	FCGuiDocument* pcGDoc;

	for(std::list<FCGuiDocument*>::iterator It = lpcDocuments.begin();It != lpcDocuments.end();It++)
	{
		if( ((*It)->GetDocument()) == pcDoc)
		{
			pcGDoc = *It;
			lpcDocuments.erase(It);
			delete pcGDoc;
		}
	}
	
}

void ApplicationWindow::OnLastWindowClosed(FCGuiDocument* pcDoc)
{
	if(!_bIsClosing)
	{
		// GuiDocument has closed the last window and get destructed
		lpcDocuments.remove(pcDoc);
		//lpcDocuments.erase(pcDoc);
		delete pcDoc;
		
		// last document closed?
		if(lpcDocuments.size() == 0 )
			// reset active document
			SetActiveDocument(0);
	}
}

// set text to the pane
void ApplicationWindow::SetPaneText(int i, QString text)
{
  if (i==1)
    _pclActionLabel->setText(text);
  else if (i==2)
    _pclSizeLabel->setText(text);
}

FCProgressBar* ApplicationWindow::GetProgressBar()
{
  return _pclProgress;
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
	return _pcActiveDocument;
	/*
	FCView* pView = GetActiveView();

	if(pView)
		return pView->GetGuiDocument();
	else
		return 0l;*/
}

void ApplicationWindow::SetActiveDocument(FCGuiDocument* pcDocument)
{
	_pcActiveDocument=pcDocument;

	GetConsole().Log("Activate Document (%p) \n",_pcActiveDocument);

	// notify all views attached to the application (not views belong to a special document)
	for(std::list<FCBaseView*>::iterator It=_LpcViews.begin();It!=_LpcViews.end();It++)
		(*It)->SetDocument(pcDocument);

}

void ApplicationWindow::AttachView(FCBaseView* pcView)
{
	_LpcViews.push_back(pcView);

}


void ApplicationWindow::DetachView(FCBaseView* pcView)
{

	_LpcViews.remove(pcView);
}

void ApplicationWindow::Update(void)
{
	// update all documents
	for(std::list<FCGuiDocument*>::iterator It = lpcDocuments.begin();It != lpcDocuments.end();It++)
	{
		(*It)->Update();
	}
	// update all the independed views
	for(std::list<FCBaseView*>::iterator It2 = _LpcViews.begin();It2 != _LpcViews.end();It2++)
	{
		(*It2)->Update();
	}

}

/// get calld if a view gets activated, this manage the whole activation scheme
void ApplicationWindow::ViewActivated(FCView* pcView)
{

	GetConsole().Log("Activate View (%p) Type=\"%s\" \n",pcView,pcView->GetName());
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


void ApplicationWindow::slotUndo()
{
	puts("ApplicationWindow::slotUndo()");
}

void ApplicationWindow::slotRedo()
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

	if(lpcDocuments.size() == 0)
	{
		e->accept();
	}else{
		// ask all documents if closable
		for (std::list<FCGuiDocument*>::iterator It = lpcDocuments.begin();It!=lpcDocuments.end();It++)
		{
			(*It)->CanClose ( e );
//			if(! e->isAccepted() ) break;
			if(! e->isAccepted() ) return;
		}
	}

	// ask all passiv views if closable
	for (std::list<FCBaseView*>::iterator It2 = _LpcViews.begin();It2!=_LpcViews.end();It2++)
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
		_bIsClosing = true;

		std::list<FCGuiDocument*>::iterator It;

		// close all views belonging to a document
		for (It = lpcDocuments.begin();It!=lpcDocuments.end();It++)
		{
			(*It)->CloseAllViews();
		}

		//detache the passiv views
		//SetActiveDocument(0);
		It2 = _LpcViews.begin();
		while (It2!=_LpcViews.end())
		{
			(*It2)->Close();
			It2 = _LpcViews.begin();
		}

		// remove all documents
		for (It = lpcDocuments.begin();It!=lpcDocuments.end();It++)
		{
			delete(*It);
		}

		_pcActivityTimer->stop();

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

    std::vector<FCToolBar*> aclToolBars = _pcWidgetMgr->getToolBars();
	  for (std::vector<FCToolBar*>::iterator It = aclToolBars.begin(); It != aclToolBars.end(); ++It)
    {
      int id = menu.insertItem((*It)->name());
      QToolBar* tb = *It;
      toolb[id] = tb;
      if (tb->isVisible())
		    menu.setItemChecked(id, true);
    }
    if (m_pTaskBar)
    {
      int id = menu.insertItem("Taskbar");
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
  if (_bControlButton)
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
  _bControlButton = (e->state() &  ControlButton);
  QextMdiMainFrm::keyPressEvent(e);
}

void ApplicationWindow::keyReleaseEvent ( QKeyEvent * e )
{
  _bControlButton = (e->state() &  ControlButton);
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
	PyBuf Name(name);

	// close old workbench
	if(_cActiveWorkbenchName != "")
	{
		PyBuf OldName ( _cActiveWorkbenchName.latin1());
		PyObject* pcOldWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, OldName.str);
		assert(pcOldWorkbench);
		GetInterpreter().RunMethodVoid(pcOldWorkbench, "Stop");
	}
	// get the python workbench object from the dictionary
	PyObject* pcWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, Name.str);

	// test if the workbench in
	assert(pcWorkbench);

  // rename with new workbench before(!!!) calling "Start"
	_cActiveWorkbenchName = name;

	// runing the start of the workbench object
	GetInterpreter().RunMethodVoid(pcWorkbench, "Start");
  _pcWidgetMgr->update(name);

	// update the Std_Workbench command and its action object
  FCCommand* pCmd = _cCommandManager.GetCommandByName("Std_Workbench");
  if (pCmd)
  {
    ((FCCmdWorkbench*)pCmd)->UpdateAction(name);
  }

	show();
}

void ApplicationWindow::UpdateWorkbenchEntrys(void)
{
	PyObject *key, *value;
	int pos = 0;
     
  FCCommand* pCmd = _cCommandManager.GetCommandByName("Std_Workbench");
  if (pCmd)
  {
  	// remove all items from the command
    ((FCCmdWorkbench*)pCmd)->Clear();

  	// insert all items
    while (PyDict_Next(_pcWorkbenchDictionary, &pos, &key, &value)) {
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
	while (PyDict_Next(_pcWorkbenchDictionary, &pos, &key, &value)) {
		/* do something interesting with the values... */
		wb.push_back(PyString_AsString(key));
	}
  return wb;
}

void ApplicationWindow::AppendRecentFile(const char* file)
{
  FCCommand* pCmd = _cCommandManager.GetCommandByName("Std_MRU");
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
		_cCommandManager.TestActive();
		// remember last call
		cLastCall.start();
	}

	_pcActivityTimer->start( 300, TRUE );	
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
  bool big = hGrp->GetBool("BigPixmaps", false);
  QString style = "Windows";
  std::vector<std::string> styles = hGrp->GetGroup("WindowStyle")->GetASCIIs("Item");
  long pos = hGrp->GetGroup("WindowStyle")->GetInt("currentItem", 0);
  if (long(styles.size()) > pos)
    style = styles[pos].c_str();
  QStyle* s = FCStyleFactory::createStyle(style);
  resize( w, h );
  move(x, y);
  if (max) showMaximized();
	//setBackgroundPixmap(QPixmap((const char*)FCBackground));
	setUsesBigPixmaps (big);
  if (s && !FCStyleFactory::isCurrentStyle(s))
  {
    QApplication::setStyle(s);
    setAreaPal(palette());
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
  QString fn = "FreeCAD.xml";
  QFile* datafile = new QFile(fn);
  if (!datafile->open(IO_ReadOnly)) 
  {
    // error opening file
    bool bMute = FCGuiConsoleObserver::bMute;
    FCGuiConsoleObserver::bMute = true;
    GetConsole().Warning((tr("Error: Cannot open file '%1' "
                             "(Maybe you're running FreeCAD the first time)\n").arg(fn)).latin1());
    FCGuiConsoleObserver::bMute = bMute;
    datafile->close();
    delete (datafile);
    return;
  }

  // open dom document
  QDomDocument doc("DockWindows");
  if (!doc.setContent(datafile)) 
  {
    GetConsole().Warning("Error:  is not a valid file\n");
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
    GetConsole().Warning("Error: is not a valid file\n");
    return;
  }

  QDomElement root = doc.documentElement();
  if (root.attribute("application") != QString("FreeCAD")) 
  {
    // right file type, wrong application
    GetConsole().Warning("Error: wrong file\n");
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
  QFile* datafile = new QFile ("FreeCAD.xml");
  if (!datafile->open(IO_WriteOnly)) 
  {
    // error opening file
    GetConsole().Warning("Error: Cannot open file\n");
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
	{"CommandAdd",            (PyCFunction) ApplicationWindow::sCommandAdd,              1},
	{"SendMsgToActiveView",        (PyCFunction) ApplicationWindow::sSendActiveView,          1},

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

PYFUNCIMP_S(ApplicationWindow,sMenuAppendItems)
{
  PyObject* pObject;
  char* psMenuName;
  char* parent = 0;
  int bForce;
  int bRemovable = 1;
  // convert args: Python->C 
  if (!PyArg_ParseTuple(args, "sOi|is", &psMenuName, &pObject, &bForce,
                                        &bRemovable, &parent ))     
    return NULL;                             // NULL triggers exception 
  if (!PyList_Check(pObject))
    return NULL;                             // NULL triggers exception 

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
    Instance->_pcWidgetMgr->addPopupMenu(psMenuName, aclItems, parent, bForce);
    if (!bRemovable)
      Instance->_pcWidgetMgr->getPopupMenu(psMenuName)->setRemovable(false);
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
    return NULL;                             // NULL triggers exception 

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

  Instance->_pcWidgetMgr->removeMenuItems(psMenuName, aclItems);

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
  int bForce;
  int bVisible=1;
  int bRemovable=1;
  // convert args: Python->C 
  if (!PyArg_ParseTuple(args, "sOi|ii", &psToolbarName, &pObject, &bForce,
                                        &bVisible, &bRemovable))     
    return NULL;                             // NULL triggers exception 
  if (!PyList_Check(pObject))
		return NULL;                                      // NULL triggers exception 

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
    Instance->_pcWidgetMgr->addToolBar(psToolbarName, aclItems, bForce);
    if (!bVisible)
      Instance->_pcWidgetMgr->getToolBar(psToolbarName)->hide();
    if (!bRemovable)
      Instance->_pcWidgetMgr->getToolBar(psToolbarName)->setRemovable(false);
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
		return NULL;                             // NULL triggers exception 

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

  Instance->_pcWidgetMgr->removeToolBarItems(psToolbarName, aclItems);
    
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
  int bForce;
  int bRemovable = 1;
  // convert args: Python->C 
  if (!PyArg_ParseTuple(args, "sOi|i", &psToolbarName, &pObject,
                                     &bForce, &bRemovable))     
    return NULL;                             // NULL triggers exception 
  if (!PyList_Check(pObject))
		return NULL;                                      // NULL triggers exception 

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
    Instance->_pcWidgetMgr->addCmdBar(psToolbarName, aclItems, bForce);
    if (!bRemovable)
      Instance->_pcWidgetMgr->getCmdBar(psToolbarName)->setRemovable(false);
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
		return NULL;                             // NULL triggers exception 

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

  Instance->_pcWidgetMgr->removeCmdBarItems(psToolbarName, aclItems);

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

	PyDict_SetItemString(Instance->_pcWorkbenchDictionary,psKey,pcObject);

	Instance->UpdateWorkbenchEntrys();

	Py_INCREF(Py_None);
	return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sWorkbenchDelete)
{
	char*       psKey;
	if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	PyDict_DelItemString(Instance->_pcWorkbenchDictionary,psKey);

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

	return Instance->_pcWorkbenchDictionary;
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

//**************************************************************************
//**************************************************************************
// FCAppConsoleObserver
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool FCGuiConsoleObserver::bMute = false;

FCGuiConsoleObserver::FCGuiConsoleObserver(ApplicationWindow *pcAppWnd)
	:_pcAppWnd(pcAppWnd){}

/// get calles when a Warning is issued
void FCGuiConsoleObserver::Warning(const char *m)
{
	if(!bMute){
		QMessageBox::warning( _pcAppWnd, "Warning",m);
		_pcAppWnd->statusBar()->message( m, 2001 );
	}
}

/// get calles when a Message is issued
void FCGuiConsoleObserver::Message(const char * m)
{
	if(!bMute)
		_pcAppWnd->statusBar()->message( m, 2001 );
}

/// get calles when a Error is issued
void FCGuiConsoleObserver::Error  (const char *m)
{
	if(!bMute)
	{
		QMessageBox::critical( _pcAppWnd, "Exception happens",m);
		_pcAppWnd->statusBar()->message( m, 2001 );
	}
}

/// get calles when a Log Message is issued
void FCGuiConsoleObserver::Log    (const char *)
{
}

//**************************************************************************
//**************************************************************************
// FCAutoWaitCursor
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void FCAutoWaitCursor::Destruct(void)
{
	// not initialized or double destruct!
	assert(_pclSingleton);
	delete _pclSingleton;
}

FCAutoWaitCursor &FCAutoWaitCursor::Instance(void)
{
	// not initialized?
	if(!_pclSingleton)
	{

#ifdef FC_OS_WIN32
		_pclSingleton = new FCAutoWaitCursor(GetCurrentThreadId(), 100);
#else
		_pclSingleton = new FCAutoWaitCursor(100);
#endif
	}

	return *_pclSingleton;
}

// getter/setter
int FCAutoWaitCursor::GetInterval()
{
  return iInterval;
}

void FCAutoWaitCursor::SetInterval(int i)
{
  iInterval = i;
}

void FCAutoWaitCursor::SetWaitCursor()
{
#	ifdef FC_OS_WIN32 // win32 api functions
		AttachThreadInput(GetCurrentThreadId(), main_threadid, true);
		SetCursor(LoadCursor(NULL, IDC_WAIT));
#	endif
}


#ifdef FC_OS_WIN32 // windows os

FCAutoWaitCursor::FCAutoWaitCursor(DWORD id, int i)
	:main_threadid(id), iInterval(i)
{
	iAutoWaitCursorMaxCount = 3;
	iAutoWaitCursorCounter  = 3;
	bOverride = false;
	start();
}

#else

FCAutoWaitCursor::FCAutoWaitCursor(int i)
	: iInterval(i)
{
	iAutoWaitCursorMaxCount = 3;
	iAutoWaitCursorCounter  = 3;
	bOverride = false;
	start();
}

#endif

void FCAutoWaitCursor::run()
{
	while (true)
	{
	  // set the thread sleeping
	  msleep(iInterval);

	  // decrements the counter
	  awcMutex.lock();
	  if (iAutoWaitCursorCounter > 0)
		iAutoWaitCursorCounter--;
	  awcMutex.unlock();

	  // set waiting cursor if the application is busy
	  if (iAutoWaitCursorCounter == 0)
	  {
		// load the waiting cursor only once
		if (bOverride == false)
		{
		  SetWaitCursor();
		  bOverride = true;
		}
	  }
	  // reset
	  else if (bOverride == true)
	  {
		// you need not to restore the old cursor because 
		// the application window does this for you :-))
		bOverride = false;
	  }
	}
}


void FCAutoWaitCursor::timeEvent()
{
  // NOTE: this slot must be connected with the timerEvent of your class
  // increments the counter
  awcMutex.lock();
  if (iAutoWaitCursorCounter < iAutoWaitCursorMaxCount)
    iAutoWaitCursorCounter++;
  awcMutex.unlock();
}



//**************************************************************************
//**************************************************************************
// FCAppConsoleObserver
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

FCBmpFactory::FCBmpFactory(void)
{

}

FCBmpFactory::~FCBmpFactory()
{

}


void FCBmpFactory::AddPath(const char* sPath)
{
	_vsPaths.push_back(sPath);
}

void FCBmpFactory::RemovePath(const char* sPath)
{
	_vsPaths.erase(std::find<std::vector<std::string>::iterator,std::string>(_vsPaths.begin(),_vsPaths.end(),sPath));
}


void FCBmpFactory::AddXPM(const char* sName, const char* pXPM)
{
	_mpXPM[sName] = pXPM;
}

void FCBmpFactory::RemoveXPM(const char* sName)
{
	_mpXPM.erase(sName);
}


QPixmap FCBmpFactory::GetPixmap(const char* sName)
{

	// first try to find it in the build in XPM
	std::map<std::string,const char*>::const_iterator It = _mpXPM.find(sName);

	if(It != _mpXPM.end())
		return QPixmap(It->second);

	// try to find it in the given directorys
	for(std::vector<std::string>::const_iterator It2 = _vsPaths.begin();It2 != _vsPaths.end();It2++)
	{
		// list dir
		QDir d( (*It2).c_str() );
		if( QFile(d.path()+QDir::separator() + sName +".bmp").exists() )
			return QPixmap(d.path()+QDir::separator()+ sName + ".bmp");
		if( QFile(d.path()+QDir::separator()+ sName + ".png").exists() )
			return QPixmap(d.path()+QDir::separator()+ sName + ".png");
		if( QFile(d.path()+QDir::separator()+ sName + ".gif").exists() )
			return QPixmap(d.path()+QDir::separator()+ sName + ".gif");
	}

	GetConsole().Warning("Can't find Pixmap:%s\n",sName);

	return QPixmap(px);

}

QPixmap FCBmpFactory::GetPixmap(const char* sName, const char* sMask, Position pos)
{
  QPixmap p1 = GetPixmap(sName);
  QPixmap p2 = GetPixmap(sMask);

  int x = 0, y = 0;

  switch (pos)
  {
    case TopLeft:
      break;
    case TopRight:
      x = p1.width () - p2.width ();
      break;
    case BottomLeft:
      y = p1.height() - p2.height();
      break;
    case BottomRight:
      x = p1.width () - p2.width ();
      y = p1.height() - p2.height();
      break;
  }

  QPixmap p = p1;
  p = FCTools::fillOpaqueRect(x, y, p2.width(), p2.height(), p);

  QPainter pt;
  pt.begin( &p );
  pt.setPen(Qt::NoPen);
  pt.drawRect(x, y, p2.width(), p2.height());
  pt.drawPixmap(x, y, p2);
  pt.end();

  return p;
}

///////////////////////////////////////////////////////////////////////////////

QStringList FCStyleFactory::styles()
{
  QStringList list;

  if ( !list.contains( "Windows" ) )
	  list << "Windows";
  if ( !list.contains( "Motif" ) )
    list << "Motif";
  if ( !list.contains( "CDE" ) )
    list << "CDE";
  if ( !list.contains( "MotifPlus" ) )
  	list << "MotifPlus";
  if ( !list.contains( "Platinum" ) )
  	list << "Platinum";
  if ( !list.contains( "SGI" ) )
  	list << "SGI";
  if ( !list.contains( "Metal" ) )
  	list << "Metal";
  if ( !list.contains( "Norwegian Wood" ) )
  	list << "Norwegian Wood";
  if ( !list.contains( "Step" ) )
  	list << "Step";

  return list;
}

QStyle* FCStyleFactory::createStyle( const QString& s)
{
  QStyle* ret = NULL;
  QString style = s.lower();

  if ( style == "windows" )
    ret = new QWindowsStyle;
  else if ( style == "motif" )
    ret = new QMotifStyle;
  else if ( style == "cde" )
    ret = new QCDEStyle;
  else if ( style == "motifplus" )
    ret = new QMotifPlusStyle;
  else if ( style == "platinum" )
    ret = new QPlatinumStyle;
  else if ( style == "sgi" )
    ret = new QSGIStyle;

#if QT_VERSION < 300

  else if ( style == "metal" )
    ret = new MetalStyle;
  else if ( style == "norwegian wood" )
    ret = new NorwegianWoodStyle;
  else if ( style == "step" )
    ret = new StepStyle;

#endif

  if(ret)
  	ret->setName(s);

  return ret;
}

bool FCStyleFactory::isCurrentStyle( QStyle* s )
{
  if (!s)
    return false;

  QString s1 = s->className();
  QString s2 = QApplication::style().className();

  return s1 == s2;
}




#include "moc_Application.cpp"
