//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : OperationGUI_PartitionDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_PARTITION_H
#define DIALOGBOX_PARTITION_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_4Sel1List_QTD.h"

#include "OperationGUI.h"

//=================================================================================
// class    : OperationGUI_PartitionDlg
// purpose  :
//=================================================================================
class OperationGUI_PartitionDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    OperationGUI_PartitionDlg(QWidget* parent = 0, const char* name = 0, OperationGUI* theOperationGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~OperationGUI_PartitionDlg();

private:
    void Init();
    void enterEvent(QEvent* e);

    OperationGUI* myOperationGUI;

    GEOM::GEOM_Gen::ListOfIOR myListShapes;
    GEOM::GEOM_Gen::ListOfIOR myListTools;
    GEOM::GEOM_Gen::ListOfIOR myListRemoveInside;
    GEOM::GEOM_Gen::ListOfIOR myListKeepInside;

    bool myOkListShapes;   /* to check when argument is defined */
    bool myOkListTools;    /* to check when argument is defined */
    bool myOkKeepShape;    /* to check when argument is defined */
    bool myOkRemoveShape;  /* to check when argument is defined */
    int myLimit;

    DlgRef_4Sel1List_QTD* GroupPoints;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ComboTextChanged();

};

#endif // DIALOGBOX_PARTITION_H
