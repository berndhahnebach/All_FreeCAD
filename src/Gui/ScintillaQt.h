#ifndef SCINTILLAQT_H
#define	SCINTILLAQT_H


#include "scintilla/Platform.h"

#include "scintilla/Scintilla.h"
#include "scintilla/SString.h"
#include "scintilla/ContractionState.h"
#include "scintilla/SVector.h"
#include "scintilla/CellBuffer.h"
#include "scintilla/CallTip.h"
#include "scintilla/KeyMap.h"
#include "scintilla/Indicator.h"
#include "scintilla/XPM.h"
#include "scintilla/LineMarker.h"
#include "scintilla/Style.h"
#include "scintilla/AutoComplete.h"
#include "scintilla/ViewStyle.h"
#include "scintilla/Documents.h"
#include "scintilla/Editor.h"

#ifdef SCI_LEXER
# include "scintilla/Accessor.h"
# include "scintilla/PropSet.h"
# include "scintilla/KeyWords.h"
#endif

#include "scintilla/ScintillaBase.h"
#include "scintilla/UniConversion.h"

#include "View.h"


// forward declaration
class FCScintEditor;

class ScintillaQt : public ScintillaBase
{
  public:
	  ScintillaQt(FCScintEditor* edit);
	  virtual ~ScintillaQt();

	  virtual sptr_t WndProc(unsigned int iMessage,uptr_t wParam, sptr_t lParam);

  private:
	  virtual void Initialise();
	  virtual void Finalise();

	  static sptr_t DirectFunction(ScintillaQt *sci, unsigned int iMessage, uptr_t wParam, sptr_t lParam);
	  
    virtual void StartDrag();
	  virtual sptr_t DefWndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam);
	  virtual void SetTicking(bool on);
	  virtual void SetMouseCapture(bool on);
	  virtual bool HaveMouseCapture();
	  virtual void ScrollText(int linesToMove);
	  virtual void SetVerticalScrollPos();
	  virtual void SetHorizontalScrollPos();
	  virtual bool ModifyScrollBars(int nMax, int nPage);
	  virtual void NotifyChange();
	  virtual void NotifyFocus(bool focus);
  	virtual int  GetCtrlID();
	  virtual void NotifyParent(SCNotification scn);
	  virtual void NotifyDoubleClick(Point pt, bool shift);
	  virtual void Copy();
	  virtual bool CanPaste();
	  virtual void Paste();
	  virtual void CreateCallTipWindow(PRectangle rc);
	  virtual void AddToPopUp(const char *label, int cmd = 0, bool enabled = true);
	  virtual void ClaimSelection();
    virtual void CopyToClipboard(const SelectionText &selectedText){}
	  int KeyDefault(int key,int modifiers);

    QString convertText(SelectionText* text);
    
  protected:
    void paintEvent(QPaintEvent* e);
	  void dragEnterEvent(QDragEnterEvent* e);
	  void dragMoveEvent(QDragMoveEvent* e);
	  void dragLeaveEvent(QDragLeaveEvent* e);
	  void dropEvent(QDropEvent* e);

  private:
    bool lastKeyDownConsumed;
	  bool capturedMouse;
	  unsigned int linesPerScroll;	///< Intellimouse support
	  int wheelDelta; ///< Wheel delta from roll
  	bool hasOKText;

	  FCScintEditor* m_pEditor;
	  QTimer m_clTimer;

  private:
    class FCScintCallTip : public QWidget
    {
      public:
	      FCScintCallTip(ScintillaQt* sci, QWidget* parent, const char * name=0);
	      ~FCScintCallTip();

      protected:
	      void paintEvent(QPaintEvent* e);
	      void mousePressEvent(QMouseEvent* e);

      private:
	      ScintillaQt *sciTE;
    };
  
  friend class FCScintEditor;
  friend class FCScintCallTip;
};

class FCScintEditor : public QWidget
{
	Q_OBJECT

  public:
	  FCScintEditor(QWidget *parent = 0,const char *name = 0, WFlags f = 0);
	  virtual ~FCScintEditor();

	  long SendScintilla(unsigned int msg,unsigned long wParam = 0, long lParam = 0);
	  virtual QSize sizeHint() const;
    void openFile(const char* fileName);

  protected:
	  bool eventFilter                   (QObject *o,QEvent *e);
	  virtual void keyPressEvent         (QKeyEvent   * e);
    virtual void keyReleaseEvent       (QKeyEvent   * e);
	  virtual void focusInEvent          (QFocusEvent * e);
	  virtual void focusOutEvent         (QFocusEvent * e);
	  virtual void mousePressEvent       (QMouseEvent * e);
	  virtual void mouseReleaseEvent     (QMouseEvent * e);
	  virtual void mouseDoubleClickEvent (QMouseEvent * e);
	  virtual void mouseMoveEvent        (QMouseEvent * e);
    virtual void mouseWheelEvent       (QWheelEvent * e);

    virtual bool focusNextPrevChild (bool next);

  private slots:
	  void slotTimer();
	  void slotVerScroll  (int value);
	  void slotHorScroll  (int value);
	  void slotContextMenu(int cmd);

  private:
	  ScintillaQt* sciTE;
	  QWidget    * view;
	  QScrollBar * verScroll;
	  QScrollBar * horScroll;
    bool _bControlButton;

    friend class ScintillaQt;
};

class FCScintEditor;
class FCScintillaDoc;
class FCScintEditView : public FCFloatingView
{
  public:
	  FCScintEditView(FCScintillaDoc* pcDoc, QWidget* parent, const char* name);
	  ~FCScintEditView();

    FCScintEditor* GetEditor() const { return view; }

  protected:
    FCScintEditor* view;
};

class FCScintillaView : public FCFloatingChildView
{
  public:
	  FCScintillaView( FCFloatingView* pcView, QWidget* parent, const char* name);
	  ~FCScintillaView();

  protected:
    virtual void closeEvent(QCloseEvent *e);
};

class FCScintillaDoc : public FCFloatingDoc
{
  Q_OBJECT

  public:
    FCScintillaDoc();
    ~FCScintillaDoc();
    void CreateView(const char* name);
    bool Save   (void);
    bool SaveAs (void);
    bool Open   (void);
};

#endif
