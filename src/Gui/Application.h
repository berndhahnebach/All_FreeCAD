/** \file Application.h
 *  \brief  
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *   
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/
#ifndef APPLICATION_H
#define APPLICATION_H


#include "qextmdi/qextmdimainfrm.h"


#define  putpix()

#include "../Base/Console.h"
#include "../App/Application.h"

class FCGuiDocument;
class QComboBox;
class FCWindow;
class QToolBar;
class FCBaseView;
class FCToolboxBar;
class FCAutoWaitCursor;
class FCStackBar;
class FCHtmlView;
class FCUndoRedoDlg;
class FCMacroManager;
class QPopupMenu;
class QToolBar;
class FCViewBar;
class FCCustomWidgetManager;
class FCProgressBar;
class FCView;
class FCCommandManager;



/** The Applcation main class
 *  This is the central class of the GUI 
 */
class GuiExport ApplicationWindow: public QextMdiMainFrm, public FCApplicationObserver
{
    Q_OBJECT
 
	
public:
	/// construction
    ApplicationWindow();
	/// destruction
    ~ApplicationWindow();

	void CreateStandardOperations();

	// Observer
	virtual void OnDocNew(FCDocument* pcDoc);
	virtual void OnDocDelete(FCDocument* pcDoc);

	/// message when a GuiDocument is about to vanish
	void OnLastWindowClosed(FCGuiDocument* pcDoc);

	/// some kind of singelton
	static ApplicationWindow* Instance;

	/** @name methodes for View handling */
	//@{
	/// send Messages to the active view
	bool SendMsgToActiveView(const char* pMsg);
	/// send Messages test to the active view
	bool SendHasMsgToActiveView(const char* pMsg);
	/// returns the active view or NULL
	FCView* GetActiveView(void);
	/// Geter for the Active View
	FCGuiDocument* GetActiveDocument(void);
	/// Attach a view (get called by the FCView constructor)
	void AttachView(FCBaseView* pcView);
	/// Detach a view (get called by the FCView destructor)
	void DetachView(FCBaseView* pcView);
	/// get calld if a view gets activated, this manage the whole activation scheme
	void ViewActivated(FCView* pcView);
	/// call update to all docuemnts an all views (costly!)
	void Update(void);
	/// call update to all views of the active document
	void UpdateActive(void);
	//@}
  void setPalette(const QPalette&);
  void setAreaPal(const QPalette&);

	/// Set the active document
	void SetActiveDocument(FCGuiDocument* pcDocument);

	/// true when the application shuting down
	bool IsClosing(void);

	/// Reference to the command manager
	FCCommandManager &GetCommandManager(void);
	
	/// Returns the widget manager
	FCCustomWidgetManager* GetCustomWidgetManager(void);

	/** @name status bar handling */
	//@{	
	/// set text to the pane
	void SetPaneText(int i, QString text);
	/// gets a pointer to the Progress bar
	FCProgressBar* GetProgressBar();
	//@}

	/** @name workbench handling */
	//@{	
	/// Activate a named workbench
	void ActivateWorkbench(const char* name);
	/// update the combo box when there are changes in the workbenches
	void UpdateWorkbenchEntrys(void);
	/// returns the name of the active workbench
	QString GetActiveWorkbench(void);
	std::vector<std::string> GetWorkbenches(void);
	//@}

  /// MRU: recent files
  void AppendRecentFile(const char* file);

	/// Get macro manager
	FCMacroManager *GetMacroMngr(void);




	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	// static python wrapper of the exported functions
	PYFUNCDEF_S(sMenuAppendItems); // append items
	PYFUNCDEF_S(sMenuRemoveItems); // remove items
	PYFUNCDEF_S(sMenuDelete);      // delete the whole menu

	PYFUNCDEF_S(sToolbarAppendItems);
	PYFUNCDEF_S(sToolbarRemoveItems);
	PYFUNCDEF_S(sToolbarDelete);

	PYFUNCDEF_S(sCommandbarAppendItems);
	PYFUNCDEF_S(sCommandbarRemoveItems);
	PYFUNCDEF_S(sCommandbarDelete);

	PYFUNCDEF_S(sWorkbenchAdd);
	PYFUNCDEF_S(sWorkbenchDelete);
	PYFUNCDEF_S(sWorkbenchActivate);
	PYFUNCDEF_S(sWorkbenchGet);

	PYFUNCDEF_S(sSendActiveView);

	PYFUNCDEF_S(sUpdateGui);
	PYFUNCDEF_S(sCreateDialog);

	PYFUNCDEF_S(sRunCommand);
	PYFUNCDEF_S(sCommandAdd);

	static PyMethodDef    Methods[]; 
 

signals:
	void sendQuit();
  void timeEvent();

public:
  void Polish();

protected: // Protected methods
	/** just fits the system menu button position to the menu position */
	virtual void resizeEvent ( QResizeEvent * );
	virtual void closeEvent ( QCloseEvent * e );
	virtual bool eventFilter( QObject* o, QEvent *e );
  virtual void keyPressEvent ( QKeyEvent * e );
  virtual void keyReleaseEvent ( QKeyEvent * e );
  virtual bool focusNextPrevChild( bool next );
	/// waiting cursor stuff 
	void timerEvent( QTimerEvent * e){emit timeEvent();}

  // windows stuff
  void LoadWindowSettings();
  void SaveWindowSettings();
  void LoadDockWndSettings();
  void SaveDockWndSettings();

public slots:
	/// this slot get frequently activatet and test the commands if they are still active
	void UpdateCmdActivity();
	/** @name methodes for the UNDO REDO handling 
	 *  this methodes are usaly used by the GUI document! Its not intended
	 *  to use them directly. If the GUI is not up, there is usaly no UNDO / REDO 
	 *  nececary.
	 */
	//@{
	void OnUndo();
	void OnRedo();
	//@}

protected slots:
  void OnShowView();
  void OnShowView(int);

private:
  struct ApplicationWindowP* d;

  // friends
  //
  friend class FCCustomWidgetManager;
};



/** The console observer for the Application window
 *  This class distribute the Messages issued on the console
 *  to the status bar. 
 *  @see FCConsole
 *  @see FCConsoleObserver
 */
class FCGuiConsoleObserver: public FCConsoleObserver
{
public:
	FCGuiConsoleObserver(ApplicationWindow *pcAppWnd);
		
	/// get calles when a Warning is issued
	virtual void Warning(const char *m);
	/// get calles when a Message is issued
	virtual void Message(const char * m);
	/// get calles when a Error is issued
	virtual void Error  (const char *m);
	/// get calles when a Log Message is issued
	virtual void Log    (const char *);

	/// Mutes the Observer, no Dialog Box will appear
	static bool bMute;
protected:
	ApplicationWindow* _pcAppWnd;
};


/** The FCAutoWaitCursor sets automatically the 
 *  waiting cursor if the application is busy
 *  @author Werner Mayer
 */
class GuiExport FCAutoWaitCursor : public QThread
{
	public:
		static void Destruct(void);
		static FCAutoWaitCursor &Instance(void);

		void SetWaitCursor();

		// Singleton
	private:
    FCAutoWaitCursor(uint id, int i);
		~FCAutoWaitCursor();

    static FCAutoWaitCursor* _pclSingleton;

	protected:
    /// run the thread, decrement an internal counter
    void run();

		QMutex mutex;
  	uint main_threadid;
		int nInterval;
		bool bActive;
		bool bRun;

	private:
		class FCAutoWaitCursorP* d;
		friend class FCAutoWaitCursorP;
};

#endif
