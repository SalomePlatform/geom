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
** Form interface generated from reading ui file 'DlgRef_Skeleton_QTD.ui'
**
** Created: Wed Aug 11 18:42:51 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DLGREF_SKELETON_QTD_H
#define DLGREF_SKELETON_QTD_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;

#if defined WNT && defined WIN32 && defined SALOME_WNT_EXPORTS
#define DLGREF_WNT_EXPORT __declspec( dllexport )
#else
#define DLGREF_WNT_EXPORT
#endif

class DLGREF_WNT_EXPORT DlgRef_Skeleton_QTD : public QDialog
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
    QHBoxLayout* GroupConstructorsLayout;
    QHBoxLayout* Layout2;
    QGridLayout* GroupBoxNameLayout;
    QGridLayout* Layout66;
};

#endif // DLGREF_SKELETON_QTD_H
