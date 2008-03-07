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
** Form interface generated from reading ui file 'DlgRef_1Sel1Spin1Check_QTD.ui'
**
** Created: Mon May 24 15:59:03 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DLGREF_1SEL1SPIN1CHECK_QTD_H
#define DLGREF_1SEL1SPIN1CHECK_QTD_H

#include "GEOM_DlgRef.hxx"

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;

class GEOM_DLGREF_EXPORT DlgRef_1Sel1Spin1Check_QTD : public QWidget
{ 
    Q_OBJECT

public:
    DlgRef_1Sel1Spin1Check_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_1Sel1Spin1Check_QTD();

    QGroupBox* GroupBox1;
    QSpinBox* SpinBox1;
    QLabel* TextLabel2;
    QLabel* TextLabel1;
    QLineEdit* LineEdit1;
    QPushButton* PushButton1;
    QCheckBox* CheckButton1;


protected:
    QGridLayout* DlgRef_1Sel1Spin1Check_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout8;
    QGridLayout* Layout2;
};

#endif // DLGREF_1SEL1SPIN1CHECK_QTD_H
