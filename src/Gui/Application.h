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
public slots:
    FCGuiDocument* onNewDoc();
	void Open(void);
	void Save(void);
	void SaveAs(void);
	void Print(void);
	void Undo(void);
	void Redo(void);
	void Cut(void);
	void Copy(void);
	void Paste(void);
    
	
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
