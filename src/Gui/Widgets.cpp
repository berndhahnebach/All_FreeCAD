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



#include "PreCompiled.h"

#ifndef _PreComp_
#	include <vector>
#	include <qaction.h>
#	include <qaccel.h>
#	include <qbutton.h>
#	include <qcolordialog.h>
#	include <qcursor.h>
#	include <qgroupbox.h>
#	include <qlabel.h>
#	include <qlayout.h>
#	include <qmovie.h>
#	include <qpainter.h>
#	include <qstatusbar.h>
#	include <qstring.h>
#	include <qthread.h>
#	include <qurl.h>
#	include <qvalidator.h>
#	include <qwidgetlist.h>
#	include <ctype.h>
#endif

#include "Widgets.h"
#include "Application.h"
#include "PrefWidgets.h"
#include "WaitCursor.h"
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
  return QFileDialog::getOpenFileName( startWith, filter, parent, name, caption );
}

QString FCFileDialog::getSaveFileName( const QString & startWith, const QString& filter,
                          				     QWidget *parent, const char* name )
{
    return getSaveFileName( startWith, filter, parent, name, QString::null  );
}

QString FCFileDialog::getSaveFileName( const QString & startWith, const QString& filter, QWidget *parent, 
                                       const char* name, const QString& caption )
{
  return QFileDialog::getSaveFileName( startWith, filter, parent, name, caption );
}

QString FCFileDialog::getSaveFileName ( const QString & initially, const QString & filter, QWidget * parent, 
                                        const QString & caption )
{
  FCFileDialog fd(AnyFile, initially, filter, parent, tr("Save Dialog"), true);
  fd.setCaption(caption);
  if ( fd.exec() )
	  return fd.selectedFileName();
	else
		return QString("");
}

FCFileDialog::FCFileDialog (Mode mode, QWidget* parent, const char* name, bool modal)
: QFileDialog(parent, name, modal)
{
}

FCFileDialog::FCFileDialog (Mode mode, const QString& dirName, const QString& filter, 
                            QWidget* parent, const char* name, bool modal)
 : QFileDialog(dirName, filter, parent, name, modal)
{
  setMode(mode);
}

FCFileDialog::~FCFileDialog()
{
}

void FCFileDialog::accept()
{
  QString fn = selectedFileName();

  if (QFile(fn).exists() && mode() == AnyFile)
  {
		QString msg = tr("'%1' already exists.\nReplace existing file?").arg(fn);
    if (QMessageBox::information(this, "Existing file", msg, "Yes", "No", QString::null, 1) == 0)
      QFileDialog::accept();
  }
  else
    QFileDialog::accept();
}

QString FCFileDialog::selectedFileName()
{
  QString fn = selectedFile();

  // if empty do nothing
  if (fn.isEmpty())
    return fn;

  // search for extension
  int pos = fn.findRev('.');
  if (pos == -1)
  {
    // try to figure out the selected filter
    QString filt = selectedFilter();
    int dot = filt.find('*');
    int blank = filt.find(' ', dot);
    int brack = filt.find(')', dot);
    if (dot != -1 && blank != -1)
    {
      QString sub = filt.mid(dot+1, blank-dot-1);
      fn = fn + sub;
    }
    else if (dot != -1 && brack != -1)
    {
      QString sub = filt.mid(dot+1, brack-dot-1);
      fn = fn + sub;
    }
  }

  return fn;
}

///////////////////////////////////////////////////////////////////////////////////

FCMessageBox::FCMessageBox(QWidget * parent, const char * name)
	: QMessageBox(parent, name), checkBox(0L), layout(0L)
{
}

FCMessageBox::FCMessageBox(const QString & caption, const QString & text, Icon icon, int button0, int button1, 
													 int button2, QWidget * parent, const char * name, bool modal, WFlags f)
	: QMessageBox(caption, text, icon, button0, button1, button2, parent, name, modal, f), 
		checkBox(0L), layout(0L)
{
	QString msg = tr("Do not show this message again");
	QString entry = QString("NotShowDialog:%1").arg(text);

	checkBox = new FCCheckBox(this, "FCCheckBox");
	checkBox->setText( msg );
	checkBox->setEntryName(entry);
	checkBox->setParamGrpPath("User parameter:BaseApp/Windows/Dialogs");
	checkBox->getHandler()->onRestore();

	// if check throw an exception to avoid to show
	// (exception is caught in the calling static function)
	if (checkBox->isChecked())
	{
		delete checkBox;
		checkBox = 0L;

		throw QString("jjj");
	}

	layout = new QGridLayout(this, 1, 1, 50);
	layout->addWidget(checkBox, 0, 0);
}

