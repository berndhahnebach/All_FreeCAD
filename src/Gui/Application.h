#ifndef APPLICATION_H
#define APPLICATION_H

#include "../Base/Export.h"

#include <qmainwindow.h>
#include <qaction.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#include <qworkspace.h>

#include <qextmdimainfrm.h>


#include "../App/Application.h"
#include "Command.h"

class FCGuiDocument;

class GuiExport ApplicationWindow: public QextMdiMainFrm, public FCApplicationObserver
{
    Q_OBJECT
 
	
public:
    ApplicationWindow();
    ~ApplicationWindow();

	static QWorkspace * getWorkspace();
	static ApplicationWindow* getApplication();
	//static QString getResourceDir();

	void CreateTestOperations();

	// Observer
	virtual void OnDocNew(FCDocument* pcDoc);
	virtual void OnDocDelete(FCDocument* pcDoc);

protected:
	FCCommandManager _cCommandManager;

signals:
	void sendQuit();

// slots for the Application signals
public slots:
    void slotNewDoc(void);
	void slotOpen  (void);
	void slotSave  (void);
	void slotSaveAs(void);
	void slotPrint (void);
	void slotUndo  (void);
	void slotRedo  (void);
	void slotCut   (void);
	void slotCopy  (void);
	void slotPaste (void);
    
	
	//void onCloseWindow();
	//void onCloseDocument(Document* theDoc);
/*
	void onViewToolBar();
	void onViewStatusBar();
	void onAbout();
	void onQuit();
	void onTranslate();
	void onCreateNewView();
	void windowsMenuAboutToShow();
        void onWindowActivated ( QWidget * w );
        void windowsMenuActivated(int id);*/
        void exportImage();/*
private:
	void createStandardOperations();
	//void createCasCadeOperations();
	//void createTranslatePopups();
	//void createWindowPopup();
	*/

protected: // Protected methods
   /** just fits the system menu button position to the menu position */
   virtual void resizeEvent ( QResizeEvent * );

private:
    int myNbDocuments;
	bool myIsDocuments;
	//stlport::vector<QAction*> vcStdActions;
	//QList<QAction> myCasCadeTranslateActions;
	QToolBar* myStdToolBar;
	QToolBar* myCasCadeBar;
	QPopupMenu* myFilePopup;
	QPopupMenu* myWindowPopup;
	QPopupMenu* myImportPopup;
	QPopupMenu* myExportPopup;
};

#endif
