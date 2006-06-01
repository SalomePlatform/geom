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
** Form interface generated from reading ui file 'DlgRef_3Check_QTD.ui'
**
** Created: Tue Aug 24 11:27:47 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DLGREF_3CHECK_QTD_H
#define DLGREF_3CHECK_QTD_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QRadioButton;

#if defined WNT && defined WIN32 && defined SALOME_WNT_EXPORTS
#define DLGREF_WNT_EXPORT __declspec( dllexport )
#else
#define DLGREF_WNT_EXPORT
#endif

class DLGREF_WNT_EXPORT DlgRef_3Check_QTD : public QWidget
{ 
    Q_OBJECT

public:
    DlgRef_3Check_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_3Check_QTD();

    QButtonGroup* GroupBox1;
    QRadioButton* RadioButton3;
    QRadioButton* RadioButton1;
    QRadioButton* RadioButton2;


protected:
    QGridLayout* DlgRef_3Check_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
};

#endif // DLGREF_3CHECK_QTD_H
