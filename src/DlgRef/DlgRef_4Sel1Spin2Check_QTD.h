//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_4Sel1Spin2Check_QTD.ui'
**
** Created: Wed Oct 25 20:32:41 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DLGREF_4SEL1SPIN2CHECK_QTD_H
#define DLGREF_4SEL1SPIN2CHECK_QTD_H

#include "GEOM_DlgRef.hxx"

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QCheckBox;
class QPushButton;
class QLineEdit;
class QLabel;
class QSpinBox;

class GEOM_DLGREF_EXPORT DlgRef_4Sel1Spin2Check_QTD : public QWidget
{
    Q_OBJECT

public:
    DlgRef_4Sel1Spin2Check_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_4Sel1Spin2Check_QTD();

    QGroupBox* GroupBox1;
    QCheckBox* CheckButton2;
    QPushButton* PushButton2;
    QLineEdit* LineEdit5;
    QLabel* TextLabel5;
    QLabel* TextLabel1;
    QLineEdit* LineEdit1;
    QSpinBox* SpinBox1;
    QCheckBox* CheckButton1;
    QLabel* TextLabel2;
    QPushButton* PushButton5;
    QPushButton* PushButton4;
    QLabel* TextLabel4;
    QPushButton* PushButton1;
    QLineEdit* LineEdit4;
    QLineEdit* LineEdit2;
    QLabel* TextLabel3;

protected:
    QVBoxLayout* DlgRef_4Sel1Spin2Check_QTDLayout;
    QVBoxLayout* GroupBox1Layout;
    QGridLayout* Layout1;

protected slots:
    virtual void languageChange();

};

#endif // DLGREF_4SEL1SPIN2CHECK_QTD_H
