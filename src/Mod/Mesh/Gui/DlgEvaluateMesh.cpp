/****************************************************************************
** Form implementation generated from reading ui file '.\DlgEvaluateMesh.ui'
**
** Created: Sa 13. Mai 12:41:24 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgEvaluateMesh.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a MeshGui::DlgEvaluateMesh as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MeshGui::DlgEvaluateMesh::DlgEvaluateMesh( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "MeshGui::DlgEvaluateMesh" );
    setSizeGripEnabled( TRUE );
    DlgEvaluateMeshLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgEvaluateMeshLayout"); 
    QSpacerItem* spacer = new QSpacerItem( 20, 70, QSizePolicy::Minimum, QSizePolicy::Expanding );
    DlgEvaluateMeshLayout->addItem( spacer, 7, 0 );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer_2 );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );

    DlgEvaluateMeshLayout->addLayout( Layout1, 8, 0 );

    groupBoxInfo = new QGroupBox( this, "groupBoxInfo" );
    groupBoxInfo->setColumnLayout(0, Qt::Vertical );
    groupBoxInfo->layout()->setSpacing( 6 );
    groupBoxInfo->layout()->setMargin( 11 );
    groupBoxInfoLayout = new QGridLayout( groupBoxInfo->layout() );
    groupBoxInfoLayout->setAlignment( Qt::AlignTop );

    lineEditName = new QLineEdit( groupBoxInfo, "lineEditName" );
    lineEditName->setFrameShape( QLineEdit::LineEditPanel );
    lineEditName->setFrameShadow( QLineEdit::Sunken );
    lineEditName->setReadOnly( TRUE );

    groupBoxInfoLayout->addMultiCellWidget( lineEditName, 0, 0, 0, 1 );

    layout3 = new QGridLayout( 0, 1, 1, 0, 6, "layout3"); 
    QSpacerItem* spacer_3 = new QSpacerItem( 31, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addItem( spacer_3, 2, 2 );
    QSpacerItem* spacer_4 = new QSpacerItem( 51, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addItem( spacer_4, 0, 2 );

    textLabel4 = new QLabel( groupBoxInfo, "textLabel4" );
    textLabel4->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    layout3->addWidget( textLabel4, 0, 3 );

    textLabel3 = new QLabel( groupBoxInfo, "textLabel3" );

    layout3->addMultiCellWidget( textLabel3, 2, 2, 0, 1 );

    textLabel2 = new QLabel( groupBoxInfo, "textLabel2" );

    layout3->addWidget( textLabel2, 1, 0 );

    textLabel5 = new QLabel( groupBoxInfo, "textLabel5" );
    textLabel5->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    layout3->addWidget( textLabel5, 1, 3 );
    QSpacerItem* spacer_5 = new QSpacerItem( 51, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addMultiCell( spacer_5, 1, 1, 1, 2 );

    textLabel6 = new QLabel( groupBoxInfo, "textLabel6" );
    textLabel6->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    layout3->addWidget( textLabel6, 2, 3 );

    textLabel1 = new QLabel( groupBoxInfo, "textLabel1" );

    layout3->addWidget( textLabel1, 0, 0 );

    groupBoxInfoLayout->addMultiCellLayout( layout3, 0, 1, 2, 2 );

    pushButtonRefresh = new QPushButton( groupBoxInfo, "pushButtonRefresh" );

    groupBoxInfoLayout->addWidget( pushButtonRefresh, 1, 0 );
    QSpacerItem* spacer_6 = new QSpacerItem( 121, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBoxInfoLayout->addItem( spacer_6, 1, 1 );

    DlgEvaluateMeshLayout->addWidget( groupBoxInfo, 0, 0 );

    groupBox2_2 = new QGroupBox( this, "groupBox2_2" );
    groupBox2_2->setColumnLayout(0, Qt::Vertical );
    groupBox2_2->layout()->setSpacing( 6 );
    groupBox2_2->layout()->setMargin( 11 );
    groupBox2_2Layout = new QHBoxLayout( groupBox2_2->layout() );
    groupBox2_2Layout->setAlignment( Qt::AlignTop );

    textLabelDuplPoints = new QLabel( groupBox2_2, "textLabelDuplPoints" );
    groupBox2_2Layout->addWidget( textLabelDuplPoints );
    QSpacerItem* spacer_7 = new QSpacerItem( 240, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBox2_2Layout->addItem( spacer_7 );

    analyzeDupPts = new QPushButton( groupBox2_2, "analyzeDupPts" );
    groupBox2_2Layout->addWidget( analyzeDupPts );

    repairDupPts = new QPushButton( groupBox2_2, "repairDupPts" );
    repairDupPts->setEnabled( FALSE );
    groupBox2_2Layout->addWidget( repairDupPts );

    DlgEvaluateMeshLayout->addWidget( groupBox2_2, 3, 0 );

    groupBox2_3 = new QGroupBox( this, "groupBox2_3" );
    groupBox2_3->setColumnLayout(0, Qt::Vertical );
    groupBox2_3->layout()->setSpacing( 6 );
    groupBox2_3->layout()->setMargin( 11 );
    groupBox2_3Layout = new QHBoxLayout( groupBox2_3->layout() );
    groupBox2_3Layout->setAlignment( Qt::AlignTop );

    textLabelNonmanifolds = new QLabel( groupBox2_3, "textLabelNonmanifolds" );
    groupBox2_3Layout->addWidget( textLabelNonmanifolds );
    QSpacerItem* spacer_8 = new QSpacerItem( 270, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBox2_3Layout->addItem( spacer_8 );

    analyzeNonmanifolds = new QPushButton( groupBox2_3, "analyzeNonmanifolds" );
    groupBox2_3Layout->addWidget( analyzeNonmanifolds );

    repairNonmanifolds = new QPushButton( groupBox2_3, "repairNonmanifolds" );
    repairNonmanifolds->setEnabled( FALSE );
    groupBox2_3Layout->addWidget( repairNonmanifolds );

    DlgEvaluateMeshLayout->addWidget( groupBox2_3, 4, 0 );

    groupBox2 = new QGroupBox( this, "groupBox2" );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    groupBox2Layout = new QHBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    textLabelDuplicatedFaces = new QLabel( groupBox2, "textLabelDuplicatedFaces" );
    groupBox2Layout->addWidget( textLabelDuplicatedFaces );
    QSpacerItem* spacer_9 = new QSpacerItem( 290, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBox2Layout->addItem( spacer_9 );

    analyzeDupFaces = new QPushButton( groupBox2, "analyzeDupFaces" );
    groupBox2Layout->addWidget( analyzeDupFaces );

    repairDupFaces = new QPushButton( groupBox2, "repairDupFaces" );
    repairDupFaces->setEnabled( FALSE );
    groupBox2Layout->addWidget( repairDupFaces );

    DlgEvaluateMeshLayout->addWidget( groupBox2, 2, 0 );

    groupBox2_4 = new QGroupBox( this, "groupBox2_4" );
    groupBox2_4->setColumnLayout(0, Qt::Vertical );
    groupBox2_4->layout()->setSpacing( 6 );
    groupBox2_4->layout()->setMargin( 11 );
    groupBox2_4Layout = new QHBoxLayout( groupBox2_4->layout() );
    groupBox2_4Layout->setAlignment( Qt::AlignTop );

    textLabelOrientation = new QLabel( groupBox2_4, "textLabelOrientation" );
    groupBox2_4Layout->addWidget( textLabelOrientation );
    QSpacerItem* spacer_10 = new QSpacerItem( 290, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBox2_4Layout->addItem( spacer_10 );

    analyzeOrientation = new QPushButton( groupBox2_4, "analyzeOrientation" );
    groupBox2_4Layout->addWidget( analyzeOrientation );

    repairOrientation = new QPushButton( groupBox2_4, "repairOrientation" );
    repairOrientation->setEnabled( FALSE );
    groupBox2_4Layout->addWidget( repairOrientation );

    DlgEvaluateMeshLayout->addWidget( groupBox2_4, 1, 0 );

    groupBox2_3_2_2 = new QGroupBox( this, "groupBox2_3_2_2" );
    groupBox2_3_2_2->setColumnLayout(0, Qt::Vertical );
    groupBox2_3_2_2->layout()->setSpacing( 6 );
    groupBox2_3_2_2->layout()->setMargin( 11 );
    groupBox2_3_2_2Layout = new QHBoxLayout( groupBox2_3_2_2->layout() );
    groupBox2_3_2_2Layout->setAlignment( Qt::AlignTop );

    textLabelDegeneration = new QLabel( groupBox2_3_2_2, "textLabelDegeneration" );
    groupBox2_3_2_2Layout->addWidget( textLabelDegeneration );
    QSpacerItem* spacer_11 = new QSpacerItem( 296, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBox2_3_2_2Layout->addItem( spacer_11 );

    analyzeDegenerated = new QPushButton( groupBox2_3_2_2, "analyzeDegenerated" );
    groupBox2_3_2_2Layout->addWidget( analyzeDegenerated );

    repairDegenerated = new QPushButton( groupBox2_3_2_2, "repairDegenerated" );
    repairDegenerated->setEnabled( FALSE );
    groupBox2_3_2_2Layout->addWidget( repairDegenerated );

    DlgEvaluateMeshLayout->addWidget( groupBox2_3_2_2, 5, 0 );

    groupBox2_3_2 = new QGroupBox( this, "groupBox2_3_2" );
    groupBox2_3_2->setColumnLayout(0, Qt::Vertical );
    groupBox2_3_2->layout()->setSpacing( 6 );
    groupBox2_3_2->layout()->setMargin( 11 );
    groupBox2_3_2Layout = new QHBoxLayout( groupBox2_3_2->layout() );
    groupBox2_3_2Layout->setAlignment( Qt::AlignTop );

    textLabelIndices = new QLabel( groupBox2_3_2, "textLabelIndices" );
    groupBox2_3_2Layout->addWidget( textLabelIndices );
    QSpacerItem* spacer_12 = new QSpacerItem( 290, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBox2_3_2Layout->addItem( spacer_12 );

    analyzeIndices = new QPushButton( groupBox2_3_2, "analyzeIndices" );
    groupBox2_3_2Layout->addWidget( analyzeIndices );

    repairIndices = new QPushButton( groupBox2_3_2, "repairIndices" );
    repairIndices->setEnabled( FALSE );
    groupBox2_3_2Layout->addWidget( repairIndices );

    DlgEvaluateMeshLayout->addWidget( groupBox2_3_2, 6, 0 );
    languageChange();
    resize( QSize(371, 579).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( pushButtonRefresh, SIGNAL( clicked() ), this, SLOT( onRefreshInfo() ) );
    connect( analyzeDupFaces, SIGNAL( clicked() ), this, SLOT( onAnalyzeDuplicatedFaces() ) );
    connect( repairDupFaces, SIGNAL( clicked() ), this, SLOT( onRepairDuplicatedFaces() ) );
    connect( analyzeDupPts, SIGNAL( clicked() ), this, SLOT( onAnalyzeDuplicatedPoints() ) );
    connect( repairDupPts, SIGNAL( clicked() ), this, SLOT( onRepairDuplicatedPoints() ) );
    connect( analyzeNonmanifolds, SIGNAL( clicked() ), this, SLOT( onAnalyzeNonManifolds() ) );
    connect( repairNonmanifolds, SIGNAL( clicked() ), this, SLOT( onRepairNonManifolds() ) );
    connect( analyzeIndices, SIGNAL( clicked() ), this, SLOT( onAnalyzeIndices() ) );
    connect( repairIndices, SIGNAL( clicked() ), this, SLOT( onRepairIndices() ) );
    connect( analyzeDegenerated, SIGNAL( clicked() ), this, SLOT( onAnalyzeDegenerations() ) );
    connect( repairDegenerated, SIGNAL( clicked() ), this, SLOT( onRepairDegenerations() ) );
    connect( analyzeOrientation, SIGNAL( clicked() ), this, SLOT( onAnalyzeOrientation() ) );
    connect( repairOrientation, SIGNAL( clicked() ), this, SLOT( onRepairOrientation() ) );

    // tab order
    setTabOrder( lineEditName, pushButtonRefresh );
    setTabOrder( pushButtonRefresh, analyzeOrientation );
    setTabOrder( analyzeOrientation, repairOrientation );
    setTabOrder( repairOrientation, analyzeDupFaces );
    setTabOrder( analyzeDupFaces, repairDupFaces );
    setTabOrder( repairDupFaces, analyzeDupPts );
    setTabOrder( analyzeDupPts, repairDupPts );
    setTabOrder( repairDupPts, analyzeNonmanifolds );
    setTabOrder( analyzeNonmanifolds, repairNonmanifolds );
    setTabOrder( repairNonmanifolds, analyzeIndices );
    setTabOrder( analyzeIndices, repairIndices );
    setTabOrder( repairIndices, analyzeDegenerated );
    setTabOrder( analyzeDegenerated, repairDegenerated );
    setTabOrder( repairDegenerated, buttonHelp );
    setTabOrder( buttonHelp, buttonCancel );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MeshGui::DlgEvaluateMesh::~DlgEvaluateMesh()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MeshGui::DlgEvaluateMesh::languageChange()
{
    setCaption( tr( "Evaluate Mesh" ) );
    buttonHelp->setText( tr( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
    buttonCancel->setText( tr( "&Close" ) );
    buttonCancel->setAccel( QKeySequence( tr( "Alt+C" ) ) );
    groupBoxInfo->setTitle( tr( "Mesh information" ) );
    lineEditName->setText( tr( "No information" ) );
    textLabel4->setText( tr( "No information" ) );
    textLabel3->setText( tr( "Number of points:" ) );
    textLabel2->setText( tr( "Number of edges:" ) );
    textLabel5->setText( tr( "No information" ) );
    textLabel6->setText( tr( "No information" ) );
    textLabel1->setText( tr( "Number of faces" ) );
    pushButtonRefresh->setText( tr( "Refresh" ) );
    groupBox2_2->setTitle( tr( "Duplicate points" ) );
    textLabelDuplPoints->setText( tr( "No information" ) );
    analyzeDupPts->setText( tr( "Analyze" ) );
    repairDupPts->setText( tr( "Repair" ) );
    groupBox2_3->setTitle( tr( "Non-Manifolds" ) );
    textLabelNonmanifolds->setText( tr( "No information" ) );
    analyzeNonmanifolds->setText( tr( "Analyze" ) );
    repairNonmanifolds->setText( tr( "Repair" ) );
    groupBox2->setTitle( tr( "Duplicate faces" ) );
    textLabelDuplicatedFaces->setText( tr( "No information" ) );
    analyzeDupFaces->setText( tr( "Analyze" ) );
    repairDupFaces->setText( tr( "Repair" ) );
    groupBox2_4->setTitle( tr( "Orientation" ) );
    textLabelOrientation->setText( tr( "No information" ) );
    analyzeOrientation->setText( tr( "Analyze" ) );
    repairOrientation->setText( tr( "Repair" ) );
    groupBox2_3_2_2->setTitle( tr( "Degenerated faces" ) );
    textLabelDegeneration->setText( tr( "No information" ) );
    analyzeDegenerated->setText( tr( "Analyze" ) );
    repairDegenerated->setText( tr( "Repair" ) );
    groupBox2_3_2->setTitle( tr( "Face indices" ) );
    textLabelIndices->setText( tr( "No information" ) );
    analyzeIndices->setText( tr( "Analyze" ) );
    repairIndices->setText( tr( "Repair" ) );
}

void MeshGui::DlgEvaluateMesh::onRefreshInfo()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onRefreshInfo(): Not implemented yet" );
}

void MeshGui::DlgEvaluateMesh::onAnalyzeDuplicatedFaces()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onAnalyzeDuplicatedFaces(): Not implemented yet" );
}

void MeshGui::DlgEvaluateMesh::onRepairDuplicatedFaces()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onRepairDuplicatedFaces(): Not implemented yet" );
}

void MeshGui::DlgEvaluateMesh::onRepairDuplicatedPoints()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onRepairDuplicatedPoints(): Not implemented yet" );
}

void MeshGui::DlgEvaluateMesh::onAnalyzeDuplicatedPoints()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onAnalyzeDuplicatedPoints(): Not implemented yet" );
}

void MeshGui::DlgEvaluateMesh::onAnalyzeNonManifolds()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onAnalyzeNonManifolds(): Not implemented yet" );
}

void MeshGui::DlgEvaluateMesh::onRepairNonManifolds()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onRepairNonManifolds(): Not implemented yet" );
}

void MeshGui::DlgEvaluateMesh::onAnalyzeDegenerations()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onAnalyzeDegenerations(): Not implemented yet" );
}

void MeshGui::DlgEvaluateMesh::onRepairDegenerations()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onRepairDegenerations(): Not implemented yet" );
}

void MeshGui::DlgEvaluateMesh::onAnalyzeIndices()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onAnalyzeIndices(): Not implemented yet" );
}

void MeshGui::DlgEvaluateMesh::onRepairIndices()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onRepairIndices(): Not implemented yet" );
}

void MeshGui::DlgEvaluateMesh::onAnalyzeOrientation()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onAnalyzeOrientation(): Not implemented yet" );
}

void MeshGui::DlgEvaluateMesh::onRepairOrientation()
{
    qWarning( "MeshGui::DlgEvaluateMesh::onRepairOrientation(): Not implemented yet" );
}

