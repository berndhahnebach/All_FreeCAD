
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

ApplicationWindow::ApplicationWindow()
    : QextMdiMainFrm( 0, "Main window", WDestructiveClose )
{
	// global access 
	Instance = this;

	stApp = this;

    // attach the console observer
	GetConsole().AttacheObserver( new FCAppConsoleObserver(this) );

	CreateTestOperations();
	//createCasCadeOperations();

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
	AddDockWindow("Help bar", hv);

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
void ApplicationWindow::AddDockWindow(const char* name,FCDockWindow *pcDocWindow,KDockWidget::DockPosition pos , int percent)
{
	// 	FCmap <FCstring,FCDockWindow*> mpcDocWindows;
	mpcDocWindows[name] = pcDocWindow;
	QString str = name;
	str += " dockable window";
	addToolWindow( pcDocWindow, pos, m_pMdi, percent, str, name);
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






//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating std commands
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



void ApplicationWindow::CreateTestOperations()
{


	_cCommandManager.AddCommand(new FCCmdNew());
	_cCommandManager.AddCommand(new FCCmdOpen());
	_cCommandManager.AddCommand(new FCCmdSave());
	_cCommandManager.AddCommand(new FCCmdSaveAs());
	_cCommandManager.AddCommand(new FCCmdUndo());
	_cCommandManager.AddCommand(new FCCmdRedo());
	_cCommandManager.AddCommand(new FCCmdPrint());
	_cCommandManager.AddCommand(new FCCmdCut());
	_cCommandManager.AddCommand(new FCCmdCopy());
	_cCommandManager.AddCommand(new FCCmdPaste());
	_cCommandManager.AddCommand(new FCCmdMDINormal());
	_cCommandManager.AddCommand(new FCCmdMDIToplevel());
	_cCommandManager.AddCommand(new FCCmdMDITabed());
	_cCommandManager.AddCommand(new FCCmdTileHor());
	_cCommandManager.AddCommand(new FCCmdTileVer());
	_cCommandManager.AddCommand(new FCCmdTilePra());
	_cCommandManager.AddCommand(new FCCmdTest1());
	_cCommandManager.AddCommand(new FCCmdTest2());

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
	//_cCommandManager.AddTo("Std_Quit",myFilePopup);

    _pcPopup = new QPopupMenu( this );
    menuBar()->insertItem( "Work", _pcPopup );
	_cCommandManager.AddTo("Std_Cut",_pcPopup);
	_cCommandManager.AddTo("Std_Copy",_pcPopup);
	_cCommandManager.AddTo("Std_Paste",_pcPopup);
		

	setMenuForSDIModeSysButtons( menuBar());
	 
}

/// send Messages to the active view
bool ApplicationWindow::SendMsgToActiveView(const char* pMsg)
{
	if(_pcActiveDocument)
		return _pcActiveDocument->SendMsgToActiveView(pMsg);
	else
		return false;
}

/// send Messages to the active view
bool ApplicationWindow::SendMsgToViews(const char* pMsg)
{
	bool bResult = false;

	for(FClist<FCGuiDocument*>::iterator It = lpcDocuments.begin();It != lpcDocuments.end();It++)
	{
		if( (*It)->SendMsgToActiveView(pMsg))
		{
			bResult = true;
			break;
		}
	}

	return bResult;
}


/// set the parameter to the active view or reset in case of 0
void ApplicationWindow::SetActive(FCGuiDocument* pcDoc)
{
	_pcActiveDocument = pcDoc;

}

/// Geter for the Active View
FCGuiDocument* ApplicationWindow::GetActiveView(void)
{
	return _pcActiveDocument;
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