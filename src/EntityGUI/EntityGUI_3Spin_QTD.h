/****************************************************************************
** Form interface generated from reading ui file 'EntityGUI_3Spin_QTD.ui'
**
** Created: Fri Jul 30 16:06:00 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ENTITYGUI_3SPIN_QTD_H
#define ENTITYGUI_3SPIN_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QPushButton;
class QSpinBox;

class EntityGUI_3Spin_QTD : public QWidget
{ 
    Q_OBJECT

public:
    EntityGUI_3Spin_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~EntityGUI_3Spin_QTD();

    QGroupBox* GroupBox1;
    QPushButton* buttonApply;
    QPushButton* buttonUndo;
    QPushButton* buttonRedo;
    QLabel* TextLabel2;
    QLabel* TextLabel1;
    QSpinBox* SpinBox1;
    QSpinBox* SpinBox2;
    QLabel* TextLabel3;
    QSpinBox* SpinBox3;


protected:
    QGridLayout* EntityGUI_3Spin_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout4;
    QGridLayout* Layout3;
    QGridLayout* Layout2;
    QGridLayout* Layout1;
};

#endif // ENTITYGUI_3SPIN_QTD_H
