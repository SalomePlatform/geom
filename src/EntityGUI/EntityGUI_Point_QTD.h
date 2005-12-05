// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
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
