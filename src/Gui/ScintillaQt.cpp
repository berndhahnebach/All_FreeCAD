/***************************************************************************
                          ScintillaQt.cpp  -  description
                             -------------------
    begin                : 2002/12/20 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file ScintillaQt.cpp
 *  \brief A Qt port to the scintilla library
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
# include <qapplication.h>
# include <qscrollbar.h>
# include <qpopupmenu.h>
# include <qstring.h>
# include <qtimer.h>
# include <qclipboard.h>
# include <qdragobject.h>
# include <qpainter.h>
#endif

#include "ScintillaQt.h"
#include "DlgEditorImp.h"
#include "scintilla/SciLexer.h"

#include "SciEditor.h"

static QWidget *PWidget(WindowID id)
{
	return reinterpret_cast<QWidget *>(id);
}

static QWidget *PWidget(Window &w) 
{
	return PWidget(w.GetID());
}

ScintillaQt::ScintillaQt(SciTEBase* edit) 
 : m_pEditor(edit)
{
	view = new QWidget(edit,0,WRepaintNoErase|WResizeNoErase);
	view->setMouseTracking(TRUE);
	view->setAcceptDrops(TRUE);

	m_pEditor->addChild(view);
	m_pEditor->setResizePolicy(QScrollView::AutoOneFit);
	connect(m_pEditor->verticalScrollBar(),SIGNAL(valueChanged(int)),SLOT(onVScroll(int)));
	connect(m_pEditor->horizontalScrollBar(),SIGNAL(valueChanged(int)),SLOT(onHScroll(int)));

	view->installEventFilter(this);
	lastKeyDownConsumed = false;

	capturedMouse = false;
	linesPerScroll = 0;
	wheelDelta = 0;   // Wheel delta from roll

	hasOKText = false;

  wMain = view;

	connect(&m_clTimer,SIGNAL(timeout()),SLOT(onTimer()));
	
	Initialise();
}

ScintillaQt::~ScintillaQt()
{ 
}

void ScintillaQt::onVScroll(int val)
{
	ScrollTo(val);
}

void ScintillaQt::onHScroll(int val)
{
	HorizontalScrollTo(val);
}

void ScintillaQt::showHScroll(bool show)
{
	if (show)
		m_pEditor->setHScrollBarMode(QScrollView::AlwaysOn);
	else
		m_pEditor->setHScrollBarMode(QScrollView::AlwaysOff);
}

void ScintillaQt::showVScroll(bool show)
{
	if (show && m_pEditor->vScrollBarMode() != QScrollView::AlwaysOn)
		m_pEditor->setVScrollBarMode(QScrollView::AlwaysOn);
	else if (!show && m_pEditor->vScrollBarMode() != QScrollView::AlwaysOff)
		m_pEditor->setVScrollBarMode(QScrollView::AlwaysOff);
}

QSize ScintillaQt::sizeHint() const
{
	int height = vs.lineHeight * pdoc->LinesTotal();

	if (horizontalScrollBarVisible)
		height += m_pEditor->horizontalScrollBar()->sizeHint().height();

	return QSize(scrollWidth,height);
}

void ScintillaQt::Initialise()
{
	SetTicking(true);
}

void ScintillaQt::Finalise()
{
	ScintillaBase::Finalise();
	SetTicking(false);
}

void ScintillaQt::StartDrag()
{
	QDragObject *dobj = new QTextDrag(convertText(&drag),PWidget(wMain));

	if (dobj->drag() && dobj->target() != PWidget(wMain))
	{
		drag.Set(0,0);
		ClearSelection();
	}
}

sptr_t ScintillaQt::WndProc(unsigned int iMessage,uptr_t wParam,sptr_t lParam)
{
	switch (iMessage)
	{
  	case SCI_GRABFOCUS:
    {
		  PWidget(wMain)->setFocus();
		  return 0;
    }

  	case SCI_GETDIRECTFUNCTION:
    {
      return reinterpret_cast<sptr_t>(DirectFunction);
    }
	
  	case SCI_GETDIRECTPOINTER:
    {
      return reinterpret_cast<sptr_t>(this);
    }
	}

	return ScintillaBase::WndProc(iMessage,wParam,lParam);
}

sptr_t ScintillaQt::DefWndProc(unsigned int,uptr_t,sptr_t)
{
	return 0;
}

void ScintillaQt::SetTicking(bool on)
{
	if (timer.ticking != on)
	{
		timer.ticking = on;

		if (timer.ticking)
			m_clTimer.start(timer.tickSize);
		else
			m_clTimer.stop();
	}

	timer.ticksToWait = caret.period;
}

void ScintillaQt::SetMouseCapture(bool on)
{
	if (mouseDownCaptures)
  {
		if (on)
		{
			PWidget(wMain)->grabMouse();
			PWidget(wMain)->grabKeyboard();
		}
		else
		{
			PWidget(wMain)->releaseMouse();
			PWidget(wMain)->releaseKeyboard();
		}
  }

	capturedMouse = on;
}

bool ScintillaQt::HaveMouseCapture()
{
	return capturedMouse;
}

void ScintillaQt::ScrollText(int linesToMove)
{
  ScintillaBase::ScrollText(linesToMove);
}

void ScintillaQt::SetVerticalScrollPos()
{
	m_pEditor->verticalScrollBar()->setValue(topLine);
}

void ScintillaQt::SetHorizontalScrollPos()
{
	m_pEditor->horizontalScrollBar()->setValue(xOffset);
}

bool ScintillaQt::ModifyScrollBars(int nMax,int nPage)
{
	m_pEditor->verticalScrollBar()->setMinValue(0);
	m_pEditor->verticalScrollBar()->setMaxValue(nMax - nPage + 1);
	m_pEditor->verticalScrollBar()->setLineStep(1);
	m_pEditor->verticalScrollBar()->setPageStep(nPage);

	m_pEditor->horizontalScrollBar()->setMinValue(0);
	m_pEditor->horizontalScrollBar()->setMaxValue(scrollWidth);
	m_pEditor->horizontalScrollBar()->setPageStep(scrollWidth / 10);

	return true;
}

void ScintillaQt::NotifyChange()
{
}

void ScintillaQt::NotifyFocus(bool focus)
{
  ScintillaBase::NotifyFocus(focus);
}

int ScintillaQt::GetCtrlID()
{
  return ScintillaBase::GetCtrlID();
}

void ScintillaQt::NotifyParent(SCNotification scn)
{
  switch (scn.nmhdr.code)
  {
	  case SCN_MARGINCLICK:
      m_pEditor->clickMargin(scn.position,scn.modifiers);
		  break;
	
    case SCN_MODIFIED:
		  if (0 != (scn.modificationType & SC_MOD_CHANGEFOLD)) 
      {
			  m_pEditor->foldChanged(scn.line, scn.foldLevelNow, scn.foldLevelPrev);
		  }
		  break;

    case SCN_DOUBLECLICK:
		  break;
  }
}

void ScintillaQt::NotifyDoubleClick(Point pt, bool shift) 
{
	ScintillaBase::NotifyDoubleClick(pt, shift);
}

void ScintillaQt::Copy()
{
	if (currentPos != anchor)
	{
		SelectionText text;
		CopySelectionRange(&text);
		QApplication::clipboard()->setText(convertText(&text));
	}
}

bool ScintillaQt::CanPaste()
{
  return ScintillaBase::CanPaste();
}

void ScintillaQt::Paste()
{
	pdoc->BeginUndoAction();
  int len;
	QString text = QApplication::clipboard()->text();
	ClearSelection();

	if (IsUnicodeMode())
	{
		QCString s = text.utf8();
		len = s.length();
		if (len)
			pdoc->InsertString(currentPos,s.data(),len);
	}
	else
	{
		const char *s = text.latin1();
		len = (s ? strlen(s) : 0);
		if (len)
			pdoc->InsertString(currentPos,s,len);
	}

	SetEmptySelection(currentPos + len);
	pdoc->EndUndoAction();
	NotifyChange();
	Redraw();
}

void ScintillaQt::CreateCallTipWindow(PRectangle rc)
{
	if (ct.wCallTip.Created() == false)
		ct.wCallTip = ct.wDraw = new ScintCallTip(&ct, this, m_pEditor);

	PWidget(ct.wCallTip)->resize(rc.Width(),rc.Height());
	ct.wCallTip.Show();
}

void ScintillaQt::AddToPopUp(const char *label,int cmd,bool enabled)
{
	QPopupMenu* menu = static_cast<QPopupMenu *>(popup.GetID());

	if (label[0] != '\0')
	{
		menu->insertItem(QObject::tr(label),this,SLOT(onContextMenu(int)),0,cmd);
		menu->setItemEnabled(cmd,enabled);
	}
	else
  {
		menu->insertSeparator();
  }
}

void ScintillaQt::ClaimSelection()
{
	// does not have a primary selection
}

void ScintillaQt::onContextMenu(int id)
{
	Command(id);
}

void ScintillaQt::onTimer()
{
	Tick();
}

void ScintillaQt::dragEnterEvent(QDragEnterEvent* e)
{
	e->accept(QTextDrag::canDecode(e));
}

void ScintillaQt::dragMoveEvent(QDragMoveEvent* e)
{
	inDragDrop = true;
	e->acceptAction();
	SetDragPosition(PositionFromLocation(Point(e->pos().x(),e->pos().y())));
}

void ScintillaQt::dragLeaveEvent(QDragLeaveEvent* e)
{
	SetDragPosition(invalidPosition);
}

void ScintillaQt::dropEvent(QDropEvent* e)
{
	QString text;

	if (QTextDrag::decode(e,text))
	{
		bool moving = (e->source() == PWidget(wMain) && e->action() == QDropEvent::Move);

		if (moving)
			e->acceptAction();

		const char *s;

		if (IsUnicodeMode())
			s = text.utf8();
		else
			s = text.latin1();

		DropAt(posDrop,s,moving,false);
		SetDragPosition(invalidPosition);
		Redraw();
	}
}

sptr_t ScintillaQt::DirectFunction(ScintillaQt *sci,unsigned int iMessage, uptr_t wParam,sptr_t lParam)
{
	return sci->WndProc(iMessage,wParam,lParam);
}

int ScintillaQt::KeyDefault(int key,int modifiers)
{
	AddChar(key);
	return 1;
}

QString ScintillaQt::convertText(SelectionText* text)
{
	QString str;

	if (text->s)
  {
		if (IsUnicodeMode())
			str = QString::fromUtf8(text->s);
		else
			str.setLatin1(text->s);
  }

	return str;
}

bool ScintillaQt::event(QEvent* e)
{
	switch (e->type())
	{
	  case QEvent::KeyPress:
		  keyPressEvent(static_cast<QKeyEvent *>(e));
		  break;
	}

	return false;
}

bool ScintillaQt::eventFilter(QObject* o, QEvent* e)
{
	bool used = TRUE;

	switch (e->type())
	{
	  case QEvent::Paint:
		  paintEvent(static_cast<QPaintEvent *>(e));
		  break;

	  case QEvent::Resize:
		  resizeEvent(static_cast<QResizeEvent *>(e));
		  break;

	  case QEvent::MouseButtonPress:
			if (m_clClick.restart() < (int)Platform::DoubleClickTime())
			  mouseDoubleClickEvent(static_cast<QMouseEvent *>(e));
			else
			  mousePressEvent(static_cast<QMouseEvent *>(e));
		  break;

	  case QEvent::MouseButtonRelease:
		  mouseReleaseEvent(static_cast<QMouseEvent *>(e));
		  break;

	  case QEvent::MouseButtonDblClick:
		  mouseDoubleClickEvent(static_cast<QMouseEvent *>(e));
		  break;

	  case QEvent::MouseMove:
		  mouseMoveEvent(static_cast<QMouseEvent *>(e));
		  break;

	  case QEvent::DragEnter:
		  dragEnterEvent(static_cast<QDragEnterEvent *>(e));
		  break;

	  case QEvent::DragMove:
		  dragMoveEvent(static_cast<QDragMoveEvent *>(e));
		  break;

	  case QEvent::DragLeave:
		  dragLeaveEvent(static_cast<QDragLeaveEvent *>(e));
		  break;

	  case QEvent::Drop:
		  dropEvent(static_cast<QDropEvent *>(e));
		  break;

	  case QEvent::Wheel:
		  mouseWheelEvent(static_cast<QWheelEvent *>(e));
		  break;

	  default:
		  used = FALSE;
	}

	return used;
}

void ScintillaQt::paintEvent(QPaintEvent* e)
{
	bool isUnicodeMode = (pdoc && pdoc->dbcsCodePage == SC_CP_UTF8);

	paintState = painting;

	const QRect& rect = e->rect();

	rcPaint.left = rect.left();
	rcPaint.top = rect.top();
	rcPaint.right = rect.right() + 1;
	rcPaint.bottom = rect.bottom() + 1;

	PRectangle rcText = GetTextRectangle();
	paintingAllText = rcPaint.Contains(rcText);

	Surface *sw = Surface::Allocate();

	if (sw)
	{
		QPainter painter(PWidget(wMain));
		sw->Init(&painter,0);
		sw->SetUnicodeMode(isUnicodeMode);
		Paint(sw,rcPaint);
		sw->Release();

		if (paintState == paintAbandoned)
		{
			paintState = painting;
			rcPaint = GetTextRectangle();
			paintingAllText = true;
			painter.end();
			painter.begin(PWidget(wMain));
			sw->Init(&painter,0);
			sw->SetUnicodeMode(isUnicodeMode);
			Paint(sw,rcPaint);
			sw->Release();
		}

		delete sw;
	}

	paintState = notPainting;
}

void ScintillaQt::resizeEvent(QResizeEvent * e)
{
	ChangeSize();
}

void ScintillaQt::keyPressEvent(QKeyEvent * e)
{
  unsigned key;

	switch (e->key())
	{
	  case Key_Down:
		  key = SCK_DOWN;
		  break;

	  case Key_Up:
		  key = SCK_UP;
		  break;

	  case Key_Left:
		  key = SCK_LEFT;
		  break;

	  case Key_Right:
		  key = SCK_RIGHT;
		  break;

	  case Key_Home:
		  key = SCK_HOME;
		  break;

	  case Key_End:
		  key = SCK_END;
		  break;

	  case Key_Prior:
		  key = SCK_PRIOR;
		  break;

	  case Key_Next:
		  key = SCK_NEXT;
		  break;

	  case Key_Delete:
		  key = SCK_DELETE;
		  break;

	  case Key_Insert:
		  key = SCK_INSERT;
		  break;

	  case Key_Escape:
		  key = SCK_ESCAPE;
		  break;

	  case Key_Backspace:
		  key = SCK_BACK;
		  break;

	  case Key_Tab:
		  key = SCK_TAB;
		  break;

	  case Key_Return:
		  key = SCK_RETURN;
		  break;

	  default:
		  if (IsUnicodeMode())
		  {
			  if (e->text().length() == 1)
			  {
				  if ((key = e->text()[0].unicode()) >= 0x80)
					  key = 0;
			  }
			  else
				  key = 0;
		  }
		  else
		  {
			  key = e->ascii();

			  if (key >= 0x01 && key <= 0x1f)
				  key += 0x40;
		  }
	}

	bool consumed = FALSE;

	if (key)
	{
		bool shift = e->state() & Qt::ShiftButton;
		bool ctrl  = e->state() & Qt::ControlButton;
		bool alt   = e->state() & Qt::AltButton;

		KeyDown(key,shift,ctrl,alt,&consumed);
	}
	else if (IsUnicodeMode())
	{
		if (e->text().length() > 0 && !e->text()[0].isNull())
		{
			QCString s = e->text().utf8();

			AddCharUTF(s.data(),s.length());

			consumed = TRUE;
		}
	}

	if (!consumed)
		e->ignore();
}

void ScintillaQt::mouseWheelEvent(QWheelEvent * e)
{
	m_pEditor->setFocus();

	if (e->state() & Qt::ShiftButton)
		QApplication::sendEvent(m_pEditor->horizontalScrollBar(), e);
	else 
		QApplication::sendEvent(m_pEditor->verticalScrollBar(), e);
}

void ScintillaQt::mousePressEvent(QMouseEvent * e)
{
	m_pEditor->setFocus();

	Point pt(e->x(), e->y());

	switch (e->button())
	{
	case Qt::LeftButton:
		{
			lastClickTime = 0;
			unsigned iClickTime = Platform::DoubleClickTime() + 1;

			bool shift = e->state() & Qt::ShiftButton;
			bool ctrl  = e->state() & Qt::ControlButton;
			bool alt   = e->state() & Qt::AltButton;

			ButtonDown(pt, iClickTime, shift, ctrl, alt);
			break;
		}
    // do the context menu stuff here
	case Qt::RightButton:
    {
      ContextMenu(Point(e->globalX(), e->globalY()));    
    }

  	default:
	  	break;
	}
}

void ScintillaQt::mouseReleaseEvent(QMouseEvent * e)
{
	if (HaveMouseCapture() && e->button() == Qt::LeftButton)
	{
		bool ctrl = e->state() & Qt::ControlButton;

		ButtonUp(Point(e->x(),e->y()),0,ctrl);
	}
}

void ScintillaQt::mouseDoubleClickEvent (QMouseEvent * e)
{
	m_pEditor->setFocus();

	if (e->button() == Qt::LeftButton)
	{
		lastClickTime = 0;
		unsigned thisClickTime = Platform::DoubleClickTime() - 1;

		bool shift = e->state() & Qt::ShiftButton;
		bool ctrl  = e->state() & Qt::ControlButton;
		bool alt   = e->state() & Qt::AltButton;

		ButtonDown(Point(e->x(),e->y()),thisClickTime,shift,ctrl,alt);
	}
}

void ScintillaQt::mouseMoveEvent(QMouseEvent * e)
{
	ButtonMove(Point(e->x(),e->y()));
}

void ScintillaQt::timerEvent(QTimerEvent * e)
{
}

ScintillaQt::ScintCallTip::ScintCallTip(CallTip* ct, ScintillaQt* sci, QWidget* parent, const char * name) 
: QWidget(parent,0,Qt::WType_TopLevel|Qt::WStyle_NoBorder|Qt::WStyle_StaysOnTop|Qt::WStyle_Tool|Qt::WX11BypassWM), sciTE(sci), callTip(ct)
{
	setFocusProxy(parent);
}

ScintillaQt::ScintCallTip::~ScintCallTip()
{
	setFocusProxy(0);
}

void ScintillaQt::ScintCallTip::paintEvent(QPaintEvent* e)
{
	Surface *surfaceWindow = Surface::Allocate();

  if (surfaceWindow)
	{
		QPainter p(this);
		surfaceWindow->Init(&p,0);
		callTip->PaintCT(surfaceWindow);
		surfaceWindow->Release();
		delete surfaceWindow;
	}
}

void ScintillaQt::ScintCallTip::mousePressEvent(QMouseEvent* e)
{
	Point pt;

	pt.x = e->x();
	pt.y = e->y();
	callTip->MouseClick(pt);
//	sciTE->CallTipClick();
}

#include "moc_ScintillaQt.cpp"
