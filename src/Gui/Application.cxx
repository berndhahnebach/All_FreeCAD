
#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qapplication.h>
#	include <qvbox.h>
#	include <qworkspace.h>
#	include <qstatusbar.h>
#	include <qmenubar.h>
#	include <qmessagebox.h>
#	include <qtoolbar.h>
#	include <qpopupmenu.h>
#endif



#include "Application.h"
#include "Document.h"
#include "CommandStd.h"
#include "Splashscreen.h"

#include "CommandLine.h"
#include "DlgDocTemplatesImp.h"
#include "DlgUndoRedo.h"
#include "ButtonGroup.h"
#include "HtmlView.h"

#include "Icons/images.cpp"
#include "icons/FCIcon.xpm"
//#include "icons/FCBackground.xpm"

//#include "FreeCADAbout.h"
  
//#include "Icons/Background3.xpm"

static ApplicationWindow* stApp;
static QWorkspace* stWs;


ApplicationWindow* ApplicationWindow::Instance = 0L;
FCAutoWaitCursor* FCAutoWaitCursor::_pclSingleton = NULL;

ApplicationWindow::ApplicationWindow()
    : QextMdiMainFrm( 0, "Main window", WDestructiveClose ),
      _pcActiveDocument(NULL)
{
  // start thread which observes the application and 
  // sets/unsets the waiting cursor if necessary
  FCAutoWaitCursor* waitCursor = &FCAutoWaitCursor::Instance();
  connect(this, SIGNAL(timeEvent()), waitCursor, SLOT(timeEvent()));
  startTimer(waitCursor->GetInterval() / 2);

	// global access 
	Instance = this;

	stApp = this;

    // attach the console observer
	GetConsole().AttacheObserver( new FCAppConsoleObserver(this) );

	CreateTestOperations();
	//createCasCadeOperations();

  // labels
  _pclActionLabel = new QLabel("Ready", statusBar(), "Action");
  _pclActionLabel->setFixedWidth(120);
  statusBar()->addWidget(_pclActionLabel,0,true);
  _pclSizeLabel = new QLabel("Dimension", statusBar(), "Dimension");
  _pclSizeLabel->setFixedWidth(120);
  statusBar()->addWidget(_pclSizeLabel,0,true);

	// Command Line +++++++++++++++++++++++++++++++++++++++++++++++++++
	GetCmdLine().SetParent(statusBar());
	statusBar()->addWidget(&FCCommandLine::Instance(), 0, true);
    statusBar()->message( tr("Ready"), 2001 );

	// Cmd Button Group +++++++++++++++++++++++++++++++++++++++++++++++
	FCCmdBar* pCmdBar = new FCCmdBar(this,"Cmd_Group");
	pCmdBar->AddTestButtons();
	AddDockWindow( "Command bar",pCmdBar);

	// Html View ++++++++++++++++++++++++++++++++++++++++++++++++++++++
	QString home = QString("index.html");
	FCHtmlView* hv = new FCHtmlView(home, this, "Help_View");
	AddDockWindow("Help bar", hv,"Command bar");

	// misc stuff
    resize( 800, 600 );
	//setBackgroundPixmap(QPixmap((const char*)FCBackground));
	//setUsesBigPixmaps (true);

}

