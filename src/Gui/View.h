#ifndef __FCview_h__
#define __FCview_h__


#include "Window.h"

class FCGuiDocument;
class FCDocument;
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
class FCBaseView //: public FCWindow
{
//	Q_OBJECT;

public:
	/** View constructor
	  * Atach the view to the given document. If the document is NULL
	  * the view will attach to the active document. Be aware! there isn't
	  * allways a active document!
	  */
	FCBaseView( FCGuiDocument* pcDocument=0l);
	/** View destructor
	  * Detach the view from the document, if Atached!
	  */
	~FCBaseView();


	/** @name methodes used by the Application and the GuiDocument 
	 */
	//@{
	/// sets the view to an oter document (called by ApplicationWindow)
	void SetDocument(FCGuiDocument* pcDocument);
	/// is send from the document in oder to close the document
	void Close(void);
	//@}

	

	/// returns the document the view is attached to
	FCGuiDocument* GetGuiDocument(){return _pcDocument;}
	/// returns the document the view is attached to
	FCDocument* GetAppDocument();
	/// indikates if the view is in passiv mode
	bool IsPassiv(void){return bIsPassiv;}


	/** @name methodes to overrride 
	 */
	//@{
	/// get called when the document is updated
	virtual void Update(void)=0;
	/// returns the name of the view (important for messages)
	virtual const char *GetName(void)=0;
	/// Mesage handler
	virtual bool OnMsg(const char* pMsg)=0;
	/// Mesage handler test
	virtual bool OnHasMsg(const char* pMsg)=0;
	/// is called when the above function is called to handle document change stuff
	virtual void OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument){};
	/// overvrit when checking on close state
	virtual bool CanClose(void){return true;}
	//@}


protected:
    FCGuiDocument*	_pcDocument;
	bool bIsDetached;
	bool bIsPassiv;

};

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
class FCView : public QextMdiChildView,public FCBaseView
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

	/// get called when the document is updated
	virtual void Update(void)=0;

	/// returns the name of the view (important for messages)
	virtual const char *GetName(void)=0;

	/// Mesage handler
	virtual bool OnMsg(const char* pMsg);
	/// Mesage handler test
	virtual bool OnHasMsg(const char* pMsg);
	/// overvrit when checking on close state
	virtual bool CanClose(void){return true;}
	/// print function of the view
	virtual void Print(QPainter& cPrinter);

signals:
	/// sends a message to the document
//	void sendCloseView(FCView* theView);
	
public slots:	
	void SetActive(void);

protected:
	/// 
	void closeEvent(QCloseEvent *e);

};



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
class FCDockView : public FCDockWindow,public FCBaseView
{
	Q_OBJECT;

public:
	/** View constructor
	  * Atach the view to the given document. If the document is NULL
	  * the view will attach to the active document. Be aware! there isn't
	  * allways a active document!
	  */
	FCDockView( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	/** View destructor
	  * Detach the view from the document, if Atached!
	  */
	~FCDockView();

	/// get called when the document is updated
	virtual void Update(void){};

	/// returns the name of the view (important for messages)
	virtual const char *GetName(void)=0;

	/// Mesage handler
	virtual bool OnMsg(const char* pMsg);
	/// Mesage handler test
	virtual bool OnHasMsg(const char* pMsg);


signals:
	/// sends a message to the document
//	void sendCloseView(FCView* theView);
	
};







/** The double view container class
 *  This class can handle two FCView instances seperated by
 *  a splitter. Usuly its used to show a FCDocTree and a FC3dView
 */
/*
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
*/
/** The singel view container class
 *  This class can handle one FCView instances 
 */
/*
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
*/

/** Holds a single view in a dock bar
 */
/*
class FCViewBar: public FCDockWindow
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
*/

#endif
