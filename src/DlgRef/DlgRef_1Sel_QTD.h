/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_1Sel_QTD.ui'
**
** Created: lun sep 22 17:38:05 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DLGREF_1SEL_QTD_H
#define DLGREF_1SEL_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

class DlgRef_1Sel_QTD : public QWidget
{ 
    Q_OBJECT

public:
    DlgRef_1Sel_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_1Sel_QTD();

    QGroupBox* GroupBox1;
    QPushButton* PushButton1;
    QLineEdit* LineEdit1;
    QLabel* TextLabel1;


protected:
    QGridLayout* DlgRef_1Sel_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
};

#endif // DLGREF_1SEL_QTD_H
