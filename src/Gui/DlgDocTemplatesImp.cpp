#include "DlgDocTemplatesImp.h"
#include <qiconview.h> 
#include <qfiledialog.h>
#include <qmessagebox.h> 
#include <python.h>
#include "../Base/Export.h"
#include "../Base/PyTools.h"
#include "../Base/Interpreter.h"


#include "Icons/Folder32.xpm"
/* 
 *  Constructs a DlgDocTemplatesImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgDocTemplatesImp::DlgDocTemplatesImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgDocTemplates( parent, name, modal, fl )
{
	PyObject *result=NULL,*result2=NULL;

	assert(PP_Run_Function("__main__","GetTemplateList","O",&result,"()") != -1);
	assert(PySequence_Check(result));

	QPixmap pixmap(Folder32);
	QString str;

	for(int i=0;i<PySequence_Size(result);i++)
	{
		result2 = PySequence_GetItem(result,i);
		assert(result2);
		assert(PyString_Check(result2));
		str = PyString_AsString(result2);
		new QIconViewItem ( TemplateIconView ,str,pixmap );
		Py_DECREF(result2);
	}
	Py_DECREF(result);
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

	if (sTemplate.isEmpty()){
		QMessageBox::information( this, "Template error",
                            "You have to choos a template\n");
	}else{
		GetInterpreter().RunFCCommand("FreeCAD.DocNew(\"%s\")",LineEdit1->text().latin1());
		accept();
	}
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
