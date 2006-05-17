/****************************************************************************
** Form implementation generated from reading ui file '.\DlgCommands.ui'
**
** Created: Mi 17. Mai 14:36:13 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgCommands.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <Gui/PrefWidgets.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgCustomCommandsBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Gui::Dialog::DlgCustomCommandsBase::DlgCustomCommandsBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgCustomCommandsBase" );
    DlgCustomCommandsBaseLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgCustomCommandsBaseLayout"); 

    GroupBox2 = new QGroupBox( this, "GroupBox2" );
    GroupBox2->setColumnLayout(0, Qt::Vertical );
    GroupBox2->layout()->setSpacing( 6 );
    GroupBox2->layout()->setMargin( 11 );
    GroupBox2Layout = new QGridLayout( GroupBox2->layout() );
    GroupBox2Layout->setAlignment( Qt::AlignTop );

    IconView1 = new Gui::CommandIconView( GroupBox2, "IconView1" );
    IconView1->setFocusPolicy( Gui::CommandIconView::StrongFocus );

    GroupBox2Layout->addWidget( IconView1, 1, 1 );

    TextLabel = new QLabel( GroupBox2, "TextLabel" );
    TextLabel->setMinimumSize( QSize( 0, 0 ) );
    TextLabel->setMaximumSize( QSize( 32767, 32767 ) );

    GroupBox2Layout->addMultiCellWidget( TextLabel, 2, 2, 0, 1 );

    TextLabelCommands = new QLabel( GroupBox2, "TextLabelCommands" );

    GroupBox2Layout->addWidget( TextLabelCommands, 0, 1 );

    ComboBoxCategory = new QListBox( GroupBox2, "ComboBoxCategory" );
    ComboBoxCategory->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, ComboBoxCategory->sizePolicy().hasHeightForWidth() ) );
    ComboBoxCategory->setMinimumSize( QSize( 125, 0 ) );
    ComboBoxCategory->setMaximumSize( QSize( 125, 32767 ) );

    GroupBox2Layout->addWidget( ComboBoxCategory, 1, 0 );

    TextLabelCategory = new QLabel( GroupBox2, "TextLabelCategory" );

    GroupBox2Layout->addWidget( TextLabelCategory, 0, 0 );

    DlgCustomCommandsBaseLayout->addWidget( GroupBox2, 0, 0 );
    languageChange();
    resize( QSize(411, 286).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // tab order
    setTabOrder( ComboBoxCategory, IconView1 );
    setTabOrder( IconView1, TextLabel );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgCustomCommandsBase::~DlgCustomCommandsBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgCustomCommandsBase::languageChange()
{
    setCaption( tr( "Commands" ) );
    GroupBox2->setTitle( QString::null );
    TextLabel->setText( QString::null );
    TextLabelCommands->setText( tr( "Commands" ) );
    TextLabelCategory->setText( tr( "Category" ) );
}

