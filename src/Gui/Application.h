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

class FCGuiDocument;
class QComboBox;
class FCDockWindow;
class QToolBar;
class FCView;

class FCAutoWaitCursor;


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

	static ApplicationWindow* getApplication();

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

	/// Get a named Toolbar or creat if not in
	QToolBar *GetToolBar(const char* name);
	/// Delete a named Toolbar
	void DelToolBar(const char* name);

	/// Add a new named Dock Window
	void          AddDockWindow(const char* name,FCDockWindow *pcDocWindow, const char* sCompanion = NULL,KDockWidget::DockPosition pos = KDockWidget::DockRight);
	/// Gets you a registered Dock Window back
	FCDockWindow *GetDockWindow(const char* name);
	/// Delete (or only remove) a named Dock Window
	void          DelDockWindow(const char* name, bool bOnlyRemove = false);

  // set text to the pane
  void SetPaneText(int i, QString text);
  FCProgressBar* GetProgressBar();



protected:
	/// Handels all commands 
	FCCommandManager _cCommandManager;

signals:
	void sendQuit();

// slots for the Application signals
public slots:
	void exportImage();

protected: // Protected methods
   /** just fits the system menu button position to the menu position */
   virtual void resizeEvent ( QResizeEvent * );

// slots for the undo/redo dialog: (not implemented yet)
protected slots:
    void slotUndo(){};
    void slotRedo(){};
    void updateUndo();
    void updateRedo();
    void executeUndoRedo();


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
	FCGuiDocument* _pcActiveDocument;
	QPopupMenu*		_pcPopup;
	FCUndoRedoDlg*	_pclUndoRedoWidget;
	QComboBox *		_pcWorkbenchCombo;
  QLabel *_pclSizeLabel, *_pclActionLabel;
  FCProgressBar *_pclProgress;

  // waiting cursor stuff 
  protected:
    void timerEvent( QTimerEvent * e)
    {
      emit timeEvent();
    }
    
  signals:
    void timeEvent();
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
	static void Destruct(void)
  {
  	// not initialized or double destruct!
    assert(_pclSingleton);
	  delete _pclSingleton;
  }

	static FCAutoWaitCursor &Instance(void)
  {
	  // not initialized?
	  if(!_pclSingleton)
	  {
#ifdef WNT
		  _pclSingleton = new FCAutoWaitCursor(GetCurrentThreadId(), 100);
#else
		  _pclSingleton = new FCAutoWaitCursor(100);
#endif
	  }

    return *_pclSingleton;
  }

  // getter/setter
  int GetInterval()
  {
    return iInterval;
  }

  void SetInterval(int i)
  {
    iInterval = i;
  }

  void SetWaitCursor()
  {
#ifdef WNT // win32 api functions
		AttachThreadInput(GetCurrentThreadId(), main_threadid, true);
		SetCursor(LoadCursor(NULL, IDC_WAIT));
#endif
  }

  // Singleton
  private:
#ifdef WNT // windows os
    FCAutoWaitCursor(DWORD id, int i)
		:main_threadid(id), iInterval(i)
    {
      iAutoWaitCursorMaxCount = 3;
      iAutoWaitCursorCounter  = 3;
      bOverride = false;
      start();
    }
#else
    FCAutoWaitCursor(int i)
		: iInterval(i)
    {
      iAutoWaitCursorMaxCount = 3;
      iAutoWaitCursorCounter  = 3;
      bOverride = false;
      start();
    }
#endif

    static FCAutoWaitCursor* _pclSingleton;

  protected:
    void run()
    {
      while (true)
      {
        // set the thread sleeping
        msleep(iInterval);

        // decrements the counter
        awcMutex.lock();
        if (iAutoWaitCursorCounter > 0)
          iAutoWaitCursorCounter--;
        awcMutex.unlock();

        // set waiting cursor if the application is busy
        if (iAutoWaitCursorCounter == 0)
        {
          // load the waiting cursor only once
          if (bOverride == false)
          {
            SetWaitCursor();
            bOverride = true;
          }
        }
        // reset
        else if (bOverride == true)
        {
          // you need not to restore the old cursor because 
          // the application window does this for you :-))
          bOverride = false;
        }
      }
    }

    QMutex awcMutex;
    int iAutoWaitCursorCounter;
    int iAutoWaitCursorMaxCount;
    int iInterval;
    bool bOverride;
#ifdef WNT
  	DWORD main_threadid;
#endif

  public slots:
    void timeEvent()
    {
      // NOTE: this slot must be connected with the timerEvent of your class
      // increments the counter
      awcMutex.lock();
      if (iAutoWaitCursorCounter < iAutoWaitCursorMaxCount)
        iAutoWaitCursorCounter++;
      awcMutex.unlock();
    }
};

#endif
