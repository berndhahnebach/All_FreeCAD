/***************************************************************************
                          HtmlView.cpp  -  description
                             -------------------
    begin                : 2002/10/19 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file HtmlView.cpp
 *  \brief The Html view module
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
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
#	include <qurl.h>
#	include <qbuttongroup.h>
#	include <qmessagebox.h>
#	include <ctype.h>
#endif

#include "HtmlView.h"
#include "Application.h"
#include "../Base/Interpreter.h"
#include "../Base/Exception.h"
#ifndef __linux
#include <direct.h>
#endif

#ifdef __linux
#  include<unistd.h> //for chdir
#  include<stdlib.h> //for system
#  ifdef _chdir
#    warning _chdir already defined, cross thumbs
#  endif
#  define _chdir chdir
#endif


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

/* XPM */
static const char *open_pixmap[] = {
"16 16 5 1",
"# c #000000",
"c c #808000",
". c None",
"b c #ffffff",
"a c #ffffff",
"..........###...",
".........#...#.#",
"..............##",
"..###........###",
".#aba#######....",
".#babababab#....",
".#ababababa#....",
".#baba##########",
".#aba#ccccccccc#",
".#ba#ccccccccc#.",
".#a#ccccccccc#..",
".##ccccccccc#...",
".###########....",
"................",
"................",
"................"};

//// FCTextBrowser //////////////////////////////////////////////////////

FCTextBrowser::FCTextBrowser(QWidget * parent, const char * name)
: QTextBrowser(parent, name)
{
  setHScrollBarMode(QScrollView::AlwaysOff);
  //setVScrollBarMode(QScrollView::AlwaysOff);
  mimeSourceFactory()->setExtensionType("HTML", "text/html;charset=iso8859-1");
  mimeSourceFactory()->setExtensionType("HTM", "text/html;charset=iso8859-1");
  mimeSourceFactory()->setExtensionType("FCParam", "text/xml;charset=UTF-8");
}

