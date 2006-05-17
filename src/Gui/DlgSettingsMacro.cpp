/****************************************************************************
** Form implementation generated from reading ui file '.\DlgSettingsMacro.ui'
**
** Created: Mi 17. Mai 14:36:15 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgSettingsMacro.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <Gui/PrefWidgets.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgSettingsMacro as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Gui::Dialog::DlgSettingsMacro::DlgSettingsMacro( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgSettingsMacro" );
    DlgSettingsMacroLayout = new QVBoxLayout( this, 11, 6, "DlgSettingsMacroLayout"); 

    GroupBox6 = new QGroupBox( this, "GroupBox6" );
    GroupBox6->setColumnLayout(0, Qt::Vertical );
    GroupBox6->layout()->setSpacing( 6 );
    GroupBox6->layout()->setMargin( 11 );
    GroupBox6Layout = new QVBoxLayout( GroupBox6->layout() );
    GroupBox6Layout->setAlignment( Qt::AlignTop );

    GroupBox8 = new QGroupBox( GroupBox6, "GroupBox8" );
    GroupBox8->setColumnLayout(0, Qt::Vertical );
    GroupBox8->layout()->setSpacing( 6 );
    GroupBox8->layout()->setMargin( 11 );
    GroupBox8Layout = new QGridLayout( GroupBox8->layout() );
    GroupBox8Layout->setAlignment( Qt::AlignTop );

    MacroPath = new Gui::PrefFileChooser( GroupBox8, "MacroPath" );
    MacroPath->setMode( Gui::PrefFileChooser::Directory );
    MacroPath->setProperty( "prefEntry", "MacroPath" );
    MacroPath->setProperty( "prefPath", "Macro" );

    GroupBox8Layout->addWidget( MacroPath, 0, 0 );
    GroupBox6Layout->addWidget( GroupBox8 );

    GroupBox7 = new QGroupBox( GroupBox6, "GroupBox7" );
    GroupBox7->setColumnLayout(0, Qt::Vertical );
    GroupBox7->layout()->setSpacing( 6 );
    GroupBox7->layout()->setMargin( 11 );
    GroupBox7Layout = new QGridLayout( GroupBox7->layout() );
    GroupBox7Layout->setAlignment( Qt::AlignTop );

    PrefCheckBox_RecordGui = new Gui::PrefCheckBox( GroupBox7, "PrefCheckBox_RecordGui" );
    PrefCheckBox_RecordGui->setChecked( TRUE );
    PrefCheckBox_RecordGui->setProperty( "prefEntry", "RecordGui" );
    PrefCheckBox_RecordGui->setProperty( "prefPath", "Macro" );

    GroupBox7Layout->addWidget( PrefCheckBox_RecordGui, 0, 0 );

    PrefCheckBox_GuiAsComment = new Gui::PrefCheckBox( GroupBox7, "PrefCheckBox_GuiAsComment" );
    PrefCheckBox_GuiAsComment->setChecked( TRUE );
    PrefCheckBox_GuiAsComment->setProperty( "prefEntry", "GuiAsComment" );
    PrefCheckBox_GuiAsComment->setProperty( "prefPath", "Macro" );

    GroupBox7Layout->addWidget( PrefCheckBox_GuiAsComment, 1, 0 );
    GroupBox6Layout->addWidget( GroupBox7 );

    groupBox4 = new QGroupBox( GroupBox6, "groupBox4" );
    groupBox4->setColumnLayout(0, Qt::Vertical );
    groupBox4->layout()->setSpacing( 6 );
    groupBox4->layout()->setMargin( 11 );
    groupBox4Layout = new QVBoxLayout( groupBox4->layout() );
    groupBox4Layout->setAlignment( Qt::AlignTop );

    PConsoleCheckBox = new Gui::PrefCheckBox( groupBox4, "PConsoleCheckBox" );
    PConsoleCheckBox->setProperty( "prefEntry", "ScriptToPyConsole" );
    PConsoleCheckBox->setProperty( "prefPath", "Macro" );
    groupBox4Layout->addWidget( PConsoleCheckBox );

    FileLogCheckBox = new Gui::PrefCheckBox( groupBox4, "FileLogCheckBox" );
    FileLogCheckBox->setProperty( "prefEntry", "ScriptToFile" );
    FileLogCheckBox->setProperty( "prefPath", "Macro" );
    groupBox4Layout->addWidget( FileLogCheckBox );

    MacroPath_2 = new Gui::PrefFileChooser( groupBox4, "MacroPath_2" );
    MacroPath_2->setMode( Gui::PrefFileChooser::File );
    MacroPath_2->setProperty( "prefEntry", "ScriptFile" );
    MacroPath_2->setProperty( "prefPath", "Macro" );
    groupBox4Layout->addWidget( MacroPath_2 );
    GroupBox6Layout->addWidget( groupBox4 );
    DlgSettingsMacroLayout->addWidget( GroupBox6 );
    languageChange();
    resize( QSize(372, 384).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( PrefCheckBox_RecordGui, SIGNAL( toggled(bool) ), PrefCheckBox_GuiAsComment, SLOT( setEnabled(bool) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgSettingsMacro::~DlgSettingsMacro()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgSettingsMacro::languageChange()
{
    setCaption( tr( "Macro" ) );
    GroupBox6->setTitle( tr( "Macro recording settings" ) );
    GroupBox8->setTitle( tr( "Macro path" ) );
    GroupBox7->setTitle( tr( "Gui commands" ) );
    PrefCheckBox_RecordGui->setText( tr( "Recording gui commands" ) );
    PrefCheckBox_GuiAsComment->setText( tr( "Record as comment" ) );
    groupBox4->setTitle( tr( "Logging Commands" ) );
    PConsoleCheckBox->setText( tr( "Show script commands in python console" ) );
    FileLogCheckBox->setText( tr( "Log all commands issued by menus to file:" ) );
    MacroPath_2->setFileName( tr( "FullScript.FCScript" ) );
}

void Gui::Dialog::DlgSettingsMacro::loadSettings()
{
    qWarning( "Gui::Dialog::DlgSettingsMacro::loadSettings(): Not implemented yet" );
}

void Gui::Dialog::DlgSettingsMacro::saveSettings()
{
    qWarning( "Gui::Dialog::DlgSettingsMacro::saveSettings(): Not implemented yet" );
}

