
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
	addToolWindow( pCmdBar, KDockWidget::DockRight, m_pMdi, 70, "Buttons", "Buttons");

	// Html View ++++++++++++++++++++++++++++++++++++++++++++++++++++++
	QString home = QString("index.html");
	FCHtmlView* hv = new FCHtmlView(home, this, "Help_View");
	addToolWindow( hv, KDockWidget::DockBottom, pCmdBar, 70, "Help", "Help");

	// misc stuff
    resize( 800, 600 );
	//setBackgroundPixmap(QPixmap(Background3));
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
// creating std commands
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#include "Icons/images.cpp"

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

	_pclUndoRedoWidget = new FCUndoRedoDlg(this, "Undo/Redo");
	connect(_pclUndoRedoWidget, SIGNAL(clickedListBox()), this, SLOT(executeUndoRedo()));

    // populate a tool bar with some actions

    _pcStdToolBar = new QToolBar( this, "file operations" );
    _pcStdToolBar->setLabel( "File" );
	_cCommandManager.AddTo("Std_New",_pcStdToolBar);
	_cCommandManager.AddTo("Std_Open",_pcStdToolBar);
	_cCommandManager.AddTo("Std_Save",_pcStdToolBar);
	_cCommandManager.AddTo("Std_Print",_pcStdToolBar);
	_pcStdToolBar->addSeparator();
	_cCommandManager.AddTo("Std_Cut",_pcStdToolBar);
	_cCommandManager.AddTo("Std_Copy",_pcStdToolBar);
	_cCommandManager.AddTo("Std_Paste",_pcStdToolBar);
	_pcStdToolBar->addSeparator();
	// Undo/Redo Toolbutton
	QToolButton* button = new QToolButtonDropDown(_pcStdToolBar, QPixmap(pUndo), _pclUndoRedoWidget);
	connect(button, SIGNAL(clicked()), this, SLOT(slotUndo()));
	connect(button, SIGNAL(updateWidgetSignal()), this, SLOT(updateUndo()));

	button = new QToolButtonDropDown(_pcStdToolBar, QPixmap(pRedo), _pclUndoRedoWidget);
	connect(button, SIGNAL(clicked()), this, SLOT(slotRedo()));
	connect(button, SIGNAL(updateWidgetSignal()), this, SLOT(updateRedo()));

	_pcStdToolBar->addSeparator();

#	include "icons/FCIcon.xpm"
	_pcWorkbenchCombo = new QComboBox(_pcStdToolBar);
	_pcWorkbenchCombo->insertItem (QPixmap(FCIcon),"Part"); 
	_pcWorkbenchCombo->insertItem (QPixmap(FCIcon),"Assambly"); 
	_pcWorkbenchCombo->insertItem (QPixmap(FCIcon),"FEM"); 
	_pcWorkbenchCombo->insertItem (QPixmap(FCIcon),"Renderer"); 

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