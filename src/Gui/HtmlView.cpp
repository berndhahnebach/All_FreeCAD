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



#include "PreCompiled.h"

#ifndef _PreComp_
#	include <qaction.h>
#	include <qbuttongroup.h>
#	include <qcombobox.h>
#	include <qcursor.h>
#	include <qmessagebox.h>
#	include <qtextbrowser.h>
#	include <qprocess.h>
#	include <qthread.h>
#	include <qurl.h>
#	include <qvalidator.h>
#	include <qwhatsthis.h>
#	include <ctype.h>
#endif

#include "HtmlView.h"
#include "HtmlViewP.h"
#include "Process.h"
#include "Application.h"
#include "PrefWidgets.h"
#include "Tools.h"
#include "../Base/Interpreter.h"
#include "../Base/Exception.h"
#include "../Base/Documentation.h"
#ifndef FC_OS_LINUX
#include <direct.h>
#endif

#ifdef FC_OS_LINUX
#  include<unistd.h> //for chdir
#  include<stdlib.h> //for system
#  ifdef _chdir
#    warning _chdir already defined, cross thumbs
#  endif
#  define _chdir chdir
#endif


#include <qvaluestack.h>
#include <qstylesheet.h>

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

// looks like MS cursor for hyperlinks
#define cb_width  32
#define cb_height 32
static unsigned char cb_bits[] = {		// cursor bitmap
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x48,0x00,0x00,0x00,0x48,
 0x00,0x00,0x00,0x48,0x00,0x00,0x00,0x48,0x00,0x00,0x00,0xc8,0x01,0x00,0x00,
 0x48,0x0e,0x00,0x00,0x48,0x32,0x00,0x00,0x48,0x52,0x00,0x80,0x4b,0x92,0x00,
 0x80,0x0c,0x90,0x00,0x80,0x08,0x80,0x00,0x00,0x09,0x80,0x00,0x00,0x0a,0x80,
 0x00,0x00,0x02,0x80,0x00,0x00,0x04,0x80,0x00,0x00,0x04,0x40,0x00,0x00,0x08,
 0x40,0x00,0x00,0x08,0x40,0x00,0x00,0x10,0x20,0x00,0x00,0x10,0x20,0x00,0x00,
 0xf0,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

#define cm_width  32
#define cm_height 32
static unsigned char cm_bits[] = {		// cursor bitmap mask
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x78,
 0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0xf8,0x01,0x00,0x00,
 0xf8,0x0f,0x00,0x00,0xf8,0x3f,0x00,0x00,0xf8,0x7f,0x00,0x80,0xfb,0xff,0x00,
 0x80,0xff,0xff,0x00,0x80,0xff,0xff,0x00,0x00,0xff,0xff,0x00,0x00,0xfe,0xff,
 0x00,0x00,0xfe,0xff,0x00,0x00,0xfc,0xff,0x00,0x00,0xfc,0x7f,0x00,0x00,0xf8,
 0x7f,0x00,0x00,0xf8,0x7f,0x00,0x00,0xf0,0x3f,0x00,0x00,0xf0,0x3f,0x00,0x00,
 0xf0,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

// --------------------------------------------------------------------------

class FCTextBrowserPrivate
{
  public:
    enum TMode {Backward, Forward, None};

    FCTextBrowserPrivate();

    QValueStack<QString> fdStack;
    QValueStack<QString> bdStack;
    QCursor * cursor;
    bool highlighted;

    bool getType (const QString& url, TDocType type);
    TMode tMode;
    int minWidth;
};

FCTextBrowserPrivate::FCTextBrowserPrivate()
{
  tMode = FCTextBrowserPrivate::None;
  minWidth = 1000;
}

bool FCTextBrowserPrivate::getType(const QString& url, TDocType type)
{
  if (url.find(".html", 0, false) != -1)
  {
    type = Html;
    return true;
  }
  else if (url.find(".tex", 0, false) != -1)
  {
    type = Tech; // LaTeX
    return true;
  }
  else if(url.find(".script", 0, false) != -1)
  {
    type = Script;
    return true;
  }

  return false;
}

// --------------------------------------------------------------------------

class FCBrowserFactoryData
{
  private:
    FCBrowserFactoryData();
    ~FCBrowserFactoryData();
    static FCBrowserFactoryData *_pcSingleton;

  public:
	  static void Destruct(void);
	  static FCBrowserFactoryData &Instance();
    QStringList mPaths;
    QStringList mRoots;
};

FCBrowserFactoryData * FCBrowserFactoryData::_pcSingleton = 0;

void FCBrowserFactoryData::Destruct(void)
{
  assert(_pcSingleton);
	delete _pcSingleton;
}

FCBrowserFactoryData & FCBrowserFactoryData::Instance()
{
	if(!_pcSingleton)
	{
		_pcSingleton = new FCBrowserFactoryData;
	}

  return *_pcSingleton;
}

FCBrowserFactoryData::FCBrowserFactoryData()
{
}

FCBrowserFactoryData::~FCBrowserFactoryData()
{
}

// --------------------------------------------------------------------------

class FCDocumentationSource : public QStoredDrag
{
  public:
    FCDocumentationSource( const char * mimeType, QString path, TDocType type )
      : QStoredDrag(mimeType, 0L, 0), mPath(path), mType(type)
    {
      QStringList paths = FCBrowserFactoryData::Instance().mPaths;
      QStringList roots = FCBrowserFactoryData::Instance().mRoots;
      QString s;
      unsigned int i = 0;
      for (QStringList::Iterator it = paths.begin(); it!=paths.end(); ++it, ++i)
      {
        s = *it; s = s.left(s.length()-1);
        if (mPath.startsWith(s))
        {
          mPath = mPath.right(mPath.length() - s.length() - 1);
          mRoot = *roots.at(i);
          break;
        }
      }
    }

    QByteArray encodedData (const char* data) const
    {
      QString fn = QString("%1%2").arg(mRoot).arg(mPath);
      int pos = fn.findRev('.'); fn = fn.left(pos);

      std::string text = GetDocumentationManager().Retrive(fn.latin1(), mType );
      QCString test = text.c_str();

      if (test.isEmpty())
      {
        test = QString(
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"
        "<html>"
        "<head>"
        "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">"
        "<title>FreeCAD Main Index</title>"
        "</head>"
        "<body bgcolor=\"#ffffff\">"
        "<table cellpadding=2 cellspacing=1 border=0  width=100% bgcolor=#E5E5E5 >"
        "<tr>"
        "<th bgcolor=#FFCC66 width=33%%>"
        "<h1>:-(</h1>"
        "<h2>Sorry, but cannot load the file because the doc manager failed to convert it into HTML.</h2>"
        "<h1>:-(</h1>"
        "</th>"
        "</tr>"
        "</table>"
        "</body></html>");
      }

      return test;
    }

  private:
    TDocType mType;
    QString  mPath;
    QString  mRoot;
};

// --------------------------------------------------------------------------

class FCHtmlViewPrivate
{
  public:
    FCHtmlViewPrivate();

    std::string aStrGroupPath;
    QString        m_FCdoc, m_FCext, m_FCscript;
    std::map<int, QString> mHistory, mBookmarks;
    bool bBackward, bForward;
    bool bHistory, bBookm;
    int  iMaxHist, iMaxBookm;
    QString        selectedURL;
    QString        m_strDocDir;
    QString        m_strCaption;
    FCProcess      m_Process;
};

FCHtmlViewPrivate::FCHtmlViewPrivate()
:	selectedURL(), 
  bHistory(false), 
	bBookm(false), 
	bBackward(false), 
	bForward(false)
{
}

// --------------------------------------------------------------------------

FCBrowserSourceFactory::FCBrowserSourceFactory()
: QMimeSourceFactory()
{
  FCBrowserFactoryData::Instance();
}

FCBrowserSourceFactory::~FCBrowserSourceFactory()
{
  FCBrowserFactoryData::Destruct();
}

const QMimeSource* FCBrowserSourceFactory::data(const QString& abs_name) const
{
  if (canConvertToHTML(abs_name))
  {
    TDocType type=Html;
    FCTextBrowserPrivate d;
    d.getType(abs_name, type);

    QString path = abs_name;
    int pos = path.findRev('/'); if (pos == -1) pos = path.findRev('\\'); 

#ifdef FC_OS_WIN32
    path = path.left(pos);
    path += '\\';
#else
    path = path.left(pos+1);
#endif

    if (FCBrowserFactoryData::Instance().mPaths.find(path) == FCBrowserFactoryData::Instance().mPaths.end())
    {
      FCBrowserFactoryData::Instance().mPaths.append(path);
      QString root = QString("FCDoc:/%1/").arg(path);
      FCBrowserFactoryData::Instance().mRoots.append(root);
      GetDocumentationManager().AddProvider(new FCDocProviderDirectory(root.latin1(),path.latin1()));
    }
   
    return new FCDocumentationSource("text/html;charset=iso8859-1", abs_name, type);
  }
  else
    return QMimeSourceFactory::data(abs_name);
}

QString FCBrowserSourceFactory::makeAbsolute(const QString& abs_or_rel_name, const QString& context) const
{
  return QMimeSourceFactory::makeAbsolute(abs_or_rel_name, context);
}

void FCBrowserSourceFactory::setText( const QString& abs_name, const QString& text )
{
  QMimeSourceFactory::setText(abs_name, text);
}

void FCBrowserSourceFactory::setImage( const QString& abs_name, const QImage& im )
{
  QMimeSourceFactory::setImage(abs_name, im);
}

void FCBrowserSourceFactory::setPixmap( const QString& abs_name, const QPixmap& pm )
{
  QMimeSourceFactory::setPixmap(abs_name, pm);
}

void FCBrowserSourceFactory::setData( const QString& abs_name, QMimeSource* data )
{
  QMimeSourceFactory::setData(abs_name, data);
}

void FCBrowserSourceFactory::setFilePath( const QStringList& s)
{
  QMimeSourceFactory::setFilePath(s);
}

QStringList FCBrowserSourceFactory::filePath() const
{
  return QMimeSourceFactory::filePath();
}

void FCBrowserSourceFactory::setExtensionType( const QString& ext, const char* mimetype )
{
  QMimeSourceFactory::setExtensionType(ext, mimetype);
}

bool FCBrowserSourceFactory::canConvertToHTML (const QString& url)
{
  TDocType type=Html;
  FCTextBrowserPrivate d;
  return d.getType(url, type);
}

// --------------------------------------------------------------------------

FCTextBrowser::FCTextBrowser(QWidget * parent, const char * name)
: QTextBrowser(parent, name)
{
  d = new FCTextBrowserPrivate;

//  setMimeSourceFactory(new FCBrowserSourceFactory);

  setHScrollBarMode(QScrollView::AlwaysOff);
  setVScrollBarMode(QScrollView::AlwaysOff);
  mimeSourceFactory()->setExtensionType("HTML", "text/html;charset=iso8859-1");
  mimeSourceFactory()->setExtensionType("HTM", "text/html;charset=iso8859-1");
  mimeSourceFactory()->setExtensionType("FCParam", "text/xml;charset=UTF-8");

  QBitmap cb( cb_width, cb_height, cb_bits, TRUE );
  QBitmap cm( cm_width, cm_height, cm_bits, TRUE );
  d->highlighted = false;
  d->cursor = new QCursor ( cb, cm, 1, 1 );			// create bitmap cursor

  connect(this, SIGNAL(highlighted(const QString&)), this, SLOT(onHighlighted(const QString&)));
  viewport()->setAcceptDrops( TRUE );
}

FCTextBrowser::~FCTextBrowser()
{
  delete d;
}

void FCTextBrowser::onHighlighted(const QString& s)
{
  d->highlighted = !s.isEmpty();
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

  QString url = mimeSourceFactory()->makeAbsolute( source, context() );
  QString txt;

  if (!source.isEmpty()) 
  {
    const QMimeSource* mime = mimeSourceFactory()->data(source, context());
    if (mime == NULL)
    {
			int type = FCTools::getURLType(source);
			if ( type == 1 )
			{
				QMessageBox::information(this, tr("File not found"), tr("File %1 does not exist.").arg(source));
				setText(tr("File not found"));
				return;
			}
			else if ( type == 2 )
			{
				QString msg = tr("Can't load '%1'.\nDo you want to start your favourite external browser instead?").arg(source);
				if (QMessageBox::information(this, "FreeCAD", msg, tr("Yes"), tr("No"), QString::null, 0) == 0)
					emit startExtBrowser(name);
				return;
			}
	  }
	  else 
	  {
  	  if (QTextDrag::decode(mime, txt) == false) 
  	  {
				QString msg = tr("Can't decode '%1'").arg(source);
        QMessageBox::information(this, "FreeCAD", msg);
        return;
	    }
    }
  }

  if ( !mark.isEmpty() ) 
  {
	  url += "#";
	  url += mark;
  }

  if (d->bdStack.isEmpty() || d->bdStack.top() != url)
    d->bdStack.push(url);

  int bdStackCount = (int)d->bdStack.count();
  if ( d->bdStack.top() == url )
  	bdStackCount--;

  if (d->tMode == FCTextBrowserPrivate::None)
    d->fdStack.clear();
  int fdStackCount = (int)d->fdStack.count();
  if ( d->fdStack.top() == url )
  	fdStackCount--;

  QTextBrowser::setSource(name);

  emit backwardAvailable( bdStackCount > 0 );
  emit forwardAvailable( fdStackCount > 0 );
}

void FCTextBrowser::setText (const QString & contents, const QString & context)
{
  QTextBrowser::setText(contents, context);
}

void FCTextBrowser::backward()
{
  if ( d->bdStack.count() <= 1)
  	return;

  d->fdStack.push( d->bdStack.pop() );

  d->tMode = FCTextBrowserPrivate::Backward;
  setSource( d->bdStack.pop() );
  d->tMode = FCTextBrowserPrivate::None;
  emit forwardAvailable( true );
}

void FCTextBrowser::forward()
{
  if ( d->fdStack.isEmpty() )
  	return;
  
  d->tMode = FCTextBrowserPrivate::Forward;
  setSource( d->fdStack.pop() );
  d->tMode = FCTextBrowserPrivate::None;
  emit forwardAvailable( !d->fdStack.isEmpty() );
}

void FCTextBrowser::setMinWidthToReach (int width)
{
  d->minWidth = width;
}

void FCTextBrowser::viewportResizeEvent (QResizeEvent* e)
{
  if (e->size().width() > d->minWidth)
    emit minWidthReached(true);
  else
    minWidthReached(false);

  QTextBrowser::viewportResizeEvent(e);
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

void FCTextBrowser::viewportMouseMoveEvent  (QMouseEvent * e)
{
  QTextBrowser::viewportMouseMoveEvent(e);
  // avoid using the ugly Qt cursor ;-)
  if (d->highlighted)
    viewport()->setCursor( *d->cursor );
}

void FCTextBrowser::contentsDropEvent(QDropEvent  * e)
{
  if (QUriDrag::canDecode(e))
  {
    QStringList fn;
    QUriDrag::decodeLocalFiles(e, fn);
    QString file = fn.first();
    QFileInfo info(file);
    if (info.exists() && info.isFile())
      setSource(file);
  }
  else if (QTextDrag::canDecode(e))
  {
    QString text;
    QTextDrag::decode(e, text);
    if (QStyleSheet::mightBeRichText(text))
      setText(text);
  }
}

void FCTextBrowser::viewportDropEvent( QDropEvent* e )
{
  if (QUriDrag::canDecode(e) || QTextDrag::canDecode(e))
    QTextBrowser::viewportDropEvent(e);
}

void FCTextBrowser::viewportDragEnterEvent  (QDragEnterEvent * e)
{
  bool can = QUriDrag::canDecode(e) || QTextDrag::canDecode(e);
  e->accept(can);
//  if (can)
//    QTextBrowser::viewportDragEnterEvent(e);
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
    : FCDockWindow( parent, name, fl ), pclPathCombo( 0 )
{
  d = new FCHtmlViewPrivate;
  d->m_Process.Attach(this);

  //initialize
  init();

  d->m_strCaption = tr("FreeCAD Help - ");
  d->m_FCdoc = QString::null/*"FCdoc://"*/;
  d->m_FCext = "FCext://";
  d->m_FCscript = QString::null/*"FCScript://"*/;

  // find the home directory of the online manual
  d->m_strDocDir = GetDocDirectory();

  QString home = d->m_strDocDir;

  // this is the complete URL of the start page
  //
  // check if home_ is absolute or relative
  QDir dir (home_);
  if (dir.isRelative())
    home.append(home_);
  else
    home = home_;

  // read the old history and bookmark entries
  ReadHistory();
  ReadBookmarks();

  // create the actual browser
  //
  pclBrowser = new FCTextBrowser(this, "FCHelpViewer");
  pclBrowser->setMinWidthToReach(240);
  // set the path where the factory is looking for if you set a new source
  pclBrowser->mimeSourceFactory()->setFilePath( d->m_strDocDir ); 
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
  pclPathCombo->hide();

  // the buttons' functionality
  pclButtonForward->setEnabled( false );
  pclButtonBack->setEnabled( false );
  connect(pclButtonBack,    SIGNAL(clicked()),                     pclBrowser,       SLOT(backward()));
  connect(pclBrowser,       SIGNAL(backwardAvailable(bool) ),      pclButtonBack,    SLOT(setEnabled(bool) ) );
  connect(pclBrowser,       SIGNAL(minWidthReached  (bool)),       this, SLOT(onMinWidthReached(bool)));
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
  d->m_Process.Detach(this);
  if (d->bHistory)
    SaveHistory();
  if (d->bBookm)
    SaveBookmarks();
  GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->Detach(this);

  delete d;
}

void FCHtmlView::init()
{
  try{
    // attach the help viewer to its preferences
    d->aStrGroupPath = "User parameter:BaseApp/Windows/HelpViewer";
    GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->Attach(this);
    GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->Notify("EnableBookmarks");
    GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->Notify("EnableHistory");
    GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->Notify("NbOfHistoryItems");
    GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->Notify("NbOfBookmarks");
  }catch(const FCException& rclE)
  {
    QMessageBox::warning(ApplicationWindow::Instance, "Wrong parameter", rclE.what());
  }
}

void FCHtmlView::SetEnableHistory  (bool b)
{
  d->bHistory = b;
}

void FCHtmlView::SetEnableBookmarks(bool b)
{
  d->bBookm = b;
}

void FCHtmlView::RefreshPage()
{
  // sorry, but this isn't the best implementation 
  // for reload of a page ;-)
  if (d->bBackward)
  {
    pclBrowser->backward();
    pclBrowser->forward();
  }
  else if (d->bForward)
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
    pclPopup->insertItem(tr("Copy"), pclBrowser, SLOT(copy()));
  }
  else
  {
    int iBack = pclPopup->insertItem(QPixmap(back_pixmap), tr("Back"), pclBrowser, SLOT(backward()));
    pclPopup->setItemEnabled(iBack, d->bBackward);
    int iForw = pclPopup->insertItem(QPixmap(forward_pixmap), tr("Forward"), pclBrowser, SLOT(forward()));
    pclPopup->setItemEnabled(iForw, d->bForward);
    pclPopup->insertItem(QPixmap(home_pixmap), tr("Home"), pclBrowser, SLOT(home()));
    pclPopup->insertSeparator();
    pclPopup->insertItem(tr("Refresh"), this, SLOT(RefreshPage()));
    pclPopup->insertSeparator();
    if (d->bHistory == true)
      pclPopup->insertItem(tr("History"), pclHistory);
    if (d->bBookm == true)
    {
      CreateBookmarkPopup();
      pclPopup->insertItem(tr("Bookmarks"), pclBookm);
    }
    if (d->bHistory == true || d->bBookm == true)
      pclPopup->insertSeparator();
  }

  pclPopup->insertItem(tr("Select all"), pclBrowser, SLOT(selectAll()));
}

void FCHtmlView::SetBackwardAvailable( bool b)
{
  d->bBackward = b;
  pclButtonBack->setEnabled(b);
}

void FCHtmlView::SetForwardAvailable( bool b)
{
  d->bForward = b;
  pclButtonForward->setEnabled(b);
}

QString FCHtmlView::GetDocDirectory()
{
  QString path = GetWindowParameter()->GetASCII("OnlineDocDir", "../Doc/free-cad.sourceforge.net").c_str();

  puts(path.latin1());
  QDir dir (path);
  dir.convertToAbs();
  path = dir.path();
  path.append("/");

  if (QDir().exists(path) == false)
  {
    int ans = QMessageBox::warning(this, tr("Path not found"),	tr("Couldn't find the path for the Online help."));
    /*
         "Propably, you should run the python script 'MakeDoc.py' before.\n"
				 "Do you want to start this script now?"), tr("Yes"), tr("No"),QString::null, 0);
		if (ans == 0)
		{
			QDir d(QDir::currentDirPath());
			d.cdUp(); d.cd("src/Tools");
			QString file = d.absPath() + "/MakeDoc.py";
			try{
				GetInterpreter().LaunchFile(file.latin1());
			}catch(...){
			}
		}*/
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
  QString browser = GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->GetASCII("LineEditBrowser", "").c_str();
  if (browser.isEmpty())
  {
    QMessageBox::information(this, tr("External browser"), tr("Please search for an external browser."));
    browser = QFileDialog::getOpenFileName();

    if (browser.isEmpty())
      QMessageBox::warning(this, tr("External browser"), tr("No external browser found."));
    else
//      GetWindowParameter()->SetASCII("External Browser", browser.latin1());
      GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->SetASCII("LineEditBrowser", browser.latin1());
  }

  return browser;
}

QString FCHtmlView::GetRelativeURL (const QString& rcAbsPath) const
{
  QString cRelPath = rcAbsPath;
  QString sCurrDir = cRelPath.lower();

  // is this a pathname inside the online manual
  if (sCurrDir.startsWith(d->m_strDocDir.lower()) == true)
  {
    QString relativeURL = cRelPath.mid(d->m_strDocDir.length());
    relativeURL.prepend( d->m_FCdoc );
    cRelPath = relativeURL;
  }

  return cRelPath;
}

QString FCHtmlView::GetAbsoluteURL (const QString& rcRelPath) const
{
  QString cAbsPath = rcRelPath;
  QString sCurrDir = cAbsPath.lower();

  // generate an absolute URL from a relative one
  if (sCurrDir.startsWith(d->m_FCdoc.lower()) == true)
  {
    QString absoluteURL = cAbsPath.mid(d->m_FCdoc.length());
    absoluteURL.prepend(d->m_strDocDir);
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
  	setCaption( d->m_strCaption + pclBrowser->context() );
	  d->selectedURL = pclBrowser->context();
  }
  else 
  {
  	setCaption( d->m_strCaption + pclBrowser->documentTitle() ) ;
	  d->selectedURL = pclBrowser->documentTitle();
  }

  d->selectedURL = GetRelativeURL(d->selectedURL);

  if ( !d->selectedURL.isEmpty() && pclPathCombo ) 
  {
  	bool exists = FALSE;
	  int i;
	  for ( i = 0; i < pclPathCombo->count(); ++i ) 
    {
	    if ( pclPathCombo->text( i ) == d->selectedURL ) 
      {
  	  	exists = TRUE;
	  	  break;
	    }
	  }
  	if ( !exists ) 
    {
	    pclPathCombo->insertItem( d->selectedURL, 0 );
	    pclPathCombo->setCurrentItem( 0 );
      if (d->bHistory)
  	    d->mHistory[pclHistory->insertItem(d->selectedURL, d->mHistory.size())] = d->selectedURL;
  	} 
    else
	    pclPathCombo->setCurrentItem( i );

    d->selectedURL = QString::null;
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
  if (lpath.startsWith(d->m_FCscript.lower()) == true)
  {
    StartScript(path, d->m_FCscript);
  }
  // start an external browser
  else if (lpath.startsWith(d->m_FCext.lower()) == true)
  {
    StartBrowser(path, d->m_FCext);
  }
}

void FCHtmlView::StartExtBrowser(QString url)
{
  StartBrowser(url, "");
}

void FCHtmlView::StartBrowser(QString path, QString protocol)
{
  QString url = path.mid(protocol.length());

  QString browser = GetBrowserDirectory();
  if (browser.isEmpty())
    return;
#if 0
  // split into absolute path and filename
  QString sPath = browser;
  sPath   = sPath.left(sPath.findRev("/") + 1);
  browser = browser.mid(browser.findRev("/") + 1);

  // append the path of your favorite browser to global path
  d->m_Process.appendToPath(sPath.latin1());
#endif
  // create the command to execute
  d->m_Process.setExecutable(browser.latin1());
  d->m_Process << url.latin1();

  if (!d->m_Process.start())
    // delete the invalid entry
    GetApplication().GetParameterGroupByPath(d->aStrGroupPath.c_str())->SetASCII("LineEditBrowser", "" );
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


  _chdir(path.latin1());

  FCProcess proc("python"); proc << script.latin1();
  if (!proc.start())
  {
		QString msg = tr("Sorry, cannot run file '%1'.").arg(script);
    QMessageBox::critical(this, "Script", msg);
  }
  else
  {
		QString msg = tr("'%1' done successfully.").arg(script);
    QMessageBox::information(this, "Script", msg);
  }

  // go to the former path
  _chdir(currPath.latin1());
}

void FCHtmlView::PathSelected( const QString & path )
{
  QString lpath  = path.lower();

  // if you want to start a script or a browser do nothing here
  // NOTE: to start a script or browser you must press Return/Enter
  if (lpath.startsWith(d->m_FCscript.lower()) || lpath.startsWith(d->m_FCext.lower()))
    return;

  // add new path if it is not inserted yet
  AddToPath(GetAbsoluteURL(path));

  // set new source
  pclBrowser->setSource( GetAbsoluteURL(path) );
  TextChanged();

  // insert to the history
  bool exists = FALSE;
  for ( std::map<int, QString>::iterator it = d->mHistory.begin(); it != d->mHistory.end(); ++it ) 
  {
  	if ( it->second == path ) 
    {
	    exists = TRUE;
	    break;
  	}
  }

  if ( !exists )
  {
    if (d->bHistory)
    	d->mHistory[pclHistory->insertItem(path, d->mHistory.size())] = path;
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
    d->mHistory[i] = it->c_str();
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
    d->mBookmarks[i] = it->c_str();
  }
}

void FCHtmlView::SaveHistory()
{
  // write the history items into file
  FCParameterGrp::handle hHistGrp = GetWindowParameter()->GetGroup("History");
  while ( int(d->mHistory.size()) > d->iMaxHist )
	  d->mHistory.erase( d->mHistory.begin() );

  long i=0;
  for (std::map<int, QString>::iterator it = d->mHistory.begin(); it != d->mHistory.end(); ++it, i++)
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
  while ( int(d->mBookmarks.size()) > d->iMaxBookm )
	  d->mBookmarks.erase( d->mBookmarks.begin() );

  long i=0;
  for (std::map<int, QString>::iterator it = d->mBookmarks.begin(); it != d->mBookmarks.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "Bookmark %d", i);
    hBookmGrp->SetASCII(szBuf, it->second.latin1());
  }
}

void FCHtmlView::HistChosen( int i )
{
  if ( d->mHistory.find( i ) != d->mHistory.end() )
  	pclBrowser->setSource( GetAbsoluteURL( d->mHistory[ i ] ));
}

void FCHtmlView::BookmChosen( int i )
{
  if ( d->mBookmarks.find( i ) != d->mBookmarks.end() )
  {
    QString sBookm = d->mBookmarks[ i ];
   	pclBrowser->setSource( sBookm );
  }
}

void FCHtmlView::AddBookmark()
{
  if (d->bBookm)
  {
    bool bFound = false;
    QString txt = caption();
    QString path = pclPathCombo->currentText();
    path = GetAbsoluteURL(path);

    for ( std::map<int, QString>::iterator it = d->mBookmarks.begin(); it != d->mBookmarks.end(); ++it )
    {
      if (it->second == path)
      {
        bFound = true;
        break;
      }
    }

    if (bFound == false)
    {
      d->mBookmarks[pclBookm->insertItem(txt, d->mBookmarks.size())] = path;
    }
  }
}

void FCHtmlView::CreateBookmarkPopup()
{
  pclBookm->clear();
  std::map<int, QString> tmp = d->mBookmarks;
  d->mBookmarks.clear();

  pclBookm->insertItem( tr( "Add Bookmark" ), this, SLOT( AddBookmark() ) );
  if (tmp.size() > 0)
    pclBookm->insertItem( tr( "Update Bookmarks" ), this, SLOT( CheckBookmarks() ) );
  pclBookm->insertSeparator();

  int i = 0;
  for (std::map<int, QString>::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
    QString txt = d->m_strCaption + it->second;
    int iPos = pclBookm->insertItem(d->m_strCaption + it->second, i++);
    d->mBookmarks[iPos] = it->second;
  }
}

void FCHtmlView::CreateHistoryPopup()
{
  pclHistory->clear();
  std::map<int, QString> tmp = d->mHistory;
  d->mHistory.clear();

  for (std::map<int, QString>::iterator it = tmp.begin(); it != tmp.end(); ++it)
  {
	  d->mHistory[pclHistory->insertItem(it->second, d->mHistory.size())] = it->second;
    AddToPath(GetAbsoluteURL(it->second));
  }
}

void FCHtmlView::CheckBookmarks()
{
  if (d->mBookmarks.size() == 0)
    return;

  int iButton = QMessageBox::information(this, "FreeCAD", tr("All unavailable bookmarks will be deleted\n"
                           "Continue ?"), tr("Yes"), tr("No"), QString::null, 1);

  if (iButton != 0) // not Ok pressed
    return;

  std::map<int, QString> mChecked;
  for (std::map<int, QString>::iterator it = d->mBookmarks.begin(); it != d->mBookmarks.end(); ++it)
  {
    const QMimeSource * mime = pclBrowser->mimeSourceFactory()->data(it->second);
    if (mime == NULL)
    {
			QString msg = tr("%1 is unavailable.\n Do you want to delete it?").arg(it->second.latin1());
      iButton = QMessageBox::information(this, "FreeCAD", msg, tr("Yes"), tr("No"), QString::null, 0);

      if (iButton != 0)
      {
        // bookmark is not OK but take it anyway
        mChecked[it->first] = it->second;
      }
    }
    else // bookmark is OK
      mChecked[it->first] = it->second;
  }

  d->mBookmarks.clear();
  d->mBookmarks = mChecked;
  CreateBookmarkPopup();

  QMessageBox::information(this, "FreeCAD", tr("All bookmarks are uptodate"));
}

void FCHtmlView::OnChange(FCSubject<FCParameterGrp::MessageType> &rCaller,FCParameterGrp::MessageType sReason)
{
  FCParameterGrp& rclGrp = ((FCParameterGrp&)rCaller);

  if (strcmp(sReason, "EnableBookmarks") == 0)
    d->bBookm = rclGrp.GetBool(sReason, false);
  else if (strcmp(sReason, "EnableHistory") == 0)
    d->bHistory = rclGrp.GetBool(sReason, false);
  else if (strcmp(sReason, "NbOfHistoryItems") == 0)
  {
    d->iMaxHist = rclGrp.GetInt(sReason, 20);
    if (!d->bHistory) d->iMaxHist = 0;
  }
  else if (strcmp(sReason, "NbOfBookmarks") == 0)
  {
    d->iMaxBookm = rclGrp.GetInt(sReason, 20);
    if (!d->bBookm) d->iMaxBookm = 0;
  }
}

void FCHtmlView::OnChange (FCSubject<FCProcess::MessageType> &rCaller,FCProcess::MessageType rcReason)
{
  if (&d->m_Process != &rCaller)
    return;

  // observe incoming signals
  switch (rcReason)
  {
    case FCBaseProcess::processStarted:
      break;
    case FCBaseProcess::processFailed:
    {
      QMessageBox::critical(this, "Browser", 
#ifdef FC_OS_WIN32
      FCBaseProcess::SystemWarning(GetLastError(), d->m_Process.executable().c_str()).c_str());
#else
      QObject::tr("Cannot start '%1'").arg(d->m_Process.executable().c_str()));
#endif
    } break;
    case FCBaseProcess::processExited:
      break;
    case FCBaseProcess::processKilled:
      break;
    case FCBaseProcess::receivedStdout:
      break;
    case FCBaseProcess::receivedStderr:
      break;
    case FCBaseProcess::wroteStdin:
      break;
    case FCBaseProcess::launchFinished:
      break;
  }
}

void FCHtmlView::onMinWidthReached (bool show)
{
  if (show)
    pclPathCombo->show();
  else
    pclPathCombo->hide();
}

//// FCWhatsThis //////////////////////////////////////////////////////

static FCWhatsThisPrivate * hh = NULL;


#define cursor_mask_width 32
#define cursor_mask_height 32
static unsigned char cursor_mask_bits[] = {
  0x01, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x07, 0x00, 0x07, 0xf8, 0x0f, 0x00,
  0x0f, 0xfc, 0x1f, 0x00, 0x1f, 0x3e, 0x1f, 0x00, 0x3f, 0x3e, 0x1f, 0x00,
  0x7f, 0x3e, 0x1f, 0x00, 0xff, 0x3e, 0x1f, 0x00, 0xff, 0x9d, 0x0f, 0x00,
  0xff, 0xc3, 0x07, 0x00, 0xff, 0xe7, 0x03, 0x00, 0x7f, 0xe0, 0x03, 0x00,
  0xf7, 0xe0, 0x03, 0x00, 0xf3, 0xe0, 0x03, 0x00, 0xe1, 0xe1, 0x03, 0x00,
  0xe0, 0xe1, 0x03, 0x00, 0xc0, 0xe3, 0x03, 0x00, 0xc0, 0xe3, 0x03, 0x00,
  0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

#define cursor_bits_width 32
#define cursor_bits_height 32
static unsigned char cursor_bits_bits[] = {
  0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0xf0, 0x07, 0x00,
  0x09, 0x18, 0x0e, 0x00, 0x11, 0x1c, 0x0e, 0x00, 0x21, 0x1c, 0x0e, 0x00,
  0x41, 0x1c, 0x0e, 0x00, 0x81, 0x1c, 0x0e, 0x00, 0x01, 0x01, 0x07, 0x00,
  0x01, 0x82, 0x03, 0x00, 0xc1, 0xc7, 0x01, 0x00, 0x49, 0xc0, 0x01, 0x00,
  0x95, 0xc0, 0x01, 0x00, 0x93, 0xc0, 0x01, 0x00, 0x21, 0x01, 0x00, 0x00,
  0x20, 0xc1, 0x01, 0x00, 0x40, 0xc2, 0x01, 0x00, 0x40, 0x02, 0x00, 0x00,
  0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

FCWhatsThisPrivate::FCWhatsThisPrivate()
  : QObject( 0, "Whats This object" )
{
  qAddPostRoutine( clearWhatsThis );
  hh = this;
  mode = Inactive;
  cursor = new QCursor( QBitmap( cursor_bits_width, cursor_bits_height, cursor_bits_bits, true ),
			                  QBitmap( cursor_mask_width, cursor_mask_height, cursor_mask_bits, true ), 1, 1 );
}

FCWhatsThisPrivate::~FCWhatsThisPrivate()
{
  if ( mode == Active )
  	QApplication::restoreOverrideCursor();

  FCWhatsThisItem* i;
  QWidget * w;
  for (std::map<QWidget*, FCWhatsThisItem*>::iterator it=mWidgetItem.begin(); it != mWidgetItem.end(); ++it)
  {
	  w = it->first;
    i = it->second;
	  i->deref();
	  if ( !i->count )
	    delete i;
  }
  mWidgetItem.clear();
  topLevelWidget.clear();

  delete cursor;

  hh = NULL;
}

FCWhatsThisPrivate::FCWhatsThisItem* FCWhatsThisPrivate::item( QWidget * widget )
{
  FCWhatsThisItem * i = NULL;
  if (hh->mWidgetItem.find(widget) != hh->mWidgetItem.end())
    i = hh->mWidgetItem[widget];
 
  if ( i )
  	FCWhatsThis::remove( widget );
  
  i = new FCWhatsThisItem;
  mWidgetItem[widget] = i;
  QWidget * t = widget->topLevelWidget();
  
  if ( topLevelWidget.find( t ) == topLevelWidget.end() )
  {
	  topLevelWidget[t] = t;
	  t->installEventFilter( this );
  }

  connect( widget, SIGNAL(destroyed()), this, SLOT(removeWidget()) );
  return i;
}

bool FCWhatsThisPrivate::eventFilter( QObject * o, QEvent * e )
{
  if ( !o || !e )
  	return false;

  switch( mode ) 
  {
    case Inactive:
 	    if ( e->type() == QEvent::Accel && ((QKeyEvent *)e)->key() == Key_F1 &&
 	         o->isWidgetType() && ((QKeyEvent *)e)->state() == ShiftButton ) 
      {
 	      QWidget * w = ((QWidget *)o)->focusWidget();
        FCWhatsThisPrivate::FCWhatsThisItem* i = NULL;
        if (w && (mWidgetItem.find(w) != mWidgetItem.end()))
        {
          i = mWidgetItem[w];
        }
 	      if ( i && !i->txt.isNull() ) 
        {
      		if ( i->whatsthis )
		        showWhatsThis( w, i->whatsthis->text( QPoint(0,0) ), w->mapToGlobal( w->rect().center() ) );
      		else
    		    showWhatsThis( w, i->txt, w->mapToGlobal( w->rect().center() ));
		  
          ((QKeyEvent *)e)->accept();
      		return true;
 	      }
 	    }
	    break;
    case Active:
    	if ( e->type() == QEvent::MouseButtonPress && o->isWidgetType() ) 
      {
  	    QWidget * w = (QWidget *) o;
	      if ( ( (QMouseEvent*)e)->button() == RightButton )
		      return false; 
  	    if ( w->customWhatsThis() )
	      	return false;
	      FCWhatsThisPrivate::FCWhatsThisItem * i = NULL;
	      while( w && !i ) 
        {
          if (mWidgetItem.find(w) != mWidgetItem.end())
            i = mWidgetItem[w];
		      if ( !i )
		        w = w->parentWidget();
	      }

  	    leaveWhatsThisMode();
	      if (!i )
      		return true;
  	    QPoint pos =  ((QMouseEvent*)e)->pos();
	      if ( i->whatsthis )
		      showWhatsThis( w, i->whatsthis->text( pos ), w->mapToGlobal(pos) );
	      else
      		showWhatsThis( w, i->txt, w->mapToGlobal(pos) );
	      return true;
	    } 
      else if ( e->type() == QEvent::MouseButtonRelease ) 
      {
  	    if ( ( (QMouseEvent*)e)->button() == RightButton )
	      	return false; 
  	    return !o->isWidgetType() || !((QWidget*)o)->customWhatsThis();
	    } 
      else if ( e->type() == QEvent::MouseMove ) 
      {
  	    return !o->isWidgetType() || !((QWidget*)o)->customWhatsThis();
    	} 
      else if ( e->type() == QEvent::KeyPress ) 
      {
  	    QKeyEvent* kev = (QKeyEvent*)e;

	      if (kev->key() == Qt::Key_Escape)
        {
      		leaveWhatsThisMode();
		      return true;
  	    }
	      else if ( kev->key() == Key_Menu || ( kev->key() == Key_F10 && kev->state() == ShiftButton ) )
		      return false;
        else if ( kev->state() == kev->stateAfter() && kev->key() != Key_Meta )
      		leaveWhatsThisMode();
	    } 
      else if ( e->type() == QEvent::MouseButtonDblClick ) 
      {
  	    return true;
	    }
    	break;
  }

  return FALSE;
}

void FCWhatsThisPrivate::createWhatsThis()
{
  if ( !hh )
  	hh = new FCWhatsThisPrivate();
}

void FCWhatsThisPrivate::clearWhatsThis()
{
  delete hh;
  hh = NULL;
}

void FCWhatsThisPrivate::leaveWhatsThisMode()
{
  if ( mode == Active ) 
  {
    QApplication::restoreOverrideCursor();
  	mode = Inactive;
	  qApp->removeEventFilter( this );
  }
}

void FCWhatsThisPrivate::showWhatsThis( QWidget * widget, const QString &text, const QPoint& ppos)
{
  currentText = text;

  TDocType type=Html;
  if (FCBrowserSourceFactory::canConvertToHTML(text) || currentText.findRev(".html") != -1)
  {
    // get text of the url
    QWidget* w = ApplicationWindow::Instance->GetCustomWidgetManager()->getDockWindow("Help bar");
    if (w->inherits("FCHtmlView"))
    {
      FCHtmlView* help = (FCHtmlView*)w;
      help->getBrowser()->setSource(text);
    }
  }
  else
  {
    // use the Qt's WhatsThis
    QWhatsThis::enterWhatsThisMode();
    QWhatsThis::leaveWhatsThisMode(text, ppos);
  }
}

void FCWhatsThisPrivate::add( QWidget * widget, FCWhatsThis* special )
{
  item( widget )->whatsthis = special;
}

void FCWhatsThisPrivate::add( QWidget * widget, const QString &text )
{
  item( widget )->txt = text;
}

//////////////////////////////////////////////////////////////////////////////////

void FCWhatsThis::add( QWidget * widget, const QString &text )
{
  FCWhatsThisPrivate::createWhatsThis();
  hh->add(widget,text);
}

void FCWhatsThis::remove( QWidget * widget )
{
  FCWhatsThisPrivate::createWhatsThis();
  FCWhatsThisPrivate::FCWhatsThisItem * i = NULL;
  if (hh->mWidgetItem.find(widget) != hh->mWidgetItem.end())
    i = hh->mWidgetItem[widget];
  if ( !i )
  	return;
  hh->mWidgetItem.erase( widget );
  i->deref();
  if ( !i->count )
	  delete i;
}

QString FCWhatsThis::textFor( QWidget * widget, const QPoint& pos)
{
  FCWhatsThisPrivate::createWhatsThis();
  FCWhatsThisPrivate::FCWhatsThisItem * i = NULL;
  if (hh->mWidgetItem.find(widget) != hh->mWidgetItem.end())
    i = hh->mWidgetItem[widget];
  if (!i)
  	return QString::null;
  return i->whatsthis? i->whatsthis->text( pos ) : i->txt;
}

FCWhatsThis::FCWhatsThis( QWidget * widget)
{
  FCWhatsThisPrivate::createWhatsThis();
  hh->add(widget,this);
}

FCWhatsThis::FCWhatsThis( QWidget * widget, QString url)
 : m_sURL(url)
{
  FCWhatsThisPrivate::createWhatsThis();
  hh->add(widget,this);
}

FCWhatsThis::~FCWhatsThis()
{
}

QString FCWhatsThis::text( const QPoint & )
{
  // returns the url containing the text
  return m_sURL;
}

void FCWhatsThis::enterWhatsThisMode()
{
  FCWhatsThisPrivate::createWhatsThis();
  
  if ( hh->mode == FCWhatsThisPrivate::Inactive ) 
  {
	  QApplication::setOverrideCursor( *hh->cursor, false );
	  hh->mode = FCWhatsThisPrivate::Active;
	  qApp->installEventFilter( hh );
  }
}

bool FCWhatsThis::inWhatsThisMode()
{
  if (!hh)
	  return false;
  return hh->mode == FCWhatsThisPrivate::Active;
}

void FCWhatsThis::leaveWhatsThisMode( const QString& text, const QPoint& pos )
{
  if ( !inWhatsThisMode() )
  	return;
  hh->leaveWhatsThisMode();
  if ( !text.isNull() )
  	hh->showWhatsThis( 0, text, pos );
}

#include "moc_HtmlView.cpp"
#include "moc_HtmlViewP.cpp"

