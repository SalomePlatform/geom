/****************************************************************************
** Form interface generated from reading ui file 'EntityGUI_Point_QTD.ui'
**
** Created: ven déc 12 11:17:08 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ENTITYGUI_POINT_QTD_H
#define ENTITYGUI_POINT_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QRadioButton;

class EntityGUI_Point_QTD : public QWidget
{ 
    Q_OBJECT

public:
    EntityGUI_Point_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~EntityGUI_Point_QTD();

    QButtonGroup* GroupPoint;
    QRadioButton* RB_Point2;
    QRadioButton* RB_Point1;
    QRadioButton* RB_Point3;


protected:
    QGridLayout* EntityGUI_Point_QTDLayout;
    QGridLayout* GroupPointLayout;
    QGridLayout* Layout1;
};

#endif // ENTITYGUI_POINT_QTD_H