FCMessageBox::~FCMessageBox()
{
	if (checkBox)
		checkBox->getHandler()->onSave();
	delete checkBox;
	delete layout;
}

int FCMessageBox::information(QWidget * parent, const QString & caption, const QString & text, 
															int button0, int button1, int button2)
{
	FCMessageBox dlg(caption, text, Information, button0, button1, button2, parent,  "FCMessageBox");
	dlg.exec();
	return 0;
}

int FCMessageBox::information(QWidget * parent, const QString & caption, const QString & text, 
															const QString & button0Text, const QString & button1Text, 
															const QString & button2Text, int defaultButtonNumber, int escapeButtonNumber)
{
	int b[3];
	b[0] = 1;
	b[1] = !button1Text.isEmpty() ? 2 : 0;
	b[2] = !button2Text.isEmpty() ? 3 : 0;

	int i;
	for( i=0; i<3; i++ ) 
	{
		if ( b[i] > 0  && defaultButtonNumber == i )
			b[i] += QMessageBox::Default;
		if ( b[i] > 0  && escapeButtonNumber == i )
			b[i] += QMessageBox::Escape;
	}

	FCMessageBox* dlg=0L;
	try{
		dlg=new FCMessageBox( caption, text, Information, b[0], b[1], b[2], parent, "FCMessageBox");
		if ( !button0Text.isEmpty() )
			dlg->setButtonText(0, button0Text);
		if ( !button1Text.isEmpty() )
			dlg->setButtonText(1, button1Text);
		if ( !button2Text.isEmpty() )
			dlg->setButtonText(2, button2Text);
		int res = dlg->exec();
		delete dlg;
		return (res-1);
	} catch (...)
	{
		return -1;
	}
}

int FCMessageBox::warning ( QWidget * parent, const QString & caption, const QString & text, 
													 int button0, int button1, int button2 )
{
	return 0;
}

int FCMessageBox::warning ( QWidget * parent, const QString & caption, const QString & text, 
													 const QString & button0Text, const QString & button1Text, 
													 const QString & button2Text, int defaultButtonNumber, int escapeButtonNumber)
{
	return 0;
}

int FCMessageBox::critical ( QWidget * parent, const QString & caption, const QString & text, 
														int button0, int button1, int button2 )
{
	return 0;
}

int FCMessageBox::critical ( QWidget * parent, const QString & caption, const QString & text, 
														const QString & button0Text, const QString & button1Text, 
														const QString & button2Text, int defaultButtonNumber, int escapeButtonNumber)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////

struct FCProgressBarPrivate
{
  int nElapsed; // in milliseconds
	int nRemaining;
  QTime measureTime;
	FCWaitingCursor* cWaitCursor;
};

FCProgressBar::FCProgressBar ( QWidget * parent, const char * name, WFlags f )
: QProgressBar (parent, name, f), Sequencer()
{
  d = new FCProgressBarPrivate;
	d->cWaitCursor = 0L;

  setFixedWidth(120);
  // this style is very nice ;-)
  setIndicatorFollowsStyle(false);
}

FCProgressBar::~FCProgressBar ()
{
  delete d;
}

void FCProgressBar::enterControlEvents()
{
  QWidgetList  *list = QApplication::allWidgets();
  QWidgetListIt it( *list );         // iterate over the widgets
  QWidget * w;
  while ( (w=it.current()) != 0 ) {  // for each widget...
      ++it;
			if (w != this)
	      w->installEventFilter(this);
  }
  delete list;                      // delete the list, not the widgets
}

void FCProgressBar::leaveControlEvents()
{
  QWidgetList  *list = QApplication::allWidgets();
  QWidgetListIt it( *list );         // iterate over the widgets
  QWidget * w;
  while ( (w=it.current()) != 0 ) {  // for each widget...
      ++it;
			if (w != this)
	      w->removeEventFilter(this);
  }
  delete list;                      // delete the list, not the widgets
}

