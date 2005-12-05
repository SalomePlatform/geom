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
** Form interface generated from reading ui file 'DlgRef_1Sel2Spin_QTD.ui'
**
** Created: jeu oct 2 11:08:05 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DLGREF_1SEL2SPIN_QTD_H
#define DLGREF_1SEL2SPIN_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;

class DlgRef_1Sel2Spin_QTD : public QWidget
{ 
    Q_OBJECT

public:
    DlgRef_1Sel2Spin_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_1Sel2Spin_QTD();

    QGroupBox* GroupBox1;
    QLabel* TextLabel3;
    QSpinBox* SpinBox1;
    QLabel* TextLabel2;
    QSpinBox* SpinBox2;
    QLineEdit* LineEdit1;
    QPushButton* PushButton1;
    QLabel* TextLabel1;


protected:
    QGridLayout* DlgRef_1Sel2Spin_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
    QGridLayout* Layout2;
};

#endif // DLGREF_1SEL2SPIN_QTD_H
