#include "../Base/Export.h"

#include <qobject.h>

#include "View.h"
#include <list>

#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>

class ApplicationWindow;
class FCDocument;

class GuiExport FCGuiDocument 
{
public:
	FCGuiDocument(FCDocument* pcDocument, ApplicationWindow * app, const char * name=0);
	~FCGuiDocument();

	ApplicationWindow* GetApplication(){return _pcAppWnd;}	
	Handle(AIS_InteractiveContext) GetContext(){return _hContext;}

	void CreateView(void); 


private:
	Handle(V3d_Viewer) Viewer(const Standard_CString aDisplay,const Standard_ExtString aName,
		const Standard_CString aDomain,const Standard_Real ViewSize , const V3d_TypeOfOrientation ViewProj,
		const Standard_Boolean ComputedMode,const Standard_Boolean aDefaultComputedMode );
private:
	ApplicationWindow*				_pcAppWnd;
#	pragma warning( disable : 4251 )
	stlport::list<FCView*>			_LpcViews;
	Handle(V3d_Viewer)				_hViewer;
	Handle(AIS_InteractiveContext)	_hContext;
#	pragma warning( default : 4251 )
	// the doc/Document
	FCDocument*						_pcDocument;
};