void FCTextBrowser::setSource (const QString & name)
{
  QString source = name;
  QString mark;
  int hash = name.find('#');
  if ( hash != -1) 
  {
    source  = name.left( hash );
    mark = name.mid( hash+1 );
  }

  QString txt;

  if (!source.isEmpty()) 
  {
    const QMimeSource* mime = mimeSourceFactory()->data(source, context());
    if (mime == NULL)
    {
      char szBuf[200];
      sprintf(szBuf, "Can't load '%s'.\nDo you want to start your favourite external browser instead?", source.latin1());
      if (QMessageBox::information(this, "FreeCAD", szBuf, "Yes", "No", QString::null, 0) == 0)
        emit startExtBrowser(name);
      return;
	  }
	  else 
	  {
  	  if (QTextDrag::decode(mime, txt) == false) 
  	  {
        char szBuf[200];
        sprintf(szBuf, "Can't decode '%s'", source.latin1());
        QMessageBox::information(this, "FreeCAD", szBuf);
        return;
	    }
    }
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

//// FCHtmlViewValidator //////////////////////////////////////////////////////

FCHtmlViewValidator::FCHtmlViewValidator ( QWidget * parent, const char * name )
: QValidator(parent, name)
{
}

FCHtmlViewValidator::~FCHtmlViewValidator ()
{
}

QValidator::State FCHtmlViewValidator::validate ( QString & txt, int & i) const
{
  if (txt.isEmpty())
    return Intermediate;
  else
    return Acceptable;
}

void FCHtmlViewValidator::fixup ( QString & txt) const
{
}

//// FCHtmlComboBox //////////////////////////////////////////////////////

FCHtmlComboBox::FCHtmlComboBox ( QWidget * parent, const char * name)
: QComboBox(parent, name)
{
  setValidator(new FCHtmlViewValidator(this));
  connect(lineEdit(), SIGNAL(returnPressed()), this, SLOT(slotKeyPressReturn));
}

FCHtmlComboBox::FCHtmlComboBox ( bool rw, QWidget * parent, const char * name)
: QComboBox(rw, parent, name)
{
  setValidator(new FCHtmlViewValidator(this));
  connect(lineEdit(), SIGNAL(returnPressed()), this, SLOT(slotKeyPressReturn()));
}

void FCHtmlComboBox::slotKeyPressReturn()
{
  emit returnPressed(currentText());
}

//// FCHtmlView //////////////////////////////////////////////////////

/* 
 *  Constructs a FCHtmlView which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
FCHtmlView::FCHtmlView( const QString& home_,  QWidget* parent,  const char* name, WFlags fl )
    : FCDockWindow( parent, name, fl ), pclPathCombo( 0 ), selectedURL(), 
      bHistory(false), bBookm(false), bBackward(false), bForward(false)

{
  //initialize
  init();

  m_strCaption = "FCHelpViewer - ";
  m_FCdoc = "FCdoc://";
  m_FCext = "FCext://";
  m_FCscript = "FCScript://";

  // find the home directory of the online manual
  m_strDocDir = GetDocDirectory();

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
  connect(pclBrowser, SIGNAL(startExtBrowser(QString)), this, SLOT(StartExtBrowser(QString)));

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

  // the 'Open' button
  pclButtonOpen = new QToolButton( pclButtonGrp, tr("Open") );
  pclButtonOpen->setProperty( "minimumSize", QSize( 25, 25 ) );
  pclButtonOpen->setProperty( "text", tr( "..." ) );
  pclButtonOpen->setProperty( "pixmap", QPixmap(open_pixmap) );
	pclButtonOpen->setAutoRaise(true);

  // the 'Path' combo box
  pclPathCombo = new FCHtmlComboBox( true, pclButtonGrp, "Paths" );
  pclPathCombo->setDuplicatesEnabled(false);
  pclPathCombo->setAutoCompletion(true);
  pclPathCombo->setProperty( "minimumSize", QSize( 25, 25 ) );
  pclPathCombo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

  // the buttons' functionality
  pclButtonForward->setEnabled( false );
  pclButtonBack->setEnabled( false );
  connect(pclButtonBack,    SIGNAL(clicked()),                     pclBrowser,       SLOT(backward()));
  connect(pclBrowser,       SIGNAL(backwardAvailable(bool) ),      pclButtonBack,    SLOT(setEnabled(bool) ) );
  connect(pclButtonForward, SIGNAL(clicked()),                     pclBrowser,       SLOT(forward()));
  connect(pclBrowser,       SIGNAL(forwardAvailable(bool) ),       pclButtonForward, SLOT(setEnabled(bool) ) );
  connect(pclButtonHome,    SIGNAL(clicked()),                     pclBrowser,       SLOT(home()));
  connect(pclButtonOpen,    SIGNAL(clicked()),                     this,             SLOT(OpenFile()));
  connect(pclPathCombo,     SIGNAL(activated( const QString & ) ), this, SLOT( PathSelected( const QString & ) ) );
  connect(pclPathCombo,     SIGNAL(returnPressed(QString)),        this, SLOT( StartScriptOrBrowser(QString)));

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
  pclButtonGrpLayout->addWidget( pclButtonOpen );
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
  if (bHistory)
    SaveHistory();
  if (bBookm)
    SaveBookmarks();
}

void FCHtmlView::init()
{
  try{
    // attach the help viewer to its preferences
    aStrGroupPath = "User parameter:BaseApp/Windows/Widget Preferences/";
    aStrGroups.push_back("EnableBookmarks");
    aStrGroups.push_back("EnableHistory");
    aStrGroups.push_back("NbOfHistoryItems");
    aStrGroups.push_back("NbOfBookmarks");
    for (std::vector<std::string>::iterator it = aStrGroups.begin(); it != aStrGroups.end(); ++it)
    {
      GetApplication().GetParameterGroupByPath((aStrGroupPath + *it).c_str())->Attach(this);
      GetApplication().GetParameterGroupByPath((aStrGroupPath + *it).c_str())->Notify();
    }
  }catch(/*const*/ FCException& rclE)
  {
    QMessageBox::warning(ApplicationWindow::Instance, "Wrong parameter", rclE.what());
  }
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

QString FCHtmlView::GetDocDirectory()
{
//  QString path = GetWindowParameter()->GetASCII("DocDir", "../Doc/Online").c_str();
  QString path = GetWindowParameter()->GetASCII("DocDir", "../src/Doc/Online").c_str();

  QDir dir (path);
  dir.convertToAbs();
  path = dir.path();
  path.append("/");

  if (QDir().exists(path) == false)
  {
    QMessageBox::warning(this, "Path not found","Couldn't find the path for the Online help.\n"
                         "Propably, you should run the python script 'MakeDoc.py' before.");
  }

  return path;
}

QString FCHtmlView::GetScriptDirectory()
{
  QString path = GetWindowParameter()->GetASCII("ScriptDir", "../src/Tools").c_str();

  QDir dir (path);
  dir.convertToAbs();
  path = dir.path();
  path.append("/");

  return path;
}

QString FCHtmlView::GetBrowserDirectory()
{
//  QString browser = GetWindowParameter()->GetASCII("External Browser", "").c_str();
  QString browser = GetApplication().GetParameterGroupByPath((aStrGroupPath + "LineEditBrowser").c_str())->GetASCII("LineEditBrowser", "").c_str();
  if (browser.isEmpty())
  {
    QMessageBox::information(this, "External browser", "Please search for an external browser.");
    browser = QFileDialog::getOpenFileName();

    if (browser.isEmpty())
      QMessageBox::warning(this, "External browser", "No external browser found.");
    else
//      GetWindowParameter()->SetASCII("External Browser", browser.latin1());
      GetApplication().GetParameterGroupByPath((aStrGroupPath + "LineEditBrowser").c_str())->SetASCII("LineEditBrowser", browser.latin1());
  }

  return browser;
}

QString FCHtmlView::GetRelativeURL (const QString& rcAbsPath) const
{
  QString cRelPath = rcAbsPath;
  QString sCurrDir = cRelPath.lower();

  // is this a pathname inside the online manual
  if (sCurrDir.startsWith(m_strDocDir.lower()) == true)
  {
    QString relativeURL = cRelPath.mid(m_strDocDir.length());
    relativeURL.prepend( m_FCdoc );
    cRelPath = relativeURL;
  }

  return cRelPath;
}

QString FCHtmlView::GetAbsoluteURL (const QString& rcRelPath) const
{
  QString cAbsPath = rcRelPath;
  QString sCurrDir = cAbsPath.lower();

  // generate an absolute URL from a relative one
  if (sCurrDir.startsWith(m_FCdoc.lower()) == true)
  {
    QString absoluteURL = cAbsPath.mid(m_FCdoc.length());
    absoluteURL.prepend(m_strDocDir);
    cAbsPath = absoluteURL;
  }

  return cAbsPath;
}

void FCHtmlView::TextChanged()
{
  // insert new item into the combo box
  //
  //
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
  QString sCurrDir = path;
  sCurrDir.truncate(sCurrDir.findRev('/'));

  QStringList sl = pclBrowser->mimeSourceFactory()->filePath();
  if (sl.contains(sCurrDir) == false)
  {
    pclBrowser->mimeSourceFactory()->addFilePath(sCurrDir);
  }
}

void FCHtmlView::StartScriptOrBrowser(QString path)
{
  QString lpath  = path.lower();

  // start a script
  if (lpath.startsWith(m_FCscript.lower()) == true)
  {
    StartScript(path, m_FCscript);
  }
  // start an external browser
  else if (lpath.startsWith(m_FCext.lower()) == true)
  {
    StartBrowser(path, m_FCext);
  }
}

void FCHtmlView::StartExtBrowser(QString url)
{
  StartBrowser(url, "");
}

void FCHtmlView::StartBrowser(QString path, QString protocol)
{
#ifdef __linux
  QString url = path.mid(protocol.length());
  if (system("mozilla "+url)!=0){
    char msgBuf[512];
    sprintf(msgBuf, "Hey, where is your browser? (Change it in %s:%d)", __FILE__,__LINE__);
    QMessageBox::critical(this, "Browser", msgBuf);
  }  
#else
  QString url = path.mid(protocol.length());

  QString browser = GetBrowserDirectory();
  if (browser.isEmpty())
    return;

  char szBuf[500];
  char szPath[5000];

  // split into absolute path and filename
  QString sPath = browser;
  sPath   = sPath.left(sPath.findRev("/") + 1);
  browser = browser.mid(browser.findRev("/") + 1);

  // create the command to execute
  sprintf(szBuf, "%s %s", browser.latin1(), url.latin1());

  // append the path of your favorite browser to global path
  //
  sprintf(szPath, "%s;%s", getenv("Path"), sPath.latin1());
	SetEnvironmentVariable ( "Path",szPath);
  sPath = QString(szPath);
	sprintf(szPath,"Path=%s",sPath.latin1());
	putenv (szPath);

#ifdef FC_OS_WIN32
  if (WinExec(szBuf, SW_SHOW) < 32) // windows
#else
  if (system(szBuf) != 0) // other OS (not windows)
#endif
  {
    sprintf(szBuf, "Sorry, cannot start '%s'", browser.latin1());
    QMessageBox::critical(this, "Browser", szBuf);
  }
#endif  
}

void FCHtmlView::StartScript(QString path, QString protocol)
{

  QString currPath = QDir::currentDirPath();
  _chdir(GetScriptDirectory().latin1());

  path = path.mid(protocol.length());

  QDir dir (path);
  dir.convertToAbs();
  path = dir.path();

  // split into absolute path and filename
  QString script = path;
  path   = path.left(path.findRev("/") + 1);
  script = script.mid(script.findRev("/") + 1);

  char szBuf[500];
  sprintf(szBuf, "python %s", script.latin1());

  _chdir(path.latin1());

  if (system(szBuf) != 0)
  {
    sprintf(szBuf, "Sorry, cannot run file '%s'.", script.latin1());
    QMessageBox::critical(this, "Script", szBuf);
  }
  else
  {
    sprintf(szBuf, "'%s' done successfully.", script.latin1());
    QMessageBox::information(this, "Script", szBuf);
  }

  // go to the former path
  _chdir(currPath.latin1());
}

void FCHtmlView::PathSelected( const QString & path )
{
  QString lpath  = path.lower();

  // if you want to start a script or a browser do nothing here
  // NOTE: to start a script or browser you must press Return/Enter
  if (lpath.startsWith(m_FCscript.lower()) || lpath.startsWith(m_FCext.lower()))
    return;

  // add new path if it is not inserted yet
  AddToPath(GetAbsoluteURL(path));

  // set new source
  pclBrowser->setSource( GetAbsoluteURL(path) );
  TextChanged();

  // insert to the history
  bool exists = FALSE;
  for ( std::map<int, QString>::iterator it = mHistory.begin(); it != mHistory.end(); ++it ) 
  {
  	if ( it->second == path ) 
    {
	    exists = TRUE;
	    break;
  	}
  }

  if ( !exists )
  {
    if (bHistory)
    	mHistory[pclHistory->insertItem(path, mHistory.size())] = path;
  }
}

bool FCHtmlView::SetMaxHistory (long lCnt)
{
  FCParameterGrp::handle hHistGrp = GetWindowParameter()->GetGroup("History");

  hHistGrp->SetInt("Max History items", lCnt);

  return true;
}

bool FCHtmlView::SetMaxBookmarks (long lCnt)
{
  FCParameterGrp::handle hBookmGrp = GetWindowParameter()->GetGroup("Bookmarks");

  hBookmGrp->SetInt("Max Bookmark items", lCnt);

  return true;
}

void FCHtmlView::ReadHistory()
{
  // get all stored history items
  FCParameterGrp::handle hHistGrp = GetWindowParameter()->GetGroup("History");
  std::vector<std::string> hist = hHistGrp->GetASCIIs("History");

  int i=0;
  for (std::vector<std::string>::iterator it = hist.begin(); it != hist.end(); ++it, i++)
  {
    mHistory[i] = it->c_str();
  }
}

void FCHtmlView::ReadBookmarks()
{
  // get all stored bookmark items
  FCParameterGrp::handle hBookmGrp = GetWindowParameter()->GetGroup("Bookmarks");
  std::vector<std::string> bookm = hBookmGrp->GetASCIIs("Bookmark");

  int i=0;
  for (std::vector<std::string>::iterator it = bookm.begin(); it != bookm.end(); ++it, i++)
  {
    mBookmarks[i] = it->c_str();
  }
}

void FCHtmlView::SaveHistory()
{
  // write the history items into file
  FCParameterGrp::handle hHistGrp = GetWindowParameter()->GetGroup("History");
  while ( int(mHistory.size()) > iMaxHist )
	  mHistory.erase( mHistory.begin() );

  long i=0;
  for (std::map<int, QString>::iterator it = mHistory.begin(); it != mHistory.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "History %d", i);
    hHistGrp->SetASCII(szBuf, it->second.latin1());
  }
}

