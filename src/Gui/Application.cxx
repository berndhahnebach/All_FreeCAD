
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

#include "Icons/x.xpm"
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

	myNbDocuments = 0;
	stApp = this;
	myIsDocuments = false;

    // create and define the central widget

    //QVBox* vb = new QVBox( this );
    //vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    //stWs = new QWorkspace( vb );
    //stWs->windowList().setAutoDelete(false);
    //setCentralWidget( vb );

	//setBackgroundPixmap(QPixmap(Background3));
	//setUsesBigPixmaps (true);
	CreateTestOperations();
	//createCasCadeOperations();

	GetCmdLine().SetParent(statusBar());
	statusBar()->addWidget(&FCCommandLine::Instance(), 0, true);

    statusBar()->message( tr("Ready"), 2001 );
    resize( 800, 600 );

	QStackBar* stack = new QStackBar;
	FCButtonGroup* mle = new FCButtonGroup(3, QButtonGroup::Horizontal, "Buttons", stack);
	stack->addPage( new QStackBarBtn( "Test", mle ) );
	stack->addPage( new QStackBarBtn( "Test", mle ) );
	stack->addPage( new QStackBarBtn( "Test", mle ) );
	stack->addPage( new QStackBarBtn( "Test", mle ) );
//  addToolWindow( new FCButtonGroup(3, QButtonGroup::Horizontal, "Buttons"), KDockWidget::DockRight, m_pMdi, 70, "Buttons", "Buttons");
	addToolWindow( stack, KDockWidget::DockRight, m_pMdi, 70, "Buttons", "Buttons");
	for (int i=0; i<30;i++)
	{
	  QToolButton* b0 = new QToolButton( /*DownArrow,*/ mle, "text" );
	  b0->setProperty( "pixmap", QPixmap(px) );
	  b0->setAutoRaise(true);
	  b0->setTextLabel("Hallo Welt", true);
	  b0->setFixedSize(32, 32);
	  mle->insert(b0);

/*	  if (i==12)
	  {
		b0->setProperty( "pixmap", QPixmap(px) );
		connect(b0, SIGNAL(clicked()), this, SLOT(close()));
	  }*/
	}

//	QTextBrowser *pBrowser = new QTextBrowser();
//	pBrowser->setText(FreeCADAbout);
//	addToolWindow( pBrowser, KDockWidget::DockRight, m_pMdi, 70, "Help", "Help");


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
	FCGuiDocument* pcGDoc=0;

	for(stlport::list<FCGuiDocument*>::iterator It = lpcDocuments.begin();It != lpcDocuments.end();It++)
	{
		if( ((*It)->GetDoc()) == pcDoc)
		{
			pcGDoc = *It;
			lpcDocuments.erase(It);
		}
	}
	delete pcGDoc;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// slots with the application std commands
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ApplicationWindow::slotNewDoc()
{

  DlgDocTemplatesImp cDlg(this,"Dialog",true);
  cDlg.exec();
  //connect(aDoc,SIGNAL(sendCloseDocument(Document*)),this,SLOT(onCloseDocument(Document*)));
  //connect(stWs,SIGNAL(windowActivated(QWidget*)),this,SLOT(onWindowActivated(QWidget*)));
}

void ApplicationWindow::slotOpen(void)
{
	statusBar()->message("Opening file...");

	QString f = QFileDialog::getOpenFileName( QString::null, "FreeCAD Standard (*.FCStd);;OpenCasCade (*.std)", this );
	if ( !f.isEmpty() ) {
		// the user selected a valid existing file
		GetApplication().Open(f.latin1());
	} else {
		// the user cancelled the dialog
		statusBar()->message("Opening aborted");
	}


}

void ApplicationWindow::slotSave(void)
{

}

void ApplicationWindow::slotSaveAs(void)
{
  statusBar()->message("Saving file under new filename...");
  QString fn = QFileDialog::getSaveFileName(0, 0, this);
  if (!fn.isEmpty())
  {
    //doc->saveAs(fn);
  }
  else
  {
    statusBar()->message("Saving aborted", 2000);
  }

  //statusBar()->message(IDS_STATUS_DEFAULT);

}

void ApplicationWindow::slotPrint(void)
{
	statusBar()->message("Printing...");
	QPrinter printer;
	if (printer.setup(this))
	{
		QPainter painter;
		painter.begin(&printer);

		///////////////////////////////////////////////////////////////////
		// TODO: Define printing by using the QPainter methods here

		painter.end();
	};

	//statusBar()->message(IDS_STATUS_DEFAULT);

}

