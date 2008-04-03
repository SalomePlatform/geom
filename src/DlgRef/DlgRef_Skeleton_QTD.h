/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_Skeleton_QTD.ui'
**
** Created: Wed Apr 2 16:42:12 2008
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
class QButtonGroup;
class QRadioButton;
class QGroupBox;
class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;

class DlgRef_Skeleton_QTD : public QDialog
{
    Q_OBJECT

public:
    DlgRef_Skeleton_QTD( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgRef_Skeleton_QTD();

    QButtonGroup* GroupConstructors;
    QRadioButton* RadioButton1;
    QRadioButton* RadioButton2;
    QRadioButton* RadioButton3;
    QRadioButton* RadioButton4;
    QRadioButton* RadioButton5;
    QGroupBox* GroupBoxName;
    QLabel* NameLabel;
    QLineEdit* ResultName;
    QGroupBox* GroupBoxPublish;
    QCheckBox* CheckBoxRestoreSS;
    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonCancel;
    QPushButton* buttonOk;
    QPushButton* buttonHelp;

protected:
    QGridLayout* DlgRef_Skeleton_QTDLayout;
    QGridLayout* Layout1;
    QHBoxLayout* GroupConstructorsLayout;
    QHBoxLayout* Layout2;
    QGridLayout* GroupBoxNameLayout;
    QGridLayout* Layout4;
    QGridLayout* GroupBoxPublishLayout;
    QGridLayout* Layout5;
    QVBoxLayout* GroupButtonsLayout;
    QGridLayout* Layout3;
    QSpacerItem* Spacer1;

protected slots:
    virtual void languageChange();

};

#endif // DLGREF_SKELETON_QTD_H
