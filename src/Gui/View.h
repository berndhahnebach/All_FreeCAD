#ifndef __FCview_h__
#define __FCview_h__


#include "Window.h"

class FCGuiDocument;
class QSplitter;
class QWidget;		


/** Base class of all windows belonging to a document
 */
class FCView : public FCWindow
{
	Q_OBJECT;

public:
	FCView( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCView();
	FCGuiDocument* GetGuiDocument(){return _pcDocument;}

	virtual const char *GetName(void)=0;

	/// Mesage handler
	virtual bool OnMsg(const char* pMsg);

signals:
	/// sends a message to the document
	void sendCloseView(FCView* theView);
	
public slots:
	void ViewMsg(const char* pMsg);        


protected:
    FCGuiDocument*	_pcDocument;

};

/** The double view container class
 *  This class can handle two FCView instances seperated by
 *  a splitter. Usuly its used to show a FCDocTree and a FC3dView
 */
class FCDoubleView: public FCViewContainer
{
	Q_OBJECT;

public:
	FCDoubleView( FCView* pcView1, FCView* pcView2, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCDoubleView();

	// a more sophisticated implementation later
	virtual FCView* GetActiveView(void){return _pcView2;}
private:
	FCView *		_pcView1;
	FCView *		_pcView2;
	QSplitter*		_pcSplitter;

};

/** The singel view container class
 *  This class can handle one FCView instances 
 */
class FCSingelView: public FCViewContainer
{
	Q_OBJECT;

public:
	FCSingelView( FCView* pcView, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCSingelView();

	virtual void resizeEvent ( QResizeEvent * e);

	virtual FCView* GetActiveView(void){return _pcView;}

private:
	FCView *		_pcView;

};


/** Holds a single view in a dock bar
 */
class FCViewBar: public FCWindow
{
	Q_OBJECT;

public:
	FCViewBar( FCView* pcView, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCViewBar();

	virtual FCView* GetView(void){return _pcView;}

protected:
	virtual void resizeEvent ( QResizeEvent * e);

private:
	FCView *		_pcView;

};


#endif