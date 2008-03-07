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
** Form interface generated from reading ui file 'DlgRef_1Sel_QTD.ui'
**
** Created: lun sep 22 17:38:05 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DlgRef_1Sel_Ext_H
#define DlgRef_1Sel_Ext_H

#include "GEOM_DlgRef.hxx"

#include "DlgRef_1Sel_QTD.h"

class QGridLayout;

class GEOM_DLGREF_EXPORT DlgRef_1Sel_Ext : public DlgRef_1Sel_QTD
{ 
    Q_OBJECT

public:
    DlgRef_1Sel_Ext( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 )
    	: DlgRef_1Sel_QTD( parent, name, fl ) {};
    ~DlgRef_1Sel_Ext() {};

    QGridLayout* getGroupBoxLayout() { return GroupBox1Layout; }
};

#endif // DlgRef_1Sel_Ext_H
