/***************************************************************************
                          HtmlView.cpp  -  description
                             -------------------
    begin                : 2002/10/19 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief The Html view module
 *  \author Werner Mayer
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
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/

 


/* XPM */
/* Drawn  by Mark Donohoe for the K Desktop Environment */
/* See http://www.kde.org */
static const char* back_pixmap[]={
"16 16 5 1",
"# c #000000",
"a c #ffffff",
"c c #808080",
"b c #c0c0c0",
". c None",
"................",
".......#........",
"......##........",
".....#a#........",
"....#aa########.",
"...#aabaaaaaaa#.",
"..#aabbbbbbbbb#.",
"...#abbbbbbbbb#.",
"...c#ab########.",
"....c#a#ccccccc.",
".....c##c.......",
"......c#c.......",
".......cc.......",
"........c.......",
"................",
"......................"};

/* XPM */
/* Drawn  by Mark Donohoe for the K Desktop Environment */
/* See http://www.kde.org */
static const char* forward_pixmap[]={
"16 16 5 1",
"# c #000000",
"a c #ffffff",
"c c #808080",
"b c #c0c0c0",
". c None",
"................",
"................",
".........#......",
".........##.....",
".........#a#....",
"..########aa#...",
"..#aaaaaaabaa#..",
"..#bbbbbbbbbaa#.",
"..#bbbbbbbbba#..",
"..########ba#c..",
"..ccccccc#a#c...",
"........c##c....",
"........c#c.....",
"........cc......",
"........c.......",
"................",
"................"};

/* XPM */
/* Drawn  by Mark Donohoe for the K Desktop Environment */
/* See http://www.kde.org */
static const char* home_pixmap[]={
"16 16 4 1",
"# c #000000",
"a c #ffffff",
"b c #c0c0c0",
". c None",
"........... ....",
"   ....##.......",
"..#...####......",
"..#..#aabb#.....",
"..#.#aaaabb#....",
"..##aaaaaabb#...",
"..#aaaaaaaabb#..",
".#aaaaaaaaabbb#.",
"###aaaaaaaabb###",
"..#aaaaaaaabb#..",
"..#aaa###aabb#..",
"..#aaa#.#aabb#..",
"..#aaa#.#aabb#..",
"..#aaa#.#aabb#..",
"..#aaa#.#aabb#..",
"..#####.######..",
"................"};

#include "HtmlView.h"
#include "Application.h"

#include <qbuttongroup.h>

#include <ctype.h>

FCTextBrowser::FCTextBrowser(QWidget * parent, const char * name)
: QTextBrowser(parent, name)
{
  pclPopup = new QPopupMenu(0L);
  connect(pclPopup, SIGNAL(aboutToShow()), this, SLOT(popupMenuAboutToShow()));
  connect( this, SIGNAL( backwardAvailable( bool ) ), this, SLOT( setBackwardAvailable( bool ) ) );
  connect( this, SIGNAL( forwardAvailable( bool ) ), this, SLOT( setForwardAvailable( bool ) ) );
}

void FCTextBrowser::viewportMousePressEvent ( QMouseEvent * e )
{
  if (e->button() == RightButton)
  {
    pclPopup->exec(QCursor::pos());
  }
  else
    QTextBrowser::viewportMousePressEvent(e);
}

void FCTextBrowser::popupMenuAboutToShow()
{
  pclPopup->clear();
  if (hasSelectedText())
  {
    pclPopup->insertItem("Copy", this, SLOT(copy()));
  }
  else
  {
    int iBack = pclPopup->insertItem(QPixmap(back_pixmap), "Back", this, SLOT(backward()));
    pclPopup->setItemEnabled(iBack, bBackward);
    int iForw = pclPopup->insertItem(QPixmap(forward_pixmap), "Forward", this, SLOT(forward()));
    pclPopup->setItemEnabled(iForw, bForward);
    pclPopup->insertItem(QPixmap(home_pixmap), "Home", this, SLOT(home()));
    pclPopup->insertSeparator();
    pclPopup->insertItem("Refresh", this, SLOT(refreshPage()));
    pclPopup->insertSeparator();
  }

  pclPopup->insertItem("Select all", this, SLOT(selectAll()));
}

void FCTextBrowser::setBackwardAvailable(bool b)
{
  bBackward = b;
}

void FCTextBrowser::setForwardAvailable (bool b)
{
  bForward = b;
}

void FCTextBrowser::refreshPage()
{
  backward();
  forward();
}

