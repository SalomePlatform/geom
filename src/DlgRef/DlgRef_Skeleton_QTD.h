/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_Skeleton_QTD.ui'
**
** Created: mar sep 23 15:06:57 2003
**      by:  The User Interface Compiler (uic)
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
class QButtonGroup;
class QGroupBox;
class QPushButton;
class QRadioButton;

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
    QButtonGroup* GroupConstructors;
    QRadioButton* RadioButton1;
    QRadioButton* RadioButton2;
    QRadioButton* RadioButton3;
    QGroupBox* GroupMedium;


protected:
    QGridLayout* DlgRef_Skeleton_QTDLayout;
    QGridLayout* Layout1;
    QHBoxLayout* GroupButtonsLayout;
    QHBoxLayout* Layout3;
    QGridLayout* GroupConstructorsLayout;
    QHBoxLayout* Layout2;
};

#endif // DLGREF_SKELETON_QTD_H
