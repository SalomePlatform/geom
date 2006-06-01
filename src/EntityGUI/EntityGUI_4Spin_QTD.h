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
** Form interface generated from reading ui file 'EntityGUI_4Spin_QTD.ui'
**
** Created: Fri Jul 30 16:06:00 2004
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
    QPushButton* buttonRedo;
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
    QGridLayout* Layout4;
    QGridLayout* Layout3;
    QGridLayout* Layout2;
    QGridLayout* Layout1;
};

#endif // ENTITYGUI_4SPIN_QTD_H
