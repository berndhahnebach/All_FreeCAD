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
#include "Application.h"
#include "scintilla/SciLexer.h"


static QWidget *PWidget(WindowID id)
{
	return reinterpret_cast<QWidget *>(id);
}

static QWidget *PWidget(Window &w) 
{
	return PWidget(w.GetID());
}

ScintillaQt::ScintillaQt(FCScintEditor* edit) 
 : m_pEditor(edit)
{
	lastKeyDownConsumed = false;

	capturedMouse = false;
	linesPerScroll = 0;
	wheelDelta = 0;   // Wheel delta from roll

	hasOKText = false;

  wMain = m_pEditor->view;

	m_pEditor->connect(&m_clTimer,SIGNAL(timeout()),SLOT(slotTimer()));
	
	Initialise();
}

ScintillaQt::~ScintillaQt()
{ 
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
	QDragObject *dobj = new QTextDrag(convertText(&drag),m_pEditor->view);

	if (dobj->drag() && dobj->target() != m_pEditor->view)
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
	m_pEditor->verScroll->setValue(topLine);
}

void ScintillaQt::SetHorizontalScrollPos()
{
	m_pEditor->horScroll->setValue(xOffset);
}

bool ScintillaQt::ModifyScrollBars(int nMax,int nPage)
{
	m_pEditor->verScroll->setMinValue(0);
	m_pEditor->verScroll->setMaxValue(nMax - nPage + 1);
	m_pEditor->verScroll->setLineStep(1);
	m_pEditor->verScroll->setPageStep(nPage);

	m_pEditor->horScroll->setMinValue(0);
	m_pEditor->horScroll->setMaxValue(scrollWidth);
	m_pEditor->horScroll->setPageStep(scrollWidth / 10);

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
		ct.wCallTip = ct.wDraw = new FCScintCallTip(this, m_pEditor);

	PWidget(ct.wCallTip)->resize(rc.Width(),rc.Height());
	ct.wCallTip.Show();
}