void ApplicationWindow::slotUndo(void)
{

}

void ApplicationWindow::slotRedo(void)
{

}

void ApplicationWindow::slotCut(void)
{

}

void ApplicationWindow::slotCopy(void)
{

}

void ApplicationWindow::slotPaste(void)
{

}




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

/*
	//myStdActions.insert(FileNewId, fileNewAction);	
    //fileCloseAction = new QAction( tr("Close", closeIcon, "Close", CTRL+Key_W, this, "close");
    //connect( fileCloseAction, SIGNAL( activated() ) , this, SLOT( onCloseWindow() ) );
	//myStdActions.insert(FileCloseId, fileCloseAction);	

    pcTemp = new QAction( "Quit", "Quit", CTRL+Key_Q, this, "quit" );
    connect( pcTemp, SIGNAL( activated() ) , qApp, SLOT( closeAllWindows() ) );
	_cCommandManager.AddCommand("Std_Quit",FCCommand(pcTemp));
*/
	_pclUndoRedoWidget = new FCUndoRedoDlg(this, "Undo/Redo");
	connect(_pclUndoRedoWidget, SIGNAL(clickedListBox()), this, SLOT(executeUndoRedo()));

/*
	viewToolAction = new QAction( tr("TBR_TOOL_BAR"), tr("MNU_TOOL_BAR"), 0, this, "toolbar");
	connect( viewToolAction, SIGNAL( activated() ) , this, SLOT( onViewToolBar() ));
	viewToolAction->setToggleAction(true);
	viewToolAction->setOn( true );
	myStdActions.insert(ViewToolId, viewToolAction);	

	viewStatusAction = new QAction( tr("TBR_STATUS_BAR"), tr("MNU_STATUS_BAR"), 0, this, "statusbar");
	connect( viewStatusAction, SIGNAL( activated() ), this, SLOT( onViewStatusBar() ));
	viewStatusAction->setToggleAction(true);
	viewStatusAction->setOn( true );
	myStdActions.insert(ViewStatusId, viewStatusAction);	

	helpAboutAction = new QAction( tr("TBR_ABOUT"), helpIcon, tr("MNU_ABOUT"), Key_F1, this, "about" );
	connect( helpAboutAction, SIGNAL( activated() ) , this, SLOT( onAbout() ) );
	myStdActions.insert(HelpAboutId, helpAboutAction);	
*/
    // populate a tool bar with some actions

    myStdToolBar = new QToolBar( this, "file operations" );
    myStdToolBar->setLabel( "File" );
	_cCommandManager.AddTo("Std_New",myStdToolBar);
	_cCommandManager.AddTo("Std_Open",myStdToolBar);
	_cCommandManager.AddTo("Std_Save",myStdToolBar);
	_cCommandManager.AddTo("Std_Print",myStdToolBar);
	myStdToolBar->addSeparator();
	_cCommandManager.AddTo("Std_Cut",myStdToolBar);
	_cCommandManager.AddTo("Std_Copy",myStdToolBar);
	_cCommandManager.AddTo("Std_Paste",myStdToolBar);
	myStdToolBar->addSeparator();
	//_cCommandManager.AddTo("Std_Undo",myStdToolBar);
	//_cCommandManager.AddTo("Std_Redo",myStdToolBar);
	//myStdToolBar->addSeparator();
	// Undo/Redo Toolbutton
	QToolButton* button = new QToolButtonDropDown(myStdToolBar, QPixmap(pUndo), _pclUndoRedoWidget);
	connect(button, SIGNAL(clicked()), this, SLOT(slotUndo()));
	connect(button, SIGNAL(updateWidgetSignal()), this, SLOT(updateUndo()));

	button = new QToolButtonDropDown(myStdToolBar, QPixmap(pRedo), _pclUndoRedoWidget);
	connect(button, SIGNAL(clicked()), this, SLOT(slotRedo()));
	connect(button, SIGNAL(updateWidgetSignal()), this, SLOT(updateRedo()));



    // popuplate a menu with all actions

    myFilePopup = new QPopupMenu( this );
    menuBar()->insertItem( "File", myFilePopup) ;
	_cCommandManager.AddTo("Std_New",myFilePopup);
	_cCommandManager.AddTo("Std_Open",myFilePopup);
	_cCommandManager.AddTo("Std_Save",myFilePopup);
	_cCommandManager.AddTo("Std_SaveAs",myFilePopup);
	myFilePopup->insertSeparator();
	_cCommandManager.AddTo("Std_Print",myFilePopup);
	myFilePopup->insertSeparator();
	//_cCommandManager.AddTo("Std_Quit",myFilePopup);

    myFilePopup = new QPopupMenu( this );
    menuBar()->insertItem( "Work", myFilePopup );
	_cCommandManager.AddTo("Std_Cut",myFilePopup);
	_cCommandManager.AddTo("Std_Copy",myFilePopup);
	_cCommandManager.AddTo("Std_Paste",myFilePopup);
	

	//QextMdiChildView *v1 = new QextMdiChildView("Test",this);
