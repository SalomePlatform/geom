/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_1Sel2Spin_QTD.ui'
**
** Created: jeu oct 2 11:08:05 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DLGREF_1SEL2SPIN_QTD_H
#define DLGREF_1SEL2SPIN_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;

class DlgRef_1Sel2Spin_QTD : public QWidget
{ 
    Q_OBJECT

public:
    DlgRef_1Sel2Spin_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_1Sel2Spin_QTD();

    QGroupBox* GroupBox1;
    QLabel* TextLabel3;
    QSpinBox* SpinBox1;
    QLabel* TextLabel2;
    QSpinBox* SpinBox2;
    QLineEdit* LineEdit1;
    QPushButton* PushButton1;
    QLabel* TextLabel1;


protected:
    QGridLayout* DlgRef_1Sel2Spin_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
    QGridLayout* Layout2;
};

#endif // DLGREF_1SEL2SPIN_QTD_H