void ScintillaQt::AddToPopUp(const char *label,int cmd,bool enabled)
{
	QPopupMenu* menu = static_cast<QPopupMenu *>(popup.GetID());

	if (label[0] != '\0')
	{
		menu->insertItem(label,m_pEditor,SLOT(slotContextMenu(int)),0,cmd);
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
		bool moving = (e->source() == m_pEditor->view && e->action() == QDropEvent::Move);

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

static bool lexersLinked = FALSE;

FCScintEditor::FCScintEditor(QWidget *parent,const char *name,WFlags f)
	: QWidget(parent,name,f), sciTE(0L)
{
	QGridLayout *layout = new QGridLayout(this,2,2);

	view = new QWidget(this,0,WRepaintNoErase|WResizeNoErase);
	view->setMouseTracking(TRUE);
	view->setAcceptDrops(TRUE);
	layout->addWidget(view,0,0);

	verScroll = new QScrollBar(Vertical,this);
	layout->addWidget(verScroll,0,1);
	connect(verScroll,SIGNAL(valueChanged(int)),SLOT(slotVerScroll(int)));

	horScroll = new QScrollBar(Horizontal,this);
	layout->addWidget(horScroll,1,0);
	connect(horScroll,SIGNAL(valueChanged(int)),SLOT(slotHorScroll(int)));

	view->installEventFilter(this);

	setFocusPolicy(WheelFocus);

	sciTE = new ScintillaQt(this);

	SendScintilla(SCI_SETCARETPERIOD,QApplication::cursorFlashTime() / 2);

	if (!lexersLinked)
	{
		Scintilla_LinkLexers();

		lexersLinked = TRUE;
	}

  
   const char font[] = "Verdana";
   const char monospace[] = "Courier";
   const short fontsize = 9;
   const char keywords[] = "and assert break class continue def del elif else except exec "
	                         "finally for from global if import in is lambda None not or "
	                         "pass print raise return try while yield";

   // set style bits, choose the right lexer (Lua) and set the keywords list
   sciTE->WndProc(SCI_SETSTYLEBITS,5,0);
   sciTE->WndProc(SCI_SETLEXER,SCLEX_PYTHON,0);
   sciTE->WndProc(SCI_SETKEYWORDS,0,(sptr_t)keywords);
   
   // set up basic features (iguides on, tab=3, tabs-to-spaces, EOL=CRLF)
   sciTE->WndProc(SCI_SETINDENTATIONGUIDES,1,0);
   sciTE->WndProc(SCI_SETTABWIDTH,3,0);
   sciTE->WndProc(SCI_SETUSETABS,0,0);
   sciTE->WndProc(SCI_SETEOLMODE,SC_EOL_CRLF,0);

   // now set up the styles (remember you have to set up font name for each style;
   // if you fail to do so, bold/italics will not work (only color will work)
   // !!colors are in format BGR!!

   // style 32: default
   sciTE->WndProc(SCI_STYLESETFONT,32, (sptr_t) font);
   sciTE->WndProc(SCI_STYLESETSIZE,32, fontsize);
   // style 0: whitespace
   sciTE->WndProc(SCI_STYLESETFORE,0, 0x808080);
   // style 1: comment (not used in Lua)
   // style 2: line comment (green)
   sciTE->WndProc(SCI_STYLESETFONT,2, (int)monospace);
   sciTE->WndProc(SCI_STYLESETSIZE,2, fontsize);
   sciTE->WndProc(SCI_STYLESETFORE,2, 0x00AA00);
   // style 3: doc comment (grey???)
   sciTE->WndProc(SCI_STYLESETFORE,3, 0x7F7F7F);      
   // style 4: numbers (blue)
   sciTE->WndProc(SCI_STYLESETFORE,4, 0xFF0000);
   // style 5: keywords (black bold)
   sciTE->WndProc(SCI_STYLESETFONT,5, (int)font);
   sciTE->WndProc(SCI_STYLESETSIZE,5, (int)fontsize);
   sciTE->WndProc(SCI_STYLESETFORE,5, 0x000000);
   sciTE->WndProc(SCI_STYLESETBOLD,5, 1);
   // style 6: double qouted strings (???)
   sciTE->WndProc(SCI_STYLESETFORE,6, 0x7F007F);
   // style 7: single quoted strings (???)
   sciTE->WndProc(SCI_STYLESETFORE,7, 0x7F007F);
   // style 8: UUIDs (IDL only, not used in Lua)
   // style 9: preprocessor directives (not used in Lua 4)
   // style 10: operators (black bold)
   sciTE->WndProc(SCI_STYLESETFONT,10, (int)font);
   sciTE->WndProc(SCI_STYLESETSIZE,10, fontsize);
   sciTE->WndProc(SCI_STYLESETFORE,10, 0x000000);
   sciTE->WndProc(SCI_STYLESETBOLD,10, 1);
   // style 11: identifiers (leave to default)
   // style 12: end of line where string is not closed (black on violet, eol-filled)
   sciTE->WndProc(SCI_STYLESETFORE,12, 0x000000);
   sciTE->WndProc(SCI_STYLESETBACK,12, 0xE0C0E0);
   sciTE->WndProc(SCI_STYLESETEOLFILLED,12, 1);
}

FCScintEditor::~FCScintEditor()
{
	delete sciTE;
}

void FCScintEditor::openFile(const char* fileName)
{
	sciTE->WndProc(SCI_CLEARALL, 0, 0);
	sciTE->WndProc(SCI_SETSAVEPOINT, 0, 0);
  sciTE->WndProc(SCI_CANCEL, 0, 0);
  sciTE->WndProc(SCI_SETUNDOCOLLECTION, 0, 0);

  QString line;
  QFile file(fileName);
   
  if( !file.open(IO_ReadOnly))
     return;

  QTextStream in(&file);
  QString text;

  while( !in.atEnd() ){
    line = in.readLine();
    text = line + "\n";
    sciTE->WndProc(SCI_ADDTEXT, text.length(), reinterpret_cast<sptr_t>(static_cast<const char *>(text.latin1())));
  }


  file.close();

  sciTE->WndProc(SCI_SETUNDOCOLLECTION, 1, 0);
	sciTE->WndProc(SCI_SETSAVEPOINT, 0, 0);
	sciTE->WndProc(SCI_GOTOPOS, 0, 0);
}

long FCScintEditor::SendScintilla(unsigned int msg,unsigned long wParam, long lParam)
{
	return sciTE->WndProc(msg,wParam,lParam);
}

bool FCScintEditor::eventFilter(QObject* o, QEvent* e)
{
	if (o != view)
		return QWidget::eventFilter(o,e);

	bool used = TRUE;

	switch (e->type())
	{
	  case QEvent::Paint:
		  sciTE->paintEvent(static_cast<QPaintEvent *>(e));
		  break;

	  case QEvent::Resize:
		  sciTE->ChangeSize();
		  break;

	  case QEvent::MouseButtonPress:
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
		  sciTE->dragEnterEvent(static_cast<QDragEnterEvent *>(e));
		  break;

	  case QEvent::DragMove:
		  sciTE->dragMoveEvent(static_cast<QDragMoveEvent *>(e));
		  break;

	  case QEvent::DragLeave:
		  sciTE->dragLeaveEvent(static_cast<QDragLeaveEvent *>(e));
		  break;

	  case QEvent::Drop:
		  sciTE->dropEvent(static_cast<QDropEvent *>(e));
		  break;

	  case QEvent::Wheel:
		  mouseWheelEvent(static_cast<QWheelEvent *>(e));
		  break;

	  default:
		  used = FALSE;
	}

	return used;
}

void FCScintEditor::slotTimer()
{
	sciTE->Tick();
}

void FCScintEditor::slotContextMenu(int cmd)
{
	sciTE->Command(cmd);
}

void FCScintEditor::focusInEvent(QFocusEvent * e)
{
	sciTE->SetFocusState(true);
}

void FCScintEditor::focusOutEvent(QFocusEvent * e)
{
	sciTE->SetFocusState(false);
}

void FCScintEditor::mousePressEvent(QMouseEvent * e)
{
	setFocus();

	Point pt(e->x(), e->y());

	switch (e->button())
	{
  	case LeftButton:
		{
			sciTE->lastClickTime = 0;
			unsigned iClickTime = Platform::DoubleClickTime() + 1;

			bool shift = e->state() & ShiftButton;
			bool ctrl  = e->state() & ControlButton;
			bool alt   = e->state() & AltButton;

			sciTE->ButtonDown(pt, iClickTime, shift, ctrl, alt);
			break;
		}
    // do the context menu stuff here
    case RightButton:
    {
      sciTE->ContextMenu(Point(e->globalX(), e->globalY()));    
    }

  	default:
	  	break;
	}
}

void FCScintEditor::mouseReleaseEvent(QMouseEvent * e)
{
	if (sciTE->HaveMouseCapture() && e->button() == LeftButton)
	{
		bool ctrl = e->state() & ControlButton;

		sciTE->ButtonUp(Point(e->x(),e->y()),0,ctrl);
	}
}

void FCScintEditor::mouseMoveEvent(QMouseEvent * e)
{
	sciTE->ButtonMove(Point(e->x(),e->y()));
}

void FCScintEditor::mouseDoubleClickEvent(QMouseEvent * e)
{
	setFocus();

	if (e->button() == LeftButton)
	{
		sciTE->lastClickTime = 0;
		unsigned thisClickTime = Platform::DoubleClickTime() - 1;

		bool shift = e->state() & ShiftButton;
		bool ctrl  = e->state() & ControlButton;
		bool alt   = e->state() & AltButton;

		sciTE->ButtonDown(Point(e->x(),e->y()),thisClickTime,shift,ctrl,alt);
	}
}

void FCScintEditor::keyPressEvent(QKeyEvent * e)
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
		  if (sciTE->IsUnicodeMode())
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
		bool shift = e->state() & ShiftButton;
		bool ctrl  = e->state() & ControlButton;
		bool alt   = e->state() & AltButton;

		sciTE->KeyDown(key,shift,ctrl,alt,&consumed);
	}
	else if (sciTE->IsUnicodeMode())
	{
		if (e->text().length() > 0 && !e->text()[0].isNull())
		{
			QCString s = e->text().utf8();

			sciTE->AddCharUTF(s.data(),s.length());

			consumed = TRUE;
		}
	}

	if (!consumed)
		e->ignore();
}

