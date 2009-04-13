/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_3Sel2Spin_QTD.ui'
**
** Created: Mon Apr 13 11:59:11 2009
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DLGREF_3SEL2SPIN_QTD_H
#define DLGREF_3SEL2SPIN_QTD_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QSpinBox;

class DlgRef_3Sel2Spin_QTD : public QWidget
{
    Q_OBJECT

public:
    DlgRef_3Sel2Spin_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_3Sel2Spin_QTD();

    QGroupBox* GroupBox1;
    QLineEdit* LineEdit1;
    QLabel* TextLabel1;
    QPushButton* PushButton1;
    QLabel* TextLabel2;
    QLineEdit* LineEdit2;
    QPushButton* PushButton2;
    QLabel* TextLabel3;
    QPushButton* PushButton3;
    QLineEdit* LineEdit3;
    QSpinBox* SpinBox2;
    QLabel* TextLabel5;
    QLabel* TextLabel4;
    QSpinBox* SpinBox1;

protected:
    QGridLayout* DlgRef_3Sel2Spin_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QSpacerItem* Spacer5;
    QGridLayout* Layout2;

protected slots:
    virtual void languageChange();

};

#endif // DLGREF_3SEL2SPIN_QTD_H
