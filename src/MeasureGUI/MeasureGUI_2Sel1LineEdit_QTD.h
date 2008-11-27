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
** Form interface generated from reading ui file 'MeasureGUI_2Sel1LineEdit_QTD.ui'
**
** Created: Tue Oct 9 14:45:55 2007
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MEASUREGUI_2SEL1LINEEDIT_QTD_H
#define MEASUREGUI_2SEL1LINEEDIT_QTD_H

#include "GEOM_MeasureGUI.hxx"

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class QPushButton;
class QLineEdit;

class GEOM_MEASUREGUI_EXPORT MeasureGUI_2Sel1LineEdit_QTD : public QWidget
{
    Q_OBJECT

public:
    MeasureGUI_2Sel1LineEdit_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~MeasureGUI_2Sel1LineEdit_QTD();

    QGroupBox* GroupBox1;
    QLabel* TextLabel1;
    QPushButton* PushButton1;
    QLineEdit* LineEdit1;
    QLabel* TextLabel2;
    QPushButton* PushButton2;
    QLineEdit* LineEdit2;
    QLabel* TextLabel3;
    QLineEdit* LineEdit3;

protected:
    QGridLayout* MeasureGUI_2Sel1LineEdit_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
    QSpacerItem* Spacer8;

protected slots:
    virtual void languageChange();

};

#endif // MEASUREGUI_2SEL1LINEEDIT_QTD_H