//	v1->setTabCaption("Tab changed");
//    v1->setBackgroundColor( Qt::white );
//	addWindow( v1);
/*   v1->setGeometry(40, 40, 400, 100);
   v1->setFont( QFont("times",20,QFont::Bold) );
   v1->setCaption("MDI Test Application");
   v1->show();
*/
	

	setMenuForSDIModeSysButtons( menuBar());
	 
//    fileCloseAction->addTo( myFilePopup );
//    myFilePopup->insertSeparator();
//    fileQuitAction->addTo( myFilePopup );

	// add a view menu
/*
	QPopupMenu * view = new QPopupMenu( this );
	menuBar()->insertItem( tr("MNU_VIEW"), view);
	viewToolAction->addTo(view);
	viewStatusAction->addTo(view);
 */   
	// add a help menu
/*
    QPopupMenu * help = new QPopupMenu( this );
    menuBar()->insertSeparator();
    menuBar()->insertItem( tr("MNU_HELP"), help );
	helpAboutAction->addTo( help );
	*/
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




/*
void ApplicationWindow::createCasCadeOperations()
{
    myCasCadeTranslateActions.setAutoDelete( true );
	
	//createTranslatePopups();
	//createWindowPopup();

    // populate a tool bar with some actions
    myCasCadeBar = new QToolBar( this, "shape operations" );
    myCasCadeBar->setLabel( tr( "Shape Operations" ) );

	//ViewOperations* anOper = new ViewOperations();

	//QList<QAction>* aList = anOper->getToolActions();

	//for(QAction* anAction = aList->first(); anAction; anAction = aList->next()) {
	//	anAction->addTo(myCasCadeBar);
	//}
	//myCasCadeBar->hide();
}*/
/*
void ApplicationWindow::createTranslatePopups()
{
	myImportPopup = new QPopupMenu(0);
	myExportPopup = new QPopupMenu(0);

	QAction* a;

	a = new QAction( tr("TBR_IMPORT_BREP"), tr("MNU_IMPORT_BREP"), 0, this, "import brep" );
	connect( a, SIGNAL( activated() ) , this, SLOT( onTranslate() ) );
	myCasCadeTranslateActions.insert(FileImportBREPId, a);	
	a->addTo(myImportPopup);

	a = new QAction( tr("TBR_EXPORT_BREP"), tr("MNU_EXPORT_BREP"), 0, this, "export brep" );
	connect( a, SIGNAL( activated() ) , this, SLOT( onTranslate() ) );
	myCasCadeTranslateActions.insert(FileExportBREPId, a);	
	a->addTo(myExportPopup);

	a = new QAction( tr("TBR_IMPORT_CSFDB"), tr("MNU_IMPORT_CSFDB"), 0, this, "import csfdb" );
	connect( a, SIGNAL( activated() ) , this, SLOT( onTranslate() ) );
	myCasCadeTranslateActions.insert(FileImportCSFDBId, a);	
	a->addTo(myImportPopup);

	a = new QAction( tr("TBR_EXPORT_CSFDB"), tr("MNU_EXPORT_CSFDB"), 0, this, "export csfdb" );
	connect( a, SIGNAL( activated() ) , this, SLOT( onTranslate() ) );
	myCasCadeTranslateActions.insert(FileExportCSFDBId, a);	
	a->addTo(myExportPopup);

	a = new QAction( tr("TBR_IMPORT_IGES"), tr("MNU_IMPORT_IGES"), 0, this, "import iges" );
	connect( a, SIGNAL( activated() ) , this, SLOT( onTranslate() ) );
	myCasCadeTranslateActions.insert(FileImportIGESId, a);	
	a->addTo(myImportPopup);

	a = new QAction( tr("TBR_EXPORT_IGES"), tr("MNU_EXPORT_IGES"), 0, this, "export iges" );
	connect( a, SIGNAL( activated() ) , this, SLOT( onTranslate() ) );
	myCasCadeTranslateActions.insert(FileExportIGESId, a);	
	a->addTo(myExportPopup);

	a = new QAction( tr("TBR_IMPORT_STEP"), tr("MNU_IMPORT_STEP"), 0, this, "import step" );
	connect( a, SIGNAL( activated() ) , this, SLOT( onTranslate() ) );
	myCasCadeTranslateActions.insert(FileImportSTEPId, a);	
	a->addTo(myImportPopup);

	a = new QAction( tr("TBR_EXPORT_STEP"), tr("MNU_EXPORT_STEP"), 0, this, "export step" );
	connect( a, SIGNAL( activated() ) , this, SLOT( onTranslate() ) );
	myCasCadeTranslateActions.insert(FileExportSTEPId, a);	
	a->addTo(myExportPopup);

	a = new QAction( tr("TBR_EXPORT_STL"), tr("MNU_EXPORT_STL"), 0, this, "export stl" );
	connect( a, SIGNAL( activated() ) , this, SLOT( onTranslate() ) );
	myCasCadeTranslateActions.insert(FileExportSTLId, a);	
	a->addTo(myExportPopup);

	a = new QAction( tr("TBR_EXPORT_VRML"), tr("MNU_EXPORT_VRML"), 0, this, "export vrml" );
	connect( a, SIGNAL( activated() ) , this, SLOT( onTranslate() ) );
	myCasCadeTranslateActions.insert(FileExportVRMLId, a);	
	a->addTo(myExportPopup);				

	myExportPopup->insertSeparator();	

	a = new QAction( tr("TBR_EXPORT_IMAGE"), tr("MNU_EXPORT_IMAGE"), 0, this, "export image" );
	connect( a, SIGNAL( activated() ) , this, SLOT( exportImage() ) );
	a->addTo(myExportPopup);				
}
*/
/*
void ApplicationWindow::createWindowPopup()
{
	myWindowPopup = new QPopupMenu(this);
	connect( myWindowPopup, SIGNAL( aboutToShow() ),
		this, SLOT( windowsMenuAboutToShow() ) );
}

void ApplicationWindow::windowsMenuAboutToShow()
{
  
  myWindowPopup->clear();
  QAction* a;
  
  QString dir = getResourceDir();
  
  a = new QAction( tr("TBR_WINDOW_NEW3D"),QPixmap( dir+tr("ICON_WINDOW_NEW3D") ), tr("MNU_WINDOW_NEW3D"), 0, this, "new3d" );
  connect( a, SIGNAL( activated() ) , this, SLOT( onCreateNewView() ) );
  a->addTo(myWindowPopup);				
  
  a = new QAction( tr("TBR_WINDOW_CASCADE"),QPixmap( dir+tr("ICON_WINDOW_CASCADE") ), tr("MNU_WINDOW_CASCADE"), 0, this, "cascade" );
  connect( a, SIGNAL( activated() ) , stWs, SLOT( cascade() ) );
  a->addTo(myWindowPopup);				
  
  a = new QAction( tr("TBR_WINDOW_TILE"),QPixmap( dir+tr("ICON_WINDOW_TILE") ), tr("MNU_WINDOW_TILE"), 0, this, "tile" );
  connect( a, SIGNAL( activated() ) , stWs, SLOT( tile() ) );
  a->addTo(myWindowPopup);				

  myWindowPopup->insertSeparator();
  QWidgetList windows = stWs->windowList();
  for ( int i = 0; i < int(windows.count()); ++i ) {
    int id = myWindowPopup->insertItem(windows.at(i)->caption(),
				     this, SLOT( windowsMenuActivated( int ) ) );
    myWindowPopup->setItemParameter( id, i );
    myWindowPopup->setItemChecked( id, stWs->activeWindow() == windows.at(i) );
  }  
}

void ApplicationWindow::windowsMenuActivated( int id )
{
    QWidget* w = stWs->windowList().at( id );
    if ( w )
	w->setFocus();
}
*/

