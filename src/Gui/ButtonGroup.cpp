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




#include "PreCompiled.h"

#ifndef _PreComp_
# include <qpainter.h>
# include <qstyle.h>
# include <qapplication.h>
# include <qcolordialog.h>
#endif

#include <qobjectlist.h>
#include "ButtonGroup.h"
#include "Application.h"
#include "DlgCustomizeImp.h"
#include "Command.h"
#include "../Base/Exception.h"


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
      m_Popup->insertItem(tr("Show buttons as icon"), this, SLOT(showIcons()));
    else
      m_Popup->insertItem(tr("Show buttons as text"), this, SLOT(showText()));
    m_Popup->insertSeparator();
  }

  int colId = m_Popup->insertItem(tr("Background color..."), this, SLOT(setNewBackgroundColor()));
  int resId = m_Popup->insertItem(tr("Reset background color"), this, SLOT(resetBackgroundColor()));
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

FCCommandBar::FCCommandBar ( const QString & label, QWidget *w, const char * name, WFlags f )
: FCToolBar(label, ApplicationWindow::Instance, w, false, name, f, "Cmdbar")
{
  // remove immediately from main window again
  ApplicationWindow::Instance->removeToolBar(this);
#if QT_VERSION >= 300
	setFrameStyle( QFrame::NoFrame );
#endif
	setOrientation( Qt::Vertical );

  m_Color = backgroundColor();
  m_Popup = new QPopupMenu(0L);
  m_Popup->setCheckable(true);
  connect(m_Popup, SIGNAL(aboutToShow()), this, SLOT(popupMenuAboutToShow()));
  setStretchableWidget( ( m_Dummy = new QWidget( this ) ) );
  bSaveColor = true;
}

FCCommandBar::~FCCommandBar ()
{
  delete m_Popup;
  delete m_Dummy;
}

void FCCommandBar::clearAll()
{
  FCToolBar::clearAll();
  m_Dummy = NULL;
}

void FCCommandBar::addedButton(QString text)
{
#if QT_VERSION > 230
  delete m_Dummy;
  m_Dummy = new QWidget(this);
  setStretchableWidget( m_Dummy );
#else
  if (!m_Dummy)
    m_Dummy = new QWidget(this);
  else
    m_Dummy->reparent(this, QPoint(0,0));
#endif
}

void FCCommandBar::mousePressEvent( QMouseEvent * e )
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

void FCCommandBar::cleanupEventFilter()
{
}

void FCCommandBar::popupMenuAboutToShow()
{
  m_Popup->clear();

  int colId = m_Popup->insertItem(tr("Background color..."), this, SLOT(setNewBackgroundColor()));
  int resId = m_Popup->insertItem(tr("Reset background color"), this, SLOT(resetBackgroundColor()));
  m_Popup->insertSeparator();
  ApplicationWindow::Instance->GetCommandManager().AddTo("Std_DlgCustomize", m_Popup);
}

void FCCommandBar::setNewBackgroundColor()
{
  QColor color = QColorDialog::getColor ( backgroundColor(), this);
  if (color.isValid())
  {
    setPalette(QPalette(color, color));
  }
}

void FCCommandBar::resetBackgroundColor()
{
  if (m_Color.isValid())
    setPalette(QPalette(m_Color, m_Color));
}

/////////////////////////////////////////////////////////////////////////////////////

FCToolboxBar::FCToolboxBar ( const QString & label, QWidget *w, const char * name, WFlags f )
: FCCommandBar(label, w, name, f)
{
}

FCToolboxBar::~FCToolboxBar ()
{
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
      ((QToolButton*)o)->setMaximumHeight(42);
    }
  }

  FCCommandBar::addedButton(text);
}

/////////////////////////////////////////////////////////////////////////////////////

FCOutlookBar::FCOutlookBar ( const QString & label, QWidget *w, const char * name, WFlags f )
: FCCommandBar(label, w, name, f)
{
}

FCOutlookBar::~FCOutlookBar ()
{
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

  FCCommandBar::addedButton(text);
}

