/****************************************************************************
** Form implementation generated from reading ui file '.\DlgKeyboard.ui'
**
** Created: Mi 17. Mai 14:36:12 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgKeyboard.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <Gui/PrefWidgets.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgCustomKeyboardBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Gui::Dialog::DlgCustomKeyboardBase::DlgCustomKeyboardBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgCustomKeyboardBase" );
    DlgCustomKeyboardBaseLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgCustomKeyboardBaseLayout"); 
    QSpacerItem* spacer = new QSpacerItem( 16, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    DlgCustomKeyboardBaseLayout->addItem( spacer, 0, 3 );
    QSpacerItem* spacer_2 = new QSpacerItem( 16, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    DlgCustomKeyboardBaseLayout->addItem( spacer_2, 1, 1 );

    textLabelAssignedShortcut = new QLabel( this, "textLabelAssignedShortcut" );

    DlgCustomKeyboardBaseLayout->addWidget( textLabelAssignedShortcut, 0, 0 );

    layout4 = new QVBoxLayout( 0, 0, 6, "layout4"); 

    TextLabelCategory = new QLabel( this, "TextLabelCategory" );
    layout4->addWidget( TextLabelCategory );

    comboBoxCategory = new QComboBox( FALSE, this, "comboBoxCategory" );
    layout4->addWidget( comboBoxCategory );

    TextLabelCommands = new QLabel( this, "TextLabelCommands" );
    layout4->addWidget( TextLabelCommands );

    listBoxCommands = new QListBox( this, "listBoxCommands" );
    listBoxCommands->setMinimumSize( QSize( 220, 0 ) );
    layout4->addWidget( listBoxCommands );

    DlgCustomKeyboardBaseLayout->addMultiCellLayout( layout4, 0, 1, 0, 0 );

    layout4_2 = new QVBoxLayout( 0, 0, 6, "layout4_2"); 

    textLabelShortcut = new QLabel( this, "textLabelShortcut" );
    layout4_2->addWidget( textLabelShortcut );

    accelLineEditShortcut = new Gui::AccelLineEdit( this, "accelLineEditShortcut" );
    accelLineEditShortcut->setEnabled( FALSE );
    layout4_2->addWidget( accelLineEditShortcut );

    textLabelNewShortcut = new QLabel( this, "textLabelNewShortcut" );
    layout4_2->addWidget( textLabelNewShortcut );

    accelLineEdit1NewShortcut = new Gui::AccelLineEdit( this, "accelLineEdit1NewShortcut" );
    accelLineEdit1NewShortcut->setFrameShape( Gui::AccelLineEdit::LineEditPanel );
    accelLineEdit1NewShortcut->setFrameShadow( Gui::AccelLineEdit::Sunken );
    layout4_2->addWidget( accelLineEdit1NewShortcut );

    textLabelAssigned = new QLabel( this, "textLabelAssigned" );
    layout4_2->addWidget( textLabelAssigned );

    listBoxAssigned = new QListBox( this, "listBoxAssigned" );
    listBoxAssigned->setEnabled( FALSE );
    listBoxAssigned->setMinimumSize( QSize( 220, 0 ) );
    layout4_2->addWidget( listBoxAssigned );

    DlgCustomKeyboardBaseLayout->addMultiCellLayout( layout4_2, 0, 1, 2, 2 );

    layout4_3 = new QVBoxLayout( 0, 0, 6, "layout4_3"); 

    pushButtonAssign = new QPushButton( this, "pushButtonAssign" );
    layout4_3->addWidget( pushButtonAssign );

    pushButtonReset = new QPushButton( this, "pushButtonReset" );
    layout4_3->addWidget( pushButtonReset );

    pushButtonResetAll = new QPushButton( this, "pushButtonResetAll" );
    layout4_3->addWidget( pushButtonResetAll );
    QSpacerItem* spacer_3 = new QSpacerItem( 41, 150, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout4_3->addItem( spacer_3 );

    DlgCustomKeyboardBaseLayout->addMultiCellLayout( layout4_3, 0, 1, 4, 4 );

    layout7 = new QVBoxLayout( 0, 0, 6, "layout7"); 

    textLabelDescriptionHeader = new QLabel( this, "textLabelDescriptionHeader" );
    layout7->addWidget( textLabelDescriptionHeader );

    textLabelDescription = new QLabel( this, "textLabelDescription" );
    layout7->addWidget( textLabelDescription );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 31, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout7->addItem( spacer_4 );

    DlgCustomKeyboardBaseLayout->addMultiCellLayout( layout7, 2, 2, 0, 4 );
    languageChange();
    resize( QSize(574, 344).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( comboBoxCategory, SIGNAL( activated(const QString&) ), this, SLOT( onGroupSelected(const QString&) ) );
    connect( listBoxCommands, SIGNAL( highlighted(const QString&) ), this, SLOT( onDescription(const QString&) ) );
    connect( pushButtonAssign, SIGNAL( clicked() ), this, SLOT( onAssign() ) );
    connect( pushButtonReset, SIGNAL( clicked() ), this, SLOT( onReset() ) );
    connect( pushButtonResetAll, SIGNAL( clicked() ), this, SLOT( onResetAll() ) );
    connect( accelLineEdit1NewShortcut, SIGNAL( textChanged(const QString&) ), this, SLOT( onShortcutPressed(const QString&) ) );

    // tab order
    setTabOrder( comboBoxCategory, listBoxCommands );
    setTabOrder( listBoxCommands, accelLineEditShortcut );
    setTabOrder( accelLineEditShortcut, accelLineEdit1NewShortcut );
    setTabOrder( accelLineEdit1NewShortcut, pushButtonAssign );
    setTabOrder( pushButtonAssign, pushButtonReset );
    setTabOrder( pushButtonReset, pushButtonResetAll );

    // buddies
    TextLabelCategory->setBuddy( comboBoxCategory );
    TextLabelCommands->setBuddy( listBoxCommands );
    textLabelNewShortcut->setBuddy( accelLineEdit1NewShortcut );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgCustomKeyboardBase::~DlgCustomKeyboardBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgCustomKeyboardBase::languageChange()
{
    setCaption( tr( "Keyboard" ) );
    textLabelAssignedShortcut->setText( QString::null );
    TextLabelCategory->setText( tr( "&Category:" ) );
    TextLabelCommands->setText( tr( "C&ommands:" ) );
    textLabelShortcut->setText( tr( "Current shortcut:" ) );
    textLabelNewShortcut->setText( tr( "Press &new shortcut:" ) );
    textLabelAssigned->setText( tr( "Currently assigned to:" ) );
    pushButtonAssign->setText( tr( "&Assign" ) );
    pushButtonAssign->setAccel( QKeySequence( tr( "Alt+A" ) ) );
    pushButtonReset->setText( tr( "&Reset" ) );
    pushButtonReset->setAccel( QKeySequence( tr( "Alt+R" ) ) );
    pushButtonResetAll->setText( tr( "Re&set All" ) );
    pushButtonResetAll->setAccel( QKeySequence( tr( "Alt+S" ) ) );
    textLabelDescriptionHeader->setText( tr( "Description:" ) );
    textLabelDescription->setText( QString::null );
}

void Gui::Dialog::DlgCustomKeyboardBase::onGroupSelected(const QString&)
{
    qWarning( "Gui::Dialog::DlgCustomKeyboardBase::onGroupSelected(const QString&): Not implemented yet" );
}

void Gui::Dialog::DlgCustomKeyboardBase::onDescription(const QString&)
{
    qWarning( "Gui::Dialog::DlgCustomKeyboardBase::onDescription(const QString&): Not implemented yet" );
}

void Gui::Dialog::DlgCustomKeyboardBase::onAssign()
{
    qWarning( "Gui::Dialog::DlgCustomKeyboardBase::onAssign(): Not implemented yet" );
}

void Gui::Dialog::DlgCustomKeyboardBase::onReset()
{
    qWarning( "Gui::Dialog::DlgCustomKeyboardBase::onReset(): Not implemented yet" );
}

void Gui::Dialog::DlgCustomKeyboardBase::onResetAll()
{
    qWarning( "Gui::Dialog::DlgCustomKeyboardBase::onResetAll(): Not implemented yet" );
}

void Gui::Dialog::DlgCustomKeyboardBase::onShortcutPressed(const QString&)
{
    qWarning( "Gui::Dialog::DlgCustomKeyboardBase::onShortcutPressed(const QString&): Not implemented yet" );
}

