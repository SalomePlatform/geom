/****************************************************************************
** Form interface generated from reading ui file 'EntityGUI_Dir2_QTD.ui'
**
** Created: ven déc 12 11:17:09 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ENTITYGUI_DIR2_QTD_H
#define ENTITYGUI_DIR2_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QRadioButton;

class EntityGUI_Dir2_QTD : public QWidget
{ 
    Q_OBJECT

public:
    EntityGUI_Dir2_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~EntityGUI_Dir2_QTD();

    QButtonGroup* GroupDir2;
    QRadioButton* RB_Dir22;
    QRadioButton* RB_Dir23;
    QRadioButton* RB_Dir21;


protected:
    QGridLayout* EntityGUI_Dir2_QTDLayout;
    QGridLayout* GroupDir2Layout;
    QGridLayout* Layout1;
};

#endif // ENTITYGUI_DIR2_QTD_H
