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
#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
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

#if QT_VER != QT_VERSION
#	error "QT Version missmatch, please set the right version in src/Config.h line 92"
#endif


#include "Application.h"
#include "Document.h"
//#include "CommandStd.h"
//#include "CommandView.h"
#include "Splashscreen.h"
#include "WidgetFactory.h"
#include "PrefWidgets.h"

#include "CommandLine.h"
#include "DlgDocTemplatesImp.h"
#include "DlgUndoRedo.h"
#include "ButtonGroup.h"
#include "HtmlView.h"

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

ApplicationWindow::ApplicationWindow()
    : QextMdiMainFrm( 0, "Main window", WDestructiveClose ),
      _pcActiveDocument(NULL)
{
	// seting up Python binding 
	(void) Py_InitModule("FreeCADGui", ApplicationWindow::Methods);

	setCaption( "FreeCAD" );

	_cActiveWorkbenchName="";

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
	_pcCmdBar = new FCCmdBar(this,"Cmd_Group");
	//_pcCmdBar->AddTestButtons();
	AddDockWindow( "Command bar",_pcCmdBar);

	// Html View ++++++++++++++++++++++++++++++++++++++++++++++++++++++
  FCParameterGrp::handle hURLGrp = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Windows/Widget Preferences/LineEditURL");
  QString home = QString(hURLGrp->GetASCII("LineEditURL", "index.html").c_str());
	_pcHtmlView = new FCHtmlView(home, this, "Help_View");
	AddDockWindow("Help bar", _pcHtmlView,"Command bar", KDockWidget::DockBottom);

	// misc stuff
    resize( 800, 600 );
	//setBackgroundPixmap(QPixmap((const char*)FCBackground));
	//setUsesBigPixmaps (true);

}

ApplicationWindow::~ApplicationWindow()
{

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
	//std::map <std::string,QToolBar*>     mpcToolBars;
	std::map <std::string,QToolBar*>::iterator It = mpcToolBars.find(name);
	if( It!=mpcToolBars.end() )
		return It->second;
	else
	{
		QToolBar *pcToolBar = new FCToolBar( this, name );
		mpcToolBars[name] = pcToolBar;
		pcToolBar->show();
		return pcToolBar;
	}
}

std::vector<QToolBar*> ApplicationWindow::GetToolBars()
{
  std::vector<QToolBar*> aclToolbars;
	for (std::map <std::string,QToolBar*>::iterator It = mpcToolBars.begin(); It != mpcToolBars.end(); ++It)
  {
    aclToolbars.push_back(It->second);
  }

  return aclToolbars;
}

/// Delete a named Toolbar
void ApplicationWindow::DelToolBar(const char* name)
{
	std::map <std::string,QToolBar*>::iterator It = mpcToolBars.find(name);
	if( It!=mpcToolBars.end() )
	{
		delete It->second;
		mpcToolBars.erase(It);
	}
}
/// Get a named Command bar view or creat if not in
FCToolboxGroup *ApplicationWindow::GetCommandBar(const char* name)
{
	//FCCmdBar* pCmdBar = (FCCmdBar*) GetDockWindow("Cmd_Group");
	if (_pcCmdBar->HasView(name))
		return _pcCmdBar->GetView(name);
	else
  {
  	FCToolboxGroup* p = _pcCmdBar->CreateView(name);
    _pcCmdBar->setCurPage(0);
		return p;
  }
}

/// Delete a named Command bar view
void ApplicationWindow::DelCommandBar(const char* name)
{
	//FCCmdBar* pCmdBar = (FCCmdBar*)GetDockWindow("Cmd_Group");
	_pcCmdBar->DeleteView(name);
}

/// Add a new named Dock Window
void ApplicationWindow::AddDockWindow(const char* name,FCDockWindow *pcDocWindow, const char* sCompanion ,KDockWidget::DockPosition pos )
{
	// 	std::map <std::string,FCDockWindow*> mpcDocWindows;
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
	std::map <std::string,FCDockWindow*>::iterator It = mpcDocWindows.find(name);
	if( It!=mpcDocWindows.end() )
		return It->second;
	else
		return 0l;
}

