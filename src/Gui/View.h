#ifndef __view_h__
#define __view_h__


#include <qmainwindow.h>
#include <qextmdichildview.h>

class FCGuiDocument;
class View3D;
class FCTree;
class QSplitter;
class QWidget;		
class QPushButton;	
class QVBoxLayout;	
class QHBoxLayout;	
class QWidgetStack;
class QTabBar;


class FCView: public QextMdiChildView
{
	Q_OBJECT

public:
	FCView( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCView();
	FCGuiDocument* getDocument();

/*
signals:
	void message(const QString&, int );
	void sendCloseView(FCView* theView);
	*/
public slots:
	void closeEvent(QCloseEvent* e);        
	void onWindowActivated ();
	void setCursor(const QCursor&);
	void dump();

protected:
	//void createViewActions();
private:
    FCGuiDocument*	_pcDocument;
	View3D*			_pcView3D;
	QSplitter*		_pcSplitter;
	FCTree*		    _pcTree;
	QWidget*		_pcWidget;
	QVBoxLayout*	_pcVBoxLayout;    
	QWidgetStack*	_pcWidgetStack;
	QTabBar*		_pcTabBar;

};


#endif