#ifndef APPLICATION_H
#define APPLICATION_H


#include <qmainwindow.h>
#include <qaction.h>
#include <qworkspace.h>
#include <qthread.h>

#include <qextmdimainfrm.h>

class FCUndoRedoDlg;
class QPopupMenu;
class QToolBar;

#include "../Base/Console.h"
#include "../App/Application.h"
#include "Command.h"
#include "Widgets.h"
#include <list>
#include <vector>
#include <map>

class FCGuiDocument;
class QComboBox;
class FCDockWindow;
class QToolBar;
class FCView;
class FCToolboxGroup;
class FCAutoWaitCursor;
class FCCmdBar;
class FCHtmlView;

/** The Bitmap Factory
  * the main purpos is to collect all build in Bitmaps and 
  * hold all paths for the extern bitmaps (files) to serve
  * as a single point of axcesing bitmaps in FreeCAD
  */
class FCBmpFactory
{
public:
	FCBmpFactory(void);
	~FCBmpFactory();
	/// Adds a path were pixmaps can be found
	void AddPath(const char* sPath);
	/// Remove a path from the list of pixmap paths
	void RemovePath(const char* sPath);

	/// Add a build in XPM pixmap under a given name
	void AddXPM(const char* sName, const char* pXPM);
	/// Remove a build in pixmap by a given name
	void RemoveXPM(const char* sName);

	/// retrive a pixmap by name
	QPixmap GetPixmap(const char* sName);

protected:
	FCmap<FCstring,const char*> _mpXPM;
	FCvector<FCstring>          _vsPaths;
};


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

	void CreateTestOperations();

	// Observer
	virtual void OnDocNew(FCDocument* pcDoc);
	virtual void OnDocDelete(FCDocument* pcDoc);

	/// message when a GuiDocument is about to vanish
	void OnLastWindowClosed(FCGuiDocument* pcDoc);

	/// some kind of singelton
	static ApplicationWindow* Instance;

	/// send Messages to the active view
	bool SendMsgToActiveView(const char* pMsg);
	/// returns the active view or NULL
	FCView* GetActiveView(void);

	/// Geter for the Active View
	FCGuiDocument* GetActiveDocument(void);


	/// Reference to the command manager
	FCCommandManager &GetCommandManager(void){return _cCommandManager;}

	/// Referenc to the bitmap factory
	FCBmpFactory     &GetBmpFactory(void){return _cBmpFactory;}

	/// Get a named Toolbar or creat if not in
	QToolBar *GetToolBar(const char* name);
	/// Delete a named Toolbar
	void DelToolBar(const char* name);

	// Get a named Command bar view or creat if not in
	FCToolboxGroup *GetCommandBar(const char* name);

	/// Delete a named Command bar view
	void DelCommandBar(const char* name);

	/// Add a new named Dock Window
	void          AddDockWindow(const char* name,FCDockWindow *pcDocWindow, const char* sCompanion = NULL,KDockWidget::DockPosition pos = KDockWidget::DockRight);
	/// Gets you a registered Dock Window back
	FCDockWindow *GetDockWindow(const char* name);
	/// Delete (or only remove) a named Dock Window
	void          DelDockWindow(const char* name, bool bOnlyRemove = false);

	// set text to the pane
	void SetPaneText(int i, QString text);
	FCProgressBar* GetProgressBar();

	/// Activate a named workbench
	void ActivateWorkbench(const char* name);
	/// update the combo box when there are changes in the workbenches
	void UpdateWorkbenchEntrys(void);

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	// static python wrapper of the exported functions
	PYFUNCDEF_S(sToolbarAddTo);
	PYFUNCDEF_S(sToolbarDelete);
	PYFUNCDEF_S(sToolbarAddSeperator);

	PYFUNCDEF_S(sCommandbarAddTo);
	PYFUNCDEF_S(sCommandbarDelete);
	PYFUNCDEF_S(sCommandbarAddSeperator);

	PYFUNCDEF_S(sWorkbenchAdd);
	PYFUNCDEF_S(sWorkbenchDelete);
	PYFUNCDEF_S(sWorkbenchActivate);
	PYFUNCDEF_S(sWorkbenchGet);

	static PyMethodDef    Methods[]; 
 

signals:
	void sendQuit();
    void timeEvent();

// slots for the Application signals
public slots:
	void exportImage();

protected: // Protected methods
   /** just fits the system menu button position to the menu position */
   virtual void resizeEvent ( QResizeEvent * );
	/// Handels all commands 
	FCCommandManager _cCommandManager;
	FCBmpFactory     _cBmpFactory;
	/// waiting cursor stuff 
    void timerEvent( QTimerEvent * e){emit timeEvent();}

// slots for the undo/redo dialog: (not implemented yet)
protected slots:
    void slotUndo(){};
    void slotRedo(){};
    void updateUndo();
    void updateRedo();
    void executeUndoRedo();

	void OnWorkbenchChange( const QString & string);

private:
#	pragma warning( disable : 4251 )
	/// list of all handled documents
 	FClist<FCGuiDocument*>         lpcDocuments;
	/// list of windows
	FCmap <FCstring,FCDockWindow*> mpcDocWindows;
	/// map of Toolbars
	FCmap <FCstring,QToolBar*>     mpcToolBars;
#	pragma warning( default : 4251 )
	/// Active document
	FCGuiDocument*   _pcActiveDocument;
	QPopupMenu*		 _pcPopup;
	FCUndoRedoDlg*	 _pclUndoRedoWidget;
	QComboBox *		 _pcWorkbenchCombo;
	QLabel *         _pclSizeLabel, *_pclActionLabel;
	FCProgressBar *  _pclProgress;
	FCCmdBar*        _pcCmdBar;
	FCHtmlView*		 _pcHtmlView;
	/// workbench python dictionary
	PyObject*		 _pcWorkbenchDictionary;
	QString			 _cActiveWorkbenchName;


    
};



/** The console observer for the Application window
 *  This class distribute the Messages issued on the console
 *  to the status bar. 
 *  @see FCConsole
 *  @see FCConsoleObserver
 */
class FCAppConsoleObserver: public FCConsoleObserver
{
public:
	FCAppConsoleObserver(ApplicationWindow *pcAppWnd);
		
	/// get calles when a Warning is issued
	virtual void Warning(const char *m);
	/// get calles when a Message is issued
	virtual void Message(const char * m);
	/// get calles when a Error is issued
	virtual void Error  (const char *m);
	/// get calles when a Log Message is issued
	virtual void Log    (const char *);
protected:
	ApplicationWindow* _pcAppWnd;
};


/** The FCAutoWaitCursor sets automatically the 
 *  waiting cursor if the application is busy
 */
class GuiExport FCAutoWaitCursor : public QObject, public QThread
{

	Q_OBJECT

public:
	static void Destruct(void);
	static FCAutoWaitCursor &Instance(void);
	// getter/setter
	int GetInterval();
	void SetInterval(int i);
	void SetWaitCursor();

  // Singleton
private:
#ifdef WNT // windows os
    FCAutoWaitCursor(DWORD id, int i);
#else
    FCAutoWaitCursor(int i);
#endif

    static FCAutoWaitCursor* _pclSingleton;

protected:
    void run();

    QMutex awcMutex;
    int iAutoWaitCursorCounter;
    int iAutoWaitCursorMaxCount;
    int iInterval;
    bool bOverride;
#ifdef WNT
  	DWORD main_threadid;
#endif

public slots:
    void timeEvent();
};







#endif