/// Delete (or only remove) a named Dock Window
void ApplicationWindow::DelDockWindow(const char* name, bool bOnlyRemove)
{
	std::map <std::string,FCDockWindow*>::iterator It = mpcDocWindows.find(name);
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
	QToolButton* button = new FCToolButtonDropDown(pcStdToolBar, QPixmap(pUndo), _pclUndoRedoWidget);
	connect(button, SIGNAL(clicked()), this, SLOT(slotUndo()));
	connect(button, SIGNAL(updateWidgetSignal()), this, SLOT(updateUndo()));

	button = new FCToolButtonDropDown(pcStdToolBar, QPixmap(pRedo), _pclUndoRedoWidget);
	connect(button, SIGNAL(clicked()), this, SLOT(slotRedo()));
	connect(button, SIGNAL(updateWidgetSignal()), this, SLOT(updateRedo()));

	pcStdToolBar->addSeparator();

	_pcWorkbenchCombo = new QComboBox(pcStdToolBar);
//	_pcWorkbenchCombo->insertItem (QPixmap(FCIcon),"<none>"); 
//	_pcWorkbenchCombo->insertItem (QPixmap(FCIcon),"<none2>"); 
	_cActiveWorkbenchName = "";
  ((FCToolBar*)GetToolBar("file operations"))->loadUserDefButtons();
	connect(_pcWorkbenchCombo, SIGNAL(activated (const QString &)), this, SLOT(OnWorkbenchChange(const QString &)));



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
	try{
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
	}catch(...)
	{
	}
#endif
}


/** just additionally fits the system menu button position to the menu position */
void ApplicationWindow::resizeEvent ( QResizeEvent *e)
{
   QextMdiMainFrm::resizeEvent( e);
   setSysButtonsAtMenuPosition();
}

void ApplicationWindow::closeEvent ( QCloseEvent * e )
{/*
  long lNbOfDocs = lpcDocuments.size();
  FCView* pView = GetActiveView();
  while (pView != NULL && lNbOfDocs > 0)
  {
    lNbOfDocs--;
    pView->close();
    if (lNbOfDocs != lpcDocuments.size())
      return; // cancel pressed
    pView = GetActiveView();
  }
*/
  QextMdiMainFrm::closeEvent(e);
}

void ApplicationWindow::exportImage()
{
  //FCView* w = (FCView*) stWs->activeWindow();
  //w->dump();
}


void ApplicationWindow::OnWorkbenchChange( const QString & string)
{
	if(_cActiveWorkbenchName != string)
	{
#ifndef FC_DEBUG
		try{
#endif
			ActivateWorkbench(string.latin1());
#ifndef FC_DEBUG
		}
		catch(...){
			throw FCException("Error in initialising Workbench!");
		}
#endif

	}
}

void ApplicationWindow::ActivateWorkbench(const char* name)
{
	// net buffer because of char* <-> const char*
	char sBuf[1024];
	assert(strlen(name) < 1022);

//	puts(name);

	// close old workbench
	if(_cActiveWorkbenchName != "")
	{
		strcpy(sBuf, _cActiveWorkbenchName.latin1());
		PyObject* pcOldWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, sBuf);
		assert(pcOldWorkbench);
		GetInterpreter().RunMethode(pcOldWorkbench, "Stop");
	}
	// get the python workbench object from the dictionary
	strcpy(sBuf, name);
	PyObject* pcWorkbench = PyDict_GetItemString(_pcWorkbenchDictionary, sBuf);

	// test if the workbench in
	assert(pcWorkbench);

	// runing the start of the workbench object
	GetInterpreter().RunMethode(pcWorkbench, "Start");

	_cActiveWorkbenchName = name;

	// set the combo box
	if(_pcWorkbenchCombo->currentText() != name){
		for(int i=0;i<_pcWorkbenchCombo->count();i++)
		{
			if(_pcWorkbenchCombo->text(i) == name)
			{
				_pcWorkbenchCombo->setCurrentItem(i);
				break;
			}
		}
	}

	show();
}

void ApplicationWindow::UpdateWorkbenchEntrys(void)
{
	PyObject *key, *value;
	int pos = 0;
     
	// remove all items from the combo box
	_pcWorkbenchCombo->clear();

	// insert all items
	while (PyDict_Next(_pcWorkbenchDictionary, &pos, &key, &value)) {
		/* do something interesting with the values... */
		_pcWorkbenchCombo->insertItem (QPixmap(FCIcon),PyString_AsString(key));
	}

}


//**************************************************************************
// Python stuff

