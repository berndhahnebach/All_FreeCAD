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
# include <qcolordialog.h>
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

FCToolboxBar::FCToolboxBar ( const QString & label, QWidget *w, const char * name, WFlags f )
: FCToolBar(label, ApplicationWindow::Instance, w, false, name, f)
{
#if QT_VER >= 300
	setFrameStyle( QFrame::NoFrame );
#endif
	setOrientation( Qt::Vertical );

  m_Color = backgroundColor();
  m_Popup = new QPopupMenu(0L);
  m_Popup->setCheckable(true);
  connect(m_Popup, SIGNAL(aboutToShow()), this, SLOT(popupMenuAboutToShow()));
  setStretchableWidget( ( m_Dummy = new QWidget( this ) ) );
}

FCToolboxBar::~FCToolboxBar ()
{
//  savePreferences();
  delete m_Popup;
  delete m_Dummy;
}

void FCToolboxBar::addedButton(QString text)
{
  const QObjectList *l = children();
  QObjectListIt it(*l);
  QObject* o = it.toLast();
  if (o)
  {
    if (o->inherits("QToolButton"))
    {
      ((QToolButton*)o)->setTextLabel(text);
      ((QToolButton*)o)->setUsesTextLabel(true);
    }
  }

  m_Dummy->reparent(this, QPoint(0,0));
}

void FCToolboxBar::restorePreferences()
{
  FCParameterGrp::handle hPGrp = hPrefGrp->GetGroup("Commandbar");
  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  std::map<std::string,FCCommand*> sCommands = cCmdMgr.GetCommands();

  std::vector<std::string> items = hPGrp->GetASCIIs(getPrefName().latin1());
  for (std::vector<std::string>::iterator it = items.begin(); it != items.end(); ++it)
  {
    sCommands[*it]->GetAction()->addTo(this);
  }

  int r = hPGrp->GetInt("red", 255);
  int g = hPGrp->GetInt("green", 255);
  int b = hPGrp->GetInt("blue", 255);
  QColor color(r, g, b);
//  if (color.isValid())
//  {
//    setPalette(QPalette(color, color));
//    setBackgroundMode(PaletteLight);
//  }
}

void FCToolboxBar::savePreferences()
{
  int i=0;
  FCParameterGrp::handle hPGrp = hPrefGrp->GetGroup("Commandbar");
  for (std::vector<std::string>::iterator it = alDroppedActions.begin(); it != alDroppedActions.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "%s%d", getPrefName().latin1(), i);
    hPGrp->SetASCII(szBuf, it->c_str());
  }

  hPGrp->SetInt("red", backgroundColor().red());
  hPGrp->SetInt("green", backgroundColor().green());
  hPGrp->SetInt("blue", backgroundColor().blue());
}

void FCToolboxBar::mousePressEvent( QMouseEvent * e )
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

void FCToolboxBar::cleanupEventFilter()
{
}

void FCToolboxBar::popupMenuAboutToShow()
{
  m_Popup->clear();

  int colId = m_Popup->insertItem("Background color...", this, SLOT(setNewBackgroundColor()));
  int resId = m_Popup->insertItem("Reset background color", this, SLOT(resetBackgroundColor()));
  m_Popup->insertSeparator();
  ApplicationWindow::Instance->GetCommandManager().AddTo("Std_DlgCustomize", m_Popup);
}

void FCToolboxBar::setNewBackgroundColor()
{
  QColor color = QColorDialog::getColor ( backgroundColor(), this);
  if (color.isValid())
  {
    setPalette(QPalette(color, color));
  }
}

void FCToolboxBar::resetBackgroundColor()
{
  if (m_Color.isValid())
    setPalette(QPalette(m_Color, m_Color));
}

/////////////////////////////////////////////////////////////////////////////////////

FCOutlookBar::FCOutlookBar ( const QString & label, QWidget *w, const char * name, WFlags f )
: FCToolBar(label, ApplicationWindow::Instance, w, false, name, f)
{
#if QT_VER >= 300
	setFrameStyle( QFrame::NoFrame );
#endif
	setOrientation( Qt::Vertical );

  m_Color = backgroundColor();
  m_Popup = new QPopupMenu(0L);
  m_Popup->setCheckable(true);
  connect(m_Popup, SIGNAL(aboutToShow()), this, SLOT(popupMenuAboutToShow()));
  setStretchableWidget( ( m_Dummy = new QWidget( this ) ) );
}

FCOutlookBar::~FCOutlookBar ()
{
//  savePreferences();
  delete m_Popup;
  delete m_Dummy;
}

void FCOutlookBar::addedButton(QString text)
{
  const QObjectList *l = children();
  QObjectListIt it(*l);
  QObject* o = it.toLast();
  if (o)
  {
    if (o->inherits("QToolButton"))
    {
      QLabel* label = new QLabel(this);
      label->setText(text);
      label->setAlignment(AlignHCenter);
      ((QWidget*)o)->setFixedSize(48,48);
    }
  }

  m_Dummy->reparent(this, QPoint(0,0));
}

void FCOutlookBar::restorePreferences()
{
  FCParameterGrp::handle hPGrp = hPrefGrp->GetGroup("Commandbar");
  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  std::map<std::string,FCCommand*> sCommands = cCmdMgr.GetCommands();

  std::vector<std::string> items = hPGrp->GetASCIIs(getPrefName().latin1());
  for (std::vector<std::string>::iterator it = items.begin(); it != items.end(); ++it)
  {
    sCommands[*it]->GetAction()->addTo(this);
  }

  int r = hPGrp->GetInt("red", 255);
  int g = hPGrp->GetInt("green", 255);
  int b = hPGrp->GetInt("blue", 255);
  QColor color(r, g, b);
//  if (color.isValid())
//  {
//    setPalette(QPalette(color, color));
//    setBackgroundMode(PaletteLight);
//  }
}

void FCOutlookBar::savePreferences()
{
  int i=0;
  FCParameterGrp::handle hPGrp = hPrefGrp->GetGroup("Commandbar");
  for (std::vector<std::string>::iterator it = alDroppedActions.begin(); it != alDroppedActions.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "%s%d", getPrefName().latin1(), i);
    hPGrp->SetASCII(szBuf, it->c_str());
  }

  hPGrp->SetInt("red", backgroundColor().red());
  hPGrp->SetInt("green", backgroundColor().green());
  hPGrp->SetInt("blue", backgroundColor().blue());
}

void FCOutlookBar::mousePressEvent( QMouseEvent * e )
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

void FCOutlookBar::cleanupEventFilter()
{
}

void FCOutlookBar::popupMenuAboutToShow()
{
  m_Popup->clear();

  int colId = m_Popup->insertItem("Background color...", this, SLOT(setNewBackgroundColor()));
  int resId = m_Popup->insertItem("Reset background color", this, SLOT(resetBackgroundColor()));
  m_Popup->insertSeparator();
  ApplicationWindow::Instance->GetCommandManager().AddTo("Std_DlgCustomize", m_Popup);
}

void FCOutlookBar::setNewBackgroundColor()
{
  QColor color = QColorDialog::getColor ( backgroundColor(), this);
  if (color.isValid())
  {
    setPalette(QPalette(color, color));
  }
}

