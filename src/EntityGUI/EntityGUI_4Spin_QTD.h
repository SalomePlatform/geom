/****************************************************************************
** Form interface generated from reading ui file 'EntityGUI_4Spin_QTD.ui'
**
** Created: Fri Apr 30 10:56:17 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ENTITYGUI_4SPIN_QTD_H
#define ENTITYGUI_4SPIN_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QPushButton;
class QSpinBox;

class EntityGUI_4Spin_QTD : public QWidget
{ 
    Q_OBJECT

public:
    EntityGUI_4Spin_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~EntityGUI_4Spin_QTD();

    QGroupBox* GroupBox1;
    QPushButton* buttonUndo;
    QPushButton* buttonApply;
    QSpinBox* SpinBox3;
    QSpinBox* SpinBox2;
    QLabel* TextLabel2;
    QSpinBox* SpinBox4;
    QLabel* TextLabel3;
    QSpinBox* SpinBox1;
    QLabel* TextLabel1;
    QLabel* TextLabel4;


protected:
    QGridLayout* EntityGUI_4Spin_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout3;
    QGridLayout* Layout2;
    QGridLayout* Layout1;
};

#endif // ENTITYGUI_4SPIN_QTD_H