void FCScintEditor::mouseWheelEvent(QWheelEvent * e)
{
	setFocus();

	if (e->state() & ShiftButton)
		QApplication::sendEvent(horScroll, e);
	else 
		QApplication::sendEvent(verScroll, e);
}

void FCScintEditor::slotVerScroll(int val)
{
	sciTE->ScrollTo(val);
}

void FCScintEditor::slotHorScroll(int val)
{
	sciTE->HorizontalScrollTo(val);
}

QSize FCScintEditor::sizeHint() const
{
	int height = sciTE->vs.lineHeight * sciTE->pdoc->LinesTotal();

	if (sciTE->horizontalScrollBarVisible)
		height += horScroll->sizeHint().height();

	return QSize(sciTE->scrollWidth,height);
}

ScintillaQt::FCScintCallTip::FCScintCallTip(ScintillaQt* sci, QWidget* parent, const char * name) 
: QWidget(parent,0,Qt::WType_TopLevel|Qt::WStyle_NoBorder|Qt::WStyle_StaysOnTop|Qt::WStyle_Tool|Qt::WX11BypassWM), sciTE(sci)
{
	setFocusProxy(parent);
}

ScintillaQt::FCScintCallTip::~FCScintCallTip()
{
	setFocusProxy(0);
}

void ScintillaQt::FCScintCallTip::paintEvent(QPaintEvent* e)
{
	Surface *surfaceWindow = Surface::Allocate();

  if (surfaceWindow)
	{
		QPainter p(this);
		surfaceWindow->Init(&p,0);
		sciTE->ct.PaintCT(surfaceWindow);
		surfaceWindow->Release();
		delete surfaceWindow;
	}
}

