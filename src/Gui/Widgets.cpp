/***************************************************************************
                          Widgets.cpp  -  description
                             -------------------
    begin                : 2002/12/20 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file Widgets.cpp
 *  \brief A set of FreeCAD widgets
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
#	include <qstring.h>
#	include <qurl.h>
#	include <ctype.h>
#endif

#include "Widgets.h"
#include "Application.h"
#include "../Base/Exception.h"

#include <qlayout.h>
#include <qdatetime.h>

#if QT_VERSION > 230
# include <qstyle.h>
#endif


QString FCFileDialog::getOpenFileName( const QString & startWith, const QString& filter,
                          				     QWidget *parent, const char* name )
{
    return getOpenFileName( startWith, filter, parent, name, QString::null  );
}

QString FCFileDialog::getOpenFileName( const QString & startWith, const QString& filter, QWidget *parent, 
                                       const char* name, const QString& caption )
{
  QString file = QFileDialog::getOpenFileName( startWith, filter, parent, name, caption );

  // set wait cursor because on windows OS this method blocks the QApplication object
  if (!file.isEmpty())
    FCAutoWaitCursor::Instance().SetWaitCursor();
  return file;
}

QString FCFileDialog::getSaveFileName( const QString & startWith, const QString& filter,
                          				     QWidget *parent, const char* name )
{
    return getSaveFileName( startWith, filter, parent, name, QString::null  );
}

QString FCFileDialog::getSaveFileName( const QString & startWith, const QString& filter, QWidget *parent, 
                                       const char* name, const QString& caption )
{
  QString file = QFileDialog::getSaveFileName( startWith, filter, parent, name, caption );

  // set wait cursor because on windows OS this method blocks the QApplication object
  if (!file.isEmpty())
    FCAutoWaitCursor::Instance().SetWaitCursor();
  return file;
}

///////////////////////////////////////////////////////////////////////////////////

FCProgressBar::FCProgressBar ( QWidget * parent, const char * name, WFlags f )
: QProgressBar (parent, name, f)
{
  setFixedWidth(120);
  // this style is very nice ;-)
  setIndicatorFollowsStyle(false);
  // update the string after timestep steps
  iTimeStep = 1;
  // so far there are no sequencer started
  iStartedProgresses = 0;
  bSeveralInstances = false;
}

// NOTE: for each call of this method you must call the
//       corresponding stop method
void FCProgressBar::Start(QString txt, int steps, bool& flag)
{
  // increment the size called this method
  iStartedProgresses++;

  // if you call this method in a for loop
  // the steps size is regarded only once 
  if (!flag || steps == 0)
    return;

  flag = false;

  // several sequencer started
  if (iStartedProgresses > 1)
  {
    bSeveralInstances = true;
    steps *= totalSteps();
    setTotalSteps(steps);
    iTimeStep = totalSteps() / iFirstTotal; 
    measureTime.restart();
  }
  else
  {
    iTotalTime = 0;
    iStep = 0;
    remainingTime = "";
    iFirstTotal = steps;
    setTotalSteps(steps);
    measureTime.start();
  }

  // print message to the satusbar
  if (iStartedProgresses == 1)
    ApplicationWindow::Instance->statusBar()->message(txt);
}

void FCProgressBar::Next()
{
  if (!isInterrupted())
    setProgress(iStep++);
}

void FCProgressBar::Stop ()
{
  iStartedProgresses--;
  if (iStartedProgresses == 0)
  {
    reset();
    bSeveralInstances = false;
  }
}

bool FCProgressBar::isInterrupted()
{
#ifdef  FC_OS_WIN32
  MSG  tMSG;

  // Escape button pressed ?
  while (PeekMessage(&tMSG, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE) == TRUE)
  {
    TranslateMessage(&tMSG);
    if (tMSG.message == WM_KEYDOWN)
    {
      if (tMSG.wParam == VK_ESCAPE)
      {
        interrupt();
        return true;
      }
    }
  }
#endif

  return false;
}

void FCProgressBar::interrupt()
{
  //resets
  reset();
  bSeveralInstances = false;
  iStartedProgresses = 0;

  FCGuiConsoleObserver::bMute = true;
  FCException exc("Aborting...");
  FCGuiConsoleObserver::bMute = false;
  throw exc;
}

void FCProgressBar::drawContents( QPainter *p )
{
  const int unit_width  = 9;
  const int unit_height = 12;
  const int total_steps = totalSteps();
  const int progress_val = progress();
  QString	progress_str;

  setIndicator ( progress_str, progress_val, total_steps );

  const QRect bar = contentsRect();

	if (total_steps) 
  {
	  int u = bar.width();
	  int pw;

    if ( u > 0 && progress_val >= INT_MAX / u && total_steps >= u )
  		pw = (u * (progress_val / u)) / (total_steps / u);
	  else
	  	pw = bar.width() * progress_val / total_steps;

    p->setPen( colorGroup().highlightedText() );
    p->setClipRect( bar.x(), bar.y(), pw, bar.height() );
    p->fillRect( bar, colorGroup().brush( QColorGroup::Highlight ) );
    p->drawText( bar, AlignCenter, progress_str );

    p->setClipRect( bar.x()+pw, bar.y(), bar.width()-pw, bar.height() );
	}

	if ( progress_val != total_steps )
    p->fillRect( bar, colorGroup().brush( QColorGroup::Background ) );

#if QT_VERSION <= 230
	p->setPen( style()==MotifStyle? colorGroup().foreground() : colorGroup().text() );
#else
  p->setPen( style().isA("QMotifStyle")? colorGroup().foreground() : colorGroup().text());
#endif
	p->drawText( bar, AlignCenter, progress_str );
}

bool FCProgressBar::setIndicator ( QString & indicator, int progress, int totalSteps )
{
	if (QProgressBar::setIndicator(indicator, progress, totalSteps) == false)
	{
		if (progress != -1)
		{
			// recalc remaining time every iTimeStep steps
			if (progress % iTimeStep == 0 && progress > 0)
			{
				// difference in ms
				int diff = measureTime.restart();
				iTotalTime += diff;
				// remaining time in s
				diff *= (totalSteps - progress) / iTimeStep;
				diff /= 1000;

				GetConsole().Log("Elapsed time: %ds, (%d of %d)\n", diff, progress, totalSteps);
//#				ifdef FC_DEBUG
//				printf("Elapsed time: %ds, (%d of %d)\n", diff, progress, totalSteps);
//#				endif

//				if (bSeveralInstances == false)
				{
					// get time format
					int second = diff %  60; diff /= 60;
					int minute = diff %  60; diff /= 60;
					int hour   = diff %  60;
					QString h,m,s;
					if (hour < 10)   
						h = tr("0%1").arg(hour);
					else
						h = tr("%1").arg(hour);
					if (minute < 10) 
						m = tr("0%1").arg(minute);
					else
						m = tr("%1").arg(minute);
					if (second < 10) 
						s = tr("0%1").arg(second);
					else
						s = tr("%1").arg(second);

					// nice formating for output
					if(hour == 0)
						if(minute == 0)
							remainingTime = tr("(%1s)").arg(s);
						else
							remainingTime = tr("(%1' %2s)").arg(m).arg(s);
					else
						remainingTime = tr("(%1h %2min)").arg(h).arg(m);
				}
			}

			char szBuf[200];
			sprintf(szBuf, "%d %% %s", (100 * progress) / totalSteps, remainingTime.latin1());
			indicator = szBuf;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////

FCListView::FCListView ( QWidget * parent, const char * name, WFlags f )
{
}

FCListView::FCListView ( QWidget * parent, const char * name )
{
}

FCListView::~FCListView ()
{
}

QListViewItem* FCListView::lastItem () const
{
  QListViewItem* item = firstChild();
  if ( item ) 
  {
  	while ( item->nextSibling() || item->firstChild() ) 
    {
	    if ( item->nextSibling() )
    		item = item->nextSibling();
	    else
		    item = item->firstChild();
  	}
  }

  return item;
}

QListViewItem* FCListView::lastItem (QListView* listview)
{
  QListViewItem* item = listview->firstChild();
  if ( item ) 
  {
  	while ( item->nextSibling() || item->firstChild() ) 
    {
	    if ( item->nextSibling() )
    		item = item->nextSibling();
	    else
		    item = item->firstChild();
  	}
  }

  return item;
}

///////////////////////////////////////////////////////////////////////////////////

FCCmdViewItem::FCCmdViewItem ( QIconView * parent, QString Action, QAction* pAct)
: QIconViewItem(parent, pAct->menuText(), pAct->iconSet().pixmap())
{
  sAction = Action;
  description = pAct->toolTip();
}

FCCmdViewItem::~FCCmdViewItem ()
{
}

QString FCCmdViewItem::text() const
{
  return description;
}

QString FCCmdViewItem::GetAction()
{
  return sAction;
}

///////////////////////////////////////////////////////////////////////////////////

FCCmdView::FCCmdView ( QWidget * parent, const char * name, WFlags f )
: QIconView(parent, name, f)
{
  // settings for the view showing the icons
  setResizeMode(Adjust);
  setItemsMovable(false);
  setWordWrapIconText(false);
  setGridX(50);
  setGridY(50);

  setSelectionMode(Extended);

  // clicking on a icon a signal with its description will be emitted
  connect(this, SIGNAL ( currentChanged ( QIconViewItem * ) ), this, SLOT ( slotSelectionChanged(QIconViewItem * ) ) );
}

FCCmdView::~FCCmdView ()
{
}

void FCCmdView::slotSelectionChanged(QIconViewItem * item)
{
  emit emitSelectionChanged(item->text());
}

QDragObject * FCCmdView::dragObject ()
{
  if ( !currentItem() )
      return 0;

  bool bFirst = true;
  FCActionDrag *ad=NULL;
  QPoint orig = viewportToContents( viewport()->mapFromGlobal( QCursor::pos() ) );
  for ( QIconViewItem *item = firstItem(); item; item = item->nextItem() ) 
  {
    if ( item->isSelected() ) 
    {
      if (typeid(*item) == typeid(FCCmdViewItem))
      {
        ad = new FCActionDrag( ((FCCmdViewItem*)item)->GetAction(), this );
        if (bFirst)
        {
          bFirst = false;
          ad->setPixmap( *currentItem()->pixmap(), QPoint( currentItem()->pixmapRect().width() / 2, 
                                                           currentItem()->pixmapRect().height() / 2 ) );
        }
      }
    }
  }

  return ad;
}

/* 
 *  Constructs a FCDlgCreateToolOrCmdBar which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FCDlgCreateToolOrCmdBar::FCDlgCreateToolOrCmdBar( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "FCDlgCreateToolOrCmdBar" );
    resize( 242, 179 ); 
    setProperty( "caption", tr( "Create Toolbar and/or Command bar" ) );
    setProperty( "sizeGripEnabled", QVariant( TRUE, 0 ) );
    FCDlgCreateToolOrCmdBarLayout = new QGridLayout( this ); 
    FCDlgCreateToolOrCmdBarLayout->setSpacing( 6 );
    FCDlgCreateToolOrCmdBarLayout->setMargin( 11 );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setProperty( "title", tr( "Toolbar/Command bar" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 0 );
    GroupBox1->layout()->setMargin( 0 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );
    GroupBox1Layout->setSpacing( 6 );
    GroupBox1Layout->setMargin( 11 );

    TextLabel = new QLabel( GroupBox1, "TextLabel" );
    TextLabel->setProperty( "text", tr( "Name:" ) );

    GroupBox1Layout->addWidget( TextLabel, 0, 0 );

    LineEditName = new QLineEdit( GroupBox1, "LineEditName" );

    GroupBox1Layout->addWidget( LineEditName, 1, 0 );

    CheckCreateCmdBar = new QCheckBox( GroupBox1, "CheckCreateCmdBar" );
    CheckCreateCmdBar->setProperty( "text", tr( "Create command bar" ) );
    CheckCreateCmdBar->setProperty( "checked", QVariant( TRUE, 0 ) );

    GroupBox1Layout->addWidget( CheckCreateCmdBar, 3, 0 );

    CheckCreateToolBar = new QCheckBox( GroupBox1, "CheckCreateToolBar" );
    CheckCreateToolBar->setProperty( "text", tr( "Create toolbar" ) );
    CheckCreateToolBar->setProperty( "checked", QVariant( TRUE, 0 ) );

    GroupBox1Layout->addWidget( CheckCreateToolBar, 2, 0 );

    FCDlgCreateToolOrCmdBarLayout->addWidget( GroupBox1, 0, 0 );

    Layout2 = new QHBoxLayout; 
    Layout2->setSpacing( 6 );
    Layout2->setMargin( 0 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setProperty( "text", tr( "&OK" ) );
    buttonOk->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
    buttonOk->setProperty( "default", QVariant( TRUE, 0 ) );
    Layout2->addWidget( buttonOk );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setProperty( "text", tr( "&Cancel" ) );
    buttonCancel->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
    Layout2->addWidget( buttonCancel );

    FCDlgCreateToolOrCmdBarLayout->addLayout( Layout2, 2, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    FCDlgCreateToolOrCmdBarLayout->addItem( spacer_2, 1, 0 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( LineEditName, CheckCreateToolBar );
    setTabOrder( CheckCreateToolBar, CheckCreateCmdBar );
    setTabOrder( CheckCreateCmdBar, buttonOk );
    setTabOrder( buttonOk, buttonCancel );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCDlgCreateToolOrCmdBar::~FCDlgCreateToolOrCmdBar()
{
    // no need to delete child widgets, Qt does it all for us
}

void FCDlgCreateToolOrCmdBar::accept () 
{
  QString txt = LineEditName->text();
  if (CheckCreateToolBar->isChecked())
    ApplicationWindow::Instance->GetCustomWidgetManager()->getToolBar(txt.latin1());
  if (CheckCreateCmdBar->isChecked())
    ApplicationWindow::Instance->GetCustomWidgetManager()->getCmdBar(txt.latin1());
  QDialog::accept();
}

#include "moc_Widgets.cpp"
