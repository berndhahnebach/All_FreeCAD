/***************************************************************************
                          ScintillaQt.h  -  description
                             -------------------
    begin                : 2002/12/20 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file ScintillaQt.h
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
#include "../Base/Parameter.h"
#include "../Base/Observer.h"

#define SciQt_MARKER_BREAKPNT 0
#define SciQt_MARKER_BOOKMARK 1
#define SciQt_COMMENT         1
#define SciQt_NUMBER          2
#define SciQt_STRING          3
#define SciQt_CHARACTER       4
#define SciQt_KEYWORDS        5
#define SciQt_TRIPLE          6
#define SciQt_TRIPLEDOUBLE    7
#define SciQt_CLASSNAME       8
#define SciQt_DEFNAME         9
#define SciQt_OPERATOR       10
#define SciQt_TEXT           11
#define SciQt_COMMENTBLOCK   12
#define SciQt_STRINGEOL      13

// forward declaration
class FCScintillaEdit;

/**
 * The Qt binding to the Scintilla editor
 * Scintilla is a free source code editing component. It comes with complete source code 
 * and a license that permits use in any free project or commercial product. 
 * As well as features found in standard text editing components, Scintilla includes features 
 * especially useful when editing and debugging source code. These include support for syntax 
 * styling, error indicators, code completion and call tips. The selection margin can contain 
 * markers like those used in debuggers to indicate breakpoints and the current line. 
 * Styling choices are more open than with many editors, allowing the use of proportional fonts, 
 * bold and italics, multiple foreground and background colours and multiple fonts. 
 *
 * The source code can be downloaded at <a href="http://www.scintilla.org"></a>. 
 */
class ScintillaQt : public ScintillaBase
{
  public:
    /** construction */
	  ScintillaQt(FCScintillaEdit* edit);
	  virtual ~ScintillaQt();

    /** Sends messages to Scintilla */
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

	  FCScintillaEdit* m_pEditor;
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
  
  friend class FCScintillaEdit;
  friend class FCScintCallTip;
  friend class FCScintillaView;
};

/**
 * The Qt editor embedding Scintilla 
 */
class FCScintillaEdit : public QWidget, public FCParameterGrp::ObserverType
{
	Q_OBJECT

  public:
	  FCScintillaEdit(QWidget *parent = 0,const char *name = 0, WFlags f = 0);
	  virtual ~FCScintillaEdit();

    /** Sends messages to Scintilla */
	  long SendScintilla(unsigned int msg,unsigned long wParam = 0, long lParam = 0);
	  virtual QSize sizeHint() const;
    ScintillaQt* getTextEditor() const;
    bool toggleBreakpoint(int nLine);
    void clearAllBreakpoints();
    void setBreakpoint(int nLine);
    void gotoLine(int nLine);
    void grabFocus();
    void setReadOnly(bool bReadOnly);
    void gotoLastLine();
    void setStackTraceLevel(int nLevel);
    int  getCurrentLine();
    QString getLine(int nLine);
    CharacterRange getSelection();
    void assignKey(int key, int mods, int cmd);
    bool presentBookmark(int nLine);
    void clearAllBookmarks();
    bool toggleBookmark(int nLine);
    void nextBookmark(bool forwardScan);
    void gotoLineEnsureVisible(int nLine);
  	void expand(int &line, bool doExpand, bool force = false, int visLevels = 0, int level = -1);
    void foldChanged(int line, int levelNow, int levelPrev);
    void foldAll();
    void toggleFold(int nLine);
    bool clickMargin(int position, int modifiers);
	  void OnChange(FCSubject<const char*> &rCaller,const char* rcReason);

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

    void loadSettings();
    void DefineMarker(int marker, int markerType, long fore, long back);

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

/**
 * A special view class which sends the messages from the application to
 * the Scintilla editor and embeds the editor in a window
 */
class FCScintillaView : public FCView
{
	public:
	  FCScintillaView( QWidget* parent, const char* name);
	  ~FCScintillaView();

    FCScintillaEdit* GetEditor() const { return view; }

    virtual const char *GetName(void){return "Scintilla";}
		virtual void Update(void){};

		/** Mesage handler.
     * Runs the action specified by pMsg.
     */
		virtual bool OnMsg(const char* pMsg);
		/** Mesage handler test
     * Checks if the action pMsg is available. This is for enabling/disabling
     * the corresponding buttons or menu items for this action.
     */
		virtual bool OnHasMsg(const char* pMsg);
		/** checking on close state */
		virtual bool CanClose(void);
		virtual void Print(QPainter& cPrinter);
    void OpenFile (const QString& fileName);

    /** @name Standard actions of the editor */
    //@{
    bool Save   (void);
    bool SaveAs (void);
    bool Open   (void);
		void Cut    (void);
		void Copy   (void);
		void Paste  (void);
    void Undo   (void);
    void Redo   (void);
    //@}

    
    void saveFile();
    bool isAlreadySavedBefore();
    QString _fileName;

	protected:
		virtual void resizeEvent(QResizeEvent* e);
    FCScintillaEdit* view;
};

#endif
