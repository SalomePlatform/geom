/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_2Spin_QTD.ui'
**
** Created: jeu sep 25 12:10:30 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DLGREF_2SPIN_QTD_H
#define DLGREF_2SPIN_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QSpinBox;

class DlgRef_2Spin_QTD : public QWidget
{ 
    Q_OBJECT

public:
    DlgRef_2Spin_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_2Spin_QTD();

    QGroupBox* GroupBox1;
    QSpinBox* SpinBox2;
    QSpinBox* SpinBox1;
    QLabel* TextLabel1;
    QLabel* TextLabel2;


protected:
    QGridLayout* DlgRef_2Spin_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
};

#endif // DLGREF_2SPIN_QTD_H
