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



#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qbuttongroup.h>
#	include <qurl.h>
#	include <ctype.h>
#endif

#include "HtmlView.h"
#include "Application.h"





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
  m_strDocDir = GetHelpDirectory();

  QString home = m_strDocDir;

  // this is the complete URL of the start page
  // WARNING: home_ must NOT contain an absolute pathname but a relative 
  //          pathname
  home.append(home_);

  // read the old history and bookmark entries
  ReadHistory();
  ReadBookmarks();

  // create the actual browser
  //
  pclBrowser = new FCTextBrowser(this, "FCHelpViewer");
  // set the path where the factory is looking for if you set a new source
  pclBrowser->mimeSourceFactory()->setFilePath( m_strDocDir ); 
  pclBrowser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  connect(pclBrowser, SIGNAL(textChanged()), this, SLOT(TextChanged()));

  // set the start page now
  if (!home.isEmpty())
  	pclBrowser->setSource(home);

  // the browser's functionality
  connect( pclBrowser, SIGNAL(backwardAvailable(bool)), this, SLOT(SetBackwardAvailable(bool)));
  connect( pclBrowser, SIGNAL(forwardAvailable (bool)), this, SLOT(SetForwardAvailable (bool)));

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
  connect(pclPathCombo,     SIGNAL(activated( const QString & ) ), this, SLOT( PathSelected( const QString & ) ) );

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
  pclPathCombo->insertItem( GetRelativeURL(home) );

  // create instances of the popup menus
  //
  pclPopup   = new QPopupMenu(0L);
  pclHistory = new QPopupMenu(0L); 
  pclBookm   = new QPopupMenu(0L);
  // insert the history & bookmarks to the menu
  SetMaxHistory(5);
  CreateHistoryPopup ();
  CreateBookmarkPopup();

  connect(pclBrowser, SIGNAL(showPopupMenu()), this, SLOT(ShowPopupMenu()));
  connect(pclPopup, SIGNAL( aboutToShow()), this, SLOT( PopupMenuAboutToShow()));
  connect( pclHistory, SIGNAL( activated( int ) ), this, SLOT( HistChosen( int ) ) );
  connect( pclBookm, SIGNAL( activated( int ) ),   this, SLOT( BookmChosen( int ) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCHtmlView::~FCHtmlView()
{
    // no need to delete child widgets, Qt does it all for us
  SaveHistory();
  SaveBookmarks();
}

void FCHtmlView::SetEnableHistory  (bool b)
{
  bHistory = b;
}

void FCHtmlView::SetEnableBookmarks(bool b)
{
  bBookm = b;
}

void FCHtmlView::RefreshPage()
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

void FCHtmlView::ShowPopupMenu()
{
  pclPopup->exec(QCursor::pos());
}

void FCHtmlView::PopupMenuAboutToShow()
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
    pclPopup->insertItem("Refresh", this, SLOT(RefreshPage()));
    pclPopup->insertSeparator();
    if (bHistory == true)
      pclPopup->insertItem("History", pclHistory);
    if (bBookm == true)
    {
      CreateBookmarkPopup();
      pclPopup->insertItem("Bookmarks", pclBookm);
    }
    if (bHistory == true || bBookm == true)
      pclPopup->insertSeparator();
  }

  pclPopup->insertItem("Select all", pclBrowser, SLOT(selectAll()));
}

void FCHtmlView::SetBackwardAvailable( bool b)
{
  bBackward = b;
  pclButtonBack->setEnabled(b);
}

void FCHtmlView::SetForwardAvailable( bool b)
{
  bForward = b;
  pclButtonForward->setEnabled(b);
}

QString FCHtmlView::GetHelpDirectory() const
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

QString FCHtmlView::GetRelativeURL (const QString& rcAbsPath) const
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

