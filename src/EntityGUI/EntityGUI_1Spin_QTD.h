/****************************************************************************
** Form interface generated from reading ui file 'EntityGUI_1Spin_QTD.ui'
**
** Created: Fri Jul 30 16:05:59 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ENTITYGUI_1SPIN_QTD_H
#define ENTITYGUI_1SPIN_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QPushButton;
class QSpinBox;

class EntityGUI_1Spin_QTD : public QWidget
{ 
    Q_OBJECT

public:
    EntityGUI_1Spin_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~EntityGUI_1Spin_QTD();

    QGroupBox* GroupBox1;
    QLabel* TextLabel1;
    QSpinBox* SpinBox1;
    QPushButton* buttonRedo;
    QPushButton* buttonUndo;
    QPushButton* buttonApply;


protected:
    QGridLayout* EntityGUI_1Spin_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout4;
    QGridLayout* Layout1;
    QGridLayout* Layout3;
    QGridLayout* Layout2;
};

#endif // ENTITYGUI_1SPIN_QTD_H
