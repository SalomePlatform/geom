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
** Form interface generated from reading ui file 'DlgRef_2Sel2List_QTD.ui'
**
** Created: Thu Mar 27 16:38:25 2008
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DLGREF_2SEL2LIST_QTD_H
#define DLGREF_2SEL2LIST_QTD_H

#include "GEOM_DlgRef.hxx"

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLineEdit;
class QComboBox;
class QLabel;
class QPushButton;

class GEOM_DLGREF_EXPORT DlgRef_2Sel2List_QTD : public QWidget
{
    Q_OBJECT

public:
    DlgRef_2Sel2List_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_2Sel2List_QTD();

    QGroupBox* GroupBox1;
    QLineEdit* LineEdit2;
    QComboBox* ComboBox2;
    QLabel* TextLabel4;
    QComboBox* ComboBox1;
    QLabel* TextLabel3;
    QLineEdit* LineEdit1;
    QLabel* TextLabel2;
    QLabel* TextLabel1;
    QPushButton* PushButton1;
    QPushButton* PushButton2;

protected:
    QVBoxLayout* DlgRef_2Sel2List_QTDLayout;
    QGridLayout* GroupBox1Layout;

protected slots:
    virtual void languageChange();

};

#endif // DLGREF_2SEL2LIST_QTD_H
