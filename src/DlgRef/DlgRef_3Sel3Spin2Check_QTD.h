/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_3Sel3Spin2Check_QTD.ui'
**
** Created: Tue Feb 12 16:21:38 2008
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
class QPushButton;
class QLabel;
class QCheckBox;
class QSpinBox;
class QLineEdit;

class DlgRef_3Sel3Spin2Check_QTD : public QWidget
{
    Q_OBJECT

public:
    DlgRef_3Sel3Spin2Check_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_3Sel3Spin2Check_QTD();

    QGroupBox* GroupBox1;
    QPushButton* PushButton1;
    QLabel* TextLabel3;
    QCheckBox* CheckBox1;
    QLabel* TextLabel6;
    QSpinBox* SpinBox_2;
    QLineEdit* LineEdit2;
    QLineEdit* LineEdit1;
    QLineEdit* LineEdit3;
    QPushButton* PushButton2;
    QLabel* TextLabel2;
    QCheckBox* CheckBox2;
    QLabel* TextLabel1;
    QPushButton* PushButton3;
    QLabel* TextLabel5;
    QSpinBox* SpinBox_1;
    QSpinBox* SpinBox_3;
    QLabel* TextLabel4;

protected:
    QVBoxLayout* DlgRef_3Sel3Spin2Check_QTDLayout;
    QHBoxLayout* GroupBox1Layout;
    QGridLayout* layout1;
    QSpacerItem* Spacer3;

protected slots:
    virtual void languageChange();

};

#endif // DLGREF_3SEL3SPIN2CHECK_QTD_H
