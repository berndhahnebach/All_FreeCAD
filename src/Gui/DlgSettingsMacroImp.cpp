#include "PreCompiled.h"

#include "Window.h"
#include "PropertyPage.h"
#include "DlgSettingsMacroImp.h"

using namespace Gui::Dialog;

/* 
 *  Constructs a CDlgSettingsMacroImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
CDlgSettingsMacroImp::CDlgSettingsMacroImp( QWidget* parent,  const char* name, WFlags fl )
    : DlgSettingsMacro( parent, name, fl )
{
	append(FCCheckBox_GuiAsComment   ->getHandler());
	append(FCCheckBox_RecordGui      ->getHandler());
	append(MacroPath      ->getHandler());
}

/*  
 *  Destroys the object and frees any allocated resources
 */
CDlgSettingsMacroImp::~CDlgSettingsMacroImp()
{
    // no need to delete child widgets, Qt does it all for us
}


void CDlgSettingsMacroImp::ChooseDir()
{
	QString cPath = QFileDialog::getExistingDirectory ();

	MacroPath->setText(cPath.latin1());
}




#include "DlgSettingsMacro.cpp"
#include "moc_DlgSettingsMacro.cpp"
#include "moc_DlgSettingsMacroImp.cpp"
