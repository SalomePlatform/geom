/****************************************************************************
** Form interface generated from reading ui file 'EntityGUI_1Sel_QTD.ui'
**
** Created: Fri Apr 30 11:23:32 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ENTITYGUI_1SEL_QTD_H
#define ENTITYGUI_1SEL_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

class EntityGUI_1Sel_QTD : public QWidget
{ 
    Q_OBJECT

public:
    EntityGUI_1Sel_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~EntityGUI_1Sel_QTD();

    QGroupBox* GroupBox1;
    QPushButton* buttonUndo;
    QPushButton* buttonApply;
    QLabel* TextLabel1;
    QPushButton* PushButton1;
    QLineEdit* LineEdit1;


protected:
    QGridLayout* EntityGUI_1Sel_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout3;
    QGridLayout* Layout2;
    QGridLayout* Layout1;
};

#endif // ENTITYGUI_1SEL_QTD_H
