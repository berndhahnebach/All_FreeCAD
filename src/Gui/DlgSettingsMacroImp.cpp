#include "PreCompiled.h"

#include "Window.h"
#include "PrefWidgets.h"
#include "DlgSettingsMacroImp.h"

/* 
 *  Constructs a DlgSettingsMacroImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
FCDlgSettingsMacro::FCDlgSettingsMacro( QWidget* parent,  const char* name, WFlags fl )
    : DlgSettingsMacro( parent, name, fl )
{
	append(FCCheckBox_GuiAsComment   ->getHandler());
	append(FCCheckBox_RecordGui      ->getHandler());
	append(MacroPath      ->getHandler());
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCDlgSettingsMacro::~FCDlgSettingsMacro()
{
    // no need to delete child widgets, Qt does it all for us
}


void FCDlgSettingsMacro::ChooseDir()
{
	QString cPath = QFileDialog::getExistingDirectory ();

	MacroPath->setText(cPath.latin1());
}




#include "DlgSettingsMacro.cpp"
#include "moc_DlgSettingsMacro.cpp"
#include "moc_DlgSettingsMacroImp.cpp"
