/****************************************************************************
** Form implementation generated from reading ui file '.\DlgGeneral.ui'
**
** Created: Sa 13. Mai 12:36:05 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgGeneral.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgGeneralBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Gui::Dialog::DlgGeneralBase::DlgGeneralBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgGeneralBase" );
    DlgGeneralBaseLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgGeneralBaseLayout"); 

    GroupBox5 = new QGroupBox( this, "GroupBox5" );
    GroupBox5->setColumnLayout(0, Qt::Vertical );
    GroupBox5->layout()->setSpacing( 6 );
    GroupBox5->layout()->setMargin( 11 );
    GroupBox5Layout = new QGridLayout( GroupBox5->layout() );
    GroupBox5Layout->setAlignment( Qt::AlignTop );

    GroupBox7 = new QGroupBox( GroupBox5, "GroupBox7" );
    GroupBox7->setColumnLayout(0, Qt::Vertical );
    GroupBox7->layout()->setSpacing( 6 );
    GroupBox7->layout()->setMargin( 11 );
    GroupBox7Layout = new QGridLayout( GroupBox7->layout() );
    GroupBox7Layout->setAlignment( Qt::AlignTop );

    layout4 = new QHBoxLayout( 0, 0, 6, "layout4"); 

    TextLabel1_4 = new QLabel( GroupBox7, "TextLabel1_4" );
    layout4->addWidget( TextLabel1_4 );

    Languages = new QComboBox( FALSE, GroupBox7, "Languages" );
    layout4->addWidget( Languages );

    GroupBox7Layout->addLayout( layout4, 0, 0 );

    GroupBox5Layout->addWidget( GroupBox7, 0, 0 );

    GroupBox3 = new QGroupBox( GroupBox5, "GroupBox3" );
    GroupBox3->setColumnLayout(0, Qt::Vertical );
    GroupBox3->layout()->setSpacing( 6 );
    GroupBox3->layout()->setMargin( 11 );
    GroupBox3Layout = new QVBoxLayout( GroupBox3->layout() );
    GroupBox3Layout->setAlignment( Qt::AlignTop );

    layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 

    TextLabel1_3 = new QLabel( GroupBox3, "TextLabel1_3" );
    layout2->addWidget( TextLabel1_3 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout2->addItem( spacer );

    RecentFiles = new Gui::PrefSpinBox( GroupBox3, "RecentFiles" );
    RecentFiles->setMaxValue( 20 );
    RecentFiles->setMinValue( 0 );
    RecentFiles->setLineStep( 1 );
    RecentFiles->setValue( 4 );
    RecentFiles->setProperty( "prefEntry", "RecentFiles" );
    RecentFiles->setProperty( "prefPath", "RecentFiles" );
    layout2->addWidget( RecentFiles );
    GroupBox3Layout->addLayout( layout2 );

    layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 

    TextLabel1 = new QLabel( GroupBox3, "TextLabel1" );
    layout3->addWidget( TextLabel1 );

    WindowStyle = new QComboBox( FALSE, GroupBox3, "WindowStyle" );
    layout3->addWidget( WindowStyle );
    GroupBox3Layout->addLayout( layout3 );

    UsesBigPixmaps = new Gui::PrefCheckBox( GroupBox3, "UsesBigPixmaps" );
    UsesBigPixmaps->setFocusPolicy( Gui::PrefCheckBox::TabFocus );
    UsesBigPixmaps->setProperty( "prefEntry", "BigPixmaps" );
    UsesBigPixmaps->setProperty( "prefPath", "General" );
    GroupBox3Layout->addWidget( UsesBigPixmaps );

    GroupBox5Layout->addWidget( GroupBox3, 1, 0 );

    GroupBox4 = new QGroupBox( GroupBox5, "GroupBox4" );
    GroupBox4->setColumnLayout(0, Qt::Vertical );
    GroupBox4->layout()->setSpacing( 6 );
    GroupBox4->layout()->setMargin( 11 );
    GroupBox4Layout = new QGridLayout( GroupBox4->layout() );
    GroupBox4Layout->setAlignment( Qt::AlignTop );

    AllowDragMenu = new Gui::PrefCheckBox( GroupBox4, "AllowDragMenu" );
    AllowDragMenu->setProperty( "prefEntry", "AllowDrag" );
    AllowDragMenu->setProperty( "prefPath", "General" );

    GroupBox4Layout->addWidget( AllowDragMenu, 0, 0 );

    GroupBox5Layout->addWidget( GroupBox4, 2, 0 );

    GroupBox9 = new QGroupBox( GroupBox5, "GroupBox9" );
    GroupBox9->setColumnLayout(0, Qt::Vertical );
    GroupBox9->layout()->setSpacing( 6 );
    GroupBox9->layout()->setMargin( 11 );
    GroupBox9Layout = new QGridLayout( GroupBox9->layout() );
    GroupBox9Layout->setAlignment( Qt::AlignTop );

    ShowCmdLine = new Gui::PrefCheckBox( GroupBox9, "ShowCmdLine" );
    ShowCmdLine->setChecked( TRUE );
    ShowCmdLine->setProperty( "prefEntry", "ShowCmdLine" );
    ShowCmdLine->setProperty( "prefPath", "CommandLine" );

    GroupBox9Layout->addWidget( ShowCmdLine, 0, 0 );

    layout1 = new QHBoxLayout( 0, 0, 6, "layout1"); 

    TextLabel1_4_2 = new QLabel( GroupBox9, "TextLabel1_4_2" );
    layout1->addWidget( TextLabel1_4_2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout1->addItem( spacer_2 );

    SizeCmdLine = new Gui::PrefSpinBox( GroupBox9, "SizeCmdLine" );
    SizeCmdLine->setValue( 20 );
    SizeCmdLine->setProperty( "prefEntry", "SizeCmdLine" );
    SizeCmdLine->setProperty( "prefPath", "CommandLine" );
    layout1->addWidget( SizeCmdLine );

    GroupBox9Layout->addLayout( layout1, 1, 0 );

    GroupBox5Layout->addWidget( GroupBox9, 3, 0 );

    GroupBox10 = new QGroupBox( GroupBox5, "GroupBox10" );
    GroupBox10->setColumnLayout(0, Qt::Vertical );
    GroupBox10->layout()->setSpacing( 6 );
    GroupBox10->layout()->setMargin( 11 );
    GroupBox10Layout = new QGridLayout( GroupBox10->layout() );
    GroupBox10Layout->setAlignment( Qt::AlignTop );

    SplashScreen = new Gui::PrefCheckBox( GroupBox10, "SplashScreen" );
    SplashScreen->setChecked( TRUE );
    SplashScreen->setProperty( "prefEntry", "AllowSplasher" );
    SplashScreen->setProperty( "prefPath", "General" );

    GroupBox10Layout->addWidget( SplashScreen, 1, 0 );

    layout7 = new QGridLayout( 0, 1, 1, 0, 6, "layout7"); 

    textLabel1_2 = new QLabel( GroupBox10, "textLabel1_2" );

    layout7->addWidget( textLabel1_2, 1, 0 );

    AutoloadModuleCombo = new Gui::PrefComboBox( GroupBox10, "AutoloadModuleCombo" );
    AutoloadModuleCombo->setProperty( "prefEntry", "AutoloadModule" );
    AutoloadModuleCombo->setProperty( "prefPath", "General" );

    layout7->addWidget( AutoloadModuleCombo, 0, 1 );

    AutoloadTabCombo = new Gui::PrefComboBox( GroupBox10, "AutoloadTabCombo" );
    AutoloadTabCombo->setProperty( "prefEntry", "AutoloadTab" );
    AutoloadTabCombo->setProperty( "prefPath", "General" );

    layout7->addWidget( AutoloadTabCombo, 1, 1 );

    textLabel1 = new QLabel( GroupBox10, "textLabel1" );

    layout7->addWidget( textLabel1, 0, 0 );

    GroupBox10Layout->addLayout( layout7, 0, 0 );

    GroupBox5Layout->addWidget( GroupBox10, 4, 0 );

    DlgGeneralBaseLayout->addWidget( GroupBox5, 0, 0 );
    languageChange();
    resize( QSize(390, 497).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // tab order
    setTabOrder( Languages, UsesBigPixmaps );
    setTabOrder( UsesBigPixmaps, WindowStyle );
    setTabOrder( WindowStyle, AllowDragMenu );
    setTabOrder( AllowDragMenu, ShowCmdLine );
    setTabOrder( ShowCmdLine, SizeCmdLine );
    setTabOrder( SizeCmdLine, RecentFiles );
    setTabOrder( RecentFiles, SplashScreen );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgGeneralBase::~DlgGeneralBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgGeneralBase::languageChange()
{
    setCaption( tr( "General" ) );
    GroupBox5->setTitle( tr( "General" ) );
    GroupBox7->setTitle( tr( "Language" ) );
    TextLabel1_4->setText( tr( "Change language:" ) );
    GroupBox3->setTitle( tr( "Main window" ) );
    TextLabel1_3->setText( tr( "Size of recent file list" ) );
    TextLabel1->setText( tr( "Window style:" ) );
    UsesBigPixmaps->setText( tr( "Use big pixmaps" ) );
    GroupBox4->setTitle( tr( "Drag and drop" ) );
    AllowDragMenu->setText( tr( "Allow drag&&drop of menu items" ) );
    AllowDragMenu->setAccel( QKeySequence( QString::null ) );
    GroupBox9->setTitle( tr( "Command line" ) );
    ShowCmdLine->setText( tr( "Show command line" ) );
    TextLabel1_4_2->setText( tr( "Size of history" ) );
    GroupBox10->setTitle( QString::null );
    SplashScreen->setText( tr( "Enable splashscreen at startup" ) );
    textLabel1_2->setText( tr( "Switch to tab of report window:" ) );
    textLabel1->setText( tr( "Auto load module after startup:" ) );
}

void Gui::Dialog::DlgGeneralBase::saveSettings()
{
    qWarning( "Gui::Dialog::DlgGeneralBase::saveSettings(): Not implemented yet" );
}

void Gui::Dialog::DlgGeneralBase::loadSettings()
{
    qWarning( "Gui::Dialog::DlgGeneralBase::loadSettings(): Not implemented yet" );
}

