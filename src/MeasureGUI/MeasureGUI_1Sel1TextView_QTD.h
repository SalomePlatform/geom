/****************************************************************************
** Form interface generated from reading ui file 'MeasureGUI_1Sel1TextView_QTD.ui'
**
** Created: mar oct 28 14:01:14 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef MEASUREGUI_1SEL1TEXTVIEW_QTD_H
#define MEASUREGUI_1SEL1TEXTVIEW_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;

class MeasureGUI_1Sel1TextView_QTD : public QWidget
{ 
    Q_OBJECT

public:
    MeasureGUI_1Sel1TextView_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~MeasureGUI_1Sel1TextView_QTD();

    QGroupBox* GroupBox1;
    QLabel* TextLabel1;
    QTextEdit* TextEdit1;
    QLineEdit* LineEdit1;
    QPushButton* PushButton1;


protected:
    QGridLayout* MeasureGUI_1Sel1TextView_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
};

#endif // MEASUREGUI_1SEL1TEXTVIEW_QTD_H