QString FCHtmlView::GetAbsoluteURL (const QString& rcRelPath) const
{
  QString cAbsPath = rcRelPath;
  QString sCurrDir = cAbsPath.lower();
  
  // if your path is an internet address do nothing
  QString protocol = QUrl(sCurrDir).protocol();
  if (protocol != "file" && protocol != "fcdoc")
  {
    return cAbsPath;
  }

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

void FCHtmlView::TextChanged()
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

  selectedURL = GetRelativeURL(selectedURL);

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
  	    mHistory[pclHistory->insertItem(selectedURL, mHistory.size())] = selectedURL;
  	} 
    else
	    pclPathCombo->setCurrentItem( i );

    selectedURL = QString::null;
  }
}

void FCHtmlView::OpenFile()
{
#ifndef QT_NO_FILEDIALOG
  QString fn = QFileDialog::getOpenFileName( QString::null, QString::null, this );
  if ( !fn.isEmpty() )
  	pclBrowser->setSource( fn );
#endif
}

void FCHtmlView::AddToPath (const QString& path)
{
  // add new path if it is not inserted yet
  QString sCurrDir = GetAbsoluteURL(path);
  sCurrDir.truncate(sCurrDir.findRev('/'));

  QStringList sl = pclBrowser->mimeSourceFactory()->filePath();
  if (sl.contains(sCurrDir) == false)
  {
    pclBrowser->mimeSourceFactory()->addFilePath(sCurrDir);
  }
}

void FCHtmlView::PathSelected( const QString &_path )
{
  // add new path if it is not inserted yet
  AddToPath(_path);

  pclBrowser->setSource( GetAbsoluteURL(_path) );
  bool exists = FALSE;
  for ( FCmap<int, QString>::iterator it = mHistory.begin(); it != mHistory.end(); ++it ) 
  {
  	if ( it->second == _path ) 
    {
	    exists = TRUE;
	    break;
  	}
  }

  if ( !exists )
  {
    if (bHistory)
    	mHistory[pclHistory->insertItem(_path, mHistory.size())] = _path;
  }
}

bool FCHtmlView::SetMaxHistory (long lCnt)
{
  FCParametrGrp::handle hParamGrp = GetParam();
  if (hParamGrp.IsNull())
    return false;
  FCParametrGrp::handle hHistGrp = hParamGrp->GetGroup("History");
  if (hHistGrp.IsNull())
    return false;

  hHistGrp->SetInt("Max History items", lCnt);

  return true;
}

void FCHtmlView::ReadHistory()
{
  FCParametrGrp::handle hParamGrp = GetParam();
  FCParametrGrp::handle hHistGrp = hParamGrp->GetGroup("History");

  int iCnt = hHistGrp->GetInt("History items");

  for (int i=0; i<iCnt; i++)
  {
    char szBuf[200];
    sprintf(szBuf, "History item %d", i);
    FCstring item = hHistGrp->GetASCII(szBuf);
    mHistory[i] = item.c_str();
  }
}

void FCHtmlView::ReadBookmarks()
{
  FCParametrGrp::handle hBookmGrp = GetParam()->GetGroup("Bookmarks");

  int iCnt = hBookmGrp->GetInt("Bookmark items");

  for (int i=0; i<iCnt; i++)
  {
    char szBuf[200];
    sprintf(szBuf, "Bookmark item %d", i);
    FCstring item = hBookmGrp->GetASCII(szBuf);
    mBookmarks[i] = item.c_str();
  }
}

void FCHtmlView::SaveHistory()
{
  FCParametrGrp::handle hHistGrp = GetParam()->GetGroup("History");

  int iMaxCnt = hHistGrp->GetInt("Max History items", 20);

  while ( int(mHistory.size()) > iMaxCnt )
	  mHistory.erase( mHistory.begin() );

  hHistGrp->SetInt("History items", mHistory.size());

  long i=0;
  for (FCmap<int, QString>::iterator it = mHistory.begin(); it != mHistory.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "History item %d", i);
    hHistGrp->SetASCII(szBuf, it->second.latin1());
  }
}

