#include "PreCompiled.h"

#ifndef _PreComp_
#	include <qiconview.h> 
#	include <qfiledialog.h>
#	include <qlineedit.h>
#endif

#include "DlgDocTemplatesImp.h"
#include "Command.h"
#include "Application.h"
#include "BitmapFactory.h"

#include "../App/Application.h"

//#include "Icons/Folder32.xpm"
/* 
 *  Constructs a DlgDocTemplatesImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgDocTemplatesImp::DlgDocTemplatesImp(  FCCommand* pcCmd, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgDocTemplates( parent, name, modal, fl ),
	  FCWindowParameter(name),
	  _pcCmd(pcCmd)
{
	std::vector<std::string> vTemplates = GetApplication().GetAllTemplates();
    
	QPixmap pixmap(Gui::BitmapFactory().GetPixmap("Folder32"));
	QString str;
	// cycling through the Templates
	for(std::vector<std::string>::iterator i=vTemplates.begin();i!=vTemplates.end();i++)
	{
		str = (*i).c_str();
		if(str != "")
			new QIconViewItem ( TemplateIconView ,str,pixmap );
	}
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgDocTemplatesImp::~DlgDocTemplatesImp()
{
    // no need to delete child widgets, Qt does it all for us
}

/* 
 * public slot
 */
void DlgDocTemplatesImp::ChoseFile()
{
    QString s( QFileDialog::getOpenFileName( QString::null, "FreeCAD Templates (*.FCTempl)", this ) );
    if ( s.isEmpty() )
        return;

	puts(s);
 
}
/* 
 * public slot
 */
void DlgDocTemplatesImp::Validate()
{
	QString sTemplate = LineEdit1->text();

	if (sTemplate.isEmpty())
		_pcCmd->DoCommand(FCCommand::Doc,"FreeCAD.DocNew()");		
	else
		_pcCmd->DoCommand(FCCommand::Doc,"FreeCAD.DocNew(\"%s\")",LineEdit1->text().latin1());
	
	accept();
}
/* 
 * public slot
 */
void DlgDocTemplatesImp::ViewChange(QIconViewItem* Item)
{
	LineEdit1->setText(Item->text());
}
/* 
 * public slot
 */
void DlgDocTemplatesImp::EditFile()
{
    qWarning( "DlgDocTemplatesImp::EditFile() not yet implemented!" ); 
}

void DlgDocTemplatesImp::IconDoubleClick(QIconViewItem* Item)
{
	LineEdit1->setText(Item->text());
	Validate();
}

#include "DlgDocTemplates.cpp"
#include "moc_DlgDocTemplates.cpp"
#include "moc_DlgDocTemplatesImp.cpp"