/////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Constructor
 *
 * This constructor allows you to set the name of the button, and tell it
 * what widget it shall contain.  It also sets the default colors.
 */
QStackBarBtn::QStackBarBtn( QWidget *object, FCStackBar *parent, const char *name ) 
 : QToolButton( parent, name ), bIsSelected( false ), w(object), pStackBar(parent)
{
  setBackgroundMode( PaletteLight ); 
}

/*!
 * \brief Another version of the constructor
 *
 * This constructor differs from the above one only in that you cannot
 * specify what widget the button shall contain.
 */
QStackBarBtn::QStackBarBtn( FCStackBar *parent, const char *name ) 
 : QToolButton( parent, name ), bIsSelected( false ), pStackBar(parent)
{
  setBackgroundMode( PaletteLight ); 
}

/*!
 * \brief destructor
 *
 */
QStackBarBtn::~QStackBarBtn()
{
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

bool QStackBarBtn::isSelected ()
{
  return bIsSelected;
}

/*!
 * \brief Sets the widget this button/page will contain
 *
 * Sets the widget for this button/page to contain
 */
void QStackBarBtn::setWidget(QWidget* widget) 
{ 
  w = widget;
  if (w)
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

void QStackBarBtn::mousePressEvent ( QMouseEvent * e )
{
  if (e->button() == RightButton)
  {
    QPoint p = ((QMouseEvent*)e)->globalPos();
    QPopupMenu menu;
    menu.setCheckable(true);

    std::map<int, QToolBar*> toolb;

    std::vector<FCToolBar*> aclToolBars = ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBars();
	  for (std::vector<FCToolBar*>::iterator It = aclToolBars.begin(); It != aclToolBars.end(); ++It)
    {
      int id = menu.insertItem((*It)->name());
      QToolBar* tb = *It;
      toolb[id] = tb;
      if (pStackBar->isPageVisible(tb))
		    menu.setItemChecked(id, true);
    }

    int id = menu.exec(p);
    if (toolb.find(id) != toolb.end()) 
    {
      QToolBar* tb = toolb[id];
      if (menu.isItemChecked(id))
      {
        pStackBar->hidePage(tb);
      }
      else 
      {
        pStackBar->showPage(tb);
    	}
    }
  }
  else
    QToolButton::mousePressEvent(e);
}

/////////////////////////////////////////////////////////////////////////////////////

/*!
  \class FCStackBar
  \brief Implements a button-bar similar to that of MS Outlook

  Use this widget to implement multiple pages of widgets in a single vertical
  column.  
  
  The following code demonstrates how to create a stackbar, and add a few pages
  to it with some widgets.
  
  For more examples, see the tests in the test directory of the distribution.
  
\code

	FCStackBar *sb;
	QListBox *l1;
	
	sb = new FCStackBar(pParent, NULL);
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
FCStackBar::FCStackBar( QWidget *parent, const char *name )
	: FCDockWindow( parent, name )
{
  m_pCurPage  = NULL;
  m_pLastBtn = NULL,
  m_pLayout   = new QVBoxLayout( this );
  m_lAnimCount = 0;
  m_bAnimated = false;

  try
  {
    // attach the command bar to its preferences
    const char* strGroupPath = "User parameter:BaseApp/Windows/CmdBar";
    GetApplication().GetParameterGroupByPath(strGroupPath)->Attach(this);
    GetApplication().GetParameterGroupByPath(strGroupPath)->Notify("SpeedAnimationCmdBar");
  }
  catch(const FCException& rclE)
  {
    QMessageBox::warning(ApplicationWindow::Instance, "Wrong parameter", rclE.what());
  }
}

/*!
 * \brief Default structor
 *
 * Destroys the list of QStackBarBtns
*/
FCStackBar::~FCStackBar()
{
  // detach the command bar
  const char* strGroupPath = "User parameter:BaseApp/Windows/CmdBar";
  GetApplication().GetParameterGroupByPath(strGroupPath)->Detach(this);
  m_mButtonView.clear();
  delete m_pLayout;
}

/*!
 * \brief Add a new page to the FCStackBar
 *
 * Call this to add a new page to the widget.
*/
bool FCStackBar::addView(QWidget* page, const QString &name)
{
  if (!page)
	{
		fprintf(stderr, "Cannot set a null widget\n");
    return false; // no valid object	
  }

  if (m_mButtonView.size() == 0)
    show(); //show again

  page->setBackgroundMode( PaletteBackground );
  QStackBarBtn *button = new QStackBarBtn( this, name.latin1() );
  button->setWidget(page);
  button->setText( name );
  button->setFixedHeight( button->sizeHint().height() );
  connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
  QScrollView *sv = new QScrollView( this );
  sv->setHScrollBarMode(QScrollView::AlwaysOff);
  sv->setVScrollBarMode(QScrollView::AlwaysOff);
  sv->setResizePolicy( QScrollView::AutoOneFit );
  sv->addChild( page );
  sv->setFrameStyle( QFrame::NoFrame );
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
  } 
  else 
  {
  	sv->hide();
  }
  
  updatePages();
  connect(sv->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(repaint()));

  return true;
}


/*!
 * \brief Checks for widget 
 *
 * Checks if there is a widget with the name 'sName' contained by the widget list
*/
bool FCStackBar::hasView(QWidget* w)
{
  for (std::map <QStackBarBtn*,QScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    QStackBarBtn* b = (it)->first;
    if (b->widget() == w)
    {
      return true;
    }
  }

  return false;
}

/*!
 * \brief Remove a page from the FCStackBar
 *
 * Call this to remove a page from the widget.
*/
bool FCStackBar::remView(QWidget* w)
{
  bool bSucceed = false;
  for (std::map <QStackBarBtn*,QScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    if (it->first->widget() == w)
    {
      bSucceed = true;
      QStackBarBtn* bt = it->first;
      if (bt == m_pLastBtn)
        m_pLastBtn = NULL;
      QScrollView* sv = it->second;
      if (sv == m_pCurPage)
        m_pCurPage = NULL;
      sv->removeChild(w);
      m_mButtonView.erase(it);
      delete bt;
      delete sv;
      break;
    }
  }

  if (m_mButtonView.size() == 0)
    hide();

  return bSucceed;
}

/*!
 * \brief Sets the current page to i
 *
 * Call this to set the current page.  The widget will refresh and
 * rearrange itself.
 */
bool FCStackBar::showView(QWidget* w)
{
  bool bSucced = false;
  for (std::map <QStackBarBtn*,QScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    if (it->first->widget() == w)
    {
  	  m_pLastBtn = it->first;
      m_pLastBtn->setSelected(true);
  	  m_pCurPage  = it->second;
	    m_pCurPage->show();
      bSucced = true;
    }
    else
    {
      it->first->setSelected(false);
      it->second->hide();
    }
  }

  return bSucced;
}

QWidget* FCStackBar::showedView()
{
  for (std::map <QStackBarBtn*,QScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    if (it->first->isSelected())
    {
      if (it->first->widget() != NULL)
      {
        return it->first->widget();
      }
    }
  }

  return NULL;
}

void FCStackBar::showPage(QWidget* w)
{
  for (std::map <QStackBarBtn*,QScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    if (it->first->widget() == w)
    {
      QStackBarBtn* page = firstPageVisible();
      it->first->show();
      if (!page)
      {
        it->first->setSelected(true);
        m_pLastBtn = it->first;
        it->second->show();
        m_pCurPage = it->second;
      }
    }
  }
}

void FCStackBar::hidePage(QWidget* w)
{
  for (std::map <QStackBarBtn*,QScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    if (it->first->widget() == w)
    {
      it->first->hide();
      if (it->first->isSelected())
      {
        it->first->setSelected(false);
        QStackBarBtn* page = firstPageVisible();
        if (page)
          page->animateClick();
        else
          it->second->hide();
      }
    }
  }
}

bool FCStackBar::isPageVisible(QWidget* w)
{
  for (std::map <QStackBarBtn*,QScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    if (it->first->widget() == w)
    {
      return it->first->isVisible();
    }
  }

  return false;
}

QStackBarBtn* FCStackBar::firstPageVisible()
{
  for (std::map <QStackBarBtn*,QScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    if (it->first->isVisible())
    {
      return it->first;
    }
  }

  return NULL;
}

void FCStackBar::buttonClicked()
{
  QStackBarBtn *tb = (QStackBarBtn*)sender();
  QScrollView *page = NULL;
  if (m_mButtonView.find( tb ) != m_mButtonView.end())
    page = m_mButtonView[tb];

  if ( !page || m_pCurPage == page || m_bAnimated == true)
  	return;
  
  tb->setSelected( true );
  
  if ( m_pLastBtn )
	  m_pLastBtn->setSelected( false );
    
  m_pLastBtn = tb;

  if ( m_pCurPage )
	  m_pCurPage->hide();
  
  if (m_pCurPage != page)
  {
    if (m_pCurPage && page && m_lAnimCount > 0)
    {
      animatePageScroll(m_pCurPage, page);
    }
    else
    {
      m_pCurPage = page;
      m_pCurPage->show();
      updatePages();
    }
  }
}

/*!
 * \brief Updates the FCStackBar
 *
 * Duh
*/
void FCStackBar::updatePages()
{
  bool after = false;
  for (std::map <QStackBarBtn*,QScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    (it)->first->setBackgroundMode( /*!after ? */PaletteBackground /*: PaletteLight*/ );
    (it)->first->update();
    after = (it->first) == m_pLastBtn;
  }
}

void FCStackBar::timerEvent ( QTimerEvent * )
{
  if (m_pAnimCurPage->height() > m_lAnimCount)
  {
    m_pAnimCurPage->setFixedHeight(m_pAnimCurPage->height() - m_lAnimCount);
    m_pAnimNewPage->setFixedHeight(m_pAnimNewPage->height() + m_lAnimCount);
  }
  else
  {
    m_bAnimated = false;
    killTimers();
    m_pAnimCurPage->setMaximumHeight(1000);
    m_pAnimCurPage->setMinimumHeight(0);
    m_pAnimNewPage->setMaximumHeight(1000);
    m_pAnimNewPage->setMinimumHeight(0);
    m_pAnimCurPage->setVScrollBarMode(QScrollView::Auto);
    m_pAnimNewPage->setVScrollBarMode(QScrollView::Auto);
    m_pAnimCurPage->hide();
    m_pAnimNewPage->show();
    m_pCurPage = m_pAnimNewPage;
    updatePages();
  }
}

void FCStackBar::animatePageScroll(QScrollView* pCurPage, QScrollView* pNewPage)
{
  if (!pCurPage || !pNewPage)
    return; // one page is invalid
  m_pAnimNewPage = pNewPage;
  m_pAnimCurPage = pCurPage;
  m_iCurHeight   = m_pAnimCurPage->height();
  m_iNewHeight   = m_pAnimNewPage->height();

  m_pAnimCurPage->setVScrollBarMode(QScrollView::AlwaysOff);
  m_pAnimNewPage->setVScrollBarMode(QScrollView::AlwaysOff);
  m_pAnimCurPage->show();
  m_pAnimNewPage->setFixedHeight(0);
  m_pAnimNewPage->show();

  m_bAnimated = true;
  startTimer(5);
}

void FCStackBar::OnChange(FCSubject<const char*> &rCaller,const char* sReason)
{
  FCParameterGrp& rclGrp = ((FCParameterGrp&)rCaller);
  if (strcmp(sReason, "SpeedAnimationCmdBar") == 0)
  {
    FCParameterGrp::handle hGrp = rclGrp.GetGroup("SpeedAnimationCmdBar");
    if (hGrp->GetInts("Value").size() == 0)
      hGrp->SetInt("Value", 100);
    m_lAnimCount = hGrp->GetInt("Value", 100);
    if (m_lAnimCount == hGrp->GetInt("MaxValue", 100))
      m_lAnimCount = 0;
    else if (m_lAnimCount == hGrp->GetInt("MinValue", 0))
      m_lAnimCount = 5;
  }
}

#include "moc_ButtonGroup.cpp"
