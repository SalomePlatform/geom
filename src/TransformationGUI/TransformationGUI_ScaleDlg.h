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
//  File   : TransformationGUI_ScaleDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_SCALE_H
#define DIALOGBOX_SCALE_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel1Spin.h"

#include "TransformationGUI.h"

#include "GEOM_ShapeTypeFilter.hxx"

//=================================================================================
// class    : TransformationGUI_ScaleDlg
// purpose  :
//=================================================================================
class TransformationGUI_ScaleDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    TransformationGUI_ScaleDlg(QWidget* parent = 0, const char* name = 0, TransformationGUI* theTransformationGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~TransformationGUI_ScaleDlg();

private :
    void Init();
    void enterEvent(QEvent* e);
    void MakeScaleSimulationAndDisplay();

    TransformationGUI* myTransformationGUI;

    Handle(GEOM_ShapeTypeFilter) myVertexFilter;

    gp_Pnt myPoint1;   /* Points containing the vector */
    bool myOkPoint1;   /* true when myPoint1 is defined */
    TopoDS_Shape myBaseTopo;
    bool myOkBaseTopo;   /* true when myBaseTopo is defined */
    GEOM::GEOM_Shape_var myGeomShape;   /* is myBaseTopo */
    Standard_Real myFactor;

    DlgRef_2Sel1Spin* GroupPoints;

private slots :
    void ClickOnOk();
    void ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ValueChangedInSpinBox(double newValue);

};

#endif // DIALOGBOX_SCALE_H
