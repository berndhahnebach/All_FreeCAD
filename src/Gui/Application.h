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
 	FClist<FCGuiDocument*> lpcDocuments;
#	pragma warning( default : 4251 )
	/// Active document
	FCGuiDocument* _pcActiveDocument;
	QToolBar*		_pcStdToolBar;
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




#endif
