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
class SciTEBase;

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
class ScintillaQt : public QObject, public ScintillaBase
{
	Q_OBJECT

  public:
    /** construction */
	  ScintillaQt(SciTEBase* edit);
	  virtual ~ScintillaQt();

    /** Sends messages to Scintilla */
	  virtual sptr_t WndProc(unsigned int iMessage,uptr_t wParam, sptr_t lParam);
		bool eventFilter(QObject* o, QEvent* e);
		bool event(QEvent* e);
		QSize sizeHint() const;
		void showHScroll(bool);
		void showVScroll(bool);

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
	  void mouseDoubleClickEvent (QMouseEvent * e);
		void resizeEvent(QResizeEvent * e);
		void keyPressEvent(QKeyEvent * e);
		void mousePressEvent(QMouseEvent * e);
		void mouseReleaseEvent(QMouseEvent * e);
		void mouseMoveEvent(QMouseEvent * e);
		void mouseWheelEvent(QWheelEvent * e);
		void timerEvent(QTimerEvent * e);

  private:
    bool lastKeyDownConsumed;
	  bool capturedMouse;
	  unsigned int linesPerScroll;	///< Intellimouse support
	  int wheelDelta; ///< Wheel delta from roll
  	bool hasOKText;

	  SciTEBase* m_pEditor;
	  QTimer m_clTimer;
		QTime  m_clClick;
	  QWidget    * view;

	private slots:
		void onContextMenu(int);
		void onTimer();
	  void onVScroll  (int value);
	  void onHScroll  (int value);

  private:
    class ScintCallTip : public QWidget
    {
      public:
	      ScintCallTip(CallTip* ct, ScintillaQt* sci, QWidget* parent, const char * name=0);
	      ~ScintCallTip();

      protected:
	      void paintEvent(QPaintEvent* e);
	      void mousePressEvent(QMouseEvent* e);

      private:
				CallTip*  callTip;
	      ScintillaQt *sciTE;
    };
};

#endif
