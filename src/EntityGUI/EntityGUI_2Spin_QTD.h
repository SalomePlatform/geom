/****************************************************************************
** Form interface generated from reading ui file 'EntityGUI_2Spin_QTD.ui'
**
** Created: Fri Jul 30 16:06:00 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ENTITYGUI_2SPIN_QTD_H
#define ENTITYGUI_2SPIN_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QPushButton;
class QSpinBox;

class EntityGUI_2Spin_QTD : public QWidget
{ 
    Q_OBJECT

public:
    EntityGUI_2Spin_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~EntityGUI_2Spin_QTD();

    QGroupBox* GroupBox1;
    QLabel* TextLabel2;
    QLabel* TextLabel1;
    QSpinBox* SpinBox1;
    QSpinBox* SpinBox2;
    QPushButton* buttonUndo;
    QPushButton* buttonRedo;
    QPushButton* buttonApply;


protected:
    QGridLayout* EntityGUI_2Spin_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout4;
    QGridLayout* Layout1;
    QGridLayout* Layout3;
    QGridLayout* Layout2;
};

#endif // ENTITYGUI_2SPIN_QTD_H
