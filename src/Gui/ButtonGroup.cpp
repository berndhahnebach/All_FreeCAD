/***************************************************************************
                          ButtonGroup.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file ButtonGroup.cpp
 *  \brief The button group module
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
# include <qpainter.h>
# include <qstyle.h>
# include <qapplication.h>
#endif

#include <qobjectlist.h>
#include "ButtonGroup.h"
#include "Application.h"
#include "DlgCustomizeImp.h"


FCButtonGroup::FCButtonGroup(QWidget * parent, const char * name)
: QButtonGroup(parent, name), m_iChildWidth(32), m_iChildHeight(32)
{
  pScrollWidget = new QScrollView(parent, "Scroll", QWidget::WPaintClever);
  setFrameStyle(QFrame::NoFrame);
  pScrollWidget->setFrameStyle(QFrame::NoFrame);
  pScrollWidget->addChild(this);
  pScrollWidget->setResizePolicy(QScrollView::AutoOneFit);
  initialize();
}

FCButtonGroup::FCButtonGroup(const QString & title, QWidget * parent, const char * name)
: QButtonGroup(title, parent, name), m_iChildWidth(32), m_iChildHeight(32)
{
  pScrollWidget = new QScrollView(parent, "Scroll", QWidget::WPaintClever);
  setFrameStyle(QFrame::NoFrame);
  pScrollWidget->setFrameStyle(QFrame::NoFrame);
  pScrollWidget->addChild(this);
  pScrollWidget->setResizePolicy(QScrollView::AutoOneFit);
  initialize();
}

FCButtonGroup::FCButtonGroup(int columns, Orientation o, QWidget * parent, const char * name)
: QButtonGroup(columns, o, parent, name), m_iChildWidth(32), m_iChildHeight(32)
{
  pScrollWidget = new QScrollView(parent, "Scroll", QWidget::WPaintClever);
  setFrameStyle(QFrame::NoFrame);
  pScrollWidget->setFrameStyle(QFrame::NoFrame);
  pScrollWidget->addChild(this);
  pScrollWidget->setResizePolicy(QScrollView::AutoOneFit);
  initialize();
}

FCButtonGroup::FCButtonGroup(int columns, Orientation o, const QString & title, QWidget * parent, const char * name)
: QButtonGroup(columns, o, title, parent, name), m_iChildWidth(32), m_iChildHeight(32)
{
  pScrollWidget = new QScrollView(parent, "Scroll", QWidget::WPaintClever);
  setFrameStyle(QFrame::NoFrame);
  pScrollWidget->setFrameStyle(QFrame::NoFrame);
  pScrollWidget->addChild(this);
  pScrollWidget->setResizePolicy(QScrollView::AutoOneFit);
  initialize();
}

FCButtonGroup::~FCButtonGroup(void)
{
}

void FCButtonGroup::initialize()
{
  m_Color = backgroundColor();

  resizeColumns();
  setFocusPolicy (ClickFocus);

  m_Popup = new QPopupMenu(0L);
  m_Popup->setCheckable(true);
  connect(m_Popup, SIGNAL(aboutToShow()), this, SLOT(popupMenuAboutToShow()));
}

void FCButtonGroup::resizeEvent (QResizeEvent * e)
{
  QButtonGroup::resizeEvent(e);

  QSize size = e->size();

  if (m_iWidth != size.width())
  {
    resizeColumns();
  }

  if (m_iHeight != size.height())
  {
    // reduce the minimum width
    setMinimumWidth(layout()->margin() + m_iChildWidth);
    resizeColumns();
  }
}

void FCButtonGroup::resizeColumns(void)
{
  int cols, rows;
  int h = height();
  int w = width ();
  int cnt = count();

  int margin = layout()->margin();

  cols = w / (margin + m_iChildWidth);
  rows = h / (margin + m_iChildHeight);

  if (rows > 0 && cols > 0)
  {
    if (cnt / rows + 1 > cols)
    {
      cols = cnt / rows;
      if (cnt % rows > 0)
        cols++;
    }
  }

  setColumns(cols);

  if (cols * (margin + m_iChildWidth) > w)
  {
    // increase the minimum width to show all buttons
    setMinimumWidth(cols * (margin + m_iChildWidth));
  }

  m_iWidth  = width();
  m_iHeight = height();
}

void FCButtonGroup::mousePressEvent( QMouseEvent * e )
{
  if (e->button() == LeftButton)
  {
    if (acceptDrops() == true)
    {
      QDragObject *drobj;
      drobj = new QTextDrag( title(), this );
      drobj->dragCopy();
    }
  }
  else if (e->button() == RightButton)
  {
    m_Popup->exec(QCursor::pos());
  }
  else if (e->button() == MidButton)
  {
  }
}

void FCButtonGroup::popupMenuAboutToShow()
{
  m_Popup->clear();

  if (count() > 0)
  {
//    m_Popup->insertItem("Side by side", this, SLOT(setNewBackgroundColor()));
//    m_Popup->insertItem("Untereinander", this, SLOT(setNewBackgroundColor()));
//    m_Popup->insertSeparator();

    if (m_Pixmaps.size() > 0)
      m_Popup->insertItem("Show buttons as icon", this, SLOT(showIcons()));
    else
      m_Popup->insertItem("Show buttons as text", this, SLOT(showText()));
    m_Popup->insertSeparator();
  }

  int colId = m_Popup->insertItem("Background color...", this, SLOT(setNewBackgroundColor()));
  int resId = m_Popup->insertItem("Reset background color", this, SLOT(resetBackgroundColor()));
}

void FCButtonGroup::setNewBackgroundColor()
{
  QColor color = QColorDialog::getColor ( backgroundColor(), this);
  if (color.isValid())
  {
    setPalette(QPalette(color, color));
  }
}

void FCButtonGroup::resetBackgroundColor()
{
  if (m_Color.isValid())
    setPalette(QPalette(m_Color, m_Color));
}

void FCButtonGroup::showIcons()
{
  int cnt = count();

  for (std::map<int, QPixmap>::const_iterator it = m_Pixmaps.begin(); it != m_Pixmaps.end(); it++)
  {
    if (it->first < cnt)
    {
      QButton* button = find(it->first);
      if (button != NULL)
        button->setPixmap(it->second);
    }
  }

  m_Pixmaps.clear();
}

void FCButtonGroup::showText()
{
  m_Pixmaps.clear();
  int cnt = count();
  for (int i=0; i<cnt; i++)
  {
    QButton* button = find(i);
    if (button != NULL)
    {
      const QPixmap* pic = button->pixmap();
      if (pic != NULL)
      {
        if (!pic->isNull())
         m_Pixmaps[i] = *button->pixmap();
      }

      button->setText(button->name(""));
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////

FCToolboxGroup::FCToolboxGroup ( QWidget * parent, const char * name)
: QVButtonGroup(parent, name)
{
  initialize(parent);
}

FCToolboxGroup::FCToolboxGroup ( const QString & title, QWidget * parent, const char * name)
: QVButtonGroup(title, parent, name)
{
  initialize(parent);
}

FCToolboxGroup::~FCToolboxGroup ()
{
  savePreferences();
}

void FCToolboxGroup::restorePreferences()
{
  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  std::map<std::string,FCCommand*> sCommands = cCmdMgr.GetCommands();

  std::vector<std::string> items = hPrefGrp->GetASCIIs(getPrefName().latin1());
  for (std::vector<std::string>::iterator it = items.begin(); it != items.end(); ++it)
  {
    sCommands[*it]->GetAction()->addTo(this);
  }
}

void FCToolboxGroup::savePreferences()
{
  int i=0;
  for (std::vector<std::string>::iterator it = alDroppedActions.begin(); it != alDroppedActions.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "%s%d", getPrefName().latin1(), i);
    hPrefGrp->SetASCII(szBuf, it->c_str());
  }
}

void FCToolboxGroup::initialize(QWidget* parent)
{
  setColumnLayout(0, Qt::Vertical );
  setFrameStyle(QFrame::NoFrame);
  if (parent) parent->setMinimumWidth(40);

  layout()->setSpacing( 0 );
  layout()->setMargin( 3 );
  ButtonGroupLayout = new QGridLayout( layout() );
  ButtonGroupLayout->setAlignment( Qt::AlignTop );
  ButtonGroupLayout->setSpacing( 1 );
  ButtonGroupLayout->setMargin( 1 );
  m_Color = backgroundColor();

  setAcceptDrops(true);
  setFocusPolicy (ClickFocus);

  m_Popup = new QPopupMenu(0L);
  m_Popup->setCheckable(true);
  connect(m_Popup, SIGNAL(aboutToShow()), this, SLOT(popupMenuAboutToShow()));

  restorePreferences();
}

bool FCToolboxGroup::addWidget(QWidget* w, int i)
{
  if (!w)
    return false;

  ButtonGroupLayout->addWidget(w, i, 0);
  
  // show if invisible
  if (!w->isVisible())
    w->show();

  return true;
}

bool FCToolboxGroup::addToolboxButton(FCToolboxButton* b, int i)
{
  connect(this, SIGNAL(signalMaximumWidth(int)), b, SLOT(slotResizeButton(int)));
  insert(b);
  return addWidget(b, i);
}

void FCToolboxGroup::paintEvent (QPaintEvent * e)
{
  QRect rect = visibleRect();
  emit signalMaximumWidth(rect.width());
  QVButtonGroup::paintEvent(e);
}

void FCToolboxGroup::mousePressEvent( QMouseEvent * e )
{
  if (e->button() == LeftButton)
  {
  }
  else if (e->button() == RightButton)
  {
    m_Popup->exec(QCursor::pos());
  }
  else if (e->button() == MidButton)
  {
  }
}

void FCToolboxGroup::dropEvent ( QDropEvent * e)
{
  QAction* pAction = FCActionDrag::pAction;
  if ( pAction ) 
  {
    pAction->addTo(this);
    alDroppedActions.push_back(pAction->name());
    FCActionDrag::pAction = NULL;
  }
}

void FCToolboxGroup::dragEnterEvent ( QDragEnterEvent * e)
{
  e->accept(FCActionDrag::canDecode(e));
}

void FCToolboxGroup::dragLeaveEvent ( QDragLeaveEvent * e)
{
}

void FCToolboxGroup::dragMoveEvent ( QDragMoveEvent * e)
{
}

void FCToolboxGroup::popupMenuAboutToShow()
{
  m_Popup->clear();

  int colId = m_Popup->insertItem("Background color...", this, SLOT(setNewBackgroundColor()));
  int resId = m_Popup->insertItem("Reset background color", this, SLOT(resetBackgroundColor()));
  m_Popup->insertSeparator();
  ApplicationWindow::Instance->GetCommandManager().AddTo("Std_DlgCustomize", m_Popup);
}

void FCToolboxGroup::setNewBackgroundColor()
{
  QColor color = QColorDialog::getColor ( backgroundColor(), this);
  if (color.isValid())
  {
    setPalette(QPalette(color, color));
  }
}

void FCToolboxGroup::resetBackgroundColor()
{
  if (m_Color.isValid())
    setPalette(QPalette(m_Color, m_Color));
}

/////////////////////////////////////////////////////////////////////////////////////

FCToolboxButton::FCToolboxButton( QWidget *parent, const char *name )
: QToolButton( parent, name ), pLastAction(NULL)
{
  tbShowText = false;
  raised = FALSE;
  setFocusPolicy( NoFocus );
  setTextAndPixmap (0, 0);
  setAutoResize( TRUE );
  setAcceptDrops ( true );
  setMinimumHeight(32);
}

FCToolboxButton::FCToolboxButton( const QString &text, const QString &tooltip, QWidget *parent, const char *name )
: QToolButton( parent, name ), pLastAction(NULL)
{
  tbShowText = true;
  raised = FALSE;
  setFocusPolicy( NoFocus );
  setTextAndPixmap (text, 0);
  setTooltip(tooltip);
  setAutoResize( TRUE );
  setAcceptDrops ( true );
  setMinimumHeight(32);
}

FCToolboxButton::FCToolboxButton( const QString &text, const QPixmap &pix, const QString &tooltip,
                                  QWidget *parent, const char *name )
: QToolButton( parent, name ), pLastAction(NULL)
{
  tbShowText = true;
  raised = FALSE;
  setFocusPolicy( NoFocus );
  setTextAndPixmap (text, pix);
  setTooltip(tooltip);
  setAutoResize( TRUE );
  setAcceptDrops ( true );
  setMinimumHeight(32);
}

FCToolboxButton::FCToolboxButton( const QString &text, const QPixmap &pix, const QString &tooltip,
                                  QObject *receiver, const char *member, QWidget *parent, const char *name )
: QToolButton( parent, name ), pLastAction(NULL)
{
  tbShowText = true;
  raised = FALSE;
  setFocusPolicy( NoFocus );
  setTextAndPixmap (text, pix);
  setTooltip(tooltip);
  // do the connection of the clicked() signal
  if (receiver && member)
      connect (this, SIGNAL(clicked ()), receiver, member);
  setAutoResize( TRUE );
  setAcceptDrops ( true );
  setMinimumHeight(32);
}

FCToolboxButton::~FCToolboxButton()
{
}

void FCToolboxButton::setTextAndPixmap( const QString &text, const QPixmap &pix)
{
  setText(text);
  setPixmap(pix);
}

void FCToolboxButton::setText(const char *t)
{
  textLabel = t;
  repaint();
}

void FCToolboxButton::setPixmap( const QPixmap& pixmap )
{
  QToolButton::setPixmap(pixmap);
  enabledPixmap=pixmap;
  repaint();
}

void FCToolboxButton::setTooltip( const QString& tooltip )
{
  if (!tooltip.isNull() && !tooltip.isEmpty())
    QToolTip::add (this, tooltip);
}

const char *FCToolboxButton::text() const
{
  return textLabel;
}

void FCToolboxButton::slotResizeButton(int width)
{
  setMaximumWidth(width);
}

QSize FCToolboxButton::sizeHint() const
{
  int w=0, h=0;

  if (pixmap()) {
    w = 5 + pixmap()->width();
    h = 5 + pixmap()->height() ;
  }

  if (text()) {
    w += fontMetrics().width(text()) + 10;
    if (fontMetrics().height() > h)
      h = fontMetrics().height();
  }
  if (w > 0 && h > 0)
    return QSize(w, h);
  else
    return QSize(1,1);
}

void FCToolboxButton::enterEvent( QEvent* )
{
  if ( isEnabled() )
  {
    raised = TRUE;
    setBackgroundMode(PaletteBackground);
    repaint(FALSE);
  }
}

void FCToolboxButton::leaveEvent( QEvent * )
{
  if( raised != FALSE )
  {
    raised = FALSE;
    setBackgroundMode(PaletteLight);
    repaint();
  }
}

void FCToolboxButton::dropEvent ( QDropEvent * e)
{
  QAction* pAction = FCActionDrag::pAction;
  if ( pAction )
  {
    setPixmap(pAction->iconSet().pixmap());
    setText(pAction->menuText());
    setTooltip(pAction->toolTip());

    // disconnect last action first
    if (pLastAction)
    {
      disconnect( this, SIGNAL( clicked() ), pLastAction, SIGNAL( activated() ) );
	    disconnect( this, SIGNAL( toggled(bool) ), pLastAction, SLOT( toolButtonToggled(bool) ) );
	    disconnect( this, SIGNAL( destroyed() ), pLastAction, SLOT( objectDestroyed() ) );
    }

    pLastAction = pAction;
    connect( this, SIGNAL( clicked() ), pAction, SIGNAL( activated() ) );
	  connect( this, SIGNAL( toggled(bool) ), pAction, SLOT( toolButtonToggled(bool) ) );
	  connect( this, SIGNAL( destroyed() ), pAction, SLOT( objectDestroyed() ) );

    if (!isEnabled())
    {
      makeDisabledPixmap();
      setPixmap(disabledPixmap);
    }
  }
}

void FCToolboxButton::dragEnterEvent ( QDragEnterEvent * e)
{
//  if (isEnabled())
  e->accept(FCActionDrag::canDecode(e));
}

void FCToolboxButton::dragLeaveEvent ( QDragLeaveEvent * e)
{
}

void FCToolboxButton::dragMoveEvent ( QDragMoveEvent * e)
{
}

void FCToolboxButton::drawButton( QPainter *_painter )
{
  paint( _painter );
}

void FCToolboxButton::drawButtonLabel( QPainter *_painter )
{
  paint( _painter );
}

void FCToolboxButton::makeDisabledPixmap()
{
  QPalette pal = palette();
  QColorGroup g = pal.disabled();

  // Find the outline of the colored portion of the normal pixmap

  QBitmap *pmm = (QBitmap*) enabledPixmap.mask();
  QPixmap pm;
  if (pmm != 0L)
  {
    pmm->setMask( *pmm );
    pm = *pmm;
  }
  else
  {
    pm.resize(enabledPixmap.size());
    enabledPixmap.fill(this, 0, 0);
  };

  // Prepare the disabledPixmap for drawing

  disabledPixmap.resize(enabledPixmap.size());
  disabledPixmap.fill( g.background() );

  // Draw the outline found above in highlight and then overlay a grey version
  // for a cheesy 3D effect ! BTW. this is the way that Qt 1.2+ does it for
  // Windows style

  QPainter p;
  p.begin( &disabledPixmap );
  p.setPen( g.light() );
  p.drawPixmap(1, 1, pm);
  p.setPen( g.text() );
  p.drawPixmap(0, 0, pm);
  p.end();
}

void FCToolboxButton::paletteChange(const QPalette &)
{
  makeDisabledPixmap();
  if ( !isEnabled() )
    setPixmap( disabledPixmap );
  repaint( TRUE );
}

void FCToolboxButton::on(bool flag)
{
  setOn(flag);
  repaint();
}

void FCToolboxButton::toggle()
{
  setOn(!isOn());
  repaint();
}

void FCToolboxButton::enable( bool enabled )
{
  setPixmap( (enabled ? enabledPixmap : disabledPixmap) );
  setEnabled( enabled );
}

void FCToolboxButton::showText(bool enable)
{
  tbShowText = enable;
  repaint();
}

void FCToolboxButton::paint( QPainter *painter )
{
  int dx=0, dy=0;
  QFontMetrics fm(fontMetrics());

  if ( isDown() || isOn() )
  {
#if QT_VER <= 230
    if ( style() == WindowsStyle )
#else
    if ( style().inherits("QWindowsStyle") )
#endif
    	qDrawWinButton( painter, 0, 0, width(), height(), colorGroup(), TRUE );
    else
    	qDrawShadePanel( painter, 0, 0, width(), height(), colorGroup(), TRUE, 2, 0L );
  }
  else if ( raised )
  {
#if QT_VER <= 230
    if ( style() == WindowsStyle )
#else
    if ( style().inherits("QWindowsStyle") )
#endif
    	qDrawWinButton( painter, 0, 0, width(), height(), colorGroup(), FALSE );
    else
    	qDrawShadePanel( painter, 0, 0, width(), height(), colorGroup(), FALSE, 2, 0L );
  }
  
  if ( pixmap() )
  {
    //dx = ( width() - pixmap()->width() ) / 2;
    dx = 5;
    dy = ( height() - pixmap()->height() ) / 2;
#if QT_VER <= 230
    if ( isDown() && style() == WindowsStyle ) 
#else
    if ( isDown() && style().inherits("QWindowsStyle") )
#endif
    {
    	dx++;
	    dy++;
    }

    painter->drawPixmap( dx, dy, *pixmap() );
  }

  if (text()&&tbShowText) 
  {
    dx = dx + (pixmap() ? pixmap()->width() : 0) + 5;
    dy = dy + (pixmap() ? (pixmap()->height() / 2) : height()/2-2) + (fm.boundingRect(text()).height() / 2);
    painter->drawText(dx, dy, text());
  }
}

/////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Constructor
 *
 * This constructor allows you to set the name of the button, and tell it
 * what widget it shall contain.  It also sets the default colors.
 */