ApplicationWindow::~ApplicationWindow()
{
	/*
	myStdActions.clear();
	myCasCadeTranslateActions.clear();
	if(myImportPopup) delete myImportPopup;
	if(myExportPopup) delete myExportPopup;
	if(myWindowPopup) delete myWindowPopup;
	if(myFilePopup) delete myFilePopup;
	if(myCasCadeBar) delete myCasCadeBar;
	if(myStdToolBar) delete myStdToolBar;
//	if(ws) delete ws;
*/
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

	for(FClist<FCGuiDocument*>::iterator It = lpcDocuments.begin();It != lpcDocuments.end();It++)
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
	// GuiDocument has closed the last window and get destructed
	lpcDocuments.remove(pcDoc);
	delete pcDoc;	
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// toolbar and dock window handling
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/// Get a named Toolbar or creat if not in
QToolBar *ApplicationWindow::GetToolBar(const char* name)
{
	//FCmap <FCstring,QToolBar*>     mpcToolBars;
	FCmap <FCstring,QToolBar*>::iterator It = mpcToolBars.find(name);
	if( It!=mpcToolBars.end() )
		return It->second;
	else
		return mpcToolBars[name] = new QToolBar( this, name );
}

/// Delete a named Toolbar
void ApplicationWindow::DelToolBar(const char* name)
{
	FCmap <FCstring,QToolBar*>::iterator It = mpcToolBars.find(name);
	if( It!=mpcToolBars.end() )
	{
		delete It->second;
		mpcToolBars.erase(It);
	}
}

/// Add a new named Dock Window
void ApplicationWindow::AddDockWindow(const char* name,FCDockWindow *pcDocWindow, const char* sCompanion ,KDockWidget::DockPosition pos )
{
	// 	FCmap <FCstring,FCDockWindow*> mpcDocWindows;
	mpcDocWindows[name] = pcDocWindow;
	QString str = name;
	str += " dockable window";
	if(sCompanion)
	{
		FCDockWindow* pcWnd = GetDockWindow(sCompanion);
		assert(pcWnd);
		addToolWindow( pcDocWindow, pos, pcWnd, 83, str, name);
	}else
		addToolWindow( pcDocWindow, pos, m_pMdi, 83, str, name);
}

/// Gets you a registered Dock Window back
FCDockWindow *ApplicationWindow::GetDockWindow(const char* name)
{
	FCmap <FCstring,FCDockWindow*>::iterator It = mpcDocWindows.find(name);
	if( It!=mpcDocWindows.end() )
		return It->second;
	else
		return 0l;
}

/// Delete (or only remove) a named Dock Window
void ApplicationWindow::DelDockWindow(const char* name, bool bOnlyRemove)
{
	FCmap <FCstring,FCDockWindow*>::iterator It = mpcDocWindows.find(name);
	if( It!=mpcDocWindows.end() )
	{
		if(!bOnlyRemove) delete It->second;
		mpcDocWindows.erase(It);
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





//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating std commands
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



void ApplicationWindow::CreateTestOperations()
{

	// register the application Standard commands from CommandStd.cpp
	CreateStdCommands();

	_pclUndoRedoWidget = new FCUndoRedoDlg(this, "Undo/Redo");
	connect(_pclUndoRedoWidget, SIGNAL(clickedListBox()), this, SLOT(executeUndoRedo()));

    // populate a tool bar with some actions

	// Standard tool bar -----------------------------------------------------------------------
    QToolBar *pcStdToolBar =  GetToolBar("file operations");
    //_pcStdToolBar->setLabel( "File" );
	_cCommandManager.AddTo("Std_New",pcStdToolBar);
	_cCommandManager.AddTo("Std_Open",pcStdToolBar);
	_cCommandManager.AddTo("Std_Save",pcStdToolBar);
	_cCommandManager.AddTo("Std_Print",pcStdToolBar);
	pcStdToolBar->addSeparator();
	_cCommandManager.AddTo("Std_Cut",pcStdToolBar);
	_cCommandManager.AddTo("Std_Copy",pcStdToolBar);
	_cCommandManager.AddTo("Std_Paste",pcStdToolBar);
	pcStdToolBar->addSeparator();
	// Undo/Redo Toolbutton
	QToolButton* button = new QToolButtonDropDown(pcStdToolBar, QPixmap(pUndo), _pclUndoRedoWidget);
	connect(button, SIGNAL(clicked()), this, SLOT(slotUndo()));
	connect(button, SIGNAL(updateWidgetSignal()), this, SLOT(updateUndo()));

	button = new QToolButtonDropDown(pcStdToolBar, QPixmap(pRedo), _pclUndoRedoWidget);
	connect(button, SIGNAL(clicked()), this, SLOT(slotRedo()));
	connect(button, SIGNAL(updateWidgetSignal()), this, SLOT(updateRedo()));

	pcStdToolBar->addSeparator();

	_pcWorkbenchCombo = new QComboBox(pcStdToolBar);
	_pcWorkbenchCombo->insertItem (QPixmap(FCIcon),"<none>"); 
	_pcWorkbenchCombo->insertItem (QPixmap(FCIcon),"Part"); 
	_pcWorkbenchCombo->insertItem (QPixmap(FCIcon),"Assambly"); 
	_pcWorkbenchCombo->insertItem (QPixmap(FCIcon),"FEM"); 
	_pcWorkbenchCombo->insertItem (QPixmap(FCIcon),"Renderer"); 

	// test tool bar -----------------------------------------------------------------------
    pcStdToolBar =  GetToolBar("Test Toolbar" );
	_cCommandManager.AddTo("Std_MDIToplevel",pcStdToolBar);
	_cCommandManager.AddTo("Std_MDITabed"   ,pcStdToolBar);
	_cCommandManager.AddTo("Std_MDINormal"  ,pcStdToolBar);
	pcStdToolBar->addSeparator();
	_cCommandManager.AddTo("Std_TilePragmatic"  ,pcStdToolBar);
	_cCommandManager.AddTo("Std_TileVertical"   ,pcStdToolBar);
	_cCommandManager.AddTo("Std_TileHoricontal" ,pcStdToolBar);
	pcStdToolBar->addSeparator();
	_cCommandManager.AddTo("Std_Test1"  ,pcStdToolBar);
	_cCommandManager.AddTo("Std_Test2"  ,pcStdToolBar);

	// view tool bar -----------------------------------------------------------------------
    pcStdToolBar =  GetToolBar("View Toolbar" );
	_cCommandManager.AddTo("Std_ViewFitAll"  ,pcStdToolBar);
	_cCommandManager.AddTo("Std_ViewAxo"     ,pcStdToolBar);
	pcStdToolBar->addSeparator();
	_cCommandManager.AddTo("Std_ViewFront"   ,pcStdToolBar);
	_cCommandManager.AddTo("Std_ViewRight"   ,pcStdToolBar);
	_cCommandManager.AddTo("Std_ViewTop"     ,pcStdToolBar);
	pcStdToolBar->addSeparator();
	_cCommandManager.AddTo("Std_ViewRear"  ,pcStdToolBar);
	_cCommandManager.AddTo("Std_ViewLeft"  ,pcStdToolBar);
	_cCommandManager.AddTo("Std_ViewBottom",pcStdToolBar);

	// test tool bar -----------------------------------------------------------------------
    // populate a menu with all actions
    _pcPopup = new QPopupMenu( this );
    menuBar()->insertItem( "File", _pcPopup) ;
	_cCommandManager.AddTo("Std_New",_pcPopup);
	_cCommandManager.AddTo("Std_Open",_pcPopup);
	_cCommandManager.AddTo("Std_Save",_pcPopup);
	_cCommandManager.AddTo("Std_SaveAs",_pcPopup);
	_pcPopup->insertSeparator();
	_cCommandManager.AddTo("Std_Print",_pcPopup);
	_pcPopup->insertSeparator();
	_cCommandManager.AddTo("Std_Quit",_pcPopup);

    _pcPopup = new QPopupMenu( this );
    menuBar()->insertItem( "Edit", _pcPopup );
	_cCommandManager.AddTo("Std_Cut",_pcPopup);
	_cCommandManager.AddTo("Std_Copy",_pcPopup);
	_cCommandManager.AddTo("Std_Paste",_pcPopup);
		
  _pcPopup = new QPopupMenu( this );
  menuBar()->insertItem( "?", _pcPopup );
	_cCommandManager.AddTo("Std_About",_pcPopup);

	setMenuForSDIModeSysButtons( menuBar());
	 
}

/// send Messages to the active view
bool ApplicationWindow::SendMsgToActiveView(const char* pMsg)
{
	FCView* pView = GetActiveView();

	if(pView){
		pView->OnMsg(pMsg);
		return true;
	}else
		return false;
}

FCView* ApplicationWindow::GetActiveView(void)
{
	QextMdiChildView * pView = activeWindow();

	if(pView /*&& (typeid(*pView) == typeid(class FCView))*/)
		return (FCView*)pView; 
	else
		return (FCView*)0l;
}


/// Geter for the Active View
FCGuiDocument* ApplicationWindow::GetActiveDocument(void)
{
	FCView* pView = GetActiveView();

	if(pView)
		return pView->getDocument();
	else
		return 0l;
}



void ApplicationWindow::updateUndo()
{
  if (_pclUndoRedoWidget)
  {
    _pclUndoRedoWidget->setMode(FCUndoRedoDlg::Undo);
    _pclUndoRedoWidget->updateUndoRedoList();
  }
}

void ApplicationWindow::updateRedo()
{
  if (_pclUndoRedoWidget)
  {
    _pclUndoRedoWidget->setMode(FCUndoRedoDlg::Redo);
    _pclUndoRedoWidget->updateUndoRedoList();
  }
}


void ApplicationWindow::executeUndoRedo()
{
  QWaitCondition().wait(5000);
}


/** just additionally fits the system menu button position to the menu position */
void ApplicationWindow::resizeEvent ( QResizeEvent *e)
{
   QextMdiMainFrm::resizeEvent( e);
   setSysButtonsAtMenuPosition();
}



ApplicationWindow* ApplicationWindow::getApplication()
{
	return stApp;
}


void ApplicationWindow::exportImage()
{
  //FCView* w = (FCView*) stWs->activeWindow();
  //w->dump();
}



//**************************************************************************
//**************************************************************************
// FCAppConsoleObserver
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



FCAppConsoleObserver::FCAppConsoleObserver(ApplicationWindow *pcAppWnd)
	:_pcAppWnd(pcAppWnd){}

/// get calles when a Warning is issued
void FCAppConsoleObserver::Warning(const char *m)
{
	_pcAppWnd->statusBar()->message( m, 2001 );
}

/// get calles when a Message is issued
void FCAppConsoleObserver::Message(const char * m)
{
	_pcAppWnd->statusBar()->message( m, 2001 );
}

/// get calles when a Error is issued
void FCAppConsoleObserver::Error  (const char *m)
{
	QMessageBox::information( _pcAppWnd, "Exception happens",m);
	_pcAppWnd->statusBar()->message( m, 2001 );
}

/// get calles when a Log Message is issued
void FCAppConsoleObserver::Log    (const char *)
{
}


#include "Application_moc.cpp"