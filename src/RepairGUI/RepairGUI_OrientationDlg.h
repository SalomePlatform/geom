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
//  File   : RepairGUI_OrientationDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_ORIENTATION_H
#define DIALOGBOX_ORIENTATION_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel1Spin1Check.h"

#include "RepairGUI.h"

//=================================================================================
// class    : DialogBox_ORIENTATION
// purpose  :
//=================================================================================
class RepairGUI_OrientationDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    RepairGUI_OrientationDlg(QWidget* parent = 0, const char* name = 0, RepairGUI* theRepairGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~RepairGUI_OrientationDlg();

private :
    void Init();
    void enterEvent(QEvent* e);
    void MakeOrientationSimulationAndDisplay();

    RepairGUI* myRepairGUI;

    GEOM::GEOM_Shape_var myGeomShape;   /* is myShape */
    TopoDS_Shape myShape;   /* topology used */
    Standard_Real myLength;   /* to simulate normal vector */
    bool myOkShape;

    DlgRef_1Sel1Spin1Check* GroupPoints;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ReverseAngle(int state);
    void ValueChangedInSpinBox(double newValue);

};

#endif // DIALOGBOX_ORIENTATION_H
