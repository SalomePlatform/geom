/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_Skeleton_QTD.ui'
**
** Created: Wed Feb 13 17:37:30 2008
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DLGREF_SKELETON_QTD_H
#define DLGREF_SKELETON_QTD_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QPushButton;
class QButtonGroup;
class QRadioButton;
class QLineEdit;
class QLabel;

class DlgRef_Skeleton_QTD : public QDialog
{
    Q_OBJECT

public:
    DlgRef_Skeleton_QTD( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgRef_Skeleton_QTD();

    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonApply;
    QPushButton* buttonCancel;
    QPushButton* buttonHelp;
    QButtonGroup* GroupConstructors;
    QRadioButton* RadioButton1;
    QRadioButton* RadioButton2;
    QRadioButton* RadioButton3;
    QRadioButton* RadioButton4;
    QGroupBox* GroupBoxName;
    QLineEdit* ResultName;
    QLabel* NameLabel;

protected:
    QGridLayout* DlgRef_Skeleton_QTDLayout;
    QGridLayout* Layout1;
    QVBoxLayout* GroupButtonsLayout;
    QGridLayout* layout3;
    QSpacerItem* Spacer1;
    QHBoxLayout* GroupConstructorsLayout;
    QHBoxLayout* Layout2;
    QGridLayout* GroupBoxNameLayout;
    QGridLayout* Layout66;

protected slots:
    virtual void languageChange();

};

#endif // DLGREF_SKELETON_QTD_H