void ScintillaQt::FCScintCallTip::mousePressEvent(QMouseEvent* e)
{
	Point pt;

	pt.x = e->x();
	pt.y = e->y();
	sciTE->ct.MouseClick(pt);
	sciTE->CallTipClick();
}

///////////////////////////////////////////////////////////////////////////////////////

FCScintEditView::FCScintEditView(FCScintillaDoc* pcDoc, QWidget* parent, const char* name)
: FCFloatingView(pcDoc, parent, name)
{
  FCScintillaView* pcScintView = new FCScintillaView(this, ApplicationWindow::Instance,"EditView");
  pcScintView->setCaption("Editor");
	pcScintView->setTabCaption("Editor");
	pcScintView->resize( 400, 300 );

  view = new FCScintEditor(_pcFrame);
  ApplicationWindow::Instance->addWindow(pcScintView);
}

FCScintEditView::~FCScintEditView()
{
}

///////////////////////////////////////////////////////////////////////////////////////

FCScintillaView::FCScintillaView( FCFloatingView* pcView, QWidget* parent, const char* name)
  : FCFloatingChildView( pcView, parent, name )
{
}

FCScintillaView::~FCScintillaView()
{
}

void FCScintillaView::closeEvent(QCloseEvent *e)
{
  GetActiveView()->GetDocument()->CanClose(e);
  if (e->isAccepted ())
  {
    FCFloatingChildView::closeEvent(e);
  }
}

///////////////////////////////////////////////////////////////////////////////////////

FCScintillaDoc::FCScintillaDoc()
{
}

FCScintillaDoc::~FCScintillaDoc()
{
}

void FCScintillaDoc::CreateView(const char* name)
{
  view = new FCScintEditView(this, 0L, name);
}

bool FCScintillaDoc::Save(void)
{
  return true;
}

bool FCScintillaDoc::SaveAs(void)
{
  return true;
}

bool FCScintillaDoc::Open(void)
{
	QString name = QFileDialog::getOpenFileName( QString::null, "Macro files (*.py *.FCMacro);;Python (*.py);;FreeCAD macro (*.FCMacro)", view);
	if ( name.isEmpty() ) 
    return false;

  ((FCScintEditView*)view)->GetEditor()->openFile(name.latin1());
   return true;
}

#include "moc_ScintillaQt.cpp"