bool FCProgressBar::eventFilter(QObject* o, QEvent* e)
{
	if (isRunning() && e != 0L)
	{
		switch ( e->type() )
		{
			// check for ESC
			case QEvent::KeyPress:
			{
				QKeyEvent* ke = (QKeyEvent*)e;
				if (ke->key() == Qt::Key_Escape)
				{
					// cancel the operation
					_bCanceled = true;
				}

				return true;
			} break;

			// ignore alle these events
			case QEvent::KeyRelease:
			case QEvent::Enter:
			case QEvent::Leave:
			{
				return true;
			} break;

			// do a system beep and ignore the event
			case QEvent::MouseButtonPress:
			{
				QApplication::beep();
				return true;
			} break;

			default:
			{
			}  break;
		}
	}

	return QProgressBar::eventFilter(o, e);
}

bool FCProgressBar::start(const char* pszStr, unsigned long steps)
{
	// base stuff
	bool ret = Sequencer::start(pszStr, steps);

	setTotalSteps(_nTotalSteps);

	if ( _nInstStarted == 1 )
  {
		enterControlEvents();
    d->nElapsed = 0;
    d->measureTime.start();
		d->cWaitCursor = new FCWaitingCursor;
  }
	else // if (_nMaxInstStarted == _nInstStarted)
    d->measureTime.restart();

	return ret;
}

bool FCProgressBar::next()
{
  if (!wasCanceled())
	{
    setProgress(_nProgress++);

		// estimate the remaining time in milliseconds
		int diff = d->measureTime.restart();
		d->nElapsed += diff;
		d->nRemaining = d->nElapsed * ( totalSteps() - _nProgress ) / _nProgress;
	}
	else
	{
		// force to abort the operation
		abort();
	}

	qApp->processEvents();

	return _nProgress < _nTotalSteps;
}

void FCProgressBar::resetBar()
{
  reset();
  setTotalSteps(0);
  setProgress(-1);
	delete d->cWaitCursor;
	d->cWaitCursor = 0L;
	leaveControlEvents();

	Sequencer::resetBar();
}

void FCProgressBar::abort()
{
  //resets
  resetBar();

	bool bMute = FCGuiConsoleObserver::bMute;
  FCGuiConsoleObserver::bMute = true;
  FCException exc("Aborting...");
  FCGuiConsoleObserver::bMute = bMute;
  throw exc;
}

void FCProgressBar::setText (const char* pszTxt)
{
	// print message to the statusbar
	QString txt = pszTxt ? pszTxt : "";
  ApplicationWindow::Instance->statusBar()->message(txt);
}


