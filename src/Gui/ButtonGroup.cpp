/***************************************************************************
                          ButtonGroup.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief The button group module
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




#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qlayout.h>
#	include <qevent.h>
#	include <qdragobject.h>
#	include <qlineedit.h>
#	include <qvariant.h>
#	include <qcolordialog.h>
#endif



#include "buttongroup.h"
#include "qsizepolicy.h"


FCButtonGroup::FCButtonGroup(QWidget * parent, const char * name)
: QButtonGroup(parent, name), m_iChildWidth(32), m_iChildHeight(32)
{
  initialize();
}

FCButtonGroup::FCButtonGroup(const QString & title, QWidget * parent, const char * name)
: QButtonGroup(title, parent, name), m_iChildWidth(32), m_iChildHeight(32)
{
  initialize();
}

FCButtonGroup::FCButtonGroup(int columns, Orientation o, QWidget * parent, const char * name)
: QButtonGroup(columns, o, parent, name), m_iChildWidth(32), m_iChildHeight(32)
{
  initialize();
}

FCButtonGroup::FCButtonGroup(int columns, Orientation o, const QString & title, QWidget * parent, const char * name)
: QButtonGroup(columns, o, title, parent, name), m_iChildWidth(32), m_iChildHeight(32)
{
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

  for (FCmap<int, QPixmap>::const_iterator it = m_Pixmaps.begin(); it != m_Pixmaps.end(); it++)
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


/*!
  \class QStackBar qstackbar.h
  \brief Implements a button-bar similar to that of MS Outlook

  Use this widget to implement multiple pages of widgets in a single vertical
  column.  Each button can have its own unselected, selected, and highlighted
  color, text color, and text font.  They can also be given a pixmap background
  instead (which will not highlight when you put the mouse over it).
  
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
	
	sb.addPage( new QStackBarBtn( "List Box 1", l1 ) );
	sb.addPage( new QStackBarBtn( "A Button", new QPushButton("Hello") ) );
	sb.addPage( new QStackBarBtn( "Another Button", new QPushButton("Yep")));

\endcode
*/


/*!
 * \brief Default constructor
 *
 * Initializes the lists for holding the page-buttons, sets the current page
 * to -1, turns mouse tracking on, sets the stackHeight to the default of
 * 18 pixels.
 */
FCCmdBar::FCCmdBar( QWidget *parent, const char *name )
	: FCDockWindow( parent, name )
{
	pButtons = new QList<QStackBarBtn>;
	pButtons->setAutoDelete(true);

	curPage = -1;
	
	setMouseTracking( true );

	curHighlight = -1;
	_stackHeight = 18;

}


/*!
 * \brief Default structor
 *
 * Destroys the list of QStackBarBtns
*/
FCCmdBar::~FCCmdBar()
{
	delete pButtons;
}


/*!
 * \brief Handles window resizes
 *
 * Forces the buttons to rearrange themselves inside the widget. It
 * also resizes the active page widget.
*/
void FCCmdBar::resizeEvent( QResizeEvent *e )
{
	rearrangeButtons(e->size().width(), e->size().height() );
}


/*!
 * \brief Rearranges the buttons to match the dimensions w,h
 *
 * This is called internally by the widget to rearrange the buttons
 * associated with the pages.
*/
void FCCmdBar::rearrangeButtons( int w, int h )
{
	int topY;
	int bottomY;
	int count = pButtons->count()-1;
	QWidget *pWidget;
	QStackBarBtn *pBtn;

	topY = (curPage+1) * _stackHeight;
	bottomY = h - ((count - curPage)*_stackHeight);
	
	pBtn = pButtons->at( curPage );
	pWidget = pBtn->widget();
	pWidget->setGeometry( 0, topY, w, bottomY-topY );
}


