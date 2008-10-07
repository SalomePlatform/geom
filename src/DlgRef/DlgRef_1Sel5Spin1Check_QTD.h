/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_1Sel5Spin1Check_QTD.ui'
**
** Created: Fri Feb 1 14:04:51 2008
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DLGREF_1SEL5SPIN1CHECK_QTD_H
#define DLGREF_1SEL5SPIN1CHECK_QTD_H

#include "GEOM_DlgRef.hxx"

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QLineEdit;

class GEOM_DLGREF_EXPORT DlgRef_1Sel5Spin1Check_QTD : public QWidget
{
    Q_OBJECT

public:
    DlgRef_1Sel5Spin1Check_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_1Sel5Spin1Check_QTD();

    QGroupBox* GroupBox1;
    QLabel* TextLabel1;
    QPushButton* PushButton1;
    QSpinBox* SpinBox2;
    QSpinBox* SpinBox3;
    QLabel* TextLabel5;
    QLabel* TextLabel3;
    QLabel* TextLabel4;
    QSpinBox* SpinBox4;
    QSpinBox* SpinBox1;
    QLabel* TextLabel6;
    QLabel* TextLabel2;
    QSpinBox* SpinBox5;
    QCheckBox* CheckBox1;
    QLineEdit* LineEdit1;

protected:
    QGridLayout* DlgRef_1Sel5Spin1Check_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
    QSpacerItem* Spacer6;
    QGridLayout* Layout2;

protected slots:
    virtual void languageChange();

};

#endif // DLGREF_1SEL5SPIN1CHECK_QTD_H
