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
}

void FCTextBrowser::setSource (const QString & name)
{
  const QMimeSource * mime = mimeSourceFactory()->data(name);

  if (mime == NULL)
  {
    char szBuf[200];
    sprintf(szBuf, "Can't load %s", name.latin1());
    QMessageBox::information(this, "FreeCAD", szBuf);
    return;
  }

  QTextBrowser::setSource(name);
}

void FCTextBrowser::setText (const QString & contents, const QString & context)
{
  QTextBrowser::setText(contents, context);
}

void FCTextBrowser::viewportMousePressEvent (QMouseEvent * e)
{
  if (e->button() == RightButton)
  {
    // show the popup menu 
    // (you have to connect this signal with a slot in your class)
    emit showPopupMenu();
  }
  else
    QTextBrowser::viewportMousePressEvent(e);
}

/* 
 *  Constructs a FCHtmlView which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
FCHtmlView::FCHtmlView( const QString& home_,  QWidget* parent,  const char* name, WFlags fl )
    : FCDockWindow( parent, name, fl ), pclPathCombo( 0 ), selectedURL(), 
      bHistory(true), bBookm(true), bBackward(false), bForward(false)

{
  m_strCaption = "FCHelpViewer - ";

  // find the home directory of the online manual
  m_strDocDir = getHelpDirectory();

  QString home = m_strDocDir;

  // this is the complete URL of the start page
  // WARNING: home_ must NOT contain an absolute pathname but a relative 
  //          pathname
  home.append(home_);

  // read the old history and bookmark entries
  QStringList history   = readHistory();
  QStringList bookmarks = readBookmarks();

  // create the actual browser
  //
  pclBrowser = new FCTextBrowser(this, "FCHelpViewer");
  // set the path where the factory is looking for if you set a new source
  pclBrowser->mimeSourceFactory()->setFilePath( m_strDocDir ); 
  pclBrowser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  connect(pclBrowser, SIGNAL(textChanged()), this, SLOT(textChanged()));

  // set the start page now
  if (!home.isEmpty())
  	pclBrowser->setSource(home);

  // the browser's functionality
  connect( pclBrowser, SIGNAL(backwardAvailable(bool)), this, SLOT(setBackwardAvailable(bool)));
  connect( pclBrowser, SIGNAL(forwardAvailable (bool)), this, SLOT(setForwardAvailable (bool)));

  // create the button group for the layout
  pclButtonGrp = new QButtonGroup(this, "ButtonGroup");
  pclButtonGrp->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  pclButtonGrp->setProperty("title", tr( "" ));
  pclButtonGrp->setColumnLayout(0, Qt::Vertical);
  pclButtonGrp->layout()->setSpacing(0);
  pclButtonGrp->layout()->setMargin (0);
  pclButtonGrpLayout = new QHBoxLayout(pclButtonGrp->layout());
  pclButtonGrpLayout->setAlignment(Qt::AlignTop);
  pclButtonGrpLayout->setSpacing(1);
  pclButtonGrpLayout->setMargin (1);

  // create the buttons
  //
  //
  // the 'Backward' button
  pclButtonBack = new QToolButton(pclButtonGrp, tr("Backward"));
  pclButtonBack->setProperty("minimumSize", QSize(25, 25));
  pclButtonBack->setProperty("text", tr( "..." ) );
  pclButtonBack->setProperty("pixmap", QPixmap(back_pixmap));
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

  // the 'Path' combo box
  pclPathCombo = new QComboBox( true, pclButtonGrp, "Paths" );
  pclPathCombo->setDuplicatesEnabled(false);
  pclPathCombo->setAutoCompletion(true);
  pclPathCombo->setProperty( "minimumSize", QSize( 160, 25 ) );
  pclPathCombo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

  // the buttons' functionality
  pclButtonForward->setEnabled( false );
  pclButtonBack->setEnabled( false );
  connect(pclButtonBack,    SIGNAL(clicked()),                     pclBrowser,       SLOT(backward()));
  connect(pclBrowser,       SIGNAL(backwardAvailable(bool) ),      pclButtonBack,    SLOT(setEnabled(bool) ) );
  connect(pclButtonForward, SIGNAL(clicked()),                     pclBrowser,       SLOT(forward()));
  connect(pclBrowser,       SIGNAL(forwardAvailable(bool) ),       pclButtonForward, SLOT(setEnabled(bool) ) );
  connect(pclButtonHome,    SIGNAL(clicked()),                     pclBrowser,       SLOT(home()));
  connect(pclPathCombo,     SIGNAL(activated( const QString & ) ), this, SLOT( pathSelected( const QString & ) ) );

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
  pclPathCombo->insertItem( getRelativeURL(home) );

  // create instances of the popup menus
  pclPopup   = new QPopupMenu(0L);
  pclHistory = new QPopupMenu(0L); 
  pclBookm   = new QPopupMenu(0L);
  pclBookm->insertItem( tr( "Add Bookmark" ), this, SLOT( addBookmark() ) );
  pclBookm->insertItem( tr( "Update Bookmarks" ), this, SLOT( checkBookmarks() ) );
  pclBookm->insertSeparator();
  connect(pclBrowser, SIGNAL(showPopupMenu()), this, SLOT(showPopupMenu()));
  connect(pclPopup, SIGNAL( aboutToShow()), this, SLOT( popupMenuAboutToShow()));

  // insert the history & bookmarks to the menu
  //
  QStringList::Iterator it = history.begin();
  for ( ; it != history.end(); ++it )
	  mHistory[pclHistory->insertItem(*it)] = *it;
  connect( pclHistory, SIGNAL( activated( int ) ), this, SLOT( histChosen( int ) ) );

  QStringList::Iterator it2 = bookmarks.begin();
  for ( ; it2 != bookmarks.end(); ++it2 )
  	mBookmarks[pclBookm->insertItem(m_strCaption + *it2)] = *it2;
  connect( pclBookm, SIGNAL( activated( int ) ),   this, SLOT( bookmChosen( int ) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCHtmlView::~FCHtmlView()
{
    // no need to delete child widgets, Qt does it all for us
  QStringList history;
  QMap<int, QString>::Iterator it = mHistory.begin();
  for ( ; it != mHistory.end(); ++it )
  	history.append( *it );

  if (history.count() > 0)
  {
    QFile f( QDir::currentDirPath() + "/.history" );
    f.open( IO_WriteOnly );
    QDataStream s( &f );
    s << history;
    f.close();
  }

  QStringList bookmarks;
  QMap<int, QString>::Iterator it2 = mBookmarks.begin();
  for ( ; it2 != mBookmarks.end(); ++it2 )
  	bookmarks.append( *it2 );

  if (bookmarks.count() > 0)
  {
    QFile f2( QDir::currentDirPath() + "/.bookmarks" );
    f2.open( IO_WriteOnly );
    QDataStream s2( &f2 );
    s2 << bookmarks;
    f2.close();
  }
}

void FCHtmlView::setEnableHistory  (bool b)
{
  bHistory = b;
}

void FCHtmlView::setEnableBookmarks(bool b)
{
  bBookm = b;
}

void FCHtmlView::refreshPage()
{
  // sorry, but this isn't the best implementation 
  // for reload of a page ;-)
  if (bBackward)
  {
    pclBrowser->backward();
    pclBrowser->forward();
  }
  else if (bForward)
  {
    pclBrowser->forward();
    pclBrowser->backward();
  }
}

void FCHtmlView::showPopupMenu()
{
  pclPopup->exec(QCursor::pos());
}

void FCHtmlView::popupMenuAboutToShow()
{
  // create and show up your context menu
  //
  pclPopup->clear();
  if (pclBrowser->hasSelectedText())
  {
    pclPopup->insertItem("Copy", pclBrowser, SLOT(copy()));
  }
  else
  {
    int iBack = pclPopup->insertItem(QPixmap(back_pixmap), "Back", pclBrowser, SLOT(backward()));
    pclPopup->setItemEnabled(iBack, bBackward);
    int iForw = pclPopup->insertItem(QPixmap(forward_pixmap), "Forward", pclBrowser, SLOT(forward()));
    pclPopup->setItemEnabled(iForw, bForward);
    pclPopup->insertItem(QPixmap(home_pixmap), "Home", pclBrowser, SLOT(home()));
    pclPopup->insertSeparator();
    pclPopup->insertItem("Refresh", this, SLOT(refreshPage()));
    pclPopup->insertSeparator();
    if (bHistory == true)
      pclPopup->insertItem("History", pclHistory);
    if (bBookm == true)
      pclPopup->insertItem("Bookmarks", pclBookm);
    if (bHistory == true || bBookm == true)
      pclPopup->insertSeparator();
  }

  pclPopup->insertItem("Select all", pclBrowser, SLOT(selectAll()));
}

void FCHtmlView::setBackwardAvailable( bool b)
{
  bBackward = b;
  pclButtonBack->setEnabled(b);
}

void FCHtmlView::setForwardAvailable( bool b)
{
  bForward = b;
  pclButtonForward->setEnabled(b);
}

QString FCHtmlView::getHelpDirectory() const
{
  // find the current directory of the online manual
  QString sCurrDir;
  QDir cCurrDirectory;

  // go to FreeCAD's root directory if possible
  if (cCurrDirectory.cdUp())
  {
    cCurrDirectory.convertToAbs();
    sCurrDir = cCurrDirectory.path();
  }
  else
  {
    // one directory up
    sCurrDir = QDir::currentDirPath();
    sCurrDir.truncate(sCurrDir.findRev('/'));
  }

  sCurrDir.append("/src/Doc/Online/");

  return sCurrDir;
}

QString FCHtmlView::getRelativeURL (const QString& rcAbsPath) const
{
  QString cRelPath = rcAbsPath;
  // is this a pathname inside the online manual
  QString sCurrDir = cRelPath.lower();
  if (sCurrDir.startsWith(m_strDocDir.lower()) == true)
  {
    int iURL = cRelPath.length();
    int iCur = m_strDocDir.length();
    QString relativeURL = cRelPath.right(iURL - iCur);
    relativeURL.prepend("FCdoc://");
    cRelPath = relativeURL;
  }

  return cRelPath;
}

QString FCHtmlView::getAbsoluteURL (const QString& rcRelPath) const
{
  QString cAbsPath = rcRelPath;
  QString sCurrDir = cAbsPath.lower();
  
  // if your path is an internet address do nothing
  if (sCurrDir.startsWith("http://") == true)
    return cAbsPath;
  if (sCurrDir.startsWith("ftp://") == true)
    return cAbsPath;

  if (sCurrDir.startsWith("fcdoc://") == true)
  {
    int iURL = cAbsPath.length();
    int iCur = 8; // length of fcdoc://
    QString absoluteURL = cAbsPath.right(iURL - iCur);
    absoluteURL.prepend(m_strDocDir);
    cAbsPath = absoluteURL;
  }
  else if (QDir(cAbsPath).isRelative())
  {
    QString relativeURL = cAbsPath;
    relativeURL.prepend("FCdoc://");

    // search for this URL
	  int i;
	  for ( i = 0; i < pclPathCombo->count(); ++i ) 
    {
      QString test = pclPathCombo->text( i );
	    if ( pclPathCombo->text( i ) == relativeURL ) 
      {
  	    pclPathCombo->setCurrentItem( i );
	  	  break;
	    }
	  }

    cAbsPath.prepend(m_strDocDir);
  }

  return cAbsPath;
}

void FCHtmlView::textChanged()
{
  if ( pclBrowser->documentTitle().isNull() ) 
  {
  	setCaption( m_strCaption + pclBrowser->context() );
	  selectedURL = pclBrowser->context();
  }
  else 
  {
  	setCaption( m_strCaption + pclBrowser->documentTitle() ) ;
	  selectedURL = pclBrowser->documentTitle();
  }

  selectedURL = getRelativeURL(selectedURL);

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
      if (bHistory)
  	    mHistory[pclHistory->insertItem(selectedURL)] = selectedURL;
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
  // add new path if it is not inserted yet
  QString sCurrDir = getAbsoluteURL(_path);
  sCurrDir.truncate(sCurrDir.findRev('/'));

  // delete the input you've just written to the combobox
  // and insert it later again if the path is correct
/*  pclPathCombo->removeItem(pclPathCombo->currentItem());*/

  QStringList sl = pclBrowser->mimeSourceFactory()->filePath();
  if (sl.contains(sCurrDir) == false)
  {
    pclBrowser->mimeSourceFactory()->addFilePath(sCurrDir);
  }

  pclBrowser->setSource( getAbsoluteURL(_path) );
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

  if ( !exists )
  {
    if (bHistory)
    	mHistory[pclHistory->insertItem(_path)] = _path;
  }
}