void FCHtmlView::SaveBookmarks()
{
  FCParametrGrp::handle hBookmGrp = GetParam()->GetGroup("Bookmarks");

  hBookmGrp->SetInt("Bookmark items", mBookmarks.size());

  long i=0;
  for (FCmap<int, QString>::iterator it = mBookmarks.begin(); it != mBookmarks.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "Bookmark item %d", i);
    hBookmGrp->SetASCII(szBuf, it->second.latin1());
  }
}

void FCHtmlView::HistChosen( int i )
{
  if ( mHistory.find( i ) != mHistory.end() )
  	pclBrowser->setSource( GetAbsoluteURL( mHistory[ i ] ));
}

void FCHtmlView::BookmChosen( int i )
{
  if ( mBookmarks.find( i ) != mBookmarks.end() )
  {
    QString sBookm = mBookmarks[ i ];
   	pclBrowser->setSource( sBookm );
  }
}

void FCHtmlView::AddBookmark()
{
  if (bBookm)
  {
    bool bFound = false;
    QString txt = caption();
    QString path = pclPathCombo->currentText();
    path = GetAbsoluteURL(path);

    for ( FCmap<int, QString>::iterator it = mBookmarks.begin(); it != mBookmarks.end(); ++it )
    {
      if (it->second == path)
      {
        bFound = true;
        break;
      }
    }

    if (bFound == false)
    {
      mBookmarks[pclBookm->insertItem(txt, mBookmarks.size())] = path;
    }
  }
}

void FCHtmlView::CreateBookmarkPopup()
{
  pclBookm->clear();
  FCmap<int, QString> tmp = mBookmarks;
  mBookmarks.clear();

  pclBookm->insertItem( tr( "Add Bookmark" ), this, SLOT( AddBookmark() ) );
  if (tmp.size() > 0)
    pclBookm->insertItem( tr( "Update Bookmarks" ), this, SLOT( CheckBookmarks() ) );
  pclBookm->insertSeparator();

  int i = 0;
  for (FCmap<int, QString>::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
    QString txt = m_strCaption + it->second;
    int iPos = pclBookm->insertItem(m_strCaption + it->second, i++);
    mBookmarks[iPos] = it->second;
  }
}

void FCHtmlView::CreateHistoryPopup()
{
  pclHistory->clear();
  FCmap<int, QString> tmp = mHistory;
  mHistory.clear();

  for (FCmap<int, QString>::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
	  mHistory[pclHistory->insertItem(it->second, mHistory.size())] = it->second;
    AddToPath(it->second);
  }
}

void FCHtmlView::CheckBookmarks()
{
  if (mBookmarks.size() == 0)
    return;

  int iButton = QMessageBox::information(this, "FreeCAD", "All unavailable bookmarks will be deleted\n"
                           "Continue ?", "Yes", "No", QString::null, 1);

  if (iButton != 0) // not Ok pressed
    return;

  FCmap<int, QString> mChecked;
  for (FCmap<int, QString>::iterator it = mBookmarks.begin(); it != mBookmarks.end(); ++it)
  {
    const QMimeSource * mime = pclBrowser->mimeSourceFactory()->data(it->second);
    if (mime == NULL)
    {
      char szBuf[200];
      sprintf(szBuf, "%s is unavailable.\n Do you want to delete it?", it->second.latin1());
      iButton = QMessageBox::information(this, "FreeCAD", szBuf, "Yes", "No", QString::null, 0);

      if (iButton != 0)
      {
        // bookmark is not OK but take it anyway
        mChecked[it->first] = it->second;
      }
    }
    else // bookmark is OK
      mChecked[it->first] = it->second;
  }

  mBookmarks.clear();
  mBookmarks = mChecked;
  CreateBookmarkPopup();

  QMessageBox::information(this, "FreeCAD", "All bookmarks are uptodate");
}


#include "HtmlView_moc.cpp"

