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
** Form interface generated from reading ui file 'MeasureGUI_1Sel1TextView_QTD.ui'
**
** Created: mar oct 28 14:01:14 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef MEASUREGUI_1SEL1TEXTVIEW_QTD_H
#define MEASUREGUI_1SEL1TEXTVIEW_QTD_H

#include "GEOM_MeasureGUI.hxx"

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;

class GEOM_MEASUREGUI_EXPORT MeasureGUI_1Sel1TextView_QTD : public QWidget
{ 
    Q_OBJECT

public:
    MeasureGUI_1Sel1TextView_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~MeasureGUI_1Sel1TextView_QTD();

    QGroupBox* GroupBox1;
    QLabel* TextLabel1;
    QTextEdit* TextEdit1;
    QLineEdit* LineEdit1;
    QPushButton* PushButton1;


protected:
    QGridLayout* MeasureGUI_1Sel1TextView_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
};

#endif // MEASUREGUI_1SEL1TEXTVIEW_QTD_H
