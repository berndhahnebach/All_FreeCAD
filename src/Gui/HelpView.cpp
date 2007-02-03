/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#include "HelpView.h"
#include "Process.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "FileDialog.h"
#include "Process.h"
#include "WhatsThis.h"
#include "Action.h"
#include "Command.h"


using namespace Gui;
using namespace Gui::DockWnd;

// --------------------------------------------------------------------------

namespace Gui {
namespace DockWnd {

class TextBrowserPrivate
{
public:
  enum TMode {Backward, Forward, None};

  Q3ValueStack<QString> fwStack;
  Q3ValueStack<QString> bwStack;

  bool bw, fw;
  TMode tMode;
  Q3Http* http;
  Q3Ftp* ftp;

  TextBrowserPrivate() : bw( false ), fw( false ), tMode( None ) 
  {
    http = new Q3Http;
    ftp  = new Q3Ftp;
  }
  
  ~TextBrowserPrivate()
  {
    delete http; delete ftp;
  }
};

// --------------------------------------------------------------------------

class DocumentationSource : public Q3StoredDrag
{
public:
  DocumentationSource( const char * mimeType, const QString& path )
      : Q3StoredDrag(mimeType, 0L, 0), _path(path)
  {
  }

  QByteArray encodedData (const char* data) const
  {
    QString test(
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
    return test.toUtf8();
  }

private:
  QString  _path;
};

} // namespace DockWnd
} // namespace Gui

// --------------------------------------------------------------------------

HelpSourceFactory::HelpSourceFactory()
    : Q3MimeSourceFactory()
{
}

HelpSourceFactory::~HelpSourceFactory()
{
}

const QMimeSource* HelpSourceFactory::data(const QString& abs_name) const
{
  const QMimeSource* mime = Q3MimeSourceFactory::data(abs_name);

  if ( mime )
  {
    const char* fm = mime->format();
    // cannot decode this
    if ( strcmp(fm, "application/octet-stream") == 0)
      return new DocumentationSource( "text/html;charset=iso8859-1", abs_name );
  }

  return mime;
}

QString HelpSourceFactory::makeAbsolute(const QString& abs_or_rel_name, const QString& context) const
{
  // check if local file or data over network
  Q3Url url(abs_or_rel_name);
  QString pro = url.protocol();

  if ( pro != "file")
    return abs_or_rel_name; // no local file, so let abs_or_rel_name unchanged
  else
    return Q3MimeSourceFactory::makeAbsolute(abs_or_rel_name, context);
}

void HelpSourceFactory::setText( const QString& abs_name, const QString& text )
{
  Q3MimeSourceFactory::setText(abs_name, text);
}

void HelpSourceFactory::setImage( const QString& abs_name, const QImage& im )
{
  Q3MimeSourceFactory::setImage(abs_name, im);
}

void HelpSourceFactory::setPixmap( const QString& abs_name, const QPixmap& pm )
{
  Q3MimeSourceFactory::setPixmap(abs_name, pm);
}

void HelpSourceFactory::setData( const QString& abs_name, QMimeSource* data )
{
  Q3MimeSourceFactory::setData(abs_name, data);
}

void HelpSourceFactory::setFilePath( const QStringList& s)
{
  Q3MimeSourceFactory::setFilePath(s);
}

QStringList HelpSourceFactory::filePath() const
{
  return Q3MimeSourceFactory::filePath();
}

void HelpSourceFactory::setExtensionType( const QString& ext, const char* mimetype )
{
  Q3MimeSourceFactory::setExtensionType(ext, mimetype);
}

// --------------------------------------------------------------------------

/* TRANSLATOR Gui::DockWnd::TextBrowser */
TextBrowser::TextBrowser(QWidget * parent, const char * name)
  : Q3TextBrowser(parent, name)
{
  WhatsThis::setHelpView( this );
  StdCmdDescription::setHelpView( this );

  d = new TextBrowserPrivate;

  setMimeSourceFactory(new HelpSourceFactory);

  setHScrollBarMode(Q3ScrollView::AlwaysOff);
  setVScrollBarMode(Q3ScrollView::AlwaysOff);
  mimeSourceFactory()->setExtensionType("HTML", "text/html;charset=iso8859-1");
  mimeSourceFactory()->setExtensionType("HTM", "text/html;charset=iso8859-1");
  mimeSourceFactory()->setExtensionType("FCParam", "text/xml;charset=UTF-8");

  setAcceptDrops( TRUE );
  viewport()->setAcceptDrops( TRUE );

  connect( d->http, SIGNAL(done(bool)), this, SLOT(done(bool)));
  connect( this, SIGNAL(backwardAvailable(bool)), this, SLOT(setBackwardAvailable(bool)));
  connect( this, SIGNAL(forwardAvailable (bool)), this, SLOT(setForwardAvailable (bool)));
}

TextBrowser::~TextBrowser()
{
  delete d;
  Q3MimeSourceFactory* hs = mimeSourceFactory ();
  delete hs;
}

void TextBrowser::setSource (const QString & name)
{
  ConsoleMsgFlags ret = Base::Console().SetEnabledMsgType("MessageBox",ConsoleMsgType::MsgType_Wrn|
                                                                       ConsoleMsgType::MsgType_Err, false);
  Q3TextBrowser::setSource(name);
  Base::Console().SetEnabledMsgType("MessageBox", ret, true);
#if 0
  QString source = name;
  QString mark;
  int hash = name.find('#');
  if ( hash != -1)
  {
    source  = name.left( hash );
    mark = name.mid( hash+1 );
  }

  QString url = mimeSourceFactory()->makeAbsolute( source, context() );

  if (!source.isEmpty())
  {
    const QMimeSource* mime = mimeSourceFactory()->data(source, context());
    if ( mime == 0 )
    {
      // check if local file or data over network
      Q3Url url(source);
      QString pro = url.protocol();
/*
      if ( pro == "http" )
      {
        QFile file("testfile");
        if (!file.open(IO_WriteOnly))
          return;
        d->http->setHost( url.host() );
        d->http->get( url.fileName(), &file );
      }
      else if ( pro == "ftp" )
      {
        QFile file("testfile");
        if (!file.open(IO_WriteOnly))
          return;
        d->ftp->connectToHost( url.host() );
        d->ftp->login();
        d->ftp->cd( url.path() );
        d->ftp->get( url.fileName(), &file );
        d->ftp->close();
      }
      else
      {
        qDebug( "Not supported protocol '%s'\n", pro.latin1() );
      }
*/
      if ( pro != "file")
      {
        QString msg = tr("Can't load '%1'.\nDo you want to start your favourite external browser instead?").arg(source);
        if ( QMessageBox::information(this, "FreeCAD", msg, QMessageBox::Yes|QMessageBox::Default, 
             QMessageBox::No|QMessageBox::Escape ) == QMessageBox::Yes )
          emit startExternalBrowser(name);
        return;
      }
    }
    else
    {
      QString txt;
      if (Q3TextDrag::decode(mime, txt) == false)
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

  if (d->bwStack.isEmpty() || d->bwStack.top() != url)
    d->bwStack.push(url);

  int bwStackCount = (int)d->bwStack.count();
  if ( d->bwStack.top() == url )
    bwStackCount--;

  if (d->tMode == TextBrowserPrivate::None)
    d->fwStack.clear();
  int fwStackCount = (int)d->fwStack.count();
  if ( fwStackCount > 0 && d->fwStack.top() == url )
    fwStackCount--;

  ConsoleMsgFlags ret = Base::Console().SetEnabledMsgType("MessageBox",ConsoleMsgType::MsgType_Wrn|
                                                                       ConsoleMsgType::MsgType_Err, false);
  setSource(name);
  Base::Console().SetEnabledMsgType("MessageBox", ret, true);

  emit backwardAvailable( bwStackCount > 0 );
  emit forwardAvailable( fwStackCount > 0 );
#endif
}

void TextBrowser::setText (const QString & contents, const QString & context)
{
  Q3TextBrowser::setText(contents, context);
}

void TextBrowser::done( bool err )
{
  if ( !err )
    setText( d->http->readAll() );
}

void TextBrowser::backward()
{
  if ( d->bwStack.count() <= 1)
    return;

  d->fwStack.push( d->bwStack.pop() );

  d->tMode = TextBrowserPrivate::Backward;
  setSource( d->bwStack.pop() );
  d->tMode = TextBrowserPrivate::None;
  forwardAvailable( true );
}

void TextBrowser::forward()
{
  if ( d->fwStack.isEmpty() )
    return;

  d->tMode = TextBrowserPrivate::Forward;
  setSource( d->fwStack.pop() );
  d->tMode = TextBrowserPrivate::None;
  forwardAvailable( !d->fwStack.isEmpty() );
}

void TextBrowser::setBackwardAvailable( bool b )
{
  d->bw = b;
}

void TextBrowser::setForwardAvailable( bool b )
{
  d->fw = b;
}

void TextBrowser::viewportContextMenuEvent ( QContextMenuEvent * e )
{
  // create and show up your context menu
  //
  Q3PopupMenu menu;
  if ( hasSelectedText() )
  {
    menu.insertItem(tr("Copy"), this, SLOT(copy()));
  }
  else
  {
    int id = menu.insertItem(Gui::BitmapFactory().pixmap("back_pixmap"), tr("Previous"), this, SLOT(backward()));
    menu.setItemEnabled( id, d->bw );
    id = menu.insertItem( Gui::BitmapFactory().pixmap("forward_pixmap"), tr("Forward"), this, SLOT(forward()));
    menu.setItemEnabled( id, d->fw );
    menu.insertItem(Gui::BitmapFactory().pixmap("home_pixmap"), tr("Home"), this, SLOT(home()));
    menu.insertSeparator();
    menu.insertItem(tr("Refresh"), this, SLOT(reload()));
    menu.insertSeparator();
  }

  menu.insertItem(tr("Select all"), this, SLOT(selectAll()));
  menu.exec(QCursor::pos());
}

void TextBrowser::contentsDropEvent(QDropEvent  * e)
{
  const QMimeData* mimeData = e->mimeData();
  if ( mimeData->hasFormat("text/x-action-items") ) {
    QByteArray itemData = mimeData->data("text/x-action-items");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

    int ctActions; dataStream >> ctActions;

    // handle the first item only
    QString action;
    dataStream >> action;

    CommandManager& rclMan = Application::Instance->commandManager();
    Command* pCmd = rclMan.getCommandByName(action.latin1());
    if (pCmd)
    {
      QString url = pCmd->getHelpUrl();
      if ( url.isEmpty() )
      {
        // cannot show help to this command
        QString txt = QString(
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"
        "<html>"
        "<body bgcolor=white text=black alink=red link=darkblue vlink=darkmagenta>"
        "<h2>"
        "  No description for '%1'"
        "</h2>"
        "<hr>"
        "</body>"
        "</html>" ).arg( action );

        setText( txt );
      }
      else
        setSource( pCmd->getHelpUrl() );
    }

    e->setDropAction(Qt::CopyAction);
    e->accept();
  } else if ( Q3UriDrag::canDecode(e) ) {
    QStringList fn;
    Q3UriDrag::decodeLocalFiles(e, fn);
    QString file = fn.first();
    QFileInfo info(file);
    if ( info.exists() && info.isFile() )
      setSource(file);

    e->setDropAction(Qt::CopyAction);
    e->accept();
  } else {
    e->ignore();
  }
}

void TextBrowser::contentsDragEnterEvent  (QDragEnterEvent * e)
{
  const QMimeData* mimeData = e->mimeData();
  if ( mimeData->hasFormat("text/x-action-items") )
    e->accept();
  else if (Q3UriDrag::canDecode(e))
    e->accept();
  else
    e->ignore();
}

void TextBrowser::contentsDragMoveEvent( QDragMoveEvent *e )
{
  const QMimeData* mimeData = e->mimeData();
  if ( mimeData->hasFormat("text/x-action-items") )
    e->accept();
  else if (Q3UriDrag::canDecode(e))
    e->accept();
  else
    e->ignore();
}

// --------------------------------------------------------------------

/* TRANSLATOR Gui::DockWnd::HelpView */

/**
 *  Constructs a HelpView which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'. \a home is the start page to show.
 */
HelpView::HelpView( const QString& start,  QWidget* parent,  const char* name, Qt::WFlags fl )
  : DockWindow( 0L, parent, name, fl )
{
  TextBrowser* browser = new TextBrowser(this, "HelpViewer");
  browser->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
  // set the path where the factory is looking for if you set a new source
  browser->mimeSourceFactory()->setFilePath( start );

  // set the start page now
  if (!start.isEmpty())
    browser->setSource( start );
  
  QHBoxLayout* layout = new QHBoxLayout();
  layout->setAlignment(Qt::AlignTop);
  layout->setSpacing(1);
  layout->setMargin (1);

  // create the group box for the layout
  QGroupBox* groupBox = new QGroupBox(this);

  // the 'Backward' button
  QToolButton* back = new QToolButton( groupBox );
  back->setPixmap( Gui::BitmapFactory().pixmap("back_pixmap") );
  back->setAutoRaise(true);
  QToolTip::add(back, tr("Previous"));

  // the 'Forward' button
  QToolButton* forward = new QToolButton( groupBox );
  forward->setPixmap( Gui::BitmapFactory().pixmap("forward_pixmap") );
  forward->setAutoRaise(true);
  QToolTip::add( forward, tr("Next"));

  // the 'Home' button
  QToolButton* home = new QToolButton( groupBox );
  home->setPixmap( Gui::BitmapFactory().pixmap("home_pixmap") );
  home->setAutoRaise(true);
  QToolTip::add( home, tr("Home"));

  // the 'Open' button
  QToolButton* open = new QToolButton( groupBox );
  open->setPixmap( Gui::BitmapFactory().pixmap("helpopen") );
  open->setAutoRaise(true);
  QToolTip::add( open, tr("Open"));

  QGridLayout* formLayout = new QGridLayout( this );
  formLayout->setSpacing( 1 );
  formLayout->setMargin ( 1 );

  // add the buttons and the space
  layout->addWidget( back );
  layout->addWidget( forward );
  layout->addWidget( home );
  layout->addWidget( open );
  QSpacerItem* spacer = new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  layout->addItem( spacer );
  groupBox->setLayout(layout);

  // add the button group with its elements and the browser to the layout
  formLayout->addWidget( groupBox, 0, 0 );
  formLayout->addWidget( browser, 1, 0 );

  connect( this, SIGNAL(setSource( const QString& )), browser, SLOT(setSource( const QString& )));
  connect( browser, SIGNAL(backwardAvailable(bool)), back, SLOT(setEnabled(bool)));
  connect( browser, SIGNAL(forwardAvailable (bool)), forward, SLOT(setEnabled(bool)));
  connect( browser, SIGNAL(startExternalBrowser(const QString&)), 
           this,        SLOT  (startExternalBrowser(const QString&)));
  connect( back,    SIGNAL(clicked()), browser, SLOT(backward()));
  connect( browser, SIGNAL(backwardAvailable(bool) ), back, SLOT(setEnabled(bool) ) );
  connect( forward, SIGNAL(clicked()), browser, SLOT(forward()));
  connect( browser, SIGNAL(forwardAvailable(bool) ), forward, SLOT(setEnabled(bool) ) );
  connect( home,    SIGNAL(clicked()), browser, SLOT(home()));
  connect( open,    SIGNAL(clicked()), this, SLOT(openHelpFile()));
  forward->setEnabled( false );
  back->setEnabled( false );
}

/**
 *  Destroys the object and frees any allocated resources
 */
HelpView::~HelpView()
{
  // no need to delete child widgets, Qt does it all for us
}

/**
 * Sets the file source \a src to the help view's text browser.
 */
void HelpView::setFileSource( const QString& src )
{
  setSource( src );
}

/**
 * Opens a file dialog to specify a help page to open.
 */
void HelpView::openHelpFile()
{
  QString fn = QFileDialog::getOpenFileName(this, tr("Open file"), QString(), tr("All Html files (*.html *.htm)"));
  if ( !fn.isEmpty() )
    setSource( fn );
}

/** 
 * Looks up into preferences to start an external browser to render sites which this class cannot do
 * right now. If now browser has been specified so far or the start fails an error message appears.
 */
void HelpView::startExternalBrowser( const QString& url )
{
  ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/OnlineHelp");
  QString browser = hGrp->GetASCII( "ExternalBrowser", "" ).c_str();

  if (browser.isEmpty())
  {
    QMessageBox::critical( this, tr("External browser"), tr("No external browser found. Specify in preferences, please") );
    return;
  }

  // create the command to execute
  Gui::Process program;
  program.setExecutable( browser );
  program << url;

  if ( !program.start() )
  {
    QMessageBox::critical( this, tr("External browser"), tr("Starting of %1 failed").arg( browser ) );
  }
}

#include "moc_HelpView.cpp"