QStackBarBtn::QStackBarBtn( QWidget *object, QWidget *parent, const char *name ) 
 : QToolButton( parent, name ), bIsSelected( false ), w(object)
{
  setBackgroundMode( PaletteLight ); 
}

/*!
 * \brief Another version of the constructor
 *
 * This constructor differs from the above one only in that you cannot
 * specify what widget the button shall contain.
 */
QStackBarBtn::QStackBarBtn( QWidget *parent, const char *name ) 
 : QToolButton( parent, name ), bIsSelected( false )
{
  setBackgroundMode( PaletteLight ); 
}

/*!
 * \brief destructor
 *
 */
QStackBarBtn::~QStackBarBtn()
{
  if (w)
    delete w;
  w = NULL;
}

/*!
 * \brief Marks/unmarks the button as selected
 *
 */
void QStackBarBtn::setSelected( bool b ) 
{ 
  bIsSelected = b; 
  update(); 
}

/*!
 * \brief Sets the widget this button/page will contain
 *
 * Sets the widget for this button/page to contain
 */
void QStackBarBtn::setWidget(QWidget* widget) 
{ 
	if( !widget )
	{
		fprintf(stderr, "QSTACKBAR ERROR - Cannot set a null widget\n");
		return;
	}

  w = widget; 
	w->hide();
}

/*!
 * \brief Returns the widget this button/page contains
 *
 * Returns the widget for this button/page
 */
QWidget* QStackBarBtn::widget() 
{ 
  return w; 
}

/*!
 * \brief Draws the button
 *
 * Draws the button
 */
void QStackBarBtn::drawButton( QPainter *p )
{
  int h = height();
  int w = width ();
  int d = h + 17;

  if ( bIsSelected == true ) 
  {
  	QFont f( p->font() );
	  f.setBold( true );
	  p->setFont( f );
  }


  QPointArray a( 7 );
  a.setPoint( 0, -1, h + 1 );
  a.setPoint( 1, -1, 1 );
  a.setPoint( 2, w - d, 1 );
  a.setPoint( 3, w - 20, h - 2 );
  a.setPoint( 4, w - 1, h - 2 );
  a.setPoint( 5, w - 1, h + 1 );
  a.setPoint( 6, -1, h + 1 );


  if ( bIsSelected == true )
  	p->setBrush( colorGroup().light() );
  else
  	p->setBrush( colorGroup().brush( QColorGroup::Background ) );

  p->setPen( colorGroup().mid().dark( 150 ) );
  p->drawPolygon( a );
  p->setPen( colorGroup().light() );
  p->drawLine( 0, 2, w - d, 2 );
  p->drawLine( w - d - 1, 2, w - 21, h - 1 );
  p->drawLine( w - 20, h - 1, w, h - 1 );
  p->setBrush( NoBrush );

  p->setPen( colorGroup().buttonText() );

  if ( p->fontMetrics().width( text() ) < w - d - 5 ) 
  {
  	p->drawText( 2, 2, w, h - 2, AlignVCenter | AlignLeft, text() );
  } 
  else 
  {
  	QString s = text().left( 1 );
	  int ew = p->fontMetrics().width( "..." );
	  int i = 1;
	  while ( p->fontMetrics().width( s ) + ew + p->fontMetrics().width( text()[i] )  < w - d - 5 )
	    s += text()[i++];
	  s += "...";
	  p->drawText( 2, 2, w, h - 2, AlignVCenter | AlignLeft, s );
  }
}

/////////////////////////////////////////////////////////////////////////////////////

static void set_background_mode( QWidget *top, QWidget::BackgroundMode bm )
{
  QObjectList *l = top->queryList( "QWidget" );
  l->append( top );
  for ( QObject *o = l->first(); o; o = l->next() )
  	( (QWidget*)o )->setBackgroundMode( bm );
  delete l;
}

/*!
  \class QStackBar
  \brief Implements a button-bar similar to that of MS Outlook

  Use this widget to implement multiple pages of widgets in a single vertical
  column.  
  
  The following code demonstrates how to create a stackbar, and add a few pages
  to it with some widgets.
  
  For more examples, see the tests in the test directory of the distribution.
  
\code

	QStackBar *sb;
	QListBox *l1;
	
	sb = new QStackBar(pParent, NULL);
	l1 = new QListBox *l1 = new QListBox( sb );
	
	l1->insertItem("Item 1");
	l1->insertItem("Item 2");
	l1->insertItem("Item 3");
	l1->insertItem("Item 4");
	
	sb.addPage( "List Box 1", l1 );
	sb.addPage( "A Button", new QPushButton("Hello") );
	sb.addPage( "Another Button", new QPushButton("Yep"));

\endcode
*/


/*!
 * \brief Default constructor
 *
 */
