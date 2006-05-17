/****************************************************************************
** Form implementation generated from reading ui file '.\DlgEditor.ui'
**
** Created: Mi 17. Mai 14:36:23 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgEditor.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <Gui/PrefWidgets.h>
#include <qlistbox.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgEditorSettingsBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Gui::Dialog::DlgEditorSettingsBase::DlgEditorSettingsBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgEditorSettingsBase" );
    DlgEditorSettingsBaseLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgEditorSettingsBaseLayout"); 

    GroupBox2_2 = new QGroupBox( this, "GroupBox2_2" );
    GroupBox2_2->setColumnLayout(0, Qt::Vertical );
    GroupBox2_2->layout()->setSpacing( 6 );
    GroupBox2_2->layout()->setMargin( 11 );
    GroupBox2_2Layout = new QGridLayout( GroupBox2_2->layout() );
    GroupBox2_2Layout->setAlignment( Qt::AlignTop );

    textEdit1 = new QTextEdit( GroupBox2_2, "textEdit1" );
    textEdit1->setWordWrap( QTextEdit::WidgetWidth );

    GroupBox2_2Layout->addWidget( textEdit1, 0, 0 );

    DlgEditorSettingsBaseLayout->addWidget( GroupBox2_2, 2, 1 );

    GroupBox5 = new QGroupBox( this, "GroupBox5" );
    GroupBox5->setColumnLayout(0, Qt::Vertical );
    GroupBox5->layout()->setSpacing( 6 );
    GroupBox5->layout()->setMargin( 11 );
    GroupBox5Layout = new QGridLayout( GroupBox5->layout() );
    GroupBox5Layout->setAlignment( Qt::AlignTop );

    TextLabel4 = new QLabel( GroupBox5, "TextLabel4" );

    GroupBox5Layout->addWidget( TextLabel4, 0, 1 );

    TextLabel3 = new QLabel( GroupBox5, "TextLabel3" );

    GroupBox5Layout->addWidget( TextLabel3, 0, 0 );

    FontSize = new QComboBox( FALSE, GroupBox5, "FontSize" );

    GroupBox5Layout->addWidget( FontSize, 1, 1 );

    FontDB = new QComboBox( FALSE, GroupBox5, "FontDB" );

    GroupBox5Layout->addWidget( FontDB, 1, 0 );

    DlgEditorSettingsBaseLayout->addWidget( GroupBox5, 0, 0 );

    GroupBox2 = new QGroupBox( this, "GroupBox2" );
    GroupBox2->setColumnLayout(0, Qt::Vertical );
    GroupBox2->layout()->setSpacing( 6 );
    GroupBox2->layout()->setMargin( 11 );
    GroupBox2Layout = new QGridLayout( GroupBox2->layout() );
    GroupBox2Layout->setAlignment( Qt::AlignTop );

    EnableLineNumber = new Gui::PrefCheckBox( GroupBox2, "EnableLineNumber" );
    EnableLineNumber->setFocusPolicy( Gui::PrefCheckBox::TabFocus );
    EnableLineNumber->setChecked( TRUE );
    EnableLineNumber->setProperty( "prefEntry", "EnableLineNumber" );
    EnableLineNumber->setProperty( "prefPath", "Editor" );

    GroupBox2Layout->addWidget( EnableLineNumber, 0, 0 );

    EnableFolding = new Gui::PrefCheckBox( GroupBox2, "EnableFolding" );
    EnableFolding->setChecked( TRUE );
    EnableFolding->setProperty( "prefEntry", "EnableFolding" );
    EnableFolding->setProperty( "prefPath", "Editor" );

    GroupBox2Layout->addWidget( EnableFolding, 1, 0 );

    DlgEditorSettingsBaseLayout->addWidget( GroupBox2, 0, 1 );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );

    GroupBox1Layout->addWidget( TextLabel1, 1, 1 );

    ColorBtn = new Gui::ColorButton( GroupBox1, "ColorBtn" );
    ColorBtn->setMinimumSize( QSize( 140, 0 ) );
    ColorBtn->setFocusPolicy( Gui::ColorButton::TabFocus );

    GroupBox1Layout->addWidget( ColorBtn, 2, 1 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupBox1Layout->addItem( spacer, 2, 2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupBox1Layout->addItem( spacer_2, 2, 0 );

    ListBox1 = new QListBox( GroupBox1, "ListBox1" );

    GroupBox1Layout->addMultiCellWidget( ListBox1, 0, 0, 0, 2 );

    DlgEditorSettingsBaseLayout->addWidget( GroupBox1, 2, 0 );

    tabIndent = new QButtonGroup( this, "tabIndent" );
    tabIndent->setColumnLayout(0, Qt::Vertical );
    tabIndent->layout()->setSpacing( 6 );
    tabIndent->layout()->setMargin( 11 );
    tabIndentLayout = new QGridLayout( tabIndent->layout() );
    tabIndentLayout->setAlignment( Qt::AlignTop );

    layout2 = new QGridLayout( 0, 1, 1, 0, 6, "layout2"); 

    tabSize = new Gui::PrefSpinBox( tabIndent, "tabSize" );
    tabSize->setMaxValue( 20 );
    tabSize->setValue( 4 );
    tabSize->setProperty( "prefEntry", "TabSize" );
    tabSize->setProperty( "prefPath", "Editor" );

    layout2->addWidget( tabSize, 0, 1 );

    indentSize = new Gui::PrefSpinBox( tabIndent, "indentSize" );
    indentSize->setMaxValue( 20 );
    indentSize->setValue( 4 );
    indentSize->setProperty( "prefEntry", "IndentSize" );
    indentSize->setProperty( "prefPath", "Editor" );

    layout2->addWidget( indentSize, 1, 1 );

    textLabel1 = new QLabel( tabIndent, "textLabel1" );

    layout2->addWidget( textLabel1, 0, 0 );

    textLabel2 = new QLabel( tabIndent, "textLabel2" );

    layout2->addWidget( textLabel2, 1, 0 );

    tabIndentLayout->addMultiCellLayout( layout2, 0, 1, 0, 0 );

    radioTabs = new Gui::PrefRadioButton( tabIndent, "radioTabs" );
    radioTabs->setChecked( TRUE );
    radioTabs->setProperty( "prefEntry", "Tabs" );
    radioTabs->setProperty( "prefPath", "Editor" );
    tabIndent->insert( radioTabs, 0 );

    tabIndentLayout->addWidget( radioTabs, 0, 1 );

    radioSpaces = new Gui::PrefRadioButton( tabIndent, "radioSpaces" );
    radioSpaces->setProperty( "prefEntry", "Spaces" );
    radioSpaces->setProperty( "prefPath", "Editor" );
    tabIndent->insert( radioSpaces, 1 );

    tabIndentLayout->addWidget( radioSpaces, 1, 1 );

    DlgEditorSettingsBaseLayout->addMultiCellWidget( tabIndent, 1, 1, 0, 1 );
    languageChange();
    resize( QSize(445, 445).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // tab order
    setTabOrder( FontDB, FontSize );
    setTabOrder( FontSize, EnableLineNumber );
    setTabOrder( EnableLineNumber, EnableFolding );
    setTabOrder( EnableFolding, tabSize );
    setTabOrder( tabSize, indentSize );
    setTabOrder( indentSize, ListBox1 );
    setTabOrder( ListBox1, ColorBtn );
    setTabOrder( ColorBtn, textEdit1 );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgEditorSettingsBase::~DlgEditorSettingsBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgEditorSettingsBase::languageChange()
{
    setCaption( tr( "Editor" ) );
    GroupBox2_2->setTitle( tr( "Sample" ) );
    textEdit1->setText( tr( "# Short Python sample\n"
"import sys\n"
"def foo(begin, end):\n"
"	i=begin\n"
"	while (i<end):\n"
"		print i\n"
"		i=i+1\n"
"		print \"Text\"\n"
"\n"
"foo(0, 20)\n"
"" ) );
    GroupBox5->setTitle( tr( "Font settings:" ) );
    TextLabel4->setText( tr( "Size:" ) );
    TextLabel3->setText( tr( "Font:" ) );
    FontSize->clear();
    FontSize->insertItem( tr( "5" ) );
    FontSize->insertItem( tr( "6" ) );
    FontSize->insertItem( tr( "7" ) );
    FontSize->insertItem( tr( "8" ) );
    FontSize->insertItem( tr( "9" ) );
    FontSize->insertItem( tr( "10" ) );
    FontSize->insertItem( tr( "11" ) );
    FontSize->insertItem( tr( "12" ) );
    FontSize->insertItem( tr( "13" ) );
    FontSize->insertItem( tr( "14" ) );
    FontSize->insertItem( tr( "15" ) );
    FontSize->insertItem( tr( "16" ) );
    FontSize->insertItem( tr( "17" ) );
    FontSize->insertItem( tr( "18" ) );
    FontSize->insertItem( tr( "19" ) );
    FontSize->insertItem( tr( "20" ) );
    FontSize->setCurrentItem( 4 );
    GroupBox2->setTitle( tr( "Window settings" ) );
    EnableLineNumber->setText( tr( "Enable line numbers" ) );
    EnableFolding->setText( tr( "Enable folding" ) );
    GroupBox1->setTitle( tr( "Colors" ) );
    TextLabel1->setText( tr( "Color:" ) );
    ColorBtn->setText( QString::null );
    ListBox1->clear();
    ListBox1->insertItem( tr( "Text" ) );
    ListBox1->insertItem( tr( "Bookmark" ) );
    ListBox1->insertItem( tr( "Breakpoint" ) );
    ListBox1->insertItem( tr( "Keyword" ) );
    ListBox1->insertItem( tr( "Comment" ) );
    ListBox1->insertItem( tr( "Block comment" ) );
    ListBox1->insertItem( tr( "Number" ) );
    ListBox1->insertItem( tr( "String" ) );
    ListBox1->insertItem( tr( "Character" ) );
    ListBox1->insertItem( tr( "Class name" ) );
    ListBox1->insertItem( tr( "Define name" ) );
    ListBox1->insertItem( tr( "Operator" ) );
    ListBox1->insertItem( tr( "Python output" ) );
    ListBox1->insertItem( tr( "Python error" ) );
    tabIndent->setTitle( tr( "Tabs" ) );
    textLabel1->setText( tr( "Tab size:" ) );
    textLabel2->setText( tr( "Indent size:" ) );
    radioTabs->setText( tr( "Keep tabs" ) );
    radioSpaces->setText( tr( "Insert spaces" ) );
}

void Gui::Dialog::DlgEditorSettingsBase::saveSettings()
{
    qWarning( "Gui::Dialog::DlgEditorSettingsBase::saveSettings(): Not implemented yet" );
}

void Gui::Dialog::DlgEditorSettingsBase::loadSettings()
{
    qWarning( "Gui::Dialog::DlgEditorSettingsBase::loadSettings(): Not implemented yet" );
}

