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
#include <list>

class FCGuiDocument;
class QComboBox;
class FCDockWindow;
class QToolBar;
class FCView;




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
#define WAITCURSORTEST
//#define WAITCURSORNOTHREAD

#ifdef WAITCURSORNOTHREAD
#  ifndef WAITCURSORTEST
#	define WAITCURSORTEST
#  endif
#endif

class GuiExport FCAutoWaitCursor : public QObject, public QThread
{
  Q_OBJECT

  public:
    FCAutoWaitCursor()
    {
      iAutoWaitCursorMaxCount = 5;
      iAutoWaitCursorCounter  = 5;
      bOverride = false;
    }

    void run()
    {
      while (true)
      {
        msleep(300);

        awcMutex.lock();
        if (iAutoWaitCursorCounter > 0)
          iAutoWaitCursorCounter--;
        awcMutex.unlock();

        if (iAutoWaitCursorCounter == 0)
        {
          if (bOverride == false)
          {
            QApplication::setOverrideCursor(WaitCursor);
            bOverride = true;
          }
        }
        else if (bOverride == true)
        {
          QApplication::restoreOverrideCursor();
          bOverride = false;
        }
      }
    }

    QMutex awcMutex;
    int iAutoWaitCursorCounter;
    int iAutoWaitCursorMaxCount;
    bool bOverride;

  public slots:
    void timeEvent()
    {
      awcMutex.lock();
      if (iAutoWaitCursorCounter < iAutoWaitCursorMaxCount)
        iAutoWaitCursorCounter++;
      awcMutex.unlock();
    }
};

/** The application main class
 */
class GuiExport FCMainApplication : public QApplication
{
  Q_OBJECT

  public:
    FCMainApplication ( int & argc, char ** argv )
      :QApplication(argc, argv)
    {
#ifdef WAITCURSORTEST
      iWaitCursor = 0;
#endif
      startTimer(100);
      waitCursor = new FCAutoWaitCursor;
#ifndef WAITCURSORNOTHREAD
      waitCursor->start();
#endif

      connect(this, SIGNAL(timeEvent()), waitCursor, SLOT(timeEvent()));
    }

  protected:
#ifdef WAITCURSORTEST
    int iWaitCursor;
#endif
    void timerEvent( QTimerEvent * e)
    {
      emit timeEvent();
#ifdef WAITCURSORTEST
      if ((iWaitCursor++) == 50)
      {
#ifdef WAITCURSORNOTHREAD
        setOverrideCursor( WaitCursor );
#endif
        QWaitCondition().wait(3000);
#ifdef WAITCURSORNOTHREAD
        restoreOverrideCursor();
#endif
        iWaitCursor = 0;
      }
#endif
    }

    FCAutoWaitCursor *waitCursor;
  signals:
    void timeEvent();
};

#endif