QStringList FCHtmlView::readHistory()
{
  QStringList history;
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

  return history;
}

QStringList FCHtmlView::readBookmarks()
{
  QStringList bookmarks;
  if ( QFile::exists( QDir::currentDirPath() + "/.bookmarks" ) ) 
  {
  	QFile f( QDir::currentDirPath() + "/.bookmarks" );
	  f.open( IO_ReadOnly );
	  QDataStream s( &f );
	  s >> bookmarks;
	  f.close();
  }

  return bookmarks;
}

void FCHtmlView::histChosen( int i )
{
  if ( mHistory.contains( i ) )
  	pclBrowser->setSource( getAbsoluteURL( mHistory[ i ] ));
}

void FCHtmlView::bookmChosen( int i )
{
  if ( mBookmarks.contains( i ) )
  {
    QString sBookm = mBookmarks[ i ];
//    int iPos = sBookm.find(m_strDocDir);
//    if (iPos != -1)
//    {
//      sBookm = sBookm.right(sBookm.length() - iPos);
    	pclBrowser->setSource( sBookm );
//    }
//    else
//    {
//      pclBrowser->setSource( sBookm );
//      mBookmarks.remove(i);
//    }
  }
}

void FCHtmlView::addBookmark()
{
  if (bBookm)
  {
    bool bFound = false;
    QString txt = caption();
    QString path = pclPathCombo->currentText();
    path = getAbsoluteURL(path);

    QMap<int, QString>::Iterator it2 = mBookmarks.begin();
    for ( ; it2 != mBookmarks.end(); ++it2 )
    {
      if (*it2 == path)
      {
        bFound = true;
        break;
      }
    }

    if (bFound == false)
    {
      mBookmarks[pclBookm->insertItem(txt)] = path;
    }
  }
}

void FCHtmlView::checkBookmarks()
{
  int iButton = QMessageBox::information(this, "FreeCAD", "All unavailable bookmarks will be deleted\n"
                           "Continue ?", "Yes", "No", QString::null, 1);

  if (iButton == 0)
  {
    // TODO:
    QMessageBox::information(this, "FreeCAD", "Not yet implemented!");

    QMessageBox::information(this, "FreeCAD", "All bookmarks updated");
  }
}


#include "HtmlView_moc.cpp"