void FCProgressBar::drawContents( QPainter *p )
{
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
			int nRemaining = d->nRemaining;
			nRemaining /= 1000;

			// get time format
			int second = nRemaining %  60; nRemaining /= 60;
			int minute = nRemaining %  60; nRemaining /= 60;
			int hour   = nRemaining %  60;
			QString h,m,s;
			if (hour < 10)   
				h = QString("0%1").arg(hour);
			else
				h = QString("%1").arg(hour);
			if (minute < 10) 
				m = QString("0%1").arg(minute);
			else
				m = QString("%1").arg(minute);
			if (second < 10) 
				s = QString("0%1").arg(second);
			else
				s = QString("%1").arg(second);

			// nice formating for output
			int steps = (100 * progress) / totalSteps;
			indicator = QString("%1% (%2:%3:%4)").arg(steps).arg(h).arg(m).arg(s);
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

class FCCmdViewItemPrivate
{
  public:
    QString sAction;
    QString description;
};

FCCmdViewItem::FCCmdViewItem ( QIconView * parent, QString Action, QAction* pAct)
: QIconViewItem(parent, pAct->menuText(), pAct->iconSet().pixmap())
{
  d = new FCCmdViewItemPrivate;
  d->sAction = Action;
  d->description = pAct->toolTip();
}

FCCmdViewItem::~FCCmdViewItem ()
{
  delete d;
}

QString FCCmdViewItem::text() const
{
  return d->description;
}

QString FCCmdViewItem::GetAction()
{
  return d->sAction;
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
  FCActionDrag::actions.clear();
  if ( !currentItem() )
      return 0;

  bool bFirst = true;
  FCActionDrag *ad=NULL;
  //  QPoint orig = viewportToContents( viewport()->mapFromGlobal( QCursor::pos() ) );
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

///////////////////////////////////////////////////////////////////////////////////

FCAccelLineEdit::FCAccelLineEdit ( QWidget * parent, const char * name )
: QLineEdit(parent, name)
{
}

void FCAccelLineEdit::keyPressEvent ( QKeyEvent * e)
{
	QString txt;
	clear();

	if (e->ascii() == 0)
		 return; // only meta key pressed

	switch(e->state())
	{
		case ControlButton:
			txt += QAccel::keyToString(CTRL+e->key());
			setText(txt);
			break;
		case ControlButton+AltButton:
			txt += QAccel::keyToString(CTRL+ALT+e->key());
			setText(txt);
			break;
		case ControlButton+ShiftButton:
			txt += QAccel::keyToString(CTRL+SHIFT+e->key());
			setText(txt);
			break;
		case ControlButton+AltButton+ShiftButton:
			txt += QAccel::keyToString(CTRL+ALT+SHIFT+e->key());
			setText(txt);
			break;
		default:
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////

/*
 *  Constructs a FCCheckListDlg which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FCCheckListDlg::FCCheckListDlg( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "FCCheckListDlg" );
    resize( 402, 270 );
    setProperty( "sizeGripEnabled", QVariant( TRUE, 0 ) );
    FCCheckListDlgLayout = new QGridLayout( this );
    FCCheckListDlgLayout->setSpacing( 6 );
    FCCheckListDlgLayout->setMargin( 11 );

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

    FCCheckListDlgLayout->addLayout( Layout2, 1, 0 );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 0 );
    GroupBox1->layout()->setMargin( 0 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );
    GroupBox1Layout->setSpacing( 6 );
    GroupBox1Layout->setMargin( 11 );

    ListView = new QListView( GroupBox1, "ListView" );
    ListView->addColumn( "Items" );
    ListView->setRootIsDecorated( TRUE );

    GroupBox1Layout->addWidget( ListView, 0, 0 );

    FCCheckListDlgLayout->addWidget( GroupBox1, 0, 0 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCCheckListDlg::~FCCheckListDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

void FCCheckListDlg::setItems(const std::vector<std::string>& items)
{
	for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
		this->items.push_back(std::make_pair<std::string, bool>(*it, true));
}

void FCCheckListDlg::setItems(const std::vector<std::pair<std::string, bool> >& items)
{
  this->items = items;
}

std::vector<std::string> FCCheckListDlg::getCheckedItems() const
{
  return checked;
}

void FCCheckListDlg::show ()
{
  for (std::vector<std::pair<std::string, bool> >::iterator it = items.begin(); it!=items.end(); ++it)
  {
		QCheckListItem* item = new QCheckListItem( ListView, it->first.c_str(), QCheckListItem::CheckBox );
		item->setEnabled(it->second);
  }

  QDialog::show();
}

void FCCheckListDlg::hide ()
{
  QListViewItemIterator it = ListView->firstChild();

  for ( ; it.current(); it++) 
  {
    if ( ((QCheckListItem*)it.current())->isOn() ) 
    {
      checked.push_back(((QCheckListItem*)it.current())->text().latin1());
	  }
  }

  QDialog::hide();
}

///////////////////////////////////////////////////////////////////////////////////

FCColorButton::FCColorButton(QWidget* parent, const char* name)
  : QButton( parent, name )
{
  setMinimumSize( minimumSizeHint() );
  connect( this, SIGNAL(clicked()), SLOT(onChooseColor()));
}

FCColorButton::FCColorButton( const QBrush& b, QWidget* parent, const char* name, WFlags f )
  : QButton( parent, name, f )
{
  col = b.color();
  setMinimumSize( minimumSizeHint() );
}

FCColorButton::~FCColorButton()
{
}

void FCColorButton::setColor( const QColor& c )
{
  col = c;
  update();
}

QColor FCColorButton::color() const
{
  return col;
}

QSize FCColorButton::sizeHint() const
{
  return QSize( 50, 25 );
}

QSize FCColorButton::minimumSizeHint() const
{
  return QSize( 50, 25 );
}

void FCColorButton::drawButton( QPainter *paint )
{
#if QT_VERSION <= 230
  style().drawBevelButton( paint, 0, 0, width(), height(), colorGroup(), isDown() );
  drawButtonLabel( paint );
  if ( hasFocus() ) 
  {
   	style().drawFocusRect( paint, style().bevelButtonRect( 0, 0, width(), height()),
		colorGroup(), &colorGroup().button() );
  }
#else
  style().drawPrimitive( QStyle::PE_ButtonBevel, paint, QRect(0, 0, width(), height()), colorGroup());
  drawButtonLabel( paint );
  if ( hasFocus() ) 
  {
    style().drawPrimitive( QStyle::PE_FocusRect, paint, style().subRect( QStyle::SR_PushButtonContents, this),
		colorGroup() );
  }
#endif
}

void FCColorButton::drawButtonLabel( QPainter *paint )
{
  QColor pen = isEnabled() ? hasFocus() ? palette().active().buttonText() : palette().inactive().buttonText()
		                                    : palette().disabled().buttonText();
  paint->setPen( pen );
	paint->setBrush( QBrush( col ) );

  paint->drawRect( width()/4, height()/4, width()/2, height()/2 );
}

void FCColorButton::onChooseColor()
{
  QColor c = QColorDialog::getColor( palette().active().background(), this );
  if ( c.isValid() )
  {
    setColor( c );
    emit changed();
  }
}

///////////////////////////////////////////////////////////////////////////////////

class FCSpinBoxPrivate
{
  public:
    FCSpinBoxPrivate();
    bool pressed;
    int nY, nStep;
};

FCSpinBoxPrivate::FCSpinBoxPrivate()
{
  pressed = false;
}

// -----------------------------------------------------------------------------------

FCSpinBox::FCSpinBox ( QWidget* parent, const char* name )
 : QSpinBox (parent, name)
{
  setMouseTracking(true);
  d = new FCSpinBoxPrivate;
}

FCSpinBox::FCSpinBox ( int minValue, int maxValue, int step, QWidget* parent, const char* name )
 : QSpinBox(minValue, maxValue, step, parent, name)
{
  setMouseTracking(true);
  d = new FCSpinBoxPrivate;
}

FCSpinBox::~FCSpinBox()
{
  delete d;
  d = 0L;
}

void FCSpinBox::mouseMoveEvent ( QMouseEvent* e )
{
  if (QWidget::mouseGrabber() == NULL && !rect().contains(e->pos()) && d->pressed )
    grabMouse(QCursor(IbeamCursor));

  if (QWidget::mouseGrabber() == this)
  {
    // get "speed" of mouse move
    int mult = d->nY - e->y();

    int nValue = value() + mult * d->nStep;
    if (nValue <= maxValue())
      setValue ( nValue );
    else
      setValue ( maxValue() );

    d->nY = e->y();
  }
  else
    QSpinBox::mouseMoveEvent(e);
}

void FCSpinBox::mousePressEvent   ( QMouseEvent* e )
{
  d->pressed = true;

  int nMax = maxValue();
  int nMin = minValue();

  if (nMax == INT_MAX || nMin == -INT_MAX)
  {
    d->nStep = 100;
  }
  else
  {
    int nRange = nMax - nMin;
    int nHeight = QApplication::desktop()->height();
    if (nRange > nHeight)
      d->nStep = int(nRange / nHeight);
    else
      d->nStep = 1;
  }

  d->nY = e->y();
}

void FCSpinBox::mouseReleaseEvent ( QMouseEvent* e )
{
  if (QWidget::mouseGrabber() == this)
    releaseMouse();
  d->pressed = false;
}

void FCSpinBox::focusOutEvent ( QFocusEvent* e )
{
  if (QWidget::mouseGrabber() == this)
    releaseMouse();
}

void FCSpinBox::wheelEvent ( QWheelEvent* e )
{
  if (isEnabled())
    QSpinBox::wheelEvent(e);
}

bool FCSpinBox::eventFilter ( QObject* o, QEvent* e )
{
  if ( o != editor() )
  	return false;

  // get the editor's mouse events
  switch (e->type())
  {
    // redirect the events to spin box (itself)
    case QEvent::MouseButtonPress:
      mousePressEvent ((QMouseEvent*)e);
      break;

    case QEvent::MouseButtonRelease:
      mouseReleaseEvent ((QMouseEvent*)e);
      break;

    case QEvent::MouseMove:
      mouseMoveEvent ((QMouseEvent*)e);
      break;

    case QEvent::FocusOut:
      focusOutEvent ((QFocusEvent*)e);
      break;

    default:
      break;
  }

  return QSpinBox::eventFilter(o, e);
}

// -------------------------------------------------------------

class FCFloatSpinBoxPrivate
{
  public:
    FCFloatSpinBoxPrivate(){}
    ~FCFloatSpinBoxPrivate()
    {
      delete m_pValidator; 
    }

    QDoubleValidator* m_pValidator;
    double            m_fDivisor;
    double            m_fEpsilon;
};

FCFloatSpinBox::FCFloatSpinBox ( QWidget * parent, const char * name )
: FCSpinBox(parent, name)
{
  d = new FCFloatSpinBoxPrivate;

  d->m_pValidator = new QDoubleValidator((double)minValue(), (double)maxValue(), 2, this, name );
  setValidator(d->m_pValidator);
  
  setDecimals(0);
}

FCFloatSpinBox::FCFloatSpinBox ( int minValue, int maxValue, int step, QWidget* parent, const char* name )
: FCSpinBox(minValue, maxValue, step, parent, name)
{
  d = new FCFloatSpinBoxPrivate;

  d->m_pValidator = new QDoubleValidator((double)minValue, (double)maxValue, 2, this, name );
  setValidator(d->m_pValidator);

  setDecimals(0);
}

FCFloatSpinBox::~FCFloatSpinBox() 
{
  delete d;
}

uint FCFloatSpinBox::decimals() const
{
  return d->m_pValidator->decimals();
}

void FCFloatSpinBox::setDecimals(uint i)
{
  d->m_pValidator->setDecimals(i);
  d->m_fDivisor = pow(10.0, double(i));
  d->m_fEpsilon = 1.0 / pow(10.0, double(i+1));
}

void FCFloatSpinBox::setMinValueFloat(double value)
{
	double fMax = d->m_fDivisor * value;
	fMax = std::max<double>(fMax, (double)-INT_MAX);
  FCSpinBox::setMinValue(int(fMax)); 
}

void FCFloatSpinBox::setMaxValueFloat(double value)
{
	double fMin = d->m_fDivisor * value;
	fMin = std::min<double>(fMin, (double)INT_MAX);
  FCSpinBox::setMaxValue(int(fMin)); 
}

double FCFloatSpinBox::minValueFloat () const
{
  return FCSpinBox::minValue() / double(d->m_fDivisor); 
}

double FCFloatSpinBox::maxValueFloat () const
{
  return FCSpinBox::maxValue() / double(d->m_fDivisor); 
}

QString FCFloatSpinBox::mapValueToText(int value)
{
  return QString::number(double(value) / d->m_fDivisor, 'f', d->m_pValidator->decimals()); 
}

int FCFloatSpinBox::mapTextToValue(bool* b)
{
  double fEps = value() > 0.0 ? d->m_fEpsilon : - d->m_fEpsilon;
  return int(text().toDouble() * d->m_fDivisor + fEps); 
}

void FCFloatSpinBox::valueChange()
{
  FCSpinBox::valueChange();
  emit valueFloatChanged( valueFloat() );
}

void FCFloatSpinBox::setValueFloat(double value)
{
  double fEps = value > 0.0 ? d->m_fEpsilon : - d->m_fEpsilon;
	double fValue = d->m_fDivisor * value + fEps;
	fValue = std::min<double>(fValue, (double) INT_MAX);
	fValue = std::max<double>(fValue, (double)-INT_MAX);
  FCSpinBox::setValue(int(fValue)); 
}

double FCFloatSpinBox::valueFloat() const
{
  return FCSpinBox::value() / double(d->m_fDivisor); 
}

void FCFloatSpinBox::stepChange () 
{
  FCSpinBox::stepChange();
}

// --------------------------------------------------------

FCAnimation* FCAnimation::_pcSingleton = 0L;

void FCAnimation::Destruct(void)
{
	// not initialized or double destruct!
  assert(_pcSingleton);
	delete _pcSingleton;
}

FCAnimation* FCAnimation::Instance(void)
{
	// not initialized?
	if(!_pcSingleton)
	{
		_pcSingleton = new FCAnimation(NULL, "Animation");
	}

  return _pcSingleton;
}

FCAnimation::FCAnimation(QWidget * parent, const char * name, WFlags f)
  : QLabel(parent, name, f)
{
  setFixedWidth(50);
  hide();
}

FCAnimation::~FCAnimation()
{
}

void FCAnimation::startAnimation()
{
#ifndef QGIF_H
  setText("Download");
#else
  setMovie(QMovie("trolltech.gif"));
#endif
#if QT_VERSION > 300
  setMovie(QMovie("trolltech.gif"));
#endif
}

void FCAnimation::stopAnimation()
{
  setText("");
}

#include "moc_Widgets.cpp"
