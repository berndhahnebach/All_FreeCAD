/****************************************************************************
** Form implementation generated from reading ui file '.\DlgActions.ui'
**
** Created: Sa 13. Mai 12:36:00 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgActions.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qlistview.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgCustomActionsBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Gui::Dialog::DlgCustomActionsBase::DlgCustomActionsBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgCustomActionsBase" );
    DlgCustomActionsBaseLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgCustomActionsBaseLayout"); 

    GroupBox7 = new QGroupBox( this, "GroupBox7" );
    GroupBox7->setColumnLayout(0, Qt::Vertical );
    GroupBox7->layout()->setSpacing( 6 );
    GroupBox7->layout()->setMargin( 11 );
    GroupBox7Layout = new QGridLayout( GroupBox7->layout() );
    GroupBox7Layout->setAlignment( Qt::AlignTop );

    buttonNew = new QPushButton( GroupBox7, "buttonNew" );

    GroupBox7Layout->addWidget( buttonNew, 9, 1 );

    buttonDelete = new QPushButton( GroupBox7, "buttonDelete" );
    buttonDelete->setEnabled( FALSE );

    GroupBox7Layout->addMultiCellWidget( buttonDelete, 9, 9, 2, 3 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupBox7Layout->addItem( spacer, 9, 4 );

    CustomActions = new QListView( GroupBox7, "CustomActions" );
    CustomActions->addColumn( tr( "Custom Macros" ) );
    CustomActions->header()->setClickEnabled( FALSE, CustomActions->header()->count() - 1 );
    CustomActions->header()->setResizeEnabled( FALSE, CustomActions->header()->count() - 1 );
    CustomActions->setResizeMode( QListView::AllColumns );

    GroupBox7Layout->addMultiCellWidget( CustomActions, 0, 9, 0, 0 );

    actionPixmapButton = new QToolButton( GroupBox7, "actionPixmapButton" );
    actionPixmapButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, actionPixmapButton->sizePolicy().hasHeightForWidth() ) );
    actionPixmapButton->setMinimumSize( QSize( 40, 30 ) );
    actionPixmapButton->setMaximumSize( QSize( 40, 30 ) );

    GroupBox7Layout->addWidget( actionPixmapButton, 7, 2 );

    TextLabel1 = new QLabel( GroupBox7, "TextLabel1" );

    GroupBox7Layout->addWidget( TextLabel1, 6, 1 );

    TextLabel2 = new QLabel( GroupBox7, "TextLabel2" );

    GroupBox7Layout->addWidget( TextLabel2, 5, 1 );

    TextLabel3 = new QLabel( GroupBox7, "TextLabel3" );

    GroupBox7Layout->addWidget( TextLabel3, 2, 1 );

    actionAccel = new Gui::AccelLineEdit( GroupBox7, "actionAccel" );
    actionAccel->setFrameShape( Gui::AccelLineEdit::LineEditPanel );
    actionAccel->setFrameShadow( Gui::AccelLineEdit::Sunken );

    GroupBox7Layout->addMultiCellWidget( actionAccel, 6, 6, 2, 4 );

    TextLabel3_2 = new QLabel( GroupBox7, "TextLabel3_2" );

    GroupBox7Layout->addWidget( TextLabel3_2, 4, 1 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupBox7Layout->addItem( spacer_2, 7, 4 );

    actionMenu = new QLineEdit( GroupBox7, "actionMenu" );
    actionMenu->setFrameShape( QLineEdit::LineEditPanel );
    actionMenu->setFrameShadow( QLineEdit::Sunken );

    GroupBox7Layout->addMultiCellWidget( actionMenu, 2, 2, 2, 4 );

    actionToolTip = new QLineEdit( GroupBox7, "actionToolTip" );
    actionToolTip->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, actionToolTip->sizePolicy().hasHeightForWidth() ) );
    actionToolTip->setFrameShape( QLineEdit::LineEditPanel );
    actionToolTip->setFrameShadow( QLineEdit::Sunken );

    GroupBox7Layout->addMultiCellWidget( actionToolTip, 3, 3, 2, 4 );

    TextLabel2_2 = new QLabel( GroupBox7, "TextLabel2_2" );

    GroupBox7Layout->addWidget( TextLabel2_2, 3, 1 );

    TextLabel5_2 = new QLabel( GroupBox7, "TextLabel5_2" );

    GroupBox7Layout->addWidget( TextLabel5_2, 7, 1 );

    actionWhatsThis = new QLineEdit( GroupBox7, "actionWhatsThis" );

    GroupBox7Layout->addMultiCellWidget( actionWhatsThis, 5, 5, 2, 4 );

    TextLabel1_2 = new QLabel( GroupBox7, "TextLabel1_2" );

    GroupBox7Layout->addWidget( TextLabel1_2, 0, 1 );

    actionName = new QLineEdit( GroupBox7, "actionName" );
    actionName->setEnabled( FALSE );
    actionName->setFrameShape( QLineEdit::LineEditPanel );
    actionName->setFrameShadow( QLineEdit::Sunken );

    GroupBox7Layout->addMultiCellWidget( actionName, 0, 0, 2, 4 );

    TextLabel1_3 = new QLabel( GroupBox7, "TextLabel1_3" );

    GroupBox7Layout->addWidget( TextLabel1_3, 1, 1 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GroupBox7Layout->addItem( spacer_3, 8, 1 );

    PixmapLabel = new QLabel( GroupBox7, "PixmapLabel" );
    PixmapLabel->setMinimumSize( QSize( 40, 40 ) );
    PixmapLabel->setMaximumSize( QSize( 40, 40 ) );
    PixmapLabel->setScaledContents( TRUE );

    GroupBox7Layout->addWidget( PixmapLabel, 7, 3 );

    actionMacros = new QComboBox( FALSE, GroupBox7, "actionMacros" );

    GroupBox7Layout->addMultiCellWidget( actionMacros, 1, 1, 2, 4 );

    actionStatus = new QLineEdit( GroupBox7, "actionStatus" );

    GroupBox7Layout->addMultiCellWidget( actionStatus, 4, 4, 2, 4 );

    DlgCustomActionsBaseLayout->addWidget( GroupBox7, 0, 0 );
    languageChange();
    resize( QSize(523, 383).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonNew, SIGNAL( clicked() ), this, SLOT( onAddCustomAction() ) );
    connect( buttonDelete, SIGNAL( clicked() ), this, SLOT( onDelCustomAction() ) );
    connect( actionPixmapButton, SIGNAL( clicked() ), this, SLOT( onCustomActionPixmap() ) );
    connect( CustomActions, SIGNAL( clicked(QListViewItem*) ), this, SLOT( onCustomActionsCanDelete(QListViewItem*) ) );
    connect( CustomActions, SIGNAL( doubleClicked(QListViewItem*) ), this, SLOT( onCustomActionsDoubleClicked(QListViewItem*) ) );

    // tab order
    setTabOrder( CustomActions, actionName );
    setTabOrder( actionName, actionMacros );
    setTabOrder( actionMacros, actionMenu );
    setTabOrder( actionMenu, actionToolTip );
    setTabOrder( actionToolTip, actionStatus );
    setTabOrder( actionStatus, actionWhatsThis );
    setTabOrder( actionWhatsThis, actionAccel );
    setTabOrder( actionAccel, buttonNew );
    setTabOrder( buttonNew, buttonDelete );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgCustomActionsBase::~DlgCustomActionsBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgCustomActionsBase::languageChange()
{
    setCaption( tr( "Macros" ) );
    GroupBox7->setTitle( tr( "Setup Custom Macros" ) );
    buttonNew->setText( tr( "Add" ) );
    buttonDelete->setText( tr( "Remove" ) );
    CustomActions->header()->setLabel( 0, tr( "Custom Macros" ) );
    actionPixmapButton->setText( tr( "..." ) );
    TextLabel1->setText( tr( "Accelerator:" ) );
    TextLabel2->setText( tr( "What's this:" ) );
    TextLabel3->setText( tr( "Menu text:" ) );
    TextLabel3_2->setText( tr( "Status text:" ) );
    TextLabel2_2->setText( tr( "Tool tip:" ) );
    TextLabel5_2->setText( tr( "Pixmap" ) );
    TextLabel1_2->setText( tr( "Name:" ) );
    TextLabel1_3->setText( tr( "Macro:" ) );
}

void Gui::Dialog::DlgCustomActionsBase::onCustomActionsCanDelete(QListViewItem*)
{
    qWarning( "Gui::Dialog::DlgCustomActionsBase::onCustomActionsCanDelete(QListViewItem*): Not implemented yet" );
}

void Gui::Dialog::DlgCustomActionsBase::onCustomActionsDoubleClicked(QListViewItem*)
{
    qWarning( "Gui::Dialog::DlgCustomActionsBase::onCustomActionsDoubleClicked(QListViewItem*): Not implemented yet" );
}

void Gui::Dialog::DlgCustomActionsBase::onAddCustomAction()
{
    qWarning( "Gui::Dialog::DlgCustomActionsBase::onAddCustomAction(): Not implemented yet" );
}

void Gui::Dialog::DlgCustomActionsBase::onDelCustomAction()
{
    qWarning( "Gui::Dialog::DlgCustomActionsBase::onDelCustomAction(): Not implemented yet" );
}

void Gui::Dialog::DlgCustomActionsBase::onCustomActionPixmap()
{
    qWarning( "Gui::Dialog::DlgCustomActionsBase::onCustomActionPixmap(): Not implemented yet" );
}

