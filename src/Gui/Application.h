#ifndef APPLICATION_H
#define APPLICATION_H


#include <qmainwindow.h>
#include <qaction.h>
#include <qworkspace.h>

#include <qextmdimainfrm.h>

class FCUndoRedoDlg;
class QPopupMenu;
class QToolBar;

#include "../Base/Console.h"
#include "../App/Application.h"
#include "Command.h"
#include <list>

class FCGuiDocument;
class QComboBox;
class FCDockWindow;
class QToolBar;




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
	/// send Messages to all views
	bool SendMsgToViews(const char* pMsg);

	/// set the parameter to the active document or reset in case of 0
	void SetActive(FCGuiDocument* pcView);
	/// Geter for the Active View
	FCGuiDocument* GetActiveView(void);


	/// Get a named Toolbar or creat if not in
	QToolBar *GetToolBar(const char* name);
	/// Delete a named Toolbar
	void      DelToolBar(const char* name);

	/// Add a new named Dock Window
	void          AddDockWindow(const char* name,FCDockWindow *pcDocWindow,KDockWidget::DockPosition pos = KDockWidget::DockRight, int percent = 82);
	/// Gets you a registered Dock Window back
	FCDockWindow *GetDockWindow(const char* name);
	/// Delete (or only remove) a named Dock Window
	void          DelDockWindow(const char* name, bool bOnlyRemove = false);



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
};



/** The console observer for the Application window
 *  This class distribute the Messages issued on the console
 *  to the status bar. 
 *  @see FCConsole
 *  @see FCConsoleObserver
#include <qapplication.h>
#include <qtimer.h>
#include <qthread.h>
#include <qcursor.h>

class FCAutoWaitCursor
{
  public:
  	FCAutoWaitCursor()
		{
//	  	if (main_threadid == 0)
			{
//			  main_threadid = GetCurrentThreadId();
//			  prev_hook = SetWindowsHookEx(WH_GETMESSAGE, getmessage_hook, 0, main_threadid);
//			  AfxBeginThread(timer_thread, 0, THREAD_PRIORITY_ABOVE_NORMAL, 1000, 0, NULL );
			}

//  		ticks = 5; // start the timer
		}

	  virtual ~FCAutoWaitCursor()
		{
//  		ticks = 0; // stop timer
		  qApp->restoreOverrideCursor();
			// restore cursor
      QPoint pt = QCursor::pos();
      QCursor::setPos(pt);
		}
  private:
	  static UINT timer_thread(LPVOID lpParameter)
		{
      // according to MSDN, need to do API call before AttachThreadInput
//  		AttachThreadInput(GetCurrentThreadId(), main_threadid, true);
//	  	for (;;)
			{
//		  	Sleep(25); // milliseconds
//			  if (ticks > 0 && --ticks == 0)
				{
          qApp->setOverrideCursor(WaitCursor);
				}
			}
    }
  
    static LRESULT CALLBACK getmessage_hook(int code, WPARAM wParam, LPARAM lParam)
		{
  		MSG * pMsg = (MSG*) lParam;
	  	if (lParam)
		  {
//  			if (pMsg->message!=0x202)
//				ticks = 0; // reset timer
	  	}
  
//      return CallNextHookEx(prev_hook, code, wParam, lParam);
		}	
  	
//    static DWORD main_threadid;
//	  static HHOOK prev_hook;
//  	static int /*volatile*/ //ticks;
//	  static QCursor /*volatile*/ prev_cursor;
//};

//DWORD FCAutoWaitCursor::main_threadid = 0;
//int /*volatile*/  FCAutoWaitCursor::ticks = 0;
//HHOOK  FCAutoWaitCursor::prev_hook = 0;
/*
class GuiExport FCMainApplication : public QApplication
{
  Q_OBJECT
public:
  FCMainApplication ( int & argc, char ** argv ):QApplication(argc, argv)
  {
    installEventFilter( this );
    timer = new QTimer( this );
//    connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );
  }
  bool notify ( QObject * receiver, QEvent * event )
  {
    return QApplication::notify(receiver, event);
  }
  protected:
      bool  eventFilter( QObject * o, QEvent * e)
      {
        FCAutoWaitCursor awc;
        timer->start( 200, TRUE );                 // 2 seconds single-shot
        return QApplication::eventFilter( o, e );    // standard event processing      
      }
  protected slots:
    void timerDone()
    {
      printf("Dauert etwas laenger...\n");
    }
  protected:
    QTimer *timer;
};



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




#endif
