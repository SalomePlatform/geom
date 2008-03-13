/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_3Sel3Spin2Check_QTD.ui'
**
** Created: Wed Mar 12 11:57:09 2008
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DLGREF_3SEL3SPIN2CHECK_QTD_H
#define DLGREF_3SEL3SPIN2CHECK_QTD_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class QSpinBox;
class QLineEdit;
class QPushButton;
class QCheckBox;

class DlgRef_3Sel3Spin2Check_QTD : public QWidget
{
    Q_OBJECT

public:
    DlgRef_3Sel3Spin2Check_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_3Sel3Spin2Check_QTD();

    QGroupBox* GroupBox1;
    QLabel* TextLabel2;
    QSpinBox* SpinBox_3;
    QLabel* TextLabel3;
    QLineEdit* LineEdit2;
    QPushButton* PushButton2;
    QLabel* TextLabel6;
    QPushButton* PushButton1;
    QCheckBox* CheckBox1;
    QSpinBox* SpinBox_2;
    QLabel* TextLabel5;
    QLabel* TextLabel1;
    QLabel* TextLabel4;
    QCheckBox* CheckBox2;
    QLineEdit* LineEdit1;
    QLineEdit* LineEdit3;
    QPushButton* PushButton3;
    QSpinBox* SpinBox_1;

protected:
    QVBoxLayout* DlgRef_3Sel3Spin2Check_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* layout1;
    QSpacerItem* Spacer3;

protected slots:
    virtual void languageChange();

};

#endif // DLGREF_3SEL3SPIN2CHECK_QTD_H
