/****************************************************************************
** Form interface generated from reading ui file 'UIFiles/DlgRef_3Sel1Check_QTD.ui'
**
** Created: Wed Feb 21 10:48:39 2007
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DLGREF_3SEL1CHECK_QTD_H
#define DLGREF_3SEL1CHECK_QTD_H

#include "GEOM_DlgRef.hxx"

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QPushButton;
class QLineEdit;
class QLabel;
class QRadioButton;

class GEOM_DLGREF_EXPORT DlgRef_3Sel1Check_QTD : public QWidget
{
    Q_OBJECT

public:
    DlgRef_3Sel1Check_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_3Sel1Check_QTD();

    QGroupBox* GroupBox1;
    QPushButton* PushButton1;
    QLineEdit* LineEdit1;
    QLabel* TextLabel1;
    QPushButton* PushButton2;
    QLineEdit* LineEdit2;
    QLabel* TextLabel2;
    QPushButton* PushButton3;
    QLineEdit* LineEdit3;
    QLabel* TextLabel3;
    QRadioButton* radioButton4;

protected:
    QGridLayout* DlgRef_3Sel1Check_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QSpacerItem* Spacer3;

protected slots:
    virtual void languageChange();

};

#endif // DLGREF_3SEL1CHECK_QTD_H
