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

#include <qlayout.h>
#include <qdatetime.h>


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

void FCProgressBar::keyPressEvent(QKeyEvent* e)
{
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

	p->setPen( style()==MotifStyle? colorGroup().foreground() : colorGroup().text() );
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
//#				ifdef _DEBUG
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

FCCmdViewItem::FCCmdViewItem ( QIconView * parent, QAction* pAct )
: QIconViewItem(parent, pAct->menuText(), pAct->iconSet().pixmap())
{
  pAction = pAct;
  description = pAction->toolTip();
}

FCCmdViewItem::~FCCmdViewItem ()
{
}

QString FCCmdViewItem::text() const
{
  return description;
}

QAction* FCCmdViewItem::GetAction()
{
  return pAction;
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

  // clicking on a icon a signal with its description will be emitted
  connect(this, SIGNAL ( selectionChanged ( QIconViewItem * ) ), this, SLOT ( slotSelectionChanged(QIconViewItem * ) ) );
}

FCCmdView::~FCCmdView ()
{
}

void FCCmdView::slotSelectionChanged(QIconViewItem * item)
{
  emit emitSelectionChanged(item->text());
}

void FCCmdView::contentsMousePressEvent ( QMouseEvent * e )
{
  // create an object for drag and drop
  QIconView::contentsMousePressEvent(e);
  QIconViewItem *item = findItem( e->pos() );
  if (item != NULL)
  {
    if (typeid(*item) == typeid(FCCmdViewItem))
    {
      FCActionDrag *ad = new FCActionDrag( ((FCCmdViewItem*)item)->GetAction(), this );
      ad->setPixmap(QPixmap(*item->pixmap()),QPoint(8,8));
      ad->dragCopy();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////

QAction* FCActionDrag::pAction = NULL;

FCActionDrag::FCActionDrag ( QAction* action, QWidget * dragSource , const char * name  )
: QStoredDrag("FCActionDrag", dragSource, name)
{
  // store the QAction object
  pAction = action;
}

FCActionDrag::~FCActionDrag ()
{
}

bool FCActionDrag::canDecode ( const QMimeSource * e )
{
  return e->provides( "FCActionDrag" );
}

bool FCActionDrag::decode ( const QMimeSource * e, QAction*  a )
{
  if (pAction)
  {
    a = pAction;
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////////

FCToolBar::FCToolBar ( const QString & label, QMainWindow *parent, QMainWindow::ToolBarDock pos, 
                       bool newLine, const char * name )
: QToolBar(label, parent, pos, newLine, name)
{
  // allow drag and drop
  setAcceptDrops(true);
}

FCToolBar::FCToolBar ( const QString & label, QMainWindow *parent, QWidget *w, bool newLine, 
                       const char * name, WFlags f )
: QToolBar(label, parent, w, newLine, name, f)
{
  // allow drag and drop
  setAcceptDrops(true);
}

FCToolBar::FCToolBar ( QMainWindow * parent, const char * name )
: QToolBar(parent, name)
{
  // allow drag and drop
  setAcceptDrops(true);
}

FCToolBar::~FCToolBar()
{
}

void FCToolBar::dropEvent ( QDropEvent * e)
{
  // create a new button
  QAction* pAction = FCActionDrag::pAction;
  if ( pAction ) 
  {
    pAction->addTo(this);
    FCActionDrag::pAction = NULL;
  }
}

void FCToolBar::dragEnterEvent ( QDragEnterEvent * e)
{
  e->accept(FCActionDrag::canDecode(e));
}

void FCToolBar::dragLeaveEvent ( QDragLeaveEvent * )
{
}

void FCToolBar::dragMoveEvent ( QDragMoveEvent * )
{
}

#include "moc_Widgets.cpp"
