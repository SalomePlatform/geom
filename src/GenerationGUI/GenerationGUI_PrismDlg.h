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
//  File   : GenerationGUI_PrismDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_PRISM_H
#define DIALOGBOX_PRISM_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel1Spin1Check.h"

#include "GenerationGUI.h"

#include "GEOM_ShapeTypeFilter.hxx"

//=================================================================================
// class    : GenerationGUI_PrismDlg
// purpose  :
//=================================================================================
class GenerationGUI_PrismDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    GenerationGUI_PrismDlg(QWidget* parent = 0, const char* name = 0, GenerationGUI* theGenerationGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~GenerationGUI_PrismDlg();

private :
    void Init();
    void enterEvent(QEvent* e);
    void MakePrismSimulationAndDisplay();

    GenerationGUI* myGenerationGUI;

    double step;
    Handle(GEOM_ShapeTypeFilter) myEdgeFilter;   /* Filters selection */

    Standard_Real myDx;
    Standard_Real myDy;
    Standard_Real myDz;
    Standard_Real myHeight;   /* Height used for prism (extrusion) */
    TopoDS_Shape myBaseTopo;  /* topology used as base of prism */  
    GEOM::GEOM_Shape_var myGeomShape;   /* that is myBaseTopo */
    bool myOkBase;
    bool myOkLine;   /* to check when arguments myDx, myDy, myDz are defined */

    DlgRef_2Sel1Spin1Check* GroupPoints;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ReverseVector(int state);
    void ValueChangedInSpinBox(double newValue);

};

#endif // DIALOGBOX_PRISM_H
