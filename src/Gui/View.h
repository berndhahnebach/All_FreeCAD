#ifndef __FCview_h__
#define __FCview_h__


#include "Window.h"

class FCGuiDocument;
class QSplitter;
class QWidget;		


/** Base class of all windows belonging to a document
 *  there are two ways of belonging to a document. The 
 *  first way is to a fixed one. The second way is to allways
 *  belong to the Active document. that means switching every time
 *  the active document is changing. It also means that the view 
 *  belongs sometimes to no document at all!
 *  @see FCTree
 *  @see FCGuiDocument
 *  @see ApplicationWindow
 */
class FCView : public FCWindow
{
	Q_OBJECT;

public:
	/** View constructor
	  * Atach the view to the given document. If the document is NULL
	  * the view will attach to the active document. Be aware! there isn't
	  * allways a active document!
	  */
	FCView( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	/** View destructor
	  * Detach the view from the document, if Atached!
	  */
	~FCView();

	/// sets the view to an oter document (called by ApplicationWindow)
	void SetDocument(FCGuiDocument* pcDocument);

	/// get called when the document is changed or updated
	virtual void Update(void)=0;

	/// returns the document the view is attached to
	FCGuiDocument* GetGuiDocument(){return _pcDocument;}

	virtual const char *GetName(void)=0;

	/// Mesage handler
	virtual bool OnMsg(const char* pMsg);

	/// is send from the document in oder to close the document
	void Close(void);

signals:
	/// sends a message to the document
	void sendCloseView(FCView* theView);
	
public slots:
	void ViewMsg(const char* pMsg);
	
	void SetActive(void);


protected:
    FCGuiDocument*	_pcDocument;
	bool bIsDetached;

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
class FCSingleView: public FCViewContainer
{
	Q_OBJECT;

public:
	FCSingleView( FCView* pcView, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCSingleView();

	virtual void resizeEvent ( QResizeEvent * e);

	virtual FCView* GetActiveView(void){return _pcView;}
protected:
	virtual void closeEvent(QCloseEvent *e);

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