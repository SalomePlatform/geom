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
//  File   : OperationGUI_FilletDlg.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_FILLET_H
#define DIALOGBOX_FILLET_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel1Spin.h"

#include "OperationGUI.h"

//=================================================================================
// class    : OperationGUI_FilletDlg
// purpose  :
//=================================================================================
class OperationGUI_FilletDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    OperationGUI_FilletDlg(QWidget* parent = 0, const char* name = 0, OperationGUI* theOperationGUI = 0, SALOME_Selection* Sel = 0, Handle(AIS_InteractiveContext) ic = 0, bool modal = FALSE, WFlags fl = 0);
    ~OperationGUI_FilletDlg();

private :
    void Init(Handle(AIS_InteractiveContext) ic);
    void enterEvent(QEvent* e);
    void closeEvent(QCloseEvent* e);
    void MakeFilletSimulationAndDisplay();

    void ResetStateOfDialog();
    void MakePreview();

    OperationGUI* myOperationGUI;

    int myConstructorId;   /* Current constructor id = radio button id */

    /* Interactive and local context management see also : bool myUseLocalContext() */
    Handle(AIS_InteractiveContext) myIC;   /* Interactive context */ 
    Standard_Integer myLocalContextId;   /* identify a local context used by this method */
    bool myUseLocalContext;   /* true when this method as opened a local context  */

    TopoDS_Shape myShape;
    bool myOkShape;
    char* myShapeIOR;
    int myShapeType;
    double myRadius;
    
    DlgRef_1Sel1Spin* Group1;
    DlgRef_1Sel1Spin* Group2;
    DlgRef_1Sel1Spin* Group3;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void ClickOnCancel();
    void ActivateThisDialog();
    void DeactivateActiveDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ValueChangedInSpinBox(double newValue);
    void ConstructorsClicked(int constructorId);

};

#endif // DIALOGBOX_FILLET_H