ApplicationWindow* ApplicationWindow::getApplication()
{
	return stApp;
}

/*
void ApplicationWindow::updateFileActions()
{
	int count = myFilePopup->count()-1;
	
	int popupCount = menuBar()->count();

	if(stWs->windowList().isEmpty())
		if(!myIsDocuments)
		{
			myFilePopup->insertItem(tr("MNU_FILE_IMPORT"),myImportPopup,-1,count++);
			myFilePopup->insertItem(tr("MNU_FILE_EXPORT"),myExportPopup,-1,count++);
			myFilePopup->insertSeparator( count++ );
			myIsDocuments = true;
			myCasCadeBar->show();
			menuBar()->insertItem(tr("MNU_WINDOW"),myWindowPopup,0,popupCount-1);
		} else {
			myFilePopup->removeItemAt(--count);
			myFilePopup->removeItemAt(--count);
			myFilePopup->removeItemAt(--count);
			myIsDocuments = false;
			myCasCadeBar->hide();
			menuBar()->removeItemAt(popupCount-2);
	}
}
*/
/*
void ApplicationWindow::onCloseWindow(){
    MDIWindow* m = (MDIWindow*)stWs->activeWindow();
	Document* d = m->getDocument();
	onCloseDocument(d);
}

void ApplicationWindow::onCloseDocument(Document* theDoc)
{
	delete theDoc;
	//updateFileActions();
}

void ApplicationWindow::onViewToolBar()
{
    bool show = myStdActions.at( ViewToolId )->isOn();
    if (  show == myStdToolBar->isVisible() )
        return;
    if ( show )
        myStdToolBar->show();
    else
        myStdToolBar->hide();
}

void ApplicationWindow::onViewStatusBar()
{
    bool show = myStdActions.at( ViewStatusId )->isOn();
    if (  show == statusBar()->isVisible() )
        return;
    if ( show )
        statusBar()->show();
    else
        statusBar()->hide();
}

void ApplicationWindow::onAbout()
{
  QMessageBox::information(this,tr("TIT_ABOUT"),tr("INF_ABOUT"), tr("BTN_OK"),
		  		       QString::null, QString::null, 0, 0);
}

void ApplicationWindow::onQuit()
{
		QWidgetList aList = stWs->windowList();
		for(MDIWindow* aWindow = (MDIWindow*) aList.first();aWindow;aWindow = (MDIWindow*) aList.next()) {
		  aWindow->closeEvent(0);
		}

		emit sendQuit();
}

void ApplicationWindow::onTranslate()
{
    const QObject* sentBy = sender();
    typedef void  (Translate::*LPFN_IMPORT) 
        ( const Handle( AIS_InteractiveContext ));//, const QString&  );

    static LPFN_IMPORT lpfnImport[] = 
                                { &Translate::importBREP,
								  &Translate::exportBREP,
								  &Translate::importCSFDB,	
								  &Translate::exportCSFDB,
                                  &Translate::importIGES, 
								  &Translate::exportIGES,
		                          &Translate::importSTEP,
		                          &Translate::exportSTEP,
								  &Translate::exportVRML,
								  &Translate::exportSTL};

    static QAction* actions[] = { myCasCadeTranslateActions.at( FileImportBREPId ),
								  myCasCadeTranslateActions.at( FileExportBREPId ),
                                  myCasCadeTranslateActions.at( FileImportCSFDBId ),
								  myCasCadeTranslateActions.at( FileExportCSFDBId ),
                                  myCasCadeTranslateActions.at( FileImportIGESId ),
								  myCasCadeTranslateActions.at( FileExportIGESId ),
                                  myCasCadeTranslateActions.at( FileImportSTEPId ),
								  myCasCadeTranslateActions.at( FileExportSTEPId ),
								  myCasCadeTranslateActions.at( FileExportVRMLId ),
								  myCasCadeTranslateActions.at( FileExportSTLId )};
	
    for ( int i = 0; i < sizeof(actions)/sizeof(QAction*); i++ )
    {
        if ( actions[i] == sentBy ) 
        {
				Translate* anTrans = new Translate(0,"translate");
				Document* d = ((MDIWindow*) stWs->activeWindow())->getDocument();
				Handle(AIS_InteractiveContext) aContext = 
					d->getContext();
                try 
                {
                    // import 
                    (anTrans->*lpfnImport[i])( aContext );
                } 
                catch ( Standard_Failure )
                {
                    // WARNING: an exception raised but we eat it
                    //   silently because the shape seems OK
                                        
                }                
                // commit transaction 
				d->fitAll();
            break;
        }
    }
}

void ApplicationWindow::onCreateNewView()
{
	Document* d = ((MDIWindow*) stWs->activeWindow())->getDocument();
	d->onCreateNewView();
}

void ApplicationWindow::onWindowActivated ( QWidget * w )
{
  ((MDIWindow*) w)->onWindowActivated();
}
*/
/* Attic
QString ApplicationWindow::getResourceDir()
{
  static QString resDir("E:\\Develop\\Cas40\\Samples\\QT\\res");
  return resDir;
}
*/
void ApplicationWindow::exportImage()
{
  //FCView* w = (FCView*) stWs->activeWindow();
  //w->dump();
}


#include "Application_moc.cpp"