FCCmdBar::FCCmdBar( QWidget *parent, const char *name )
	: FCDockWindow( parent, name )
{
    m_pCurPage  = NULL;
    m_pLastPage = NULL,
    m_pLayout   = new QVBoxLayout( this );
    m_lAnimCount = 10;
}

/*!
 * \brief Default structor
 *
 * Destroys the list of QStackBarBtns
*/
FCCmdBar::~FCCmdBar()
{
  m_lButtons.clear();
}

/*!
 * \brief Add a new page to the QStackBar
 *
 * Call this to add a new page to the widget.
*/
void FCCmdBar::addPage( const QString &name, QWidget *page )
{
  if (!page)
	{
		fprintf(stderr, "Cannot set a null widget\n");
    return; // no valid object	
  }

  if (m_lButtons.size() == 0)
    show(); //show again

  page->setBackgroundMode( PaletteBackground );
  QStackBarBtn *button = new QStackBarBtn( this, name.latin1() );
  button->setWidget(page);
  m_lButtons.push_back( button );
  button->setText( name );
  button->setFixedHeight( button->sizeHint().height() );
  connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
  QScrollView *sv = new QScrollView( this );
  sv->setHScrollBarMode(QScrollView::AlwaysOff);
  sv->setResizePolicy( QScrollView::AutoOneFit );
  sv->addChild( page );
  sv->setFrameStyle( QFrame::NoFrame );
  sv->verticalScrollBar()->setStyle(new FCWindowsStyle);
  page->show();
  m_mViews[button] = sv;
  m_pLayout->addWidget( button );
  m_pLayout->addWidget( sv );
  button->show();

  if ( m_mViews.size() == 1 ) 
  {
  	m_pCurPage  = sv;
	  m_pLastPage = button;
	  m_pLastPage->setSelected( true );
	  sv->show();
	  set_background_mode( m_pCurPage, PaletteLight );
  } 
  else 
  {
  	sv->hide();
  }
  
  updatePages();
  connect(sv->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(repaint()));
}