/*!
 * \brief Paints the QStackBar
 *
 * Duh
*/
void FCCmdBar::paintEvent( QPaintEvent * )
{
	QPainter *pPaint;
	QBrush brush;
	QStackBarBtn *pBtn;

	pPaint = new QPainter(this);

	brush.setStyle( QBrush::SolidPattern );

	
	int i;

	// Draw the buttons before the current button
	pBtn = NULL;
	for( i=0; i<curPage; i++ )
	{
		// Get the QStackBarBtn
		if( !pBtn )
			pBtn = pButtons->first();
		else
			pBtn = pButtons->next();

		brush.setColor( pBtn->color()->base() );
		if( pBtn->pixmap() )
		{
			printf("Pixmap\n");
			brush.setPixmap( *(pBtn->pixmap()) );
		}
		else
			brush.setStyle( QBrush::SolidPattern );

		
		pPaint->setPen( pBtn->fontColor() );
	
		qDrawWinButton( pPaint, 0, i*_stackHeight, width(), _stackHeight, *(pBtn->color()), false, &brush  );

		pPaint->setFont( *(pBtn->font()) );
		pPaint->drawText( 0, i*_stackHeight-1, width(), _stackHeight, AlignHCenter | AlignVCenter, pBtn->label() );
	}


	// Draw the current button
	if( !pBtn )
		pBtn = pButtons->first();
	else
		pBtn = pButtons->next();

	brush.setColor( pBtn->selColor()->base() );
	if( pBtn->pixmap() )
		brush.setPixmap( *(pBtn->pixmap()) );
	else
		brush.setStyle( QBrush::SolidPattern );

	qDrawWinButton( pPaint, 0, i*_stackHeight, width(), _stackHeight, *(pBtn->selColor()), false, &brush  );
	pPaint->setFont( *(pBtn->selFont()) );
	pPaint->setPen( pBtn->fontSelColor());
	pPaint->drawText( 0, i*_stackHeight-1, width(), _stackHeight, AlignHCenter | AlignVCenter, pBtn->label() );


	// Draw the bottom buttons
	pBtn = NULL;
	for( i=pButtons->count()-1; i>curPage; i-- )
	{
		// Get the QStackBarBtn
		if( !pBtn )
			pBtn = pButtons->last();
		else
			pBtn = pButtons->prev();
			
		brush.setColor( pBtn->color()->base() );
		if( pBtn->pixmap() )
			brush.setPixmap( *(pBtn->pixmap()) );
		else
			brush.setStyle( QBrush::SolidPattern );


		qDrawWinButton( pPaint, 0, height()-((pButtons->count()-i)*_stackHeight), width(), _stackHeight, *(pBtn->color()), false, &brush );

		pPaint->setFont( *(pBtn->font()) );
		pPaint->setPen( pBtn->fontColor());

		pPaint->drawText( 0, height()-((pButtons->count()-i)*_stackHeight)-1, width(), _stackHeight, AlignHCenter | AlignVCenter, pBtn->label() );
	}

	delete pPaint;
}


/*!
 * \brief Add a new page to the QStackBar
 *
 * Call this to add a new page to the widget.
*/
void FCCmdBar::addPage( QStackBarBtn *pBtn )
{
	pButtons->append( pBtn );
	curPage = pButtons->count() - 1;
}


/*!
 * \brief Sets the current page to i
 *
 * Call this to set the current page.  The widget will refresh and
 * rearrange itself.
 */
void FCCmdBar::setCurPage( int i )
{
	if( i >= (int)pButtons->count() || i < 0 )
		return;

	QStackBarBtn *pBtn;

	if( curPage >= 0 )
	{
		pBtn = pButtons->at( curPage );
		pBtn->widget()->hide();
	}
	
	curPage = i;
	pBtn = pButtons->at( curPage );
	pBtn->widget()->show();


	rearrangeButtons( width(), height() );
	repaint(false);
}


/*!
 * \brief Returns the button at mx, my
 *
 * This is used internally to find out what button is located
 * at a given mouse coordinate.  -1 is returned if nothing
 * is found.
*/
int FCCmdBar::whichButton( int mx, int my )
{
	int numBottom;	// number of buttons on the bottom stack

	// quick death
	if( curPage < 0 )
		return -1;

	if( mx < 0 || mx >= width() )
		return -1;
	
	numBottom = pButtons->count()-curPage-1;
	
	if( my >=0 && my < (curPage+1)*_stackHeight )
		return my / _stackHeight;
	else if( my >= height()-(numBottom*_stackHeight) && my < height() )
		return curPage + ( (my-(height()-(numBottom*_stackHeight)))/_stackHeight) + 1;

	return -1;
}


/*!
 * \brief Mouse press handler
 *
 * Selects the page under the mouse cursor if it can, and before that
 * it emits the pageSelected signal.
*/
void FCCmdBar::mousePressEvent( QMouseEvent *e )
{
	int buttonNum = whichButton( e->x(), e->y() );

	if( buttonNum < 0 )
		return;

	emit pageSelected( buttonNum );
	setCurPage( buttonNum );
}


