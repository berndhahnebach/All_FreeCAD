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
#include "DlgEditorImp.h"
#include "scintilla/SciLexer.h"


static QWidget *PWidget(WindowID id)
{
	return reinterpret_cast<QWidget *>(id);
}

static QWidget *PWidget(Window &w) 
{
	return PWidget(w.GetID());
}

ScintillaQt::ScintillaQt(FCScintillaEdit* edit) 
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

static bool lexersLinked = FALSE;

FCScintillaEdit::FCScintillaEdit(QWidget *parent,const char *name,WFlags f)
	: QWidget(parent,name,f), sciTE(0L), _bControlButton(false)
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

  GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Windows/Editor")->Attach(this);
  loadSettings();
}

void FCScintillaEdit::DefineMarker(int marker, int markerType, long fore, long back) 
{
	SendScintilla(SCI_MARKERDEFINE, marker, markerType);
	SendScintilla(SCI_MARKERSETFORE, marker, fore);
	SendScintilla(SCI_MARKERSETBACK, marker, back);
}

FCScintillaEdit::~FCScintillaEdit()
{
  GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Windows/Editor")->Detach(this);
	delete sciTE;
}

void FCScintillaEdit::OnChange(FCSubject<const char*> &rCaller,const char* rcReason)
{
  FCParameterGrp& rclGrp = ((FCParameterGrp&)rCaller);
  
  const char font[] = "Verdana";
  const char monospace[] = "Courier";
  const short fontsize = rclGrp.GetGroup("FontSize")->GetInt("currentItem", 3) + 6;

  // style 32: default
  SendScintilla(SCI_STYLESETFONT,32, (sptr_t) font);
  SendScintilla(SCI_STYLESETSIZE,32, fontsize);
  
  // set up basic features (iguides on, tab=3, tabs-to-spaces, EOL=CRLF)
  SendScintilla(SCI_SETINDENTATIONGUIDES,1,0);
  SendScintilla(SCI_SETTABWIDTH,3,0);
  SendScintilla(SCI_SETUSETABS,0,0);
  SendScintilla(SCI_SETEOLMODE,SC_EOL_CRLF,0);

  if (strcmp(rcReason, "Lexer") == 0)
  {
    int l = rclGrp.GetInt("Lexer");

    if (l == 0)
    {
      const char keywords[] = "and assert break class continue def del elif else except exec "
	                            "finally for from global if import in is lambda None not or "
	                            "pass print raise return try while yield";
      // set style bits, choose the lexer and set the keywords list
      SendScintilla(SCI_SETSTYLEBITS,5,0);
      SendScintilla(SCI_SETLEXER,SCLEX_PYTHON,0);
      SendScintilla(SCI_SETKEYWORDS,0,(sptr_t)keywords);
    }
    else if (l == 1)
    {
      const char keywords[] = "XML";
      SendScintilla(SCI_SETSTYLEBITS,5,0);
      SendScintilla(SCI_SETLEXER,SCLEX_XML,0);
      SendScintilla(SCI_SETKEYWORDS,0,(sptr_t)keywords);
    }
    else
    {
      SendScintilla(SCI_SETLEXER,SCLEX_NULL,0);
    }
  }
  else if (strcmp(rcReason, "Text") == 0)
  {
    // text color
    SendScintilla(SCI_STYLESETFORE,SciQt_TEXT, rclGrp.GetInt("Text", GetDefCol().GetColor("Text")));
  }
  else if (strcmp(rcReason, "Comment") == 0)
  {
    // comment color
    SendScintilla(SCI_STYLESETFONT,SciQt_COMMENT, (int)monospace);
    SendScintilla(SCI_STYLESETSIZE,SciQt_COMMENT, fontsize);
    SendScintilla(SCI_STYLESETFORE,SciQt_COMMENT, rclGrp.GetInt("Comment", GetDefCol().GetColor("Comment")));
  }
  else if (strcmp(rcReason, "Block comment") == 0)
  {
    // block comment color
    SendScintilla(SCI_STYLESETFONT,SciQt_COMMENTBLOCK, (int)monospace);
    SendScintilla(SCI_STYLESETSIZE,SciQt_COMMENTBLOCK, fontsize);
    SendScintilla(SCI_STYLESETFORE,SciQt_COMMENTBLOCK, rclGrp.GetInt("Block comment", GetDefCol().GetColor("Block comment")));
  }
  else if (strcmp(rcReason, "Number") == 0)
  {
    // number color
    SendScintilla(SCI_STYLESETFORE,SciQt_NUMBER, rclGrp.GetInt("Number", GetDefCol().GetColor("Number")));
  }
  else if (strcmp(rcReason, "String") == 0)
  {
	  // string color
    SendScintilla(SCI_STYLESETFORE,SciQt_STRING, rclGrp.GetInt("String", GetDefCol().GetColor("String")));      
  }
  else if (strcmp(rcReason, "Keyword") == 0)
  {
	  // keyword color
    SendScintilla(SCI_STYLESETFONT,SciQt_KEYWORDS, (int)font);
    SendScintilla(SCI_STYLESETSIZE,SciQt_KEYWORDS, (int)fontsize);
    SendScintilla(SCI_STYLESETFORE,SciQt_KEYWORDS, rclGrp.GetInt("Keyword", GetDefCol().GetColor("Keyword")));
    SendScintilla(SCI_STYLESETBOLD,SciQt_KEYWORDS, 1);
  }
  else if (strcmp(rcReason, "Breakpoint") == 0)
  {
	  // breakpoint color
	  SendScintilla(SCI_MARKERDEFINE,  SciQt_MARKER_BREAKPNT, SC_MARK_CIRCLE);
	  SendScintilla(SCI_MARKERSETFORE, SciQt_MARKER_BREAKPNT, rclGrp.GetInt("Breakpoint", GetDefCol().GetColor("Breakpoint")));
	  SendScintilla(SCI_MARKERSETBACK, SciQt_MARKER_BREAKPNT, rclGrp.GetInt("Breakpoint", GetDefCol().GetColor("Breakpoint")));
  }
  else if (strcmp(rcReason, "Bookmark") == 0)
  {
	  // bookmark color
	  SendScintilla(SCI_MARKERDEFINE,  SciQt_MARKER_BOOKMARK, SC_MARK_ARROW);
	  SendScintilla(SCI_MARKERSETFORE, SciQt_MARKER_BOOKMARK, rclGrp.GetInt("Bookmark", GetDefCol().GetColor("Bookmark")));
	  SendScintilla(SCI_MARKERSETBACK, SciQt_MARKER_BOOKMARK, rclGrp.GetInt("Bookmark", GetDefCol().GetColor("Bookmark")));
  }
  else if (strcmp(rcReason, "Character") == 0)
  {
    // character
    SendScintilla(SCI_STYLESETFORE,SciQt_CHARACTER, rclGrp.GetInt("Character", GetDefCol().GetColor("Character")));
    // triple
    SendScintilla(SCI_STYLESETFORE,SciQt_TRIPLE, rclGrp.GetInt("Character", GetDefCol().GetColor("Character")));
    // triple double
    SendScintilla(SCI_STYLESETFORE,SciQt_TRIPLEDOUBLE, rclGrp.GetInt("Character", GetDefCol().GetColor("Character")));
    // end of line
    SendScintilla(SCI_STYLESETFORE,SciQt_STRINGEOL, rclGrp.GetInt("Character", GetDefCol().GetColor("Character")));
  }
  else if (strcmp(rcReason, "Class name") == 0)
  {
    // class name
    SendScintilla(SCI_STYLESETFORE,SciQt_CLASSNAME, rclGrp.GetInt("Class name", GetDefCol().GetColor("Class name")));
  }
  else if (strcmp(rcReason, "Define name") == 0)
  {
    // define name
    SendScintilla(SCI_STYLESETFORE,SciQt_DEFNAME, rclGrp.GetInt("Define name", GetDefCol().GetColor("Define name")));
  }
  else if (strcmp(rcReason, "Operator") == 0)
  {
    // operator
    SendScintilla(SCI_STYLESETFORE,SciQt_OPERATOR, rclGrp.GetInt("Operator", GetDefCol().GetColor("Operator")));
  }
  else if (strcmp(rcReason, "Text Selection") == 0)
  {
  }
  else if (strcmp(rcReason, "EnableFolding") == 0)
  {
	  if (rclGrp.GetBool("EnableFolding", true))
    {
      SendScintilla(SCI_SETPROPERTY, (uptr_t) "fold", (sptr_t) "1");
	    SendScintilla(SCI_SETPROPERTY, (uptr_t) "fold.compact", (sptr_t) "1");
	    SendScintilla(SCI_SETPROPERTY, (uptr_t) "fold.symbols", (sptr_t) "1");
	    SendScintilla(SCI_SETPROPERTY, (uptr_t) "styling.within.preprocessor", (sptr_t) "1");
	    // set styles.
	    long fore = 0;
	    long back = 0x00ffffff;

	    SendScintilla(SCI_SETFOLDFLAGS, 16, 0);
	    // to put the folder markers in the line number region
	    //SendScintilla(SCI_SETMARGINMASKN, 0, SC_MASK_FOLDERS);
	    SendScintilla(SCI_SETMODEVENTMASK, SC_MOD_CHANGEFOLD, 0);

	    // create a margin column for the folding symbols
	    SendScintilla(SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL);
	    SendScintilla(SCI_SETMARGINWIDTHN, 2, /*foldMargin ? foldMarginWidth :*/ 16);
	    SendScintilla(SCI_SETMARGINMASKN, 2, SC_MASK_FOLDERS);
	    SendScintilla(SCI_SETMARGINSENSITIVEN, 2, 1);
	    DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS, back, fore);
	    DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_PLUS, back, fore);
	    DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY, back, fore);
	    DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY, back, fore);
	    DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY, back, fore);
	    DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY, back, fore);
	    DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY, back, fore);
    }
  }
  else if (strcmp(rcReason, "EnableLineNumber") == 0)
  {
	  // line number
	  if (rclGrp.GetBool("EnableLineNumber", true))
	  {
		  SendScintilla(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
		  int pixelWidth = 3 * SendScintilla(SCI_TEXTWIDTH, STYLE_LINENUMBER, (int)"9");
		  SendScintilla(SCI_SETMARGINWIDTHN, 0, pixelWidth);
    }
    else
    {
		  SendScintilla(SCI_SETMARGINTYPEN, 0, 0);
    }
  }
  else
  {
	  // margin stuff
	  SendScintilla(SCI_SETMARGINTYPEN,  1, SC_MARGIN_SYMBOL);
	  SendScintilla(SCI_SETMARGINWIDTHN, 1, 10);
	  SendScintilla(SCI_SETMARGINSENSITIVEN, 1, TRUE);
  }
}

void FCScintillaEdit::loadSettings()
{
  FCParameterGrp::handle hGrp = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Windows/Editor");
  hGrp->Notify("");
  hGrp->Notify("Lexer");
  hGrp->Notify("Text");
  hGrp->Notify("Comment");
  hGrp->Notify("Block comment");
  hGrp->Notify("Number");
  hGrp->Notify("String");
  hGrp->Notify("Keyword");
  hGrp->Notify("Breakpoint");
  hGrp->Notify("Bookmark");
  hGrp->Notify("Character");
  hGrp->Notify("Class name");
  hGrp->Notify("Define name");
  hGrp->Notify("Operator");
  hGrp->Notify("EnableLineNumber");
  hGrp->Notify("EnableFolding");
}

long FCScintillaEdit::SendScintilla(unsigned int msg,unsigned long wParam, long lParam)
{
	return sciTE->WndProc(msg,wParam,lParam);
}

bool FCScintillaEdit::eventFilter(QObject* o, QEvent* e)
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

void FCScintillaEdit::slotTimer()
{
	sciTE->Tick();
}

void FCScintillaEdit::slotContextMenu(int cmd)
{
	sciTE->Command(cmd);
}

void FCScintillaEdit::focusInEvent(QFocusEvent * e)
{
	sciTE->SetFocusState(true);
}

void FCScintillaEdit::focusOutEvent(QFocusEvent * e)
{
	sciTE->SetFocusState(false);
}

bool FCScintillaEdit::focusNextPrevChild (bool next)
{
  // if CTRL is pressed
  if (_bControlButton)
    return QWidget::focusNextPrevChild(next);
  return false;
}

void FCScintillaEdit::mousePressEvent(QMouseEvent * e)
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

void FCScintillaEdit::mouseReleaseEvent(QMouseEvent * e)
{
	if (sciTE->HaveMouseCapture() && e->button() == LeftButton)
	{
		bool ctrl = e->state() & ControlButton;

		sciTE->ButtonUp(Point(e->x(),e->y()),0,ctrl);
	}
}

void FCScintillaEdit::mouseMoveEvent(QMouseEvent * e)
{
	sciTE->ButtonMove(Point(e->x(),e->y()));
}

void FCScintillaEdit::mouseDoubleClickEvent(QMouseEvent * e)
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

void FCScintillaEdit::keyPressEvent(QKeyEvent * e)
{
  _bControlButton = (e->state() &  ControlButton);

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

    case Key_F9:
      if (e->state() & ControlButton)
      {
        if (e->state() & ShiftButton)
          clearAllBreakpoints();
        else
          GetConsole().Message("Disable breakpoint: not yet implemented\n");
      }
      else
        toggleBreakpoint(getCurrentLine());
      return;

    case Key_F2:
      if (e->state() & ControlButton)
        if (e->state() & ShiftButton)
          clearAllBookmarks();
        else
          toggleBookmark(getCurrentLine());
      else if (e->state() & ShiftButton)
        nextBookmark(false);
      else
        nextBookmark(true);
      return;

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

void FCScintillaEdit::keyReleaseEvent ( QKeyEvent * e )
{
  _bControlButton = (e->state() &  ControlButton);
  QWidget::keyReleaseEvent (e);
}

void FCScintillaEdit::mouseWheelEvent(QWheelEvent * e)
{
	setFocus();

	if (e->state() & ShiftButton)
		QApplication::sendEvent(horScroll, e);
	else 
		QApplication::sendEvent(verScroll, e);
}

void FCScintillaEdit::slotVerScroll(int val)
{
	sciTE->ScrollTo(val);
}

void FCScintillaEdit::slotHorScroll(int val)
{
	sciTE->HorizontalScrollTo(val);
}

QSize FCScintillaEdit::sizeHint() const
{
	int height = sciTE->vs.lineHeight * sciTE->pdoc->LinesTotal();

	if (sciTE->horizontalScrollBarVisible)
		height += horScroll->sizeHint().height();

	return QSize(sciTE->scrollWidth,height);
}

ScintillaQt* FCScintillaEdit::getTextEditor() const
{
  return sciTE;
}

bool FCScintillaEdit::toggleBreakpoint(int nLine)
{
	if ( sciTE->WndProc (SCI_MARKERGET, nLine,0) & 1 )
	{
		sciTE->WndProc(SCI_MARKERDELETE, nLine, 0);
		return false;
	}
	else
  {
  	sciTE->WndProc(SCI_MARKERADD, nLine, 0);
	return true;
  }
}

void FCScintillaEdit::clearAllBreakpoints()
{
	sciTE->WndProc(SCI_MARKERDELETEALL, SciQt_MARKER_BREAKPNT, 0);
}

void FCScintillaEdit::setBreakpoint(int nLine)
{
	sciTE->WndProc(SCI_MARKERADD, nLine, 0);
}

void FCScintillaEdit::gotoLine(int nLine)
{
	sciTE->WndProc(SCI_GOTOLINE, nLine, 0);

	sciTE->WndProc(SCI_MARKERDELETEALL, 1, 0);
	sciTE->WndProc(SCI_MARKERADD, nLine, 1);
}

void FCScintillaEdit::grabFocus()
{
	sciTE->WndProc(SCI_GRABFOCUS, 0, 0);
}

void FCScintillaEdit::setReadOnly(bool bReadOnly)
{
	sciTE->WndProc(SCI_SETREADONLY, bReadOnly, 0);
}

QString FCScintillaEdit::getLine(int nLine)
{
	QString strLine;
	int nLineLength = sciTE->WndProc(SCI_LINELENGTH, nLine, 0);
  char* szBuf = new char[nLineLength];
	if ( nLineLength>0 )
	{
		sciTE->WndProc(SCI_GETLINE, nLine, (int)szBuf);
		strLine = szBuf;
	}

	return strLine;
}

void FCScintillaEdit::gotoLastLine()
{
	int nLine = sciTE->WndProc(SCI_GETLINECOUNT, 0, 0);
	sciTE->WndProc(SCI_GOTOLINE, nLine-1, 0);
}

int FCScintillaEdit::getCurrentLine()
{
	return sciTE->WndProc(SCI_LINEFROMPOSITION, sciTE->WndProc(SCI_GETCURRENTPOS, 0, 0), 0);
}

void FCScintillaEdit::setStackTraceLevel(int nLevel)
{
	sciTE->WndProc(SCI_MARKERDELETEALL, 0, 0);
	sciTE->WndProc(SCI_MARKERADD, nLevel, 0);
}

CharacterRange FCScintillaEdit::getSelection() 
{
	CharacterRange crange;
	crange.cpMin = sciTE->WndProc(SCI_GETSELECTIONSTART, 0, 0);
	crange.cpMax = sciTE->WndProc(SCI_GETSELECTIONEND, 0, 0);
	return crange;
}

void FCScintillaEdit::assignKey(int key, int mods, int cmd) 
{
	sciTE->WndProc(SCI_ASSIGNCMDKEY,
	           Platform::LongFromTwoShorts(static_cast<short>(key),
	                                       static_cast<short>(mods)), cmd);
}

void FCScintillaEdit::clearAllBookmarks()
{
	sciTE->WndProc(SCI_MARKERDELETEALL, SciQt_MARKER_BOOKMARK, 0);
}

bool FCScintillaEdit::presentBookmark(int nLine) 
{
	if (nLine == -1)
		nLine = getCurrentLine();
	int state = sciTE->WndProc(SCI_MARKERGET, nLine, 0);
	return state & (1 << SciQt_MARKER_BOOKMARK);
}

bool FCScintillaEdit::toggleBookmark(int nLine) 
{
	if ( sciTE->WndProc (SCI_MARKERGET, nLine,1) & 2 )
  {
		sciTE->WndProc(SCI_MARKERDELETE, nLine, 1);
    return false;
  }
	else
  {
  	sciTE->WndProc(SCI_MARKERADD, nLine, 1);
    return true;
  }
}

void FCScintillaEdit::nextBookmark(bool forwardScan) 
{
	int nLine = getCurrentLine();
	int sci_marker = SCI_MARKERNEXT;
	int lineStart = nLine + 1;
	int lineRetry = 0;
	if (!forwardScan) 
  {
		lineStart = nLine - 1;
		lineRetry = sciTE->WndProc(SCI_GETLINECOUNT, 0, 0L);
		sci_marker = SCI_MARKERPREVIOUS;
	}
	int nextLine = sciTE->WndProc(sci_marker, lineStart, 1 << SciQt_MARKER_BOOKMARK);
	if (nextLine < 0)
		nextLine = sciTE->WndProc(sci_marker, lineRetry, 1 << SciQt_MARKER_BOOKMARK);
	if (nextLine < 0 || nextLine == nLine)
		GetConsole().Warning("No other bookmark\n");
	else
		gotoLineEnsureVisible(nextLine);
}

void FCScintillaEdit::gotoLineEnsureVisible(int nLine) 
{
	sciTE->WndProc(SCI_ENSUREVISIBLEENFORCEPOLICY, nLine, 0);
	sciTE->WndProc(SCI_GOTOLINE, nLine, 0);
}

void FCScintillaEdit::foldChanged(int line, int levelNow, int levelPrev) 
{
	if (levelNow & SC_FOLDLEVELHEADERFLAG) 
  {
		if (!(levelPrev & SC_FOLDLEVELHEADERFLAG)) 
    {
			sciTE->WndProc(SCI_SETFOLDEXPANDED, line, 1);
		}
	} 
  else if (levelPrev & SC_FOLDLEVELHEADERFLAG) 
  {
		if (!sciTE->WndProc(SCI_GETFOLDEXPANDED, line, 0)) 
    {
			expand(line, true, false, 0, levelPrev);
		}
	}
}

void FCScintillaEdit::toggleFold(int nLine)
{
	SendScintilla(SCI_TOGGLEFOLD,nLine);
}

void FCScintillaEdit::expand(int &line, bool doExpand, bool force, int visLevels, int level) 
{
	int lineMaxSubord = sciTE->WndProc(SCI_GETLASTCHILD, line, level & SC_FOLDLEVELNUMBERMASK);
	line++;
	while (line <= lineMaxSubord) 
  {
		if (force) 
    {
			if (visLevels > 0)
				sciTE->WndProc(SCI_SHOWLINES, line, line);
			else
				sciTE->WndProc(SCI_HIDELINES, line, line);
		}
    else 
    {
			if (doExpand)
				sciTE->WndProc(SCI_SHOWLINES, line, line);
		}
		int levelLine = level;
		if (levelLine == -1)
			levelLine = sciTE->WndProc(SCI_GETFOLDLEVEL, line, 0);
		if (levelLine & SC_FOLDLEVELHEADERFLAG) {
			if (force) 
      {
				if (visLevels > 1)
					sciTE->WndProc(SCI_SETFOLDEXPANDED, line, 1);
				else
					sciTE->WndProc(SCI_SETFOLDEXPANDED, line, 0);
				expand(line, doExpand, force, visLevels - 1);
			} 
      else 
      {
				if (doExpand) 
        {
					if (!sciTE->WndProc(SCI_GETFOLDEXPANDED, line, 0))
						sciTE->WndProc(SCI_SETFOLDEXPANDED, line, 1);
					expand(line, true, force, visLevels - 1);
				}
        else 
        {
					expand(line, false, force, visLevels - 1);
				}
			}
		} 
    else 
    {
			line++;
		}
	}
}

void FCScintillaEdit::foldAll() 
{
	sciTE->WndProc(SCI_COLOURISE, 0, -1);
	int maxLine = sciTE->WndProc(SCI_GETLINECOUNT, 0, 0);
	bool expanding = true;
	for (int lineSeek = 0; lineSeek < maxLine; lineSeek++) 
  {
		if (sciTE->WndProc(SCI_GETFOLDLEVEL, lineSeek, 0) & SC_FOLDLEVELHEADERFLAG) 
    {
			expanding = !sciTE->WndProc(SCI_GETFOLDEXPANDED, lineSeek, 0);
			break;
		}
	}
	for (int line = 0; line < maxLine; line++) 
  {
		int level = sciTE->WndProc(SCI_GETFOLDLEVEL, line, 0);
		if ((level & SC_FOLDLEVELHEADERFLAG) && (SC_FOLDLEVELBASE == (level & SC_FOLDLEVELNUMBERMASK))) 
    {
			if (expanding) 
      {
				sciTE->WndProc(SCI_SETFOLDEXPANDED, line, 1);
				expand(line, true, false, 0, level);
				line--;
			}
      else 
      {
				int lineMaxSubord = sciTE->WndProc(SCI_GETLASTCHILD, line, -1);
				sciTE->WndProc(SCI_SETFOLDEXPANDED, line, 0);
				if (lineMaxSubord > line)
					sciTE->WndProc(SCI_HIDELINES, line + 1, lineMaxSubord);
			}
		}
	}
}

bool FCScintillaEdit::clickMargin(int position, int modifiers) 
{
	int lineClick = sciTE->WndProc(SCI_LINEFROMPOSITION, position, 0);
	if ((modifiers & SCMOD_SHIFT) && (modifiers & SCMOD_CTRL)) 
  {
		foldAll();
	} 
  else 
  {
		int levelClick = sciTE->WndProc(SCI_GETFOLDLEVEL, lineClick, 0);
		if (levelClick & SC_FOLDLEVELHEADERFLAG) 
    {
			if (modifiers & SCMOD_SHIFT) 
      {
				sciTE->WndProc(SCI_SETFOLDEXPANDED, lineClick, 1);
				expand(lineClick, true, true, 100, levelClick);
			} 
      else if (modifiers & SCMOD_CTRL) 
      {
				if (sciTE->WndProc(SCI_GETFOLDEXPANDED, lineClick, 0)) 
        {
					sciTE->WndProc(SCI_SETFOLDEXPANDED, lineClick, 0);
					expand(lineClick, false, true, 0, levelClick);
				} 
        else 
        {
					sciTE->WndProc(SCI_SETFOLDEXPANDED, lineClick, 1);
					expand(lineClick, true, true, 100, levelClick);
				}
			} 
      else 
      {
				sciTE->WndProc(SCI_TOGGLEFOLD, lineClick, 0);
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

FCScintillaView::FCScintillaView( QWidget* parent, const char* name)
: FCView(0,parent, name)  
{
	setCaption("Editor");
	setTabCaption("Editor");
	resize( 400, 300 );

	view = new FCScintillaEdit(this);
}

FCScintillaView::~FCScintillaView()
{
}

void FCScintillaView::resizeEvent(QResizeEvent* e)
{
	view->resize(e->size().width(),e->size().height());
	FCView::resizeEvent( e);
}

bool FCScintillaView::OnMsg(const char* pMsg)
{
	if (strcmp(pMsg,"Save")==0){
		Save();
		return true;
	}
	if (strcmp(pMsg,"SaveAs")==0){
		SaveAs();
		return true;
	}
	if (strcmp(pMsg,"Cut")==0){
		Cut();
		return true;
	}
	if (strcmp(pMsg,"Copy")==0){
		Copy();
		return true;
	}
	if (strcmp(pMsg,"Paste")==0){
		Paste();
		return true;
	}
	if (strcmp(pMsg,"Undo")==0){
		Undo();
		return true;
	}
	if (strcmp(pMsg,"Redo")==0){
		Redo();
		return true;
	}

	GetConsole().Log("FCScintillaView::OnMsg() unhandled \"%s\"\n",pMsg);
	return false;
}

// Message handler test
bool FCScintillaView::OnHasMsg(const char* pMsg)
{
	if (strcmp(pMsg,"Save")==0)  return true;
	if (strcmp(pMsg,"Print")==0) return true;
	if (strcmp(pMsg,"Cut")==0)
  {
		bool writable = !view->getTextEditor()->WndProc(SCI_GETREADONLY, 0, 0);
    return (writable && (view->getTextEditor()->SelectionEmpty() == false));
  }
	if (strcmp(pMsg,"Copy")==0)
  {
    return !(view->getTextEditor()->SelectionEmpty());
  }
	if (strcmp(pMsg,"Paste")==0)
  {
    QClipboard *cb = QApplication::clipboard();
    QString text;

    // Copy text from the clipboard (paste)
    text = cb->text();

    return (text && view->getTextEditor()->CanPaste());
  }
	if (strcmp(pMsg,"Undo")==0)
  {
    return view->SendScintilla(SCI_CANUNDO);
  }
	if (strcmp(pMsg,"Redo")==0)
  {
    return view->SendScintilla(SCI_CANREDO);
  }
	return false;
}

bool FCScintillaView::CanClose(void)
{
  if (!view->SendScintilla(SCI_GETMODIFY))
    return true;

  switch(QMessageBox::warning( this, "Unsaved document","Save file before close?","Yes","No","Cancel",0,2))
	{
		case 0:
			return Save();
		case 1:
			return true;
		case 2:
			return false;
		default:
			return false;
	}
}

bool FCScintillaView::Save(void)
{
  if (!view->SendScintilla(SCI_GETMODIFY))
    return true;

  // check if saved ever before
	if (isAlreadySavedBefore())
  {
    saveFile();
    return true;
  }
	else
  {
		return SaveAs();
  }
}

bool FCScintillaView::SaveAs(void)
{
	QString fn = QFileDialog::getSaveFileName(QString::null, "FreeCAD macro (*.FCMacro);;Python (*.py)", this);
	if (!fn.isEmpty())
	{
    _fileName = fn;
    saveFile();
    return true;
	}
	else
	{
    return false;
	}
}

bool FCScintillaView::Open(void)
{
	QString file = QFileDialog::getOpenFileName(QString::null, "Macro files (*.py *.FCMacro);;Python (*.py);;FreeCAD macro (*.FCMacro)", this);
	if ( file.isEmpty() ) 
		return false;

	OpenFile(file);
	setCaption(file);
	QString name = file.left(file.findRev('.'));
	setTabCaption(name);

	return true;
}

void FCScintillaView::OpenFile (const QString& fileName)
{
	view->getTextEditor()->WndProc(SCI_CLEARALL, 0, 0);
	view->getTextEditor()->WndProc(SCI_SETSAVEPOINT, 0, 0);
  view->getTextEditor()->WndProc(SCI_CANCEL, 0, 0);
  view->getTextEditor()->WndProc(SCI_SETUNDOCOLLECTION, 0, 0);

  QString line;
  _fileName = fileName;
  QFile file(fileName);
   
  if( !file.open(IO_ReadOnly))
     return;

  QTextStream in(&file);
  QString text;

  while( !in.atEnd() ){
    line = in.readLine();
    text = line + "\n";
    view->getTextEditor()->WndProc(SCI_ADDTEXT, text.length(), reinterpret_cast<sptr_t>(static_cast<const char *>(text.latin1())));
  }

  file.close();

  view->getTextEditor()->WndProc(SCI_SETUNDOCOLLECTION, 1, 0);
	view->getTextEditor()->WndProc(SCI_SETSAVEPOINT, 0, 0);
	view->getTextEditor()->WndProc(SCI_GOTOPOS, 0, 0);
}

void FCScintillaView::Cut(void)
{
  view->SendScintilla(SCI_CUT);
}

void FCScintillaView::Copy(void)
{
  view->SendScintilla(SCI_COPY);
}

void FCScintillaView::Paste(void)
{
  view->SendScintilla(SCI_PASTE);
}

void FCScintillaView::Undo(void)
{
  view->SendScintilla(SCI_UNDO);
}

void FCScintillaView::Redo(void)
{
  view->SendScintilla(SCI_REDO);
}

void FCScintillaView::Print(QPainter& cPrinter)
{
	// no printing yet ;-)
	assert(0);
}

void FCScintillaView::saveFile()
{
	QFile file(_fileName);
  if( !file.open(IO_WriteOnly))
    return;

  const int blockSize = 131072;

	TextRange tr;
	char data[blockSize + 1];
	int lengthDoc = view->getTextEditor()->WndProc(SCI_GETLENGTH, 0, 0);
	
  QTextStream out(&file);
  for (int i = 0; i < lengthDoc; i += blockSize) 
	{
		int grabSize = lengthDoc - i;
		if (grabSize > blockSize)
			grabSize = blockSize;

		tr.chrg.cpMin = i;
		tr.chrg.cpMax = i + grabSize;
		tr.lpstrText = data;
		view->getTextEditor()->WndProc(SCI_GETTEXTRANGE, 0, long(&tr));

    out << data;
	}

	view->getTextEditor()->WndProc(SCI_SETSAVEPOINT, 0,0);

  file.close();
  view->SendScintilla(SCI_SETSAVEPOINT);
  
  return;
}

bool FCScintillaView::isAlreadySavedBefore()
{
  return (!_fileName.isEmpty());
}

#include "moc_ScintillaQt.cpp"
