/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_3Check_QTD.ui'
**
** Created: Tue Aug 24 11:27:47 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DLGREF_3CHECK_QTD_H
#define DLGREF_3CHECK_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QRadioButton;

class DlgRef_3Check_QTD : public QWidget
{ 
    Q_OBJECT

public:
    DlgRef_3Check_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_3Check_QTD();

    QButtonGroup* GroupBox1;
    QRadioButton* RadioButton3;
    QRadioButton* RadioButton1;
    QRadioButton* RadioButton2;


protected:
    QGridLayout* DlgRef_3Check_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
};

#endif // DLGREF_3CHECK_QTD_H