void FCCmdBar::buttonClicked()
{
  QStackBarBtn *tb = (QStackBarBtn*)sender();
  QWidget *page = NULL;
  if (m_mViews.find( tb ) != m_mViews.end())
    page = m_mViews[tb];

  if ( !page || m_pCurPage == page )
  	return;
  
  tb->setSelected( true );
  
  if ( m_pLastPage )
	  m_pLastPage->setSelected( false );
    
  m_pLastPage = tb;

  if ( m_pCurPage )
	  m_pCurPage->hide();
    
  m_pCurPage = page;
  m_pCurPage->show();
  set_background_mode( m_pCurPage, PaletteLight );
  updatePages();
}

/*!
 * \brief Updates the QStackBar
 *
 * Duh
*/
void FCCmdBar::updatePages()
{
  bool after = false;
  for (std::list<QStackBarBtn*>::iterator it = m_lButtons.begin(); it != m_lButtons.end(); ++it)
  {
    (*it)->setBackgroundMode( !after ? PaletteBackground : PaletteLight );
    (*it)->update();
    after = (*it) == m_pLastPage;
  }
}

void FCCmdBar::timerEvent ( QTimerEvent * )
{
}

/*!
 * \brief Checks for widget 
 *
 * Checks if there is a widget with the name 'sName' contained by the widget list
*/
bool FCCmdBar::HasView(const char *sName)
{
  for (std::list<QStackBarBtn*>::iterator it = m_lButtons.begin(); it != m_lButtons.end(); ++it)
  {
    QStackBarBtn* b = (*it);
    if (b->text() == sName)
    {
      return true;
    }
  }

  return false;
}

