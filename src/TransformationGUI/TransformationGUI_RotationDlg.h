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
//  File   : TransformationGUI_RotationDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_ROTATION_H
#define DIALOGBOX_ROTATION_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel1Spin1Check.h"

#include "TransformationGUI.h"

#include "GEOM_ShapeTypeFilter.hxx"
#include <gp_Dir.hxx>

//=================================================================================
// class    : TransformationGUI_RotationDlg
// purpose  :
//=================================================================================
class TransformationGUI_RotationDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    TransformationGUI_RotationDlg(QWidget* parent = 0, const char* name = 0, TransformationGUI* theTransformationGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~TransformationGUI_RotationDlg();

private :
    void Init();
    void enterEvent(QEvent* e);
    void MakeRotationSimulationAndDisplay();

    TransformationGUI* myTransformationGUI;

    Handle(GEOM_ShapeTypeFilter) myEdgeFilter;   /* Filters selection */

    TopoDS_Shape myBase;
    GEOM::GEOM_Shape_var myGeomShape;   /* is myBase */
    gp_Pnt myLoc;
    gp_Dir myDir;    
    Standard_Real myAngle;
    bool myOkBase; 
    bool myOkAxis;

    DlgRef_2Sel1Spin1Check* GroupPoints;

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

#endif // DIALOGBOX_ROTATION_H
