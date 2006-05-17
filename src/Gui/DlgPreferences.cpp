/****************************************************************************
** Form implementation generated from reading ui file '.\DlgPreferences.ui'
**
** Created: Mi 17. Mai 14:36:20 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgPreferences.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qwidgetstack.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgPreferences as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Gui::Dialog::DlgPreferences::DlgPreferences( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgPreferences" );
    setSizeGripEnabled( TRUE );
    DlgPreferencesLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgPreferencesLayout"); 

    layout9 = new QHBoxLayout( 0, 0, 6, "layout9"); 
    QSpacerItem* spacer = new QSpacerItem( 210, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout9->addItem( spacer );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    layout9->addWidget( buttonOk );

    buttonApply = new QPushButton( this, "buttonApply" );
    buttonApply->setAutoDefault( TRUE );
    layout9->addWidget( buttonApply );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    layout9->addWidget( buttonCancel );

    DlgPreferencesLayout->addLayout( layout9, 1, 0 );

    layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 

    listBox = new QListBox( this, "listBox" );
    listBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, listBox->sizePolicy().hasHeightForWidth() ) );
    listBox->setMinimumSize( QSize( 120, 0 ) );
    listBox->setFrameShape( QListBox::StyledPanel );
    listBox->setFrameShadow( QListBox::Sunken );
    layout3->addWidget( listBox );

    tabWidgetStack = new QWidgetStack( this, "tabWidgetStack" );
    tabWidgetStack->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, tabWidgetStack->sizePolicy().hasHeightForWidth() ) );

    WStackPage = new QWidget( tabWidgetStack, "WStackPage" );
    tabWidgetStack->addWidget( WStackPage, 0 );
    layout3->addWidget( tabWidgetStack );

    DlgPreferencesLayout->addLayout( layout3, 0, 0 );
    languageChange();
    resize( QSize(570, 454).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( listBox, SIGNAL( highlighted(int) ), this, SLOT( onPrefPageClicked(int) ) );
    connect( buttonApply, SIGNAL( clicked() ), this, SLOT( apply() ) );

    // tab order
    setTabOrder( listBox, buttonOk );
    setTabOrder( buttonOk, buttonApply );
    setTabOrder( buttonApply, buttonCancel );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgPreferences::~DlgPreferences()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgPreferences::languageChange()
{
    setCaption( tr( "Preferences" ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonApply->setText( tr( "&Apply" ) );
    buttonApply->setAccel( QKeySequence( tr( "Alt+A" ) ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
}

void Gui::Dialog::DlgPreferences::onPrefPageClicked(int)
{
    qWarning( "Gui::Dialog::DlgPreferences::onPrefPageClicked(int): Not implemented yet" );
}

void Gui::Dialog::DlgPreferences::apply()
{
    qWarning( "Gui::Dialog::DlgPreferences::apply(): Not implemented yet" );
}

