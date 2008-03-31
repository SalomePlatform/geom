/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_2Sel2List_QTD.ui'
**
** Created: Thu Mar 27 16:38:25 2008
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DLGREF_2SEL2LIST_QTD_H
#define DLGREF_2SEL2LIST_QTD_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLineEdit;
class QComboBox;
class QLabel;
class QPushButton;

class DlgRef_2Sel2List_QTD : public QWidget
{
    Q_OBJECT

public:
    DlgRef_2Sel2List_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_2Sel2List_QTD();

    QGroupBox* GroupBox1;
    QLineEdit* LineEdit2;
    QComboBox* ComboBox2;
    QLabel* TextLabel4;
    QComboBox* ComboBox1;
    QLabel* TextLabel3;
    QLineEdit* LineEdit1;
    QLabel* TextLabel2;
    QLabel* TextLabel1;
    QPushButton* PushButton1;
    QPushButton* PushButton2;

protected:
    QVBoxLayout* DlgRef_2Sel2List_QTDLayout;
    QGridLayout* GroupBox1Layout;

protected slots:
    virtual void languageChange();

};

#endif // DLGREF_2SEL2LIST_QTD_H
