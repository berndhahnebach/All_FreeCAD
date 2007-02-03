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

#ifndef _PreComp_
# include <qlayout.h>
# include <qmessagebox.h>
# include <qpainter.h>
# include <q3scrollview.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <QTimerEvent>
#include <Q3Frame>
#include <QMouseEvent>
#include <Q3VBoxLayout>
#endif

#include "StackBar.h"
#include "Application.h"
#include "MainWindow.h"

#include "../Base/Exception.h"

using namespace Gui::DockWnd;

/**
 * \brief Constructor
 *
 * This constructor allows you to set the name of the button, and tell it
 * what widget it shall contain.  It also sets the default colors.
 */
StackBarButton::StackBarButton( QWidget *object, StackBar *parent, const char *name ) 
 : QToolButton( parent, name ), bIsSelected( false ), w(object), pStackBar(parent)
{
  setBackgroundMode( Qt::PaletteLight ); 
}

/**
 * \brief Another version of the constructor
 *
 * This constructor differs from the above one only in that you cannot
 * specify what widget the button shall contain.
 */
StackBarButton::StackBarButton( StackBar *parent, const char *name ) 
 : QToolButton( parent, name ), bIsSelected( false ), pStackBar(parent)
{
  setBackgroundMode( Qt::PaletteLight ); 
}

/**
 * \brief Destruction
 */
StackBarButton::~StackBarButton()
{
  w = NULL;
}

/**
 * \brief Marks/unmarks the button as selected.
 */
void StackBarButton::setSelected( bool b ) 
{ 
  bIsSelected = b; 
  update(); 
}

/**
 * Returns TRUE if the button is selected, otherwise returns FALSE.
 */
bool StackBarButton::isSelected ()
{
  return bIsSelected;
}

/**
 * \brief Sets the widget this button/page will contain
 *
 * Sets the widget for this button to contain.
 */
void StackBarButton::setWidget(QWidget* widget) 
{ 
  w = widget;
  if (w)
    w->hide();
}

/**
 * \brief Returns the widget this button/page contains
 *
 * Returns the widget for this button.
 */
QWidget* StackBarButton::widget() 
{ 
  return w; 
}

/**
 * \brief Draws the button.
 */
void StackBarButton::drawButton( QPainter *p )
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


  Q3PointArray a( 7 );
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
  p->setBrush( Qt::NoBrush );

  p->setPen( colorGroup().buttonText() );

  if ( p->fontMetrics().width( text() ) < w - d - 5 ) 
  {
    p->drawText( 2, 2, w, h - 2, Qt::AlignVCenter | Qt::AlignLeft, text() );
  } 
  else 
  {
    QString s = text().left( 1 );
    int ew = p->fontMetrics().width( "..." );
    int i = 1;
    while ( p->fontMetrics().width( s ) + ew + p->fontMetrics().width( text()[i] )  < w - d - 5 )
      s += text()[i++];
    s += "...";
    p->drawText( 2, 2, w, h - 2, Qt::AlignVCenter | Qt::AlignLeft, s );
  }
}

void StackBarButton::mousePressEvent ( QMouseEvent * e )
{
  if (e->button() == Qt::RightButton)
  {
  }
  else
    QToolButton::mousePressEvent(e);
}

// --------------------------------------------------------------------


/**
 * \brief Default constructor
 *
 * Initializes the lists for holding the page-buttons.
 */
StackBar::StackBar( QWidget *parent, const char *name )
  : DockWindow( 0L, parent, name )
{
  m_pCurPage  = NULL;
  m_pLastBtn = NULL,
  m_pLayout   = new Q3VBoxLayout( this );
  m_lAnimCount = 0;
  m_bAnimated = false;

  try
  {
    // attach the commandbar to its preferences
    const char* strGroupPath = "User parameter:BaseApp/Preferences/CommandBar";
    App::GetApplication().GetParameterGroupByPath(strGroupPath)->Attach(this);
    App::GetApplication().GetParameterGroupByPath(strGroupPath)->Notify("SpeedAnimationCmdBar");
  }
  catch(const Base::Exception& rclE)
  {
    QMessageBox::warning(getMainWindow(), "Wrong parameter", rclE.what());
  }
}

/**
 * \brief Default destructor
 *
 * Destroys the list of StackBarButtons.
 */
StackBar::~StackBar()
{
  // detach the command bar
  const char* strGroupPath = "User parameter:BaseApp/Windows/CommandBar";
  App::GetApplication().GetParameterGroupByPath(strGroupPath)->Detach(this);
  m_mButtonView.clear();
  delete m_pLayout;
}

/**
 * \brief Add a new page to the StackBar
 *
 * Call this to add a new page \a page to the widget.with the label \a name.
 */
bool StackBar::addItem(QWidget* page, const QString &name)
{
  if (!page)
  {
    fprintf(stderr, "Cannot set a null widget\n");
    return false; // no valid object	
  }

  if (m_mButtonView.size() == 0)
    show(); //show again

  page->setBackgroundMode( Qt::PaletteBackground );
  StackBarButton *button = new StackBarButton( this, name.latin1() );
  button->setWidget(page);
  button->setText( tr(name) );
  button->setFixedHeight( button->sizeHint().height() );
  connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
  Q3ScrollView *sv = new Q3ScrollView( this );
  sv->setHScrollBarMode(Q3ScrollView::AlwaysOff);
  sv->setVScrollBarMode(Q3ScrollView::AlwaysOff);
  sv->setResizePolicy( Q3ScrollView::AutoOneFit );
  sv->addChild( page );
  sv->setFrameStyle( Q3Frame::NoFrame );
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


/**
 * \brief Checks for widget 
 *
 * Checks if there is the widget \a w contained by the widget list and returns its index, 
 * otherwise returns -1.
 */
int StackBar::indexOf( QWidget* w ) const
{
  int idx = 0;
  for (std::map <StackBarButton*,Q3ScrollView*>::const_iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it, ++idx)
  {
    StackBarButton* b = (it)->first;
    if (b->widget() == w)
    {
      return idx;
    }
  }

  return -1;
}

/**
 * \brief Remove the page from the StackBar
 *
 * Call this to remove the page \a w from the widget.
 */
bool StackBar::removeItem(QWidget* w)
{
  bool bSucceed = false;
  for (std::map <StackBarButton*,Q3ScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    if (it->first->widget() == w)
    {
      bSucceed = true;
      StackBarButton* bt = it->first;
      if (bt == m_pLastBtn)
        m_pLastBtn = NULL;
      Q3ScrollView* sv = it->second;
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

/**
 * \brief Sets the current page
 *
 * Call this to set the current page \a w.  The widget will refresh and
 * rearrange itself.
 */
void StackBar::setCurrentItem(QWidget* w)
{
  for (std::map <StackBarButton*,Q3ScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    if (it->first->widget() == w)
    {
      m_pLastBtn = it->first;
      m_pLastBtn->setSelected(true);
      m_pCurPage  = it->second;
      m_pCurPage->show();
    }
    else
    {
      it->first->setSelected(false);
      it->second->hide();
    }
  }
}

/**
 * \brief Returns the current page
 *
 * Call this to get the current page.
 */
QWidget* StackBar::currentItem() const
{
  for (std::map <StackBarButton*,Q3ScrollView*>::const_iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
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

/**
 * If \a b is TRUE then the widget \a w is set visible, otherwise \a w is invisible.
 */
void StackBar::setItemVisible( QWidget* w, bool b )
{
  if ( b )
  {
    for (std::map <StackBarButton*,Q3ScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
    {
      if (it->first->widget() == w)
      {
        StackBarButton* page = firstPageVisible();
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
  else
  {
    for (std::map <StackBarButton*,Q3ScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
    {
      if (it->first->widget() == w)
      {
        it->first->hide();
        if (it->first->isSelected())
        {
          it->first->setSelected(false);
          StackBarButton* page = firstPageVisible();
          if (page)
            page->animateClick();
          else
            it->second->hide();
        }
      }
    }
  }
}

/**
 * Returns TRUE whether the widget \a w is visible, otherwise returns FALSE.
 */
bool StackBar::isItemVisible(QWidget* w) const
{
  for (std::map <StackBarButton*,Q3ScrollView*>::const_iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    if (it->first->widget() == w)
    {
      return it->first->isVisible();
    }
  }

  return false;
}

StackBarButton* StackBar::firstPageVisible()
{
  for (std::map <StackBarButton*,Q3ScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    if (it->first->isVisible())
    {
      return it->first;
    }
  }

  return NULL;
}

void StackBar::buttonClicked()
{
  StackBarButton *tb = (StackBarButton*)sender();
  Q3ScrollView *page = NULL;
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

/**
 * \brief Updates the StackBar
 *
 * Duh
 */
void StackBar::updatePages()
{
  bool after = false;
  for (std::map <StackBarButton*,Q3ScrollView*>::iterator it = m_mButtonView.begin(); it != m_mButtonView.end(); ++it)
  {
    (it)->first->setBackgroundMode( /*!after ? */Qt::PaletteBackground /*: PaletteLight*/ );
    (it)->first->update();
    after = (it->first) == m_pLastBtn;
  }
}

/** 
 * @see animatePageScroll() 
 */
void StackBar::timerEvent ( QTimerEvent * )
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
    m_pAnimCurPage->setVScrollBarMode(Q3ScrollView::Auto);
    m_pAnimNewPage->setVScrollBarMode(Q3ScrollView::Auto);
    m_pAnimCurPage->hide();
    m_pAnimNewPage->show();
    m_pCurPage = m_pAnimNewPage;
    updatePages();
  }
}

/** 
 * Delays the change of a page after a new was set to active. 
 */
void StackBar::animatePageScroll(Q3ScrollView* pCurPage, Q3ScrollView* pNewPage)
{
  if (!pCurPage || !pNewPage)
    return; // one page is invalid
  m_pAnimNewPage = pNewPage;
  m_pAnimCurPage = pCurPage;
  m_iCurHeight   = m_pAnimCurPage->height();
  m_iNewHeight   = m_pAnimNewPage->height();

  m_pAnimCurPage->setVScrollBarMode(Q3ScrollView::AlwaysOff);
  m_pAnimNewPage->setVScrollBarMode(Q3ScrollView::AlwaysOff);
  m_pAnimCurPage->show();
  m_pAnimNewPage->setFixedHeight(0);
  m_pAnimNewPage->show();

  m_bAnimated = true;
  startTimer(5);
}

/**
 * Checks which settings have been changed and updates the internal stuff.
 */
void StackBar::OnChange(Base::Subject<const char*> &rCaller,const char* sReason)
{
  ParameterGrp& rclGrp = ((ParameterGrp&)rCaller);
  if (strcmp(sReason, "SpeedAnimationCmdBar") == 0)
  {
    ParameterGrp::handle hGrp = rclGrp.GetGroup("SpeedAnimationCmdBar");
    if (hGrp->GetInts("Value").size() == 0)
      hGrp->SetInt("Value", 100);
    m_lAnimCount = hGrp->GetInt("Value", 100);
    if (m_lAnimCount == hGrp->GetInt("MaxValue", 100))
      m_lAnimCount = 0;
    else if (m_lAnimCount == hGrp->GetInt("MinValue", 0))
      m_lAnimCount = 5;
  }
}

#include "moc_StackBar.cpp"
