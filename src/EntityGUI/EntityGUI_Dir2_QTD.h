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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
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
