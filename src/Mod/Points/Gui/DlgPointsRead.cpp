/****************************************************************************
** Form implementation generated from reading ui file '.\DlgPointsRead.ui'
**
** Created: Mi 17. Mai 14:50:08 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgPointsRead.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a PointsGui::DlgPointsRead as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PointsGui::DlgPointsRead::DlgPointsRead( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "PointsGui::DlgPointsRead" );
    DlgPointsReadLayout = new QVBoxLayout( this, 11, 6, "DlgPointsReadLayout"); 

    layout5 = new QHBoxLayout( 0, 0, 6, "layout5"); 

    textLabel1 = new QLabel( this, "textLabel1" );
    layout5->addWidget( textLabel1 );

    comboBoxTrmplate = new QComboBox( FALSE, this, "comboBoxTrmplate" );
    layout5->addWidget( comboBoxTrmplate );
    DlgPointsReadLayout->addLayout( layout5 );

    groupBox1 = new QGroupBox( this, "groupBox1" );
    groupBox1->setColumnLayout(0, Qt::Vertical );
    groupBox1->layout()->setSpacing( 6 );
    groupBox1->layout()->setMargin( 11 );
    groupBox1Layout = new QVBoxLayout( groupBox1->layout() );
    groupBox1Layout->setAlignment( Qt::AlignTop );

    layout1 = new QHBoxLayout( 0, 0, 6, "layout1"); 

    textLabel2 = new QLabel( groupBox1, "textLabel2" );
    layout1->addWidget( textLabel2 );

    buttonGroup2 = new QButtonGroup( groupBox1, "buttonGroup2" );
    buttonGroup2->setColumnLayout(0, Qt::Vertical );
    buttonGroup2->layout()->setSpacing( 6 );
    buttonGroup2->layout()->setMargin( 11 );
    buttonGroup2Layout = new QHBoxLayout( buttonGroup2->layout() );
    buttonGroup2Layout->setAlignment( Qt::AlignTop );

    radioButtonIgnore = new QRadioButton( buttonGroup2, "radioButtonIgnore" );
    buttonGroup2Layout->addWidget( radioButtonIgnore );

    radioButtonNbrOfPoints = new QRadioButton( buttonGroup2, "radioButtonNbrOfPoints" );
    buttonGroup2Layout->addWidget( radioButtonNbrOfPoints );
    layout1->addWidget( buttonGroup2 );
    groupBox1Layout->addLayout( layout1 );

    layout4 = new QHBoxLayout( 0, 0, 6, "layout4"); 

    layout3 = new QVBoxLayout( 0, 0, 6, "layout3"); 

    textLabel3 = new QLabel( groupBox1, "textLabel3" );
    layout3->addWidget( textLabel3 );

    textLabel4 = new QLabel( groupBox1, "textLabel4" );
    layout3->addWidget( textLabel4 );
    layout4->addLayout( layout3 );

    layout2 = new QVBoxLayout( 0, 0, 6, "layout2"); 

    lineEditClusterStart = new QLineEdit( groupBox1, "lineEditClusterStart" );
    lineEditClusterStart->setFrameShape( QLineEdit::LineEditPanel );
    lineEditClusterStart->setFrameShadow( QLineEdit::Sunken );
    layout2->addWidget( lineEditClusterStart );

    lineEditComments = new QLineEdit( groupBox1, "lineEditComments" );
    lineEditComments->setFrameShape( QLineEdit::LineEditPanel );
    lineEditComments->setFrameShadow( QLineEdit::Sunken );
    layout2->addWidget( lineEditComments );
    layout4->addLayout( layout2 );
    groupBox1Layout->addLayout( layout4 );
    DlgPointsReadLayout->addWidget( groupBox1 );

    groupBox2 = new QGroupBox( this, "groupBox2" );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    groupBox2Layout = new QVBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    layout9 = new QHBoxLayout( 0, 0, 6, "layout9"); 

    layout8 = new QVBoxLayout( 0, 0, 6, "layout8"); 

    textLabel6 = new QLabel( groupBox2, "textLabel6" );
    layout8->addWidget( textLabel6 );

    textLabel7 = new QLabel( groupBox2, "textLabel7" );
    layout8->addWidget( textLabel7 );

    textLabel8 = new QLabel( groupBox2, "textLabel8" );
    layout8->addWidget( textLabel8 );

    textLabel8_2 = new QLabel( groupBox2, "textLabel8_2" );
    layout8->addWidget( textLabel8_2 );

    textLabel8_2_2 = new QLabel( groupBox2, "textLabel8_2_2" );
    layout8->addWidget( textLabel8_2_2 );
    layout9->addLayout( layout8 );

    layout7 = new QVBoxLayout( 0, 0, 6, "layout7"); 

    comboBoxSeperator = new QComboBox( FALSE, groupBox2, "comboBoxSeperator" );
    comboBoxSeperator->setEditable( TRUE );
    layout7->addWidget( comboBoxSeperator );

    comboBoxPointsFormat = new QComboBox( FALSE, groupBox2, "comboBoxPointsFormat" );
    layout7->addWidget( comboBoxPointsFormat );

    comboBoxNextBlock1 = new QComboBox( FALSE, groupBox2, "comboBoxNextBlock1" );
    layout7->addWidget( comboBoxNextBlock1 );

    comboBoxNextBlock2 = new QComboBox( FALSE, groupBox2, "comboBoxNextBlock2" );
    layout7->addWidget( comboBoxNextBlock2 );

    comboBoxNextBlock3 = new QComboBox( FALSE, groupBox2, "comboBoxNextBlock3" );
    layout7->addWidget( comboBoxNextBlock3 );
    layout9->addLayout( layout7 );
    groupBox2Layout->addLayout( layout9 );
    DlgPointsReadLayout->addWidget( groupBox2 );

    groupBox3 = new QGroupBox( this, "groupBox3" );
    groupBox3->setColumnLayout(0, Qt::Vertical );
    groupBox3->layout()->setSpacing( 6 );
    groupBox3->layout()->setMargin( 11 );
    groupBox3Layout = new QVBoxLayout( groupBox3->layout() );
    groupBox3Layout->setAlignment( Qt::AlignTop );

    lineEditFileName = new QLineEdit( groupBox3, "lineEditFileName" );
    lineEditFileName->setReadOnly( TRUE );
    groupBox3Layout->addWidget( lineEditFileName );

    layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 

    textLabel5 = new QLabel( groupBox3, "textLabel5" );
    layout6->addWidget( textLabel5 );

    lineEditPreviewLines = new QLineEdit( groupBox3, "lineEditPreviewLines" );
    layout6->addWidget( lineEditPreviewLines );
    groupBox3Layout->addLayout( layout6 );

    textEditPreview = new QTextEdit( groupBox3, "textEditPreview" );
    groupBox3Layout->addWidget( textEditPreview );
    DlgPointsReadLayout->addWidget( groupBox3 );
    languageChange();
    resize( QSize(342, 550).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PointsGui::DlgPointsRead::~DlgPointsRead()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PointsGui::DlgPointsRead::languageChange()
{
    setCaption( tr( "ASCII points import" ) );
    textLabel1->setText( tr( "Template:" ) );
    groupBox1->setTitle( tr( "Special lines" ) );
    textLabel2->setText( tr( "First line:" ) );
    buttonGroup2->setTitle( tr( "Action" ) );
    radioButtonIgnore->setText( tr( "Ignore" ) );
    radioButtonNbrOfPoints->setText( tr( "Nbr. of Points" ) );
    textLabel3->setText( tr( "Cluster by lines starting with:" ) );
    textLabel4->setText( tr( "Ignore lines starting with:" ) );
    groupBox2->setTitle( tr( "Point format" ) );
    textLabel6->setText( tr( "Number seperator:" ) );
    textLabel7->setText( tr( "Points format:" ) );
    textLabel8->setText( tr( "Next block:" ) );
    textLabel8_2->setText( tr( "Next block:" ) );
    textLabel8_2_2->setText( tr( "Next block:" ) );
    comboBoxSeperator->clear();
    comboBoxSeperator->insertItem( tr( "," ) );
    comboBoxSeperator->insertItem( tr( ";" ) );
    comboBoxSeperator->insertItem( tr( "\\t" ) );
    comboBoxSeperator->insertItem( tr( "\\w" ) );
    comboBoxPointsFormat->clear();
    comboBoxPointsFormat->insertItem( tr( "X,Y,Z" ) );
    comboBoxPointsFormat->insertItem( tr( "X,Y" ) );
    comboBoxNextBlock1->clear();
    comboBoxNextBlock1->insertItem( tr( "none" ) );
    comboBoxNextBlock1->insertItem( tr( "I,J,K (normal vector)" ) );
    comboBoxNextBlock1->insertItem( tr( "I,K (normal vector 2D)" ) );
    comboBoxNextBlock1->insertItem( tr( "R,G,B (color)" ) );
    comboBoxNextBlock1->insertItem( tr( "I (Gray value)" ) );
    comboBoxNextBlock2->clear();
    comboBoxNextBlock2->insertItem( tr( "none" ) );
    comboBoxNextBlock2->insertItem( tr( "I,J,K (normal vector)" ) );
    comboBoxNextBlock2->insertItem( tr( "I,K (normal vector 2D)" ) );
    comboBoxNextBlock2->insertItem( tr( "R,G,B (color)" ) );
    comboBoxNextBlock2->insertItem( tr( "I (Gray value)" ) );
    comboBoxNextBlock3->clear();
    comboBoxNextBlock3->insertItem( tr( "none" ) );
    comboBoxNextBlock3->insertItem( tr( "I,J,K (normal vector)" ) );
    comboBoxNextBlock3->insertItem( tr( "I,K (normal vector 2D)" ) );
    comboBoxNextBlock3->insertItem( tr( "R,G,B (color)" ) );
    comboBoxNextBlock3->insertItem( tr( "I (Gray value)" ) );
    groupBox3->setTitle( tr( "Preview" ) );
    textLabel5->setText( tr( "Nbr. of previewd lines:" ) );
    lineEditPreviewLines->setText( tr( "100" ) );
}

