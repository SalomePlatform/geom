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