/*!
 * \brief Handles the movement of the mouse
 *
 * Checks if the mouse cursor is over a button, if it is, it highlights
 * it and unhighlights the old button.  Also emits the signal pageHighlighted.
*/
void FCCmdBar::mouseMoveEvent( QMouseEvent *e )
{
	int buttonNum;
	QBrush brush;
	QStackBarBtn *pBtn;
	QFont *pFont;
	QColorGroup *pCGroup;
	QColor fcolor;

	if( curPage < 0 )
		return;

	brush.setStyle( QBrush::SolidPattern );

	buttonNum = whichButton( e->x(), e->y() );
	
	// This button is already highlighted, leave
	if( buttonNum == curHighlight )
		return;

	QPainter *pPaint = new QPainter(this);
	// Otherwise, undraw the last button if we have one
	if( curHighlight >= 0 )
	{
		pBtn = pButtons->at( curHighlight );

		// If we are the current page, undraw in the selected color & font
		if( curHighlight == curPage )
		{
			brush.setColor( pBtn->selColor()->base() );
			pFont = pBtn->selFont();
			pCGroup = pBtn->selColor();
			fcolor = pBtn->fontSelColor();
		}
		else
		{
			brush.setColor( pBtn->color()->base() );
			pFont = pBtn->font();
			pCGroup = pBtn->color();
			fcolor = pBtn->fontColor();
		}

		if( pBtn->pixmap() )
			brush.setPixmap( *(pBtn->pixmap()) );
		else
			brush.setStyle( QBrush::SolidPattern );
	
		pPaint->setFont( *pFont );
	
		if( curHighlight <= curPage )
		{
			qDrawWinButton( pPaint, 0, curHighlight*_stackHeight, width(), _stackHeight, *pCGroup, false, &brush  );

			pPaint->setPen( fcolor );
			pPaint->drawText( 0, curHighlight*_stackHeight-1, width(), _stackHeight, AlignHCenter | AlignVCenter, pBtn->label() );
		}
		else
		{
			// bottom button
			qDrawWinButton( pPaint, 0, height()-((pButtons->count()-curHighlight)*_stackHeight), width(), _stackHeight, *pCGroup, false, &brush );
			pPaint->setPen( fcolor );
			pPaint->drawText( 0, height()-((pButtons->count()-curHighlight)*_stackHeight)-1, width(), _stackHeight, AlignHCenter | AlignVCenter, pBtn->label() );
		}
	}


	// And draw this onea
	curHighlight = buttonNum;
	if( curHighlight >= 0 )
	{
		pBtn = pButtons->at(curHighlight);
		brush.setColor( pBtn->hiColor()->base() );
		if( pBtn->pixmap() )
			brush.setPixmap( *(pBtn->pixmap()) );
		else
			brush.setStyle( QBrush::SolidPattern );

		pCGroup = pBtn->hiColor();

		if( curHighlight==curPage )
		{
			pPaint->setFont( *(pBtn->selFont()) );
			fcolor = pBtn->fontSelColor();
		}
		else
		{
			pPaint->setFont( *(pBtn->hiFont()) );
			fcolor = pBtn->fontHiColor();
		}

		pPaint->setPen( fcolor );
		if( curHighlight <= curPage )
		{
			qDrawWinButton( pPaint, 0, curHighlight*_stackHeight, width(), _stackHeight, *pCGroup, false, &brush  );	
			pPaint->drawText( 0, curHighlight*_stackHeight-1, width(), _stackHeight, AlignHCenter | AlignVCenter, pBtn->label() );
		}
		else
		{
			// bottom button
			qDrawWinButton( pPaint, 0, height()-((pButtons->count()-curHighlight)*_stackHeight), width(), _stackHeight, *pCGroup, false, &brush );		
			pPaint->drawText( 0, height()-((pButtons->count()-curHighlight)*_stackHeight)-1, width(), _stackHeight, AlignHCenter | AlignVCenter, pBtn->label() );
		}
	}

	emit pageHighlighted( curHighlight );

	delete pPaint;

}

/*!
 * \brief Sets the button height
 *
 * Use this to set the height of all the buttons.  All buttons
 * are the same height.
 *
 * \code
      // make the buttons 30 pixels heigh
	  pStackBar->setButtonHeight(30);
   \endcode
*/
void FCCmdBar::setButtonHeight( int i )
{
	_stackHeight = i;

	rearrangeButtons( width(), height() );
}

#include "Icons/x.xpm"

