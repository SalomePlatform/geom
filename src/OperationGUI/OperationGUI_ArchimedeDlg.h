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
//  File   : OperationGUI_ArchimedeDlg.h
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_ARCHIMEDE_H
#define DIALOGBOX_ARCHIMEDE_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel3Spin.h"

#include "OperationGUI.h"

//#include "SALOME_InteractiveObject.hxx"

//=================================================================================
// class    : OperationGUI_ArchimedeDlg
// purpose  :
//=================================================================================
class OperationGUI_ArchimedeDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    OperationGUI_ArchimedeDlg(QWidget* parent = 0, const char* name = 0, OperationGUI* theOperationGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~OperationGUI_ArchimedeDlg();

private:
    void Init();
    void enterEvent(QEvent* e);

    double step;

    OperationGUI* myOperationGUI;

    Handle(SALOME_InteractiveObject) myIO; 
    Standard_Real myWeight;
    Standard_Real myWaterDensity;
    Standard_Real myMeshingDeflection;
    bool myOkIO; 

    DlgRef_1Sel3Spin* GroupPoints;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ValueChangedInSpinBox(double newValue);

};

#endif // DIALOGBOX_ARCHIMEDE_H
