/***************************************************************************
                          SciEditor.h  -  description
                             -------------------
    begin                : 2002/12/20 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file SciEditor.h
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
 

#ifndef SCIEDITOR_H
#define	SCIEDITOR_H

#include "ScintillaQt.h"

/**
 * The Qt editor embedding Scintilla 
 */
class SciTEBase : public QScrollView
{
	Q_OBJECT

  public:
	  SciTEBase(QWidget *parent = 0,const char *name = 0, WFlags f = 0);
	  virtual ~SciTEBase();

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

  protected:
	  virtual void keyPressEvent   (QKeyEvent   * e);
    virtual void keyReleaseEvent (QKeyEvent   * e);
	  virtual void focusInEvent    (QFocusEvent * e);
	  virtual void focusOutEvent   (QFocusEvent * e);

    virtual bool focusNextPrevChild (bool next);

    void DefineMarker(int marker, int markerType, long fore, long back);

  protected:
	  ScintillaQt* sciQt;
    bool _bControlButton;
};

class FCMacroEditor : public SciTEBase, public FCParameterGrp::ObserverType
{
  public:
	  FCMacroEditor(QWidget *parent = 0,const char *name = 0, WFlags f = 0);
		~FCMacroEditor();
	  void OnChange(FCSubject<const char*> &rCaller,const char* rcReason);
    void loadSettings();
};

class FCPythonConsole : public SciTEBase
{
	public:
		FCPythonConsole(QWidget *parent = 0,const char *name = 0, WFlags f = 0);
		~FCPythonConsole();

	protected:
		virtual void keyPressEvent(QKeyEvent * e);
		int nInsertTabs;
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

    SciTEBase* GetEditor() const { return view; }

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
    SciTEBase* view;
};

#endif