void FCCmdBar::AddTestButtons(void)
{
	FCCmdBar* stack = this;
	for (int ii = 0; ii < 4; ii++)
	{
	  char szBuf[20];
	  sprintf(szBuf, "Page No. %d", ii);
		FCButtonGroup* mle = new FCButtonGroup(3, QButtonGroup::Horizontal, "Buttons", stack);
	  stack->addPage( new QStackBarBtn( szBuf, mle ) );
		  for (int i=0; i<30;i++)
		  {
			QToolButton* b0 = new QToolButton( /*DownArrow,*/ mle, "text" );
			b0->setProperty( "pixmap", QPixmap(px) );
			b0->setAutoRaise(true);
			b0->setTextLabel("Hallo Welt", true);
			b0->setFixedSize(32, 32);
			mle->insert(b0);
		  }
	}
	stack->setCurPage(1);
}



/*!
 * \brief One version of the constructor
 *
 * This constructor allows you to set the name of the button, and tell it
 * what widget it shall contain.  It also sets the default colors to
 * 200,200,200, and the hilight color to 220,220,220.  It sets the font
 * colors to black, and the selection-font to bold.
 */
QStackBarBtn::QStackBarBtn(QString s, QWidget *w)
{
	_label = s;

	QColor base(200,200,200);

	_color = new QColorGroup( base, base, base.light(), base.dark(), base.light(), base, base, base, base );

	base.setRgb(220,220,220);
	_hiColor = new QColorGroup( base, base, base.light(), base.dark(), base.light(), base, base, base, base );
	_selColor = new QColorGroup( *_color );
	pWidget = w;

	_fcolor = _fselcolor = _fhicolor = QColor(0,0,0);

	pPixmap = NULL;

	if( !w )
	{
		fprintf(stderr, "QSTACKBAR ERROR - Cannot set a null widget\n");
		return;
	}

	_selFont.setBold(true);

	w->hide();
}

/*!
 * \brief Another version of the constructor
 *
 * This constructor differs from the above one only in that it allows you
 * to specify a color for the buttons.  It will set the base color to that
 * color, and the hilight color to a lighter version of that color.
 */
QStackBarBtn::QStackBarBtn(QString s, QWidget *w, QColor c)
{
	_label = s;
	pWidget = w;


	QColor base(c);
	_color = new QColorGroup( base, base, base.light(), base.dark(), base.light(), base, base, base, base );
	base = base.light();
	_hiColor = new QColorGroup( base, base, base.light(), base.dark(), base.light(), base, base, base, base );
	_selColor = new QColorGroup( *_color );

	if( !w )
	{
		fprintf(stderr, "QSTACKBAR ERROR - Cannot set a null widget\n");
		return;
	}

	_fcolor = _fselcolor = _fhicolor = QColor(0,0,0);

	_selFont.setBold(true);

	pPixmap = NULL;

	w->hide();
}

/*!
 * \brief Sets the widget this button/page will contain
 *
 * Sets the widget for this button/page to contain
 */
void QStackBarBtn::setWidget( QWidget *w )
{
	if( !w )
	{
		fprintf(stderr, "QSTACKBAR ERROR - Cannot set a null widget\n");
		return;
	}

	pWidget = w;
	w->hide();
}

/*!
 * \brief Destructor
 *
 * Deletes the widget, and the color groups
 */
QStackBarBtn::~QStackBarBtn()
{
	delete pWidget;
	delete _color;
	delete _hiColor;
	delete _selColor;

	if( pPixmap )
		delete pPixmap;
}

/*!
 * \brief Set the base color of the button
 */
void QStackBarBtn::setColor( QColor c )
{
	delete _color;

	_color = new QColorGroup( c, c, c.light(), c.dark(), c.light(), c,c,c,c );
}

/*! 
 *\brief Set the pixmap for the button background
 */
void QStackBarBtn::setPixmap( QPixmap *pPix )
{
	if( pPixmap )
		delete pPixmap;

	pPixmap = pPix;
}

/*!
 * \brief Sets the hilight color of this button
 */
void QStackBarBtn::setHiColor( QColor c )
{
	delete _hiColor;

	_hiColor = new QColorGroup( c, c, c.light(), c.dark(), c.light(), c,c,c,c );
}


/*!
 * \brief Sets the selection color of this button
 */
void QStackBarBtn::setSelColor( QColor c )
{
	delete _selColor;

	_selColor = new QColorGroup( c, c, c.light(), c.dark(), c.light(), c,c,c,c );
}



#include "ButtonGroup_moc.cpp"