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

#include "scintilla/ScintillaBase.h"
#include "scintilla/UniConversion.h"


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
	  int KeyDefault(int key,int modifiers);
	  void ReconfigureScrollBars();

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

	  QWidget *PWindow(Window &w)
	  {
		  return reinterpret_cast<QWidget *>(w.GetID());
	  }

	  FCScintEditor* m_pEditor;
	  QTimer m_clTimer;
  
  friend class FCScintEditor;
};

class FCScintEditor : public QWidget
{
	Q_OBJECT

  public:
	  FCScintEditor(QWidget *parent = 0,const char *name = 0, WFlags f = 0);
	  virtual ~FCScintEditor();

	  long SendScintilla(unsigned int msg,unsigned long wParam = 0, long lParam = 0);
	  virtual QSize sizeHint() const;

  protected:
	  bool eventFilter                   (QObject *o,QEvent *e);
	  virtual void keyPressEvent         (QKeyEvent   * e);
	  virtual void focusInEvent          (QFocusEvent * e);
	  virtual void focusOutEvent         (QFocusEvent * e);
	  virtual void mousePressEvent       (QMouseEvent * e);
	  virtual void mouseReleaseEvent     (QMouseEvent * e);
	  virtual void mouseDoubleClickEvent (QMouseEvent * e);
	  virtual void mouseMoveEvent        (QMouseEvent * e);

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

    friend class ScintillaQt;
};

#endif
