// Scintilla source code edit control
// PlatQT.cpp - implementation of platform facilities on Qt

#include "PreCompiled.h"

#ifndef _PreComp_
# include <qapplication.h>
# include <qstring.h>
# include <qwidget.h>
# include <qlistbox.h>
# include <qpopupmenu.h>
# include <qcursor.h>
# include <qpixmap.h>
# include <qdatetime.h>
# include <qpainter.h>
# include <qfont.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "scintilla/Platform.h"
#include "scintilla/XPM.h"
#include "../Base/Console.h"


// globals
static QFont *PFont(FontID id)
{
	return reinterpret_cast<QFont *>(id);
}

static QFont *PFont(Font &f) 
{
	return reinterpret_cast<QFont *>(f.GetID());
}

static QWidget *PWidget(WindowID id)
{
	return reinterpret_cast<QWidget *>(id);
}

static QWidget *PWidget(Window &w) 
{
	return PWidget(w.GetID());
}

static QPainter *PSurface(SurfaceID id)
{
	return reinterpret_cast<QPainter *>(id);
}

static QPopupMenu *PMenu(MenuID id)
{
	return reinterpret_cast<QPopupMenu *>(id);
}

///////////////////////////////////////////////////////////////////////////////////////

Point Point::FromLong(long lpoint)
{
	return Point(Platform::LowShortFromLong (lpoint),
		           Platform::HighShortFromLong(lpoint));
}

///////////////////////////////////////////////////////////////////////////////////////

Palette::Palette()
{
	used = 0;
	allowRealization = false;
}

Palette::~Palette()
{
	Release();
}

void Palette::Release()
{
	used = 0;
}

void Palette::WantFind(ColourPair &cp,bool want)
{
	if (want == false)
		cp.allocated.Set(cp.desired.AsLong());
}

void Palette::Allocate(Window& win)
{
}

///////////////////////////////////////////////////////////////////////////////////////

Font::Font() 
 : id(0)
{
}

Font::~Font()
{
}

void Font::Create(const char *faceName,int,int size,bool bold,bool italic)
{
	Release();

	QFont* font = new QFont();

	if (faceName[0] == '-')
  {
		font->setRawName(faceName);
  }
	else
	{
		font->setFamily(faceName);
		font->setPointSize(size);
		font->setBold(bold);
		font->setItalic(italic);
	}

	id = font;
}

void Font::Release()
{
	if (id)
	{
		delete PFont(id);
		id = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

class SurfaceImpl : public Surface
{
  public:
	  SurfaceImpl();
	  virtual ~SurfaceImpl();

	  void Init(WindowID wid);
	  void Init(SurfaceID sid, WindowID wid);
	  void InitPixMap(int width, int height, Surface *surface_, WindowID wid);

	  void Release();
	  bool Initialised();
	  void PenColour(ColourAllocated fore);
	  int LogPixelsY();
	  int DeviceHeightFont(int points);
	  void MoveTo(int x_, int y_);
	  void LineTo(int x_, int y_);
	  void Polygon(Point *pts, int npts, ColourAllocated fore, ColourAllocated back);
	  void RectangleDraw(PRectangle rc, ColourAllocated fore, ColourAllocated back);
	  void FillRectangle(PRectangle rc, ColourAllocated back);
	  void FillRectangle(PRectangle rc, Surface &surfacePattern);
	  void RoundedRectangle(PRectangle rc, ColourAllocated fore, ColourAllocated back);
	  void Ellipse(PRectangle rc, ColourAllocated fore, ColourAllocated back);
	  void Copy(PRectangle rc, Point from, Surface &surfaceSource);

	  void DrawTextBase(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore);
	  void DrawTextNoClip(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore, ColourAllocated back);
	  void DrawTextClipped(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore, ColourAllocated back);
	  void DrawTextTransparent(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore);
	  void MeasureWidths(Font &font_, const char *s, int len, int *positions);
	  int WidthText(Font &font_, const char *s, int len);
	  int WidthChar(Font &font_, char ch);
	  int Ascent(Font &font_);
	  int Descent(Font &font_);
	  int InternalLeading(Font &font_);
	  int ExternalLeading(Font &font_);
	  int Height(Font &font_);
	  int AverageCharWidth(Font &font_);

	  int SetPalette(Palette *pal, bool inBackGround);
	  void SetClip(PRectangle rc);
	  void FlushCachedState();

	  void SetUnicodeMode(bool unicodeMode_);
	  void SetDBCSMode(int codePage);

  private:
	  bool setFont(Font &font_);
	  QString convertString(const char *s,int len);
	  QColor convertColour(const ColourAllocated &col);

	  bool m_bUnicodeMode;
	  bool m_bPainter;
	  QPainter* m_pPainter;
};

SurfaceImpl::SurfaceImpl()
 : m_bUnicodeMode(false), m_bPainter(false), m_pPainter(NULL)
{
}

SurfaceImpl::~SurfaceImpl()
{
	Release();
}

void SurfaceImpl::Release()
{
	if (m_pPainter && m_bPainter)
	{
		QPaintDevice *pd = m_pPainter->device();
		delete m_pPainter;
		delete pd;
	}

	m_pPainter = NULL;
}

bool SurfaceImpl::Initialised()
{
  return (m_pPainter != NULL);
}

void SurfaceImpl::Init(WindowID WID_NAME)
{
	Release();

	m_pPainter = new QPainter(new QWidget());
	m_bPainter = true;
}

void SurfaceImpl::Init(SurfaceID sid, WindowID WID_NAME)
{
	Release();

	m_pPainter = PSurface(sid);
	m_bPainter = false;
}

void SurfaceImpl::InitPixMap(int width, int height, Surface *, WindowID WID_NAME)
{
	Release();

	m_pPainter = new QPainter(new QPixmap(width,height));
	m_bPainter = true;
}

void SurfaceImpl::PenColour(ColourAllocated fore)
{
	m_pPainter->setPen(convertColour(fore));
}

int SurfaceImpl::LogPixelsY() 
{
	return 72;
}

int SurfaceImpl::DeviceHeightFont(int points) 
{
	int logPix = LogPixelsY();
	return (points * logPix + logPix / 2) / 72;
}

void SurfaceImpl::MoveTo(int x_, int y_)
{
  m_pPainter->moveTo(x_, y_);
}

void SurfaceImpl::LineTo(int x_, int y_) 
{
  m_pPainter->lineTo(x_, y_);
}

void SurfaceImpl::Polygon(Point *pts, int npts, ColourAllocated fore, ColourAllocated back)
{
	QPointArray qpts(npts);

	for (int i = 0; i < npts; ++i)
		qpts.setPoint(i,pts[i].x,pts[i].y);

	m_pPainter->setPen(convertColour(fore));
	m_pPainter->setBrush(convertColour(back));
	m_pPainter->drawPolygon(qpts);
}

void SurfaceImpl::RectangleDraw(PRectangle rc, ColourAllocated fore, ColourAllocated back)
{
	m_pPainter->setPen(convertColour(fore));
	m_pPainter->setBrush(convertColour(back));
	m_pPainter->drawRect(rc.left,rc.top, rc.right-rc.left-1,rc.bottom-rc.top-1);
}

void SurfaceImpl::FillRectangle(PRectangle rc, ColourAllocated back)
{
	m_pPainter->setPen(Qt::NoPen);
	m_pPainter->setBrush(convertColour(back));
	m_pPainter->drawRect(rc.left,rc.top, rc.right - rc.left,rc.bottom - rc.top);
}

void SurfaceImpl::FillRectangle(PRectangle rc, Surface &surfacePattern)
{
	SurfaceImpl &si = static_cast<SurfaceImpl &>(surfacePattern);
	QPixmap *pm = static_cast<QPixmap *>(si.m_pPainter->device());

	if (pm)
	{
		QBrush brush(Qt::black, *pm);
		m_pPainter->setBrush(brush);
		m_pPainter->drawRect(rc.left,rc.top, rc.right - rc.left,rc.bottom - rc.top);
	}
	else
		FillRectangle(rc,ColourAllocated(0));
}

void SurfaceImpl::RoundedRectangle(PRectangle rc, ColourAllocated fore, ColourAllocated back)
{
	m_pPainter->setPen(convertColour(fore));
	m_pPainter->setBrush(convertColour(back));
	m_pPainter->drawRoundRect(rc.left,rc.top, rc.right - rc.left,rc.bottom - rc.top);
}

void SurfaceImpl::Ellipse(PRectangle rc, ColourAllocated fore, ColourAllocated back)
{
	m_pPainter->setPen(convertColour(fore));
	m_pPainter->setBrush(convertColour(back));
	m_pPainter->drawEllipse(rc.left,rc.top,rc.right - rc.left,rc.bottom - rc.top);
}

void SurfaceImpl::Copy(PRectangle rc, Point from, Surface &surfaceSource)
{
	SurfaceImpl &si = static_cast<SurfaceImpl &>(surfaceSource);
	QPixmap *pm = static_cast<QPixmap *>(si.m_pPainter -> device());

	if (pm)
  {
		m_pPainter->drawPixmap(rc.left, rc.top, *pm, from.x, from.y, rc.right - rc. left, rc.bottom - rc.top);
  }
}

void SurfaceImpl::DrawTextBase(PRectangle rc, Font &font_, int ybase, const char *s, int len, ColourAllocated fore) 
{
	QString qs = convertString(s,len);
  setFont(font_);
	m_pPainter->setBackgroundMode(Qt::TransparentMode);
	m_pPainter->setPen(convertColour(fore));
	m_pPainter->drawText(rc.left,ybase,qs);
}

void SurfaceImpl::DrawTextNoClip(PRectangle rc, Font &font_, int ybase, const char *s,int len,ColourAllocated fore, ColourAllocated back)
{
	FillRectangle(rc, back);
	DrawTextBase(rc, font_, ybase, s, len, fore);
}

void SurfaceImpl::DrawTextClipped(PRectangle rc, Font &font_, int ybase, const char *s,int len,ColourAllocated fore, ColourAllocated back)
{
	SetClip(rc);
	FillRectangle(rc, back);
	DrawTextBase(rc, font_, ybase, s, len, fore);
	m_pPainter->setClipping(FALSE);
}

void SurfaceImpl::DrawTextTransparent(PRectangle rc, Font &font_, int ybase, const char *s,int len, ColourAllocated fore)
{
	QString qs = convertString(s,len);

	setFont(font_);
	m_pPainter->setBackgroundMode(Qt::TransparentMode);
	m_pPainter->setPen(convertColour(fore));
	m_pPainter->drawText(rc.left,ybase,qs);
}

void SurfaceImpl::MeasureWidths(Font &font_, const char *s, int len, int *positions)
{
	if (setFont(font_))
	{
		int totalWidth = 0, ui = 0;
		QString qs = convertString(s,len);
		QFontMetrics fm = m_pPainter -> fontMetrics();

		for (int i = 0; i < int(qs.length()); ++i)
		{
			totalWidth += fm.width(qs[i]);

			int l = (m_bUnicodeMode ? QString(qs[i]).utf8().length() : 1);

			while (l--)
				positions[ui++] = totalWidth;
		}
	}
	else
		for (int i = 0; i < len; ++i)
			positions[i] = i + 1;
}

int SurfaceImpl::WidthText(Font &font_, const char *s, int len)
{
	if (setFont(font_))
	{
		QString qs = convertString(s,len);
		return m_pPainter->fontMetrics().width(qs,qs.length());
	}

	return 1;
}

int SurfaceImpl::WidthChar(Font &font_, char ch)
{
	if (setFont(font_))
  {
		return m_pPainter->fontMetrics().width(ch);
  }

	return 1;
}

int SurfaceImpl::Ascent(Font &font_)
{
	if (setFont(font_))
  {
		return m_pPainter->fontMetrics().ascent();
  }

	return 1;
}

int SurfaceImpl::Descent(Font &font_)
{
	if (setFont(font_))
  {
		return m_pPainter->fontMetrics().descent() + 1;
  }

	return 1;
}

int SurfaceImpl::InternalLeading(Font &font_) 
{
  return 0;
}

int SurfaceImpl::ExternalLeading(Font &font_)
{
	if (setFont(font_))
  {
		return m_pPainter->fontMetrics().leading();
  }

	return 0;
}

int SurfaceImpl::Height(Font &font_)
{
	if (setFont(font_))
  {
		return m_pPainter->fontMetrics().height();
  }

	return 1;
}

int SurfaceImpl::AverageCharWidth(Font &font_) 
{
  return WidthChar(font_, 'n');
}

int SurfaceImpl::SetPalette(Palette *pal, bool inBackGround) 
{
	// Handled in palette allocation for Qt so this does nothing
  return 0;
}

void SurfaceImpl::SetClip(PRectangle rc)
{
	m_pPainter->setClipRect(rc.left,rc.top,rc.right - rc.left,rc.bottom - rc.top);
}

void SurfaceImpl::FlushCachedState() 
{
  m_pPainter->flush();
}

void SurfaceImpl::SetUnicodeMode(bool unicodeMode_) 
{
  m_bUnicodeMode = unicodeMode_;
}

void SurfaceImpl::SetDBCSMode(int codePage) 
{
}

Surface *Surface::Allocate()
{
	return new SurfaceImpl;
}

bool SurfaceImpl::setFont(Font &font_)
{
	QFont *f = PFont(font_.GetID());

	if (f)
  {
		m_pPainter->setFont(*f);
  }

	return f;
}

QString SurfaceImpl::convertString(const char *s, int len)
{
	if (m_bUnicodeMode)
		return QString::fromUtf8(s, len);

	QString str;
	str.setLatin1(s,len);

	return str;
}

QColor SurfaceImpl::convertColour(const ColourAllocated &col)
{
	long c = col.AsLong();
	return QColor(c & 0xff,(c >> 8) & 0xff,(c >> 16) & 0xff);
}

///////////////////////////////////////////////////////////////////////////////////////

Window::~Window()
{
}

void Window::Destroy()
{
	QWidget *w = PWidget(id);

	if (w)
	{
		delete w;
		id = NULL;
	}
}

bool Window::HasFocus()
{
	return PWidget(id)->hasFocus();
}

PRectangle Window::GetPosition()
{
	// Before any size allocated pretend its 1000 wide so not scrolled
	PRectangle rect(0,0,1000,1000);

	QWidget *w = PWidget(id);
	if (w)
	{
		const QRect &r = w->geometry();
		rect.left   = r.left();
		rect.top    = r.top();
		rect.right  = r.right() + 1;
		rect.bottom = r.bottom() + 1;
	}

	return rect;
}

void Window::SetPosition(PRectangle rc)
{
	PWidget(id)->setGeometry(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
}

void Window::SetPositionRelative(PRectangle rc,Window relativeTo)
{
	QWidget *rel = PWidget(relativeTo.id);
	QPoint pos = rel->mapToGlobal(rel->pos());

	int x = pos.x() + rc.left;
	int y = pos.y() + rc.top;

	PWidget(id)->setGeometry(x, y, rc.right - rc. left, rc.bottom - rc.top);
}

PRectangle Window::GetClientPosition()
{
	// On Qt the client position is the window position
	return GetPosition();
}

void Window::Show(bool show)
{
	QWidget *w = PWidget(id);
  show ? w->show() : w->hide();
}

void Window::InvalidateAll()
{
	QWidget *w = PWidget(id);
	if (w)
		w->update();
}

void Window::InvalidateRectangle(PRectangle rc)
{
	QWidget *w = PWidget(id);
	if (w)
		w->update(rc.left,rc.top,rc.right - rc.left,rc.bottom - rc.top);
}

void Window::SetFont(Font &font)
{
	PWidget(id)->setFont(*PFont(font.GetID()));
}

void Window::SetCursor(Cursor curs)
{
	QCursorShape cur;

	switch (curs)
	{
	  case cursorText:
		  cur = IbeamCursor;
		  break;

	  case cursorUp:
		  cur = UpArrowCursor;
		  break;

	  case cursorWait:
		  cur = WaitCursor;
		  break;

	  case cursorHoriz:
		  cur = SizeHorCursor;
		  break;

	  case cursorVert:
		  cur = SizeVerCursor;
		  break;

	  case cursorHand:
		  cur = PointingHandCursor;
		  break;

	  default:
		  cur = ArrowCursor;
	}

	PWidget(id)->setCursor(cur);
}

void Window::SetTitle(const char *s)
{
	PWidget(id)->setCaption(s);
}

///////////////////////////////////////////////////////////////////////////////////////

class ListBoxX : public ListBox
{
  public:
	  ListBoxX();
	  virtual ~ListBoxX();

	  virtual void SetFont(Font &font);
	  virtual void Create(Window &parent, int ctrlID, int lineHeight_, bool unicodeMode_);
	  virtual void SetAverageCharWidth(int width);
	  virtual void SetVisibleRows(int rows);
	  virtual PRectangle GetDesiredRect();
	  virtual int CaretFromEdge();
	  virtual void Clear();
	  virtual void Append(char *s, int type = -1);
	  virtual int Length();
	  virtual void Select(int n);
	  virtual int GetSelection();
	  virtual int Find(const char *prefix);
	  virtual void GetValue(int n, char *value, int len);
	  virtual void Sort();
	  virtual void RegisterImage(int type, const char *xpm_data);
	  virtual void ClearRegisteredImages();
	  virtual void SetDoubleClickAction(CallBackAction action, void *data);

	  CallBackAction doubleClickAction;
	  void *doubleClickActionData;

  private:
	  QListBox *listbox;
    std::map<int, QPixmap> xset;
};


ListBoxX::ListBoxX()
 : doubleClickAction(NULL), doubleClickActionData(NULL), listbox(NULL)
{
}

ListBoxX::~ListBoxX()
{
}

void ListBoxX::Create(Window &parent,int,int,bool)
{
}

void ListBoxX::SetFont(Font &font)
{
	QFont *f = reinterpret_cast<QFont *>(font.GetID());
	if (f)
		listbox->setFont(*f);
}

void ListBoxX::SetAverageCharWidth(int)
{
}

void ListBoxX::SetVisibleRows(int)
{
}

PRectangle ListBoxX::GetDesiredRect()
{
	PRectangle rc(0,0,100,100);

	if (listbox)
	{
		QSize sh = listbox->sizeHint();
		rc.right = sh.width();
		rc.bottom = sh.height();
	}

	return rc;
}

int ListBoxX::CaretFromEdge()
{
	int dist = 0;

  for (std::map<int, QPixmap>::const_iterator it = xset.begin(); it != xset.end(); ++it)
	{
		int w = it->second.width();
		if (dist < w)
			dist = w;
	}

	if (listbox)
		dist += listbox->frameWidth();
	dist += 3;

	return dist;
}

void ListBoxX::Clear()
{
	listbox->clear();
}

void ListBoxX::Append(char *s,int type)
{
  std::map<int, QPixmap>::const_iterator it;

	if (type < 0 || (it = xset.find(type)) == xset.end())
		listbox->insertItem(s);
	else
		listbox->insertItem(it->second, s);
}

int ListBoxX::Length()
{
	return (listbox ? listbox->numRows() : 0);
}

void ListBoxX::Select(int n)
{
	listbox->setSelected(n, true);
}

int ListBoxX::GetSelection()
{
	return listbox->currentItem();
}

int ListBoxX::Find(const char *prefix)
{
	return listbox->index(listbox->findItem(QString(prefix)));
}

void ListBoxX::GetValue(int n,char *value,int len)
{
	QString s = listbox->text(n);

	if (s.isNull() || len <= 0)
		value[0] = '\0';
	else
	{
		strncpy(value,s.latin1(),len - 1);
		value[len - 1] = '\0';
	}
}

void ListBoxX::Sort()
{
	listbox->sort();
}

void ListBoxX::RegisterImage(int type,const char *xpm_data)
{
	xset[type] = *reinterpret_cast<const QPixmap *>(xpm_data);
}

void ListBoxX::ClearRegisteredImages()
{
	xset.clear();
}

void ListBoxX::SetDoubleClickAction(CallBackAction action,void *data)
{
	doubleClickAction = action;
	doubleClickActionData = data;
}

ListBox::ListBox()
{
}

ListBox::~ListBox()
{
}

ListBox *ListBox::Allocate()
{
	return new ListBoxX();
}

///////////////////////////////////////////////////////////////////////////////////////

Menu::Menu() 
 : id(0)
{
}

void Menu::CreatePopUp()
{
	Destroy();
	id = new QPopupMenu;
}

void Menu::Destroy()
{
	QPopupMenu *m = PMenu(id);

	if (m)
	{
		delete m;
		id = 0;
	}
}

void Menu::Show(Point pt,Window &)
{
	PMenu(id)->popup(QPoint(pt.x,pt.y));
}

///////////////////////////////////////////////////////////////////////////////////////

ElapsedTime::ElapsedTime()
{
	QTime curTime = QTime::currentTime();

	bigBit = curTime.hour() * 60 * 60 + curTime.minute() * 60 + curTime.second();
	littleBit = curTime.msec();
}

double ElapsedTime::Duration(bool reset)
{
	long endBigBit, endLittleBit;
	QTime curTime = QTime::currentTime();

	endBigBit = curTime.hour() * 60 * 60 + curTime.minute() * 60 + curTime.second();
	endLittleBit = curTime.msec();

	double duration = endBigBit - bigBit;

	if (duration < 0 || (duration == 0 && endLittleBit < littleBit))
		duration += 24 * 60 * 60;

	duration += (endLittleBit - littleBit) / 1000.0;

	if (reset)
	{
		bigBit = endBigBit;
		littleBit = endLittleBit;
	}

	return duration;
}

///////////////////////////////////////////////////////////////////////////////////////

ColourDesired Platform::Chrome()
{
	return ColourDesired(0xe0,0xe0,0xe0);
}

ColourDesired Platform::ChromeHighlight()
{
	return ColourDesired(0xff,0xff,0xff);
}

const char *Platform::DefaultFont()
{
	return QApplication::font().family();
}

int Platform::DefaultFontSize()
{
	return QApplication::font().pointSize();
}

unsigned int Platform::DoubleClickTime()
{
	return QApplication::doubleClickInterval();
}

bool Platform::MouseButtonBounce()
{
	return true;
}

void Platform::DebugDisplay(const char *s)
{
	GetConsole().Message("%s",s);
}

bool Platform::IsKeyDown(int)
{
	return false;
}

long Platform::SendScintilla(WindowID w, unsigned int msg, unsigned long wParam, long lParam)
{
  //TODO
  return 0;	
  //return static_cast<ScintillaBase *>(PWidget(w)->parentWidget())->WndProc(msg,wParam,lParam);
}

long Platform::SendScintillaPointer(WindowID w, unsigned int msg, unsigned long wParam, void *lParam)
{
  //TODO
  return 0;	
  //return static_cast<ScintillaBase *>(PWidget(w)->parentWidget())->WndProc(msg,wParam,reinterpret_cast<long>(lParam));
}

bool Platform::IsDBCSLeadByte(int codepage,char ch)
{
	return false;
}

int Platform::DBCSCharLength(int codePage,const char *s)
{
	return 1;
}

int Platform::DBCSCharMaxLength()
{
	return 2;
}

int Platform::Minimum(int a,int b)
{
	if (a < b)
		return a;
	else
		return b;
}

int Platform::Maximum(int a,int b)
{
	if (a > b)
		return a;
	else
		return b;
}


//#define TRACE

#ifdef TRACE

void Platform::DebugPrintf(const char *format, ...)
{
	char buffer[2000];
	va_list pArguments;

	va_start(pArguments,format);
	vsprintf(buffer,format,pArguments);
	va_end(pArguments);

	DebugDisplay(buffer);
}
#else

void Platform::DebugPrintf(const char *, ...)
{
}

#endif

static bool assertionPopUps = true;

bool Platform::ShowAssertionPopUps(bool assertionPopUps_)
{
	bool ret = assertionPopUps;
	assertionPopUps = assertionPopUps_;
	return ret;
}

void Platform::Assert(const char *c,const char *file,int line)
{
	qFatal("Assertion [%s] failed at %s %d\n",c,file,line);
}

int Platform::Clamp(int val,int minVal,int maxVal)
{
	if (val > maxVal)
		val = maxVal;

	if (val < minVal)
		val = minVal;

	return val;
}
