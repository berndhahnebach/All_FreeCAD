/****************************************************************************
** Form implementation generated from reading ui file '.\DlgMacroExecute.ui'
**
** Created: Mi 17. Mai 14:36:07 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgMacroExecute.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qheader.h>
#include <qlistview.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgMacroExecute as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Gui::Dialog::DlgMacroExecute::DlgMacroExecute( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgMacroExecute" );
    DlgMacroExecuteLayout = new QVBoxLayout( this, 11, 6, "DlgMacroExecuteLayout"); 

    Layout4 = new QGridLayout( 0, 1, 1, 0, 6, "Layout4"); 

    Layout3 = new QGridLayout( 0, 1, 1, 0, 6, "Layout3"); 

    CreateButton = new QPushButton( this, "CreateButton" );
    CreateButton->setEnabled( TRUE );

    Layout3->addWidget( CreateButton, 3, 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout3->addItem( spacer, 2, 0 );

    DeleteButton = new QPushButton( this, "DeleteButton" );
    DeleteButton->setEnabled( FALSE );

    Layout3->addWidget( DeleteButton, 4, 0 );

    ExecuteButton = new QPushButton( this, "ExecuteButton" );
    ExecuteButton->setEnabled( FALSE );

    Layout3->addWidget( ExecuteButton, 0, 0 );

    EditButton = new QPushButton( this, "EditButton" );
    EditButton->setEnabled( FALSE );

    Layout3->addWidget( EditButton, 5, 0 );

    CancelButton = new QPushButton( this, "CancelButton" );

    Layout3->addWidget( CancelButton, 1, 0 );

    Layout4->addLayout( Layout3, 0, 1 );

    MacroNameGroup = new QGroupBox( this, "MacroNameGroup" );
    MacroNameGroup->setColumnLayout(0, Qt::Vertical );
    MacroNameGroup->layout()->setSpacing( 6 );
    MacroNameGroup->layout()->setMargin( 11 );
    MacroNameGroupLayout = new QVBoxLayout( MacroNameGroup->layout() );
    MacroNameGroupLayout->setAlignment( Qt::AlignTop );

    LineEditMacroName = new QLineEdit( MacroNameGroup, "LineEditMacroName" );
    LineEditMacroName->setEnabled( FALSE );
    LineEditMacroName->setFrameShape( QLineEdit::LineEditPanel );
    LineEditMacroName->setFrameShadow( QLineEdit::Sunken );
    MacroNameGroupLayout->addWidget( LineEditMacroName );

    MacrosListView = new QListView( MacroNameGroup, "MacrosListView" );
    MacrosListView->addColumn( tr( "Macros" ) );
    MacrosListView->addColumn( tr( "Type" ) );
    MacrosListView->setEnabled( TRUE );
    MacroNameGroupLayout->addWidget( MacrosListView );

    Layout4->addWidget( MacroNameGroup, 0, 0 );

    DestinationGroup = new QGroupBox( this, "DestinationGroup" );
    DestinationGroup->setColumnLayout(0, Qt::Vertical );
    DestinationGroup->layout()->setSpacing( 6 );
    DestinationGroup->layout()->setMargin( 11 );
    DestinationGroupLayout = new QHBoxLayout( DestinationGroup->layout() );
    DestinationGroupLayout->setAlignment( Qt::AlignTop );

    ComboBoxPath = new QComboBox( FALSE, DestinationGroup, "ComboBoxPath" );
    ComboBoxPath->setEnabled( FALSE );
    ComboBoxPath->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, ComboBoxPath->sizePolicy().hasHeightForWidth() ) );
    DestinationGroupLayout->addWidget( ComboBoxPath );

    DestButton = new QPushButton( DestinationGroup, "DestButton" );
    DestButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)0, 0, 0, DestButton->sizePolicy().hasHeightForWidth() ) );
    DestButton->setMinimumSize( QSize( 23, 0 ) );
    DestButton->setMaximumSize( QSize( 23, 32767 ) );
    DestinationGroupLayout->addWidget( DestButton );

    Layout4->addWidget( DestinationGroup, 1, 0 );

    DoneButton = new QPushButton( this, "DoneButton" );
    DoneButton->setEnabled( TRUE );

    Layout4->addWidget( DoneButton, 1, 1 );
    DlgMacroExecuteLayout->addLayout( Layout4 );
    languageChange();
    resize( QSize(344, 316).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( CancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( DestButton, SIGNAL( clicked() ), this, SLOT( onNewFolder() ) );
    connect( CreateButton, SIGNAL( clicked() ), this, SLOT( onCreate() ) );
    connect( DeleteButton, SIGNAL( clicked() ), this, SLOT( onDelete() ) );
    connect( EditButton, SIGNAL( clicked() ), this, SLOT( onEdit() ) );
    connect( DoneButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( ExecuteButton, SIGNAL( clicked() ), this, SLOT( onExecute() ) );
    connect( MacrosListView, SIGNAL( doubleClicked(QListViewItem*) ), this, SLOT( onListDoubleClicked(QListViewItem*) ) );
    connect( MacrosListView, SIGNAL( selectionChanged(QListViewItem*) ), this, SLOT( onNewListItemPicked(QListViewItem*) ) );

    // tab order
    setTabOrder( LineEditMacroName, MacrosListView );
    setTabOrder( MacrosListView, ExecuteButton );
    setTabOrder( ExecuteButton, CancelButton );
    setTabOrder( CancelButton, CreateButton );
    setTabOrder( CreateButton, DeleteButton );
    setTabOrder( DeleteButton, EditButton );
    setTabOrder( EditButton, ComboBoxPath );
    setTabOrder( ComboBoxPath, DestButton );
    setTabOrder( DestButton, DoneButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgMacroExecute::~DlgMacroExecute()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgMacroExecute::languageChange()
{
    setCaption( tr( "Execute macro" ) );
    CreateButton->setText( tr( "Create" ) );
    DeleteButton->setText( tr( "Delete macro" ) );
    ExecuteButton->setText( tr( "Execute" ) );
    EditButton->setText( tr( "Edit" ) );
    CancelButton->setText( tr( "Cancel" ) );
    MacroNameGroup->setTitle( tr( "Macro name:" ) );
    MacrosListView->header()->setLabel( 0, tr( "Macros" ) );
    MacrosListView->header()->setLabel( 1, tr( "Type" ) );
    DestinationGroup->setTitle( tr( "Macro destination:" ) );
    DestButton->setText( tr( "..." ) );
    DoneButton->setText( tr( "Done" ) );
}

void Gui::Dialog::DlgMacroExecute::onCreate()
{
    qWarning( "Gui::Dialog::DlgMacroExecute::onCreate(): Not implemented yet" );
}

void Gui::Dialog::DlgMacroExecute::onDelete()
{
    qWarning( "Gui::Dialog::DlgMacroExecute::onDelete(): Not implemented yet" );
}

void Gui::Dialog::DlgMacroExecute::onEdit()
{
    qWarning( "Gui::Dialog::DlgMacroExecute::onEdit(): Not implemented yet" );
}

void Gui::Dialog::DlgMacroExecute::onExecute()
{
    qWarning( "Gui::Dialog::DlgMacroExecute::onExecute(): Not implemented yet" );
}

void Gui::Dialog::DlgMacroExecute::onNewFolder()
{
    qWarning( "Gui::Dialog::DlgMacroExecute::onNewFolder(): Not implemented yet" );
}

void Gui::Dialog::DlgMacroExecute::onNewListItemPicked(QListViewItem*)
{
    qWarning( "Gui::Dialog::DlgMacroExecute::onNewListItemPicked(QListViewItem*): Not implemented yet" );
}

void Gui::Dialog::DlgMacroExecute::onListDoubleClicked(QListViewItem*)
{
    qWarning( "Gui::Dialog::DlgMacroExecute::onListDoubleClicked(QListViewItem*): Not implemented yet" );
}