/* 
 *  Constructs a FCHtmlView which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
FCHtmlView::FCHtmlView( const QString& home_,  const QString& path, QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl ), pclPathCombo( 0 ), selectedURL()
{
  readHistory();
  readBookmarks();

  // create the actual browser
  pclBrowser = new FCTextBrowser( this, "HelpBrowser" );
  pclBrowser->mimeSourceFactory()->setFilePath( path );
  pclBrowser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  connect( pclBrowser, SIGNAL( textChanged() ), this, SLOT( textChanged() ) );

  if ( !home_.isEmpty() )
  	pclBrowser->setSource( home_ );

  // the browser's functionality
  connect( pclBrowser, SIGNAL( backwardAvailable( bool ) ), this, SLOT( setBackwardAvailable( bool ) ) );
  connect( pclBrowser, SIGNAL( forwardAvailable( bool ) ), this, SLOT( setForwardAvailable( bool ) ) );

  // create the button group for the layout
  pclButtonGrp = new QButtonGroup( this, "ButtonGroup" );
  pclButtonGrp->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  pclButtonGrp->setProperty( "title", tr( "" ) );
  pclButtonGrp->setColumnLayout(0, Qt::Vertical );
  pclButtonGrp->layout()->setSpacing( 0 );
  pclButtonGrp->layout()->setMargin( 0 );
  pclButtonGrpLayout = new QHBoxLayout( pclButtonGrp->layout() );
  pclButtonGrpLayout->setAlignment( Qt::AlignTop );
  pclButtonGrpLayout->setSpacing( 1 );
  pclButtonGrpLayout->setMargin ( 1 );

  // create the buttons
  //
  //
  // the 'Backward' button
  pclButtonBack = new QToolButton( pclButtonGrp, tr("Backward") );
  pclButtonBack->setProperty( "minimumSize", QSize( 25, 25 ) );
  pclButtonBack->setProperty( "text", tr( "..." ) );
  pclButtonBack->setProperty( "pixmap", QPixmap(back_pixmap) );
	pclButtonBack->setAutoRaise(true);

  // the 'Forward' button
  pclButtonForward = new QToolButton( pclButtonGrp, tr("Forward") );
  pclButtonForward->setProperty( "minimumSize", QSize( 25, 25 ) );
  pclButtonForward->setProperty( "text", tr( "..." ) );
  pclButtonForward->setProperty( "pixmap", QPixmap(forward_pixmap) );
	pclButtonForward->setAutoRaise(true);

  // the 'Home' button
  pclButtonHome = new QToolButton( pclButtonGrp, tr("Home") );
  pclButtonHome->setProperty( "minimumSize", QSize( 25, 25 ) );
  pclButtonHome->setProperty( "text", tr( "..." ) );
  pclButtonHome->setProperty( "pixmap", QPixmap(home_pixmap) );
	pclButtonHome->setAutoRaise(true);

  // the 'Path' combo box (it's the history)
  pclPathCombo = new QComboBox( true, pclButtonGrp, "Paths" );
  pclPathCombo->setProperty( "minimumSize", QSize( 160, 25 ) );
  pclPathCombo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

  // the buttons' functionality
  pclButtonForward->setEnabled( false );
  pclButtonBack->setEnabled( false );
  connect(pclButtonBack, SIGNAL(clicked()), pclBrowser, SLOT(backward()));
  connect(pclBrowser, SIGNAL( backwardAvailable(bool) ), pclButtonBack, SLOT( setEnabled(bool) ) );
  connect(pclButtonForward, SIGNAL(clicked()), pclBrowser, SLOT(forward()));
  connect(pclBrowser, SIGNAL( forwardAvailable(bool) ), pclButtonForward, SLOT( setEnabled(bool) ) );
  connect(pclButtonHome, SIGNAL(clicked()), pclBrowser, SLOT(home()));
  connect(pclPathCombo, SIGNAL( activated( const QString & ) ), this, SLOT( pathSelected( const QString & ) ) );

  // make the layout of the browser
  //
  //
  pclFormLayout = new QGridLayout( this ); 
  pclFormLayout->setSpacing( 1 );
  pclFormLayout->setMargin ( 1 );

  // add the three buttons and the combo box
  pclButtonGrpLayout->addWidget( pclButtonBack );
  pclButtonGrpLayout->addWidget( pclButtonForward );
  pclButtonGrpLayout->addWidget( pclButtonHome );
  QSpacerItem* spacer = new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  pclButtonGrpLayout->addItem( spacer );
  pclButtonGrpLayout->addWidget( pclPathCombo );

  // add the button group with its elements and the browser to the layout
  pclFormLayout->addWidget( pclButtonGrp, 0, 0 );
  pclFormLayout->addWidget( pclBrowser,   1, 0 );

  // insert the start page to the history
  pclPathCombo->insertItem( home_ );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCHtmlView::~FCHtmlView()
{
    // no need to delete child widgets, Qt does it all for us
  history.clear();
  QMap<int, QString>::Iterator it = mHistory.begin();
  for ( ; it != mHistory.end(); ++it )
  	history.append( *it );

  QFile f( QDir::currentDirPath() + "/.history" );
  f.open( IO_WriteOnly );
  QDataStream s( &f );
  s << history;
  f.close();

  bookmarks.clear();
  QMap<int, QString>::Iterator it2 = mBookmarks.begin();
  for ( ; it2 != mBookmarks.end(); ++it2 )
  	bookmarks.append( *it2 );

  QFile f2( QDir::currentDirPath() + "/.bookmarks" );
  f2.open( IO_WriteOnly );
  QDataStream s2( &f2 );
  s2 << bookmarks;
  f2.close();
}

void FCHtmlView::setBackwardAvailable( bool b)
{
  pclButtonBack->setEnabled(b);
}

void FCHtmlView::setForwardAvailable( bool b)
{
  pclButtonForward->setEnabled(b);
}

void FCHtmlView::textChanged()
{
  if ( pclBrowser->documentTitle().isNull() ) 
  {
  	setCaption( "FreeCAD - Helpviewer - " + pclBrowser->context() );
	  selectedURL = pclBrowser->context();
  }
  else 
  {
  	setCaption( "FreeCAD - Helpviewer - " + pclBrowser->documentTitle() ) ;
	  selectedURL = pclBrowser->documentTitle();
  }
    
  if ( !selectedURL.isEmpty() && pclPathCombo ) 
  {
  	bool exists = FALSE;
	  int i;
	  for ( i = 0; i < pclPathCombo->count(); ++i ) 
    {
	    if ( pclPathCombo->text( i ) == selectedURL ) 
      {
  	  	exists = TRUE;
	  	  break;
	    }
	  }
  	if ( !exists ) 
    {
	    pclPathCombo->insertItem( selectedURL, 0 );
	    pclPathCombo->setCurrentItem( 0 );
//	    mHistory[ hist->insertItem( selectedURL ) ] = selectedURL;
  	} 
    else
	    pclPathCombo->setCurrentItem( i );

    selectedURL = QString::null;
  }
}

void FCHtmlView::openFile()
{
#ifndef QT_NO_FILEDIALOG
  QString fn = QFileDialog::getOpenFileName( QString::null, QString::null, this );
  if ( !fn.isEmpty() )
  	pclBrowser->setSource( fn );
#endif
}

void FCHtmlView::pathSelected( const QString &_path )
{
  pclBrowser->setSource( _path );
  QMap<int, QString>::Iterator it = mHistory.begin();
  bool exists = FALSE;
  for ( ; it != mHistory.end(); ++it ) 
  {
  	if ( *it == _path ) 
    {
	    exists = TRUE;
	    break;
  	}
  }

//  if ( !exists )
//  	mHistory[ hist->insertItem( _path ) ] = _path;
}

void FCHtmlView::readHistory()
{
  if ( QFile::exists( QDir::currentDirPath() + "/.history" ) ) 
  {
  	QFile f( QDir::currentDirPath() + "/.history" );
	  f.open( IO_ReadOnly );
	  QDataStream s( &f );
	  s >> history;
	  f.close();
	  while ( history.count() > 20 )
	    history.remove( history.begin() );
  }
}

void FCHtmlView::readBookmarks()
{
  if ( QFile::exists( QDir::currentDirPath() + "/.bookmarks" ) ) 
  {
  	QFile f( QDir::currentDirPath() + "/.bookmarks" );
	  f.open( IO_ReadOnly );
	  QDataStream s( &f );
	  s >> bookmarks;
	  f.close();
  }
}

void FCHtmlView::histChosen( int i )
{
  if ( mHistory.contains( i ) )
  	pclBrowser->setSource( mHistory[ i ] );
}

void FCHtmlView::bookmChosen( int i )
{
  if ( mBookmarks.contains( i ) )
  	pclBrowser->setSource( mBookmarks[ i ] );
}

void FCHtmlView::addBookmark()
{
//  mBookmarks[ bookm->insertItem( caption() ) ] = caption();
}

void FCHtmlView::mouseDoubleClickEvent ( QMouseEvent * e )
{
  pclBrowser->selectAll();
  pclBrowser->copy();
}

#include "HtmlView_moc.cpp"