void FCOutlookBar::resetBackgroundColor()
{
  if (m_Color.isValid())
    setPalette(QPalette(m_Color, m_Color));
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
//  QObjectList *l = top->queryList( "QWidget" );
//  l->append( top );
//  for ( QObject *o = l->first(); o; o = l->next() )
//  	( (QWidget*)o )->setBackgroundMode( bm );
//  delete l;
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
    m_pLastBtn = NULL,
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
  m_mButtonView.clear();
  delete m_pLayout;
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
  m_mButtonView[button] = sv;
  m_pLayout->addWidget( button );
  m_pLayout->addWidget( sv );
  button->show();

  if ( m_mButtonView.size() == 1 ) 
  {
  	m_pCurPage  = sv;
	  m_pLastBtn = button;
	  m_pLastBtn->setSelected( true );
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
  if (m_mButtonView.find( tb ) != m_mButtonView.end())
    page = m_mButtonView[tb];

  if ( !page || m_pCurPage == page )
  	return;
  
  tb->setSelected( true );
  
  if ( m_pLastBtn )
	  m_pLastBtn->setSelected( false );
    
  m_pLastBtn = tb;

  if ( m_pCurPage )
	  m_pCurPage->hide();
  
  if (m_pCurPage != page)
  {
    if (m_lAnimCount > 0)
    {
      animatePageScroll(m_pCurPage, page);
      return;
    }
  }

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
    after = (*it) == m_pLastBtn;
  }
}

void FCCmdBar::timerEvent ( QTimerEvent * )
{
  if (m_pAnimCurPage->height() > 10)
  {
    m_pAnimCurPage->setFixedHeight(m_pAnimCurPage->height() - 10);
    m_pAnimNewPage->setFixedHeight(m_pAnimNewPage->height() + 10);
  }
  else
  {
    killTimers();
    m_pAnimCurPage->setMaximumHeight(1000);
    m_pAnimCurPage->setMinimumHeight(0);
    m_pAnimNewPage->setMaximumHeight(1000);
    m_pAnimNewPage->setMinimumHeight(0);
    ((QScrollView*)m_pAnimCurPage)->setVScrollBarMode(QScrollView::Auto);
    ((QScrollView*)m_pAnimNewPage)->setVScrollBarMode(QScrollView::Auto);
    m_pAnimCurPage->hide();
    m_pAnimNewPage->show();
    m_pCurPage = m_pAnimNewPage;
    set_background_mode( m_pCurPage, PaletteLight );
    updatePages();
  }
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

FCToolBar* FCCmdBar::GetView(const char *sName)
{
  for (std::list<QStackBarBtn*>::iterator it = m_lButtons.begin(); it != m_lButtons.end(); ++it)
  {
    QStackBarBtn* b = (*it);
    if (b->text() == sName)
    {
      return (FCToolBar*)b->widget();
    }
  }

#ifdef FC_DEBUG
    printf("Widget %s not found\n", sName);
#endif

  return NULL;
}

FCToolBar* FCCmdBar::CreateView(const char *sName, BarMode mode)
{
  FCToolBar* bg;

  switch (mode)
  {
  case TOOLBOX:
    bg = new FCToolboxBar(sName, this, sName);
    break;
  case OUTLOOK:
    bg = new FCOutlookBar(sName, this, sName);
    break;
  };
  
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

#ifdef FC_DEBUG
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
  std::map <QWidget*, QWidget*>::iterator it;

  // delete also the corresponding scroll view
  if ( (it=m_mButtonView.find(b)) != m_mButtonView.end())
  {
    QWidget* w = m_mButtonView[b];
    m_mButtonView.erase(it);
    delete w;
  }

  delete b;
  if (m_lButtons.size() == 0)
    hide();
}

void FCCmdBar::animatePageScroll(QWidget* pCurPage, QWidget* pNewPage)
{
  m_pAnimNewPage = pNewPage;
  m_pAnimCurPage = pCurPage;
  m_iCurHeight   = m_pAnimCurPage->height();
  m_iNewHeight   = m_pAnimNewPage->height();

  ((QScrollView*)m_pAnimCurPage)->setVScrollBarMode(QScrollView::AlwaysOff);
  ((QScrollView*)m_pAnimNewPage)->setVScrollBarMode(QScrollView::AlwaysOff);
  m_pAnimCurPage->show();
  m_pAnimNewPage->setFixedHeight(0);
  m_pAnimNewPage->show();

  startTimer(m_lAnimCount);
}

#include "moc_ButtonGroup.cpp"
