/****************************************************************************
** Form interface generated from reading ui file '.\DlgEvaluateMesh.ui'
**
** Created: Mi 17. Mai 14:47:27 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MESHGUI_DLGEVALUATEMESH_H
#define MESHGUI_DLGEVALUATEMESH_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QGroupBox;
class QLineEdit;
class QLabel;

namespace MeshGui {
class DlgEvaluateMesh : public QDialog
{
    Q_OBJECT

public:
    DlgEvaluateMesh( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgEvaluateMesh();

    QPushButton* buttonHelp;
    QPushButton* buttonCancel;
    QGroupBox* groupBoxInfo;
    QLineEdit* lineEditName;
    QLabel* textLabel4;
    QLabel* textLabel3;
    QLabel* textLabel2;
    QLabel* textLabel5;
    QLabel* textLabel6;
    QLabel* textLabel1;
    QPushButton* pushButtonRefresh;
    QGroupBox* groupBox2_2;
    QLabel* textLabelDuplPoints;
    QPushButton* analyzeDupPts;
    QPushButton* repairDupPts;
    QGroupBox* groupBox2_3;
    QLabel* textLabelNonmanifolds;
    QPushButton* analyzeNonmanifolds;
    QPushButton* repairNonmanifolds;
    QGroupBox* groupBox2;
    QLabel* textLabelDuplicatedFaces;
    QPushButton* analyzeDupFaces;
    QPushButton* repairDupFaces;
    QGroupBox* groupBox2_4;
    QLabel* textLabelOrientation;
    QPushButton* analyzeOrientation;
    QPushButton* repairOrientation;
    QGroupBox* groupBox2_3_2_2;
    QLabel* textLabelDegeneration;
    QPushButton* analyzeDegenerated;
    QPushButton* repairDegenerated;
    QGroupBox* groupBox2_3_2;
    QLabel* textLabelIndices;
    QPushButton* analyzeIndices;
    QPushButton* repairIndices;

public slots:
    virtual void onRefreshInfo();
    virtual void onAnalyzeDuplicatedFaces();
    virtual void onRepairDuplicatedFaces();
    virtual void onRepairDuplicatedPoints();
    virtual void onAnalyzeDuplicatedPoints();
    virtual void onAnalyzeNonManifolds();
    virtual void onRepairNonManifolds();
    virtual void onAnalyzeDegenerations();
    virtual void onRepairDegenerations();
    virtual void onAnalyzeIndices();
    virtual void onRepairIndices();
    virtual void onAnalyzeOrientation();
    virtual void onRepairOrientation();

protected:
    QGridLayout* DlgEvaluateMeshLayout;
    QHBoxLayout* Layout1;
    QGridLayout* groupBoxInfoLayout;
    QGridLayout* layout3;
    QHBoxLayout* groupBox2_2Layout;
    QHBoxLayout* groupBox2_3Layout;
    QHBoxLayout* groupBox2Layout;
    QHBoxLayout* groupBox2_4Layout;
    QHBoxLayout* groupBox2_3_2_2Layout;
    QHBoxLayout* groupBox2_3_2Layout;

protected slots:
    virtual void languageChange();

};
}

#endif // MESHGUI_DLGEVALUATEMESH_H