// FCApplication Methods						// Methods structure
PyMethodDef ApplicationWindow::Methods[] = {
	{"ToolbarAddTo",          (PyCFunction) ApplicationWindow::sToolbarAddTo,            1},
	{"ToolbarDelete",         (PyCFunction) ApplicationWindow::sToolbarDelete,           1},
	{"ToolbarAddSeperator",   (PyCFunction) ApplicationWindow::sToolbarAddSeperator,     1},
	{"ToolbarLoadSettings",   (PyCFunction) ApplicationWindow::sToolbarLoadSettings,     1},
	{"CommandbarAddTo",       (PyCFunction) ApplicationWindow::sCommandbarAddTo,         1},
	{"CommandbarLoadSettings",(PyCFunction) ApplicationWindow::sCommandbarLoadSettings,  1},
	{"CommandbarDelete",      (PyCFunction) ApplicationWindow::sCommandbarDelete,        1},
	{"CommandbarAddSeperator",(PyCFunction) ApplicationWindow::sCommandbarAddSeperator,  1},
	{"WorkbenchAdd",          (PyCFunction) ApplicationWindow::sWorkbenchAdd,            1},
	{"WorkbenchDelete",       (PyCFunction) ApplicationWindow::sWorkbenchDelete,         1},
	{"WorkbenchActivate",     (PyCFunction) ApplicationWindow::sWorkbenchActivate,       1},
	{"WorkbenchGet",          (PyCFunction) ApplicationWindow::sWorkbenchGet,            1},

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
	try{
		Instance->_cCommandManager.AddTo(psCmdName,pcBar);
	}catch(FCException e) {
		//e.ReportException();
		PyErr_SetString(PyExc_AssertionError, e.what());		
		return NULL;
	}catch(...){
		PyErr_SetString(PyExc_RuntimeError, "unknown error");
		return NULL;
	}
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

PYFUNCIMP_S(ApplicationWindow,sToolbarLoadSettings)
{
	char *psToolbarName;
	if (!PyArg_ParseTuple(args, "s", &psToolbarName))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 

	FCToolBar * pcBar = (FCToolBar*)Instance->GetToolBar(psToolbarName);
	pcBar->loadUserDefButtons();
    return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sCommandbarAddTo)
{
	char *psToolbarName,*psCmdName;
	if (!PyArg_ParseTuple(args, "ss", &psToolbarName,&psCmdName))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 

	FCToolboxGroup * pcBar = Instance->GetCommandBar(psToolbarName);
	try{
		Instance->_cCommandManager.AddTo(psCmdName,pcBar);
	}catch(FCException e) {
		//e.ReportException();
		PyErr_SetString(PyExc_AssertionError, e.what());		
		return NULL;
	}catch(...){

		PyErr_SetString(PyExc_RuntimeError, "unknown error");
		return NULL;
	}
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

PYFUNCIMP_S(ApplicationWindow,sCommandbarLoadSettings)
{
	char *psCmdbarName;
	if (!PyArg_ParseTuple(args, "s", &psCmdbarName))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 

  Instance->GetCommandBar(psCmdbarName)->loadUserDefButtons();
  return Py_None;
} 


PYFUNCIMP_S(ApplicationWindow,sWorkbenchAdd)
{
	char*       psKey;
	PyObject*   pcObject;
	if (!PyArg_ParseTuple(args, "sO", &psKey,&pcObject))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	Py_INCREF(pcObject);

	PyDict_SetItemString(Instance->_pcWorkbenchDictionary,psKey,pcObject);

	Instance->UpdateWorkbenchEntrys();

	return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sWorkbenchDelete)
{
	char*       psKey;
	if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	PyDict_DelItemString(Instance->_pcWorkbenchDictionary,psKey);

    return Py_None;
} 

PYFUNCIMP_S(ApplicationWindow,sWorkbenchActivate)
{
	char*       psKey;
	if (!PyArg_ParseTuple(args, "s", &psKey))     // convert args: Python->C 
		return NULL;										// NULL triggers exception 

	Instance->ActivateWorkbench(psKey);
    return Py_None;
}

PYFUNCIMP_S(ApplicationWindow,sWorkbenchGet)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

	return Instance->_pcWorkbenchDictionary;
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
	if(!bMute)
		_pcAppWnd->statusBar()->message( m, 2001 );
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
		QMessageBox::information( _pcAppWnd, "Exception happens",m);
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
	}

	GetConsole().Warning("Cant find Pixmap:%s\n",sName);

	return QPixmap(px);

}







#include "moc_Application.cpp"
