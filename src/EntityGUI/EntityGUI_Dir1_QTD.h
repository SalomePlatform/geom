/****************************************************************************
** Form interface generated from reading ui file 'EntityGUI_Dir1_QTD.ui'
**
** Created: ven déc 12 11:17:09 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ENTITYGUI_DIR1_QTD_H
#define ENTITYGUI_DIR1_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QRadioButton;

class EntityGUI_Dir1_QTD : public QWidget
{ 
    Q_OBJECT

public:
    EntityGUI_Dir1_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~EntityGUI_Dir1_QTD();

    QButtonGroup* GroupDir1;
    QRadioButton* RB_Dir12;
    QRadioButton* RB_Dir13;
    QRadioButton* RB_Dir11;
    QRadioButton* RB_Dir14;


protected:
    QGridLayout* EntityGUI_Dir1_QTDLayout;
    QGridLayout* GroupDir1Layout;
    QGridLayout* Layout1;
};

#endif // ENTITYGUI_DIR1_QTD_H