void FCHtmlView::SaveBookmarks()
{
  // write the bookmark items into file
  FCParameterGrp::handle hBookmGrp = GetWindowParameter()->GetGroup("Bookmarks");
  while ( int(mBookmarks.size()) > iMaxBookm )
	  mBookmarks.erase( mBookmarks.begin() );

  long i=0;
  for (std::map<int, QString>::iterator it = mBookmarks.begin(); it != mBookmarks.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "Bookmark %d", i);
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

    for ( std::map<int, QString>::iterator it = mBookmarks.begin(); it != mBookmarks.end(); ++it )
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
  std::map<int, QString> tmp = mBookmarks;
  mBookmarks.clear();

  pclBookm->insertItem( tr( "Add Bookmark" ), this, SLOT( AddBookmark() ) );
  if (tmp.size() > 0)
    pclBookm->insertItem( tr( "Update Bookmarks" ), this, SLOT( CheckBookmarks() ) );
  pclBookm->insertSeparator();

  int i = 0;
  for (std::map<int, QString>::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
    QString txt = m_strCaption + it->second;
    int iPos = pclBookm->insertItem(m_strCaption + it->second, i++);
    mBookmarks[iPos] = it->second;
  }
}

void FCHtmlView::CreateHistoryPopup()
{
  pclHistory->clear();
  std::map<int, QString> tmp = mHistory;
  mHistory.clear();

  for (std::map<int, QString>::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
	  mHistory[pclHistory->insertItem(it->second, mHistory.size())] = it->second;
    AddToPath(GetAbsoluteURL(it->second));
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

  std::map<int, QString> mChecked;
  for (std::map<int, QString>::iterator it = mBookmarks.begin(); it != mBookmarks.end(); ++it)
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

void FCHtmlView::OnChange(FCSubject &rCaller)
{
  FCParameterGrp& rclGrp = ((FCParameterGrp&)rCaller);
  std::string name = rclGrp.GetGroupName();
  std::vector<std::string>::iterator pos;
  if (( pos = std::find(aStrGroups.begin(), aStrGroups.end(), name)) != aStrGroups.end())
  {
    int i = pos - aStrGroups.begin();
    switch (i)
    {
    case 0:
      bBookm = rclGrp.GetBool(name.c_str(), false);
      break;
    case 1:
      bHistory = rclGrp.GetBool(name.c_str(), false);
      break;
    case 2:
      iMaxHist = rclGrp.GetInt(name.c_str(), 20);
      if (!bHistory) iMaxHist = 0;
      break;
    case 3:
      iMaxBookm = rclGrp.GetInt(name.c_str(), 20);
      if (!bBookm) iMaxBookm = 0;
      break;
    };
  }
}


#include "moc_HtmlView.cpp"

