
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
//#include "CommandStd.h"
//#include "CommandView.h"
#include "Splashscreen.h"

#include "CommandLine.h"
#include "DlgDocTemplatesImp.h"
#include "DlgUndoRedo.h"
#include "ButtonGroup.h"
#include "HtmlView.h"

#include "Icons/images.cpp"
#include "icons/FCIcon.xpm"
#include "icons/x.xpm"
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
	// seting up Python binding 
	(void) Py_InitModule("FreeCADGui", ApplicationWindow::Methods);

	setCaption( "FreeCAD" );

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
	AddDockWindow("Help bar", hv,"Command bar", KDockWidget::DockBottom);

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
		return mpcToolBars[name] = new FCToolBar( this, name );
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
/// Get a named Command bar view or creat if not in
FCToolboxGroup *ApplicationWindow::GetCommandBar(const char* name)
{
	FCCmdBar* pCmdBar = (FCCmdBar*) GetDockWindow("Cmd_Group");
	FCToolboxGroup * p = pCmdBar->GetView(name);
	if(p)
		return p;
	else
		return pCmdBar->CreateView(name);
}

/// Delete a named Command bar view
void ApplicationWindow::DelCommandBar(const char* name)
{
	FCCmdBar* pCmdBar = (FCCmdBar*)GetDockWindow("Cmd_Group");
	pCmdBar->DeleteView(name);
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

FCProgressBar* ApplicationWindow::GetProgressBar()
{
  return _pclProgress;
}





//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creating std commands
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



void ApplicationWindow::CreateTestOperations()
{
	_cBmpFactory.AddPath("../../FreeCADIcons");
	_cBmpFactory.AddPath("../Icons");
	_cBmpFactory.GetPixmap("Function");

	// register the application Standard commands from CommandStd.cpp
	CreateStdCommands();
	CreateViewStdCommands();

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

/*	// test tool bar -----------------------------------------------------------------------
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
*/
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
#if 0
  bool b=true; int bi=200;
  bool c=true; int ci=5;

  GetProgressBar()->Start("text", bi, b);
  for (int i=0; i<bi; i++)
  {
    QWaitCondition().wait(i*20);

    GetProgressBar()->Start("Hallo", ci, c);
    for (int j=0;j<ci;j++)
    {
      QWaitCondition().wait(j*10);
      GetProgressBar()->Next();
    }
    GetProgressBar()->Stop();

    GetProgressBar()->Next();
  }

  GetProgressBar()->Stop();
#else
  bool b=true; int bi=50;
  bool c=true; int ci=20;

  GetProgressBar()->Start("Very long operation", bi, b);
  for (int i=0; i<bi; i++)
  {
    QWaitCondition().wait(i*10);

    GetProgressBar()->Start("Hallo", ci, c);
    for (int j=0;j<ci;j++)
    {
      QWaitCondition().wait(j*20);
      GetProgressBar()->Next();
    }
    GetProgressBar()->Stop();

    GetProgressBar()->Next();
  }

  GetProgressBar()->Stop();
#endif
}


/** just additionally fits the system menu button position to the menu position */
void ApplicationWindow::resizeEvent ( QResizeEvent *e)
{
   QextMdiMainFrm::resizeEvent( e);
   setSysButtonsAtMenuPosition();
}


void ApplicationWindow::exportImage()
{
  //FCView* w = (FCView*) stWs->activeWindow();
  //w->dump();
}




//**************************************************************************
// Python stuff

// FCApplication Methods						// Methods structure
PyMethodDef ApplicationWindow::Methods[] = {
	{"ToolbarAddTo",          (PyCFunction) ApplicationWindow::sToolbarAddTo,       1},
	{"ToolbarDelete",         (PyCFunction) ApplicationWindow::sToolbarDelete,     1},
	{"ToolbarAddSeperator",   (PyCFunction) ApplicationWindow::sToolbarAddSeperator,     1},
	{"CommandbarAddTo",       (PyCFunction) ApplicationWindow::sCommandbarAddTo,       1},
	{"CommandbarDelete",      (PyCFunction) ApplicationWindow::sCommandbarDelete,     1},
	{"CommandbarAddSeperator",(PyCFunction) ApplicationWindow::sCommandbarAddSeperator,     1},

	{NULL, NULL}		/* Sentinel */
};

PYFUNCIMP_S(ApplicationWindow,sToolbarAddSeperator)
{
	char *psToolbarName;
	if (!PyArg_ParseTuple(args, "s", &psToolbarName))     // convert args: Python->C 
		return NULL;                                      // NULL triggers exception 

	QToolBar * pcBar = Instance->GetToolBar(psToolbarName);
	pcBar->addSeparator();
	return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sToolbarAddTo)
{
	char *psToolbarName,*psCmdName;
	if (!PyArg_ParseTuple(args, "ss", &psToolbarName,&psCmdName))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 

	QToolBar * pcBar = Instance->GetToolBar(psToolbarName);
	Instance->_cCommandManager.AddTo(psCmdName,pcBar);
    return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sToolbarDelete)
{
    char *psToolbarName;
    if (!PyArg_ParseTuple(args, "s", &psToolbarName))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	Instance->DelToolBar(psToolbarName);
    return Py_None;
}
PYFUNCIMP_S(ApplicationWindow,sCommandbarAddSeperator)
{
	char *psToolbarName;
	if (!PyArg_ParseTuple(args, "s", &psToolbarName))     // convert args: Python->C 
		return NULL;                                      // NULL triggers exception 

	FCToolboxGroup * pcBar = Instance->GetCommandBar(psToolbarName);
	//pcBar->addSeparator(); not implemented yet
	return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sCommandbarAddTo)
{
	char *psToolbarName,*psCmdName;
	if (!PyArg_ParseTuple(args, "ss", &psToolbarName,&psCmdName))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 

	FCToolboxGroup * pcBar = Instance->GetCommandBar(psToolbarName);
	Instance->_cCommandManager.AddTo(psCmdName,pcBar);
    return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sCommandbarDelete)
{
    char *psToolbarName;
    if (!PyArg_ParseTuple(args, "s", &psToolbarName))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 
	Instance->DelCommandBar(psToolbarName);
    return Py_None;
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

#ifdef WNT
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
#	ifdef WNT // win32 api functions
		AttachThreadInput(GetCurrentThreadId(), main_threadid, true);
		SetCursor(LoadCursor(NULL, IDC_WAIT));
#	endif
}


#ifdef WNT // windows os

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
	_vsPaths.erase(FCfind<FCvector<FCstring>::iterator,FCstring>(_vsPaths.begin(),_vsPaths.end(),sPath));
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
	FCmap<FCstring,const char*>::const_iterator It = _mpXPM.find(sName);

	if(It != _mpXPM.end())
		return QPixmap(It->second);

	// try to find it in the given directorys
	for(FCvector<FCstring>::const_iterator It2 = _vsPaths.begin();It2 != _vsPaths.end();It2++)
	{
		// list dir
		QDir d( (*It2).c_str() );
		if( QFile(d.path()+QDir::separator() + sName +".bmp").exists() )
			return QPixmap(d.path()+QDir::separator()+ sName + ".bmp");
		if( QFile(d.path()+QDir::separator()+ sName + ".png").exists() )
			return QPixmap(d.path()+QDir::separator()+ sName + ".png");
	}

	GetConsole().Warning("Cant find Pixmap:%s\n",sName);

	return QPixmap(px);

}







#include "moc_Application.cpp"