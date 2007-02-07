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
** Form interface generated from reading ui file 'DlgRef_Skeleton_QTD.ui'
**
** Created: Fri Mar 17 11:18:06 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DLGREF_SKELETON_QTD_H
#define DLGREF_SKELETON_QTD_H

#include "GEOM_DlgRef.hxx"

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QPushButton;
class QButtonGroup;
class QRadioButton;
class QLineEdit;
class QLabel;

class GEOM_DLGREF_EXPORT DlgRef_Skeleton_QTD : public QDialog
{
    Q_OBJECT

public:
    DlgRef_Skeleton_QTD( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgRef_Skeleton_QTD();

    QGroupBox* GroupMedium;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonApply;
    QPushButton* buttonCancel;
    QPushButton* buttonHelp;
    QButtonGroup* GroupConstructors;
    QRadioButton* RadioButton1;
    QRadioButton* RadioButton2;
    QRadioButton* RadioButton3;
    QRadioButton* RadioButton4;
    QGroupBox* GroupBoxName;
    QLineEdit* ResultName;
    QLabel* NameLabel;

protected:
    QGridLayout* DlgRef_Skeleton_QTDLayout;
    QGridLayout* Layout1;
    QHBoxLayout* GroupButtonsLayout;
    QHBoxLayout* Layout3;
    QSpacerItem* Spacer1;
    QHBoxLayout* GroupConstructorsLayout;
    QHBoxLayout* Layout2;
    QGridLayout* GroupBoxNameLayout;
    QGridLayout* Layout66;

protected slots:
    virtual void languageChange();

};

#endif // DLGREF_SKELETON_QTD_H
