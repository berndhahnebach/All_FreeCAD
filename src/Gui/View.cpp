#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qvbox.h>
#	include <qtoolbar.h>
#	include <qfiledialog.h>
#	include <qapplication.h>
#	include <qmessagebox.h>
#	include <qsplitter.h>
#	include <qwidgetstack.h>
#	include <qpushbutton.h>
#	include <qtabbar.h>
#	include <qlayout.h>
#endif



//#include "MDIWindow.h"
#include "Document.h"
//#include "ViewOperations.h"
#include "View3D.h"
#include "Tree.h"




FCView::FCView( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags )
    :QextMdiChildView( parent, name, wflags ),
	 _pcDocument(pcDocument)
{
	
	QVBox* vb2;
	//vb= new QVBox( this );
    QVBoxLayout* vb = new QVBoxLayout(this);

	//vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	// set the splitter frames
	_pcSplitter = new QSplitter( QSplitter::Horizontal, this, "Main");
	vb->addWidget(_pcSplitter);
	vb2 = new QVBox( _pcSplitter );
	_pcSplitter->setResizeMode(vb2,QSplitter::KeepSize);
	QValueList<int> size;
	size.append(180);
	size.append(100);
	_pcSplitter->setSizes (size);

	vb2->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
	_pcWidget	= new QWidget (vb2);
	//_pcWidget	= new QWidget (_pcSplitter);
	_pcTree		= new FCTree(_pcDocument,_pcWidget,"Tree");
	_pcTabBar	= new QTabBar(_pcWidget);
	_pcTabBar->setShape(QTabBar::RoundedBelow);
	_pcTabBar->addTab(new QTab("3D"));
	_pcTabBar->addTab(new QTab("Raw"));

	_pcVBoxLayout		= new QVBoxLayout( _pcWidget ); // A layout on the widget
    _pcVBoxLayout->addWidget( _pcTree );
	_pcVBoxLayout->addWidget( _pcTabBar );

	vb2 = new QVBox( _pcSplitter );
	vb2->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	_pcView3D = new View3D(_pcDocument,vb2);
	
	//createViewActions();
	
}

FCView::~FCView()
{
  delete _pcView3D;
}

FCGuiDocument* FCView::getDocument()
{
	return _pcDocument;
}

void FCView::closeEvent(QCloseEvent* e)
{
	emit sendCloseView(this);
}
/*

void FCView::fitAll()
{
	myView->fitAll();
}

void FCView::createViewActions()
{
    // populate a tool bar with some actions

    QToolBar* aToolBar = new QToolBar( this, "view operations" );
    aToolBar->setLabel( tr( "View Operations" ) );
	
	QList<QAction>* aList = myOperations->getViewActions();

	for(QAction* a = aList->first();a;a = aList->next())
		a->addTo(aToolBar);

	connect(myOperations,SIGNAL(fitAll()),myView,SLOT(fitAll()));
	connect(myOperations,SIGNAL(fitArea()),myView,SLOT(fitArea()));
	connect(myOperations,SIGNAL(zoom()),myView,SLOT(zoom()));
	connect(myOperations,SIGNAL(pan()),myView,SLOT(pan()));
	connect(myOperations,SIGNAL(rotation()),myView,SLOT(rotation()));
	connect(myOperations,SIGNAL(globalPan()),myView,SLOT(globalPan()));
	connect(myOperations,SIGNAL(front()),myView,SLOT(front()));
	connect(myOperations,SIGNAL(back()),myView,SLOT(back()));
	connect(myOperations,SIGNAL(top()),myView,SLOT(top()));
	connect(myOperations,SIGNAL(bottom()),myView,SLOT(bottom()));
	connect(myOperations,SIGNAL(left()),myView,SLOT(left()));
	connect(myOperations,SIGNAL(right()),myView,SLOT(right()));
	connect(myOperations,SIGNAL(axo()),myView,SLOT(axo()));
	connect(myOperations,SIGNAL(reset()),myView,SLOT(reset()));
	connect(myOperations,SIGNAL(hlrOn()),myView,SLOT(hlrOn()));
	connect(myOperations,SIGNAL(hlrOff()),myView,SLOT(hlrOff()));

	aList->at(ViewOperations::ViewHlrOffId)->setOn(true);

}
*/
void FCView::onWindowActivated ()
{
  //myOperations->onSelectionChanged();
}

void FCView::setCursor(const QCursor& aCursor)
{
  _pcView3D->setCursor(aCursor);
}

void FCView::dump()
{
	static QString filter = "Images Files (*.bmp *.gif *.xwd)";
	QFileDialog fd ( 0, 0, true );     
	fd.setFilters( filter );
	fd.setCaption ( tr("INF_APP_IMPORT") );
	int ret = fd.exec(); 
    
	/* update the desktop after the dialog is closed */
	qApp->processEvents();

	QString file (ret == QDialog::Accepted ? fd.selectedFile() : QString::null);

	if ( !file.isNull() ) {
	  QApplication::setOverrideCursor( Qt::waitCursor );
	  if(!_pcView3D->ScreenDump( (const Standard_CString) file.latin1())) {
	    QApplication::restoreOverrideCursor();                
	    QMessageBox::information ( qApp->mainWidget(),tr("TIT_ERROR"), tr("INF_ERROR"), tr("BTN_OK"),
		  		       QString::null, QString::null, 0, 0);
	    qApp->processEvents();  /* update desktop */	
	  } else 
	    QApplication::restoreOverrideCursor();                
	}
}


#include "View_moc.cpp"