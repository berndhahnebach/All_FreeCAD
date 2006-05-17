/****************************************************************************
** Form implementation generated from reading ui file '.\DlgSettingsImage.ui'
**
** Created: Mi 17. Mai 14:36:16 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgSettingsImage.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qspinbox.h>
#include <Gui/PrefWidgets.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgSettingsImage as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Gui::Dialog::DlgSettingsImage::DlgSettingsImage( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgSettingsImage" );
    DlgSettingsImageLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgSettingsImageLayout"); 

    groupBoxProp = new QGroupBox( this, "groupBoxProp" );
    groupBoxProp->setColumnLayout(0, Qt::Vertical );
    groupBoxProp->layout()->setSpacing( 6 );
    groupBoxProp->layout()->setMargin( 11 );
    groupBoxPropLayout = new QGridLayout( groupBoxProp->layout() );
    groupBoxPropLayout->setAlignment( Qt::AlignTop );

    textLabelImageType = new QLabel( groupBoxProp, "textLabelImageType" );

    groupBoxPropLayout->addWidget( textLabelImageType, 0, 0 );

    textLabelColor = new QLabel( groupBoxProp, "textLabelColor" );

    groupBoxPropLayout->addWidget( textLabelColor, 1, 0 );

    comboColor = new QComboBox( FALSE, groupBoxProp, "comboColor" );

    groupBoxPropLayout->addWidget( comboColor, 1, 1 );

    comboImageType = new QComboBox( FALSE, groupBoxProp, "comboImageType" );

    groupBoxPropLayout->addWidget( comboImageType, 0, 1 );
    QSpacerItem* spacer = new QSpacerItem( 121, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBoxPropLayout->addItem( spacer, 2, 0 );

    checkTransparent = new QCheckBox( groupBoxProp, "checkTransparent" );

    groupBoxPropLayout->addWidget( checkTransparent, 2, 1 );

    DlgSettingsImageLayout->addWidget( groupBoxProp, 1, 0 );

    groupBoxComment = new QGroupBox( this, "groupBoxComment" );
    groupBoxComment->setColumnLayout(0, Qt::Vertical );
    groupBoxComment->layout()->setSpacing( 6 );
    groupBoxComment->layout()->setMargin( 11 );
    groupBoxCommentLayout = new QVBoxLayout( groupBoxComment->layout() );
    groupBoxCommentLayout->setAlignment( Qt::AlignTop );

    textEditComment = new QTextEdit( groupBoxComment, "textEditComment" );
    textEditComment->setVScrollBarMode( QTextEdit::Auto );
    textEditComment->setHScrollBarMode( QTextEdit::Auto );
    textEditComment->setTextFormat( QTextEdit::PlainText );
    textEditComment->setWordWrap( QTextEdit::NoWrap );
    groupBoxCommentLayout->addWidget( textEditComment );

    layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 

    pushButtonInsertTimeDate = new QPushButton( groupBoxComment, "pushButtonInsertTimeDate" );
    layout2->addWidget( pushButtonInsertTimeDate );

    pushButtonInsertMatrix = new QPushButton( groupBoxComment, "pushButtonInsertMatrix" );
    layout2->addWidget( pushButtonInsertMatrix );

    pushButtonInsertMIBA = new QPushButton( groupBoxComment, "pushButtonInsertMIBA" );
    layout2->addWidget( pushButtonInsertMIBA );
    groupBoxCommentLayout->addLayout( layout2 );

    DlgSettingsImageLayout->addWidget( groupBoxComment, 2, 0 );

    groupBoxDim = new QGroupBox( this, "groupBoxDim" );
    groupBoxDim->setColumnLayout(0, Qt::Vertical );
    groupBoxDim->layout()->setSpacing( 6 );
    groupBoxDim->layout()->setMargin( 11 );
    groupBoxDimLayout = new QGridLayout( groupBoxDim->layout() );
    groupBoxDimLayout->setAlignment( Qt::AlignTop );

    layout4 = new QHBoxLayout( 0, 0, 6, "layout4"); 

    textLabelWidth = new QLabel( groupBoxDim, "textLabelWidth" );
    layout4->addWidget( textLabelWidth );

    spinWidth = new QSpinBox( groupBoxDim, "spinWidth" );
    spinWidth->setMaxValue( 32767 );
    spinWidth->setMinValue( 1 );
    layout4->addWidget( spinWidth );

    groupBoxDimLayout->addMultiCellLayout( layout4, 0, 0, 0, 4 );

    layout5 = new QHBoxLayout( 0, 0, 6, "layout5"); 

    textLabelHeight = new QLabel( groupBoxDim, "textLabelHeight" );
    layout5->addWidget( textLabelHeight );

    spinHeight = new QSpinBox( groupBoxDim, "spinHeight" );
    spinHeight->setMaxValue( 32767 );
    spinHeight->setMinValue( 1 );
    layout5->addWidget( spinHeight );

    groupBoxDimLayout->addMultiCellLayout( layout5, 1, 1, 0, 4 );

    layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 

    textLabelResolution = new QLabel( groupBoxDim, "textLabelResolution" );
    layout6->addWidget( textLabelResolution );

    spinResolution = new Gui::FloatSpinBox( groupBoxDim, "spinResolution" );
    spinResolution->setMaxValue( 2540 );
    spinResolution->setMinValue( 1 );
    spinResolution->setValue( 72 );
    layout6->addWidget( spinResolution );

    groupBoxDimLayout->addMultiCellLayout( layout6, 2, 2, 0, 4 );

    textLabel1 = new QLabel( groupBoxDim, "textLabel1" );

    groupBoxDimLayout->addWidget( textLabel1, 3, 0 );

    pushButtonRatioScreen = new QPushButton( groupBoxDim, "pushButtonRatioScreen" );

    groupBoxDimLayout->addWidget( pushButtonRatioScreen, 3, 1 );

    pushButtonRatio4x3 = new QPushButton( groupBoxDim, "pushButtonRatio4x3" );

    groupBoxDimLayout->addWidget( pushButtonRatio4x3, 3, 2 );

    pushButtonRatio16x9 = new QPushButton( groupBoxDim, "pushButtonRatio16x9" );

    groupBoxDimLayout->addWidget( pushButtonRatio16x9, 3, 3 );

    pushButtonRatio1x1 = new QPushButton( groupBoxDim, "pushButtonRatio1x1" );

    groupBoxDimLayout->addWidget( pushButtonRatio1x1, 3, 4 );

    DlgSettingsImageLayout->addWidget( groupBoxDim, 0, 0 );
    languageChange();
    resize( QSize(463, 582).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButtonRatioScreen, SIGNAL( clicked() ), this, SLOT( onRatioScreen() ) );
    connect( checkTransparent, SIGNAL( toggled(bool) ), comboColor, SLOT( setDisabled(bool) ) );
    connect( pushButtonInsertMIBA, SIGNAL( clicked() ), this, SLOT( onInsertMIBA() ) );
    connect( pushButtonInsertMatrix, SIGNAL( clicked() ), this, SLOT( onInsertViewMatrix() ) );
    connect( pushButtonInsertTimeDate, SIGNAL( clicked() ), this, SLOT( onInsertDateTime() ) );
    connect( pushButtonRatio4x3, SIGNAL( clicked() ), this, SLOT( onRatio4x3() ) );
    connect( pushButtonRatio16x9, SIGNAL( clicked() ), this, SLOT( onRatio16x9() ) );
    connect( pushButtonRatio1x1, SIGNAL( clicked() ), this, SLOT( onRatio1x1() ) );

    // tab order
    setTabOrder( spinWidth, spinHeight );
    setTabOrder( spinHeight, spinResolution );
    setTabOrder( spinResolution, pushButtonRatioScreen );
    setTabOrder( pushButtonRatioScreen, pushButtonRatio4x3 );
    setTabOrder( pushButtonRatio4x3, pushButtonRatio16x9 );
    setTabOrder( pushButtonRatio16x9, pushButtonRatio1x1 );
    setTabOrder( pushButtonRatio1x1, comboImageType );
    setTabOrder( comboImageType, comboColor );
    setTabOrder( comboColor, checkTransparent );
    setTabOrder( checkTransparent, textEditComment );
    setTabOrder( textEditComment, pushButtonInsertTimeDate );
    setTabOrder( pushButtonInsertTimeDate, pushButtonInsertMatrix );
    setTabOrder( pushButtonInsertMatrix, pushButtonInsertMIBA );

    // buddies
    textLabelImageType->setBuddy( comboImageType );
    textLabelColor->setBuddy( comboColor );
    textLabelWidth->setBuddy( spinWidth );
    textLabelHeight->setBuddy( spinHeight );
    textLabelResolution->setBuddy( spinResolution );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgSettingsImage::~DlgSettingsImage()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgSettingsImage::languageChange()
{
    setCaption( tr( "Image settings" ) );
    groupBoxProp->setTitle( tr( "Image properties" ) );
    textLabelImageType->setText( tr( "&Image format:" ) );
    textLabelColor->setText( tr( "Back&ground color:" ) );
    comboColor->clear();
    comboColor->insertItem( tr( "Current" ) );
    comboColor->insertItem( tr( "White" ) );
    comboColor->insertItem( tr( "Black" ) );
    comboColor->insertItem( tr( "Red" ) );
    comboColor->insertItem( tr( "Green" ) );
    comboColor->insertItem( tr( "Blue" ) );
    comboColor->insertItem( tr( "Cyan" ) );
    comboColor->insertItem( tr( "Magenta" ) );
    comboColor->insertItem( tr( "Yellow" ) );
    comboColor->insertItem( tr( "Gray" ) );
    comboImageType->clear();
    comboImageType->insertItem( tr( "Grayscale Image" ) );
    comboImageType->insertItem( tr( "RGB Image" ) );
    checkTransparent->setText( tr( "Transparent background" ) );
    groupBoxComment->setTitle( tr( "Picture comment" ) );
    pushButtonInsertTimeDate->setText( tr( "Insert &datetime" ) );
    pushButtonInsertTimeDate->setAccel( QKeySequence( tr( "Alt+D" ) ) );
    pushButtonInsertMatrix->setText( tr( "Insert &view matrix" ) );
    pushButtonInsertMatrix->setAccel( QKeySequence( tr( "Alt+V" ) ) );
    pushButtonInsertMIBA->setText( tr( "Insert &MIBA" ) );
    pushButtonInsertMIBA->setAccel( QKeySequence( tr( "Alt+M" ) ) );
    groupBoxDim->setTitle( tr( "Image dimensions" ) );
    textLabelWidth->setText( tr( "&Width:" ) );
    spinWidth->setSuffix( tr( " Pixel" ) );
    textLabelHeight->setText( tr( "&Height:" ) );
    spinHeight->setSuffix( tr( " Pixel" ) );
    textLabelResolution->setText( tr( "&Resolution:" ) );
    spinResolution->setSuffix( tr( " Pixel/Inch" ) );
    textLabel1->setText( tr( "Aspect ratio:" ) );
    pushButtonRatioScreen->setText( tr( "&Screen" ) );
    pushButtonRatioScreen->setAccel( QKeySequence( tr( "Alt+S" ) ) );
    pushButtonRatio4x3->setText( tr( "&4:3" ) );
    pushButtonRatio4x3->setAccel( QKeySequence( tr( "Alt+4" ) ) );
    pushButtonRatio16x9->setText( tr( "1&6:9" ) );
    pushButtonRatio16x9->setAccel( QKeySequence( tr( "Alt+6" ) ) );
    pushButtonRatio1x1->setText( tr( "&1:1" ) );
    pushButtonRatio1x1->setAccel( QKeySequence( tr( "Alt+1" ) ) );
}

void Gui::Dialog::DlgSettingsImage::onRatioScreen()
{
    qWarning( "Gui::Dialog::DlgSettingsImage::onRatioScreen(): Not implemented yet" );
}

void Gui::Dialog::DlgSettingsImage::onInsertMIBA()
{
    qWarning( "Gui::Dialog::DlgSettingsImage::onInsertMIBA(): Not implemented yet" );
}

void Gui::Dialog::DlgSettingsImage::onInsertViewMatrix()
{
    qWarning( "Gui::Dialog::DlgSettingsImage::onInsertViewMatrix(): Not implemented yet" );
}

void Gui::Dialog::DlgSettingsImage::onInsertDateTime()
{
    qWarning( "Gui::Dialog::DlgSettingsImage::onInsertDateTime(): Not implemented yet" );
}

void Gui::Dialog::DlgSettingsImage::onSelectedFilter(const QString&)
{
    qWarning( "Gui::Dialog::DlgSettingsImage::onSelectedFilter(const QString&): Not implemented yet" );
}

void Gui::Dialog::DlgSettingsImage::onRatio4x3()
{
    qWarning( "Gui::Dialog::DlgSettingsImage::onRatio4x3(): Not implemented yet" );
}

void Gui::Dialog::DlgSettingsImage::onRatio16x9()
{
    qWarning( "Gui::Dialog::DlgSettingsImage::onRatio16x9(): Not implemented yet" );
}

void Gui::Dialog::DlgSettingsImage::onRatio1x1()
{
    qWarning( "Gui::Dialog::DlgSettingsImage::onRatio1x1(): Not implemented yet" );
}