FCToolboxGroup* FCCmdBar::GetView(const char *sName)
{
  for (std::list<QStackBarBtn*>::iterator it = m_lButtons.begin(); it != m_lButtons.end(); ++it)
  {
    QStackBarBtn* b = (*it);
    if (b->text() == sName)
    {
      return (FCToolboxGroup*)b->widget();
    }
  }

#ifdef _DEBUG
    printf("Widget %s not found\n", sName);
#endif

  return NULL;
}

FCToolboxGroup* FCCmdBar::CreateView(const char *sName)
{
  FCToolboxGroup* bg = new FCToolboxGroup("", this);
  addPage( sName, bg );
  return bg;
}

void FCCmdBar::DeleteView(const char *sName)
{
  for (std::list<QStackBarBtn*>::iterator it = m_lButtons.begin(); it != m_lButtons.end(); ++it)
  {
    if ((*it)->text() == sName)
    {
      remPage(*it);
      return;
    }
  }

#ifdef _DEBUG
    printf("Widget %s not found\n", sName);
#endif
}

/*!
 * \brief Sets the current page to i
 *
 * Call this to set the current page.  The widget will refresh and
 * rearrange itself.
 */
void FCCmdBar::setCurPage( int i )
{
	if( i >= (int)m_lButtons.size() || i < 0 )
		return;

  std::list<QStackBarBtn*>::iterator it = m_lButtons.begin();
  for (; i > 0; i--) it++;
	QStackBarBtn *b = (*it);

  if (b)
  {
    b->animateClick();
    set_background_mode( m_pCurPage, PaletteLight );
  }
}

/*!
 * \brief Remove a page from the QStackBar
 *
 * Call this to remove a page from the widget.
*/
void FCCmdBar::remPage( QStackBarBtn * b)
{
	m_lButtons.remove( b );
  delete b;
  if (m_lButtons.size() == 0)
    hide();
}

#include "moc_ButtonGroup.cpp"
