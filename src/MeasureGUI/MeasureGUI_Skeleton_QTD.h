/****************************************************************************
** Form interface generated from reading ui file 'MeasureGUI_Skeleton_QTD.ui'
**
** Created: lun oct 27 11:23:09 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef MEASUREGUI_SKELETON_QTD_H
#define MEASUREGUI_SKELETON_QTD_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QGroupBox;
class QPushButton;
class QRadioButton;

class MeasureGUI_Skeleton_QTD : public QDialog
{ 
    Q_OBJECT

public:
    MeasureGUI_Skeleton_QTD( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MeasureGUI_Skeleton_QTD();

    QGroupBox* GroupButtons;
    QPushButton* buttonClose;
    QButtonGroup* GroupConstructors;
    QRadioButton* RadioButton1;
    QGroupBox* GroupMedium;


protected:
    QGridLayout* MeasureGUI_Skeleton_QTDLayout;
    QGridLayout* Layout1;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* Layout3;
    QGridLayout* GroupConstructorsLayout;
    QHBoxLayout* Layout2;
};

#endif // MEASUREGUI_SKELETON_QTD_H
