/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_2Sel_QTD.ui'
**
** Created: mar sep 23 16:05:09 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DLGREF_2SEL_QTD_H
#define DLGREF_2SEL_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

class DlgRef_2Sel_QTD : public QWidget
{ 
    Q_OBJECT

public:
    DlgRef_2Sel_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_2Sel_QTD();

    QGroupBox* GroupBox1;
    QPushButton* PushButton1;
    QLineEdit* LineEdit2;
    QPushButton* PushButton2;
    QLabel* TextLabel1;
    QLabel* TextLabel2;
    QLineEdit* LineEdit1;


protected:
    QGridLayout* DlgRef_2Sel_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
};

#endif // DLGREF_2SEL_QTD_H
