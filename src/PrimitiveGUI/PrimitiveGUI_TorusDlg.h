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
//  File   : PrimitiveGUI_TorusDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_TORUS_H
#define DIALOGBOX_TORUS_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel2Spin.h"
#include "DlgRef_2Spin.h"

#include "PrimitiveGUI.h"

#include "GEOM_EdgeFilter.hxx"
#include "GEOM_ShapeTypeFilter.hxx"

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

//=================================================================================
// class    : PrimitiveGUI_TorusDlg
// purpose  :
//=================================================================================
class PrimitiveGUI_TorusDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    PrimitiveGUI_TorusDlg(QWidget* parent = 0, const char* name = 0, PrimitiveGUI* thePrimitiveGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~PrimitiveGUI_TorusDlg();

private:
    void Init();
    void enterEvent(QEvent* e);
    void MakeTorusSimulationAndDisplay();

    PrimitiveGUI* myPrimitiveGUI;

    double step;
    int myConstructorId;
    Handle(GEOM_ShapeTypeFilter) myVertexFilter;
    Handle(GEOM_EdgeFilter) myEdgeFilter;  /* Filter selection */

    gp_Pnt myPoint1;   /* topology used  */
    gp_Dir myDir;
    bool myOkPoint1;   /* to check when arguments is defined */
    bool myOkDir;

    Standard_Real myRadius1;
    Standard_Real myRadius2;

    DlgRef_2Sel2Spin* GroupPoints;
    DlgRef_2Spin* GroupDimensions;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ConstructorsClicked(int constructorId);
    void ValueChangedInSpinBox(double newValue);

};

#endif // DIALOGBOX_TORUS_H
