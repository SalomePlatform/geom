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
//  File   : GenerationGUI_FillingDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_FILLING_H
#define DIALOGBOX_FILLING_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel5Spin.h"

#include "GenerationGUI.h"

#include "GEOM_ShapeTypeFilter.hxx"

//=================================================================================
// class    : GenerationGUI_FillingDlg
// purpose  :
//=================================================================================
class GenerationGUI_FillingDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    GenerationGUI_FillingDlg(QWidget* parent = 0, const char* name = 0, GenerationGUI* theGenerationGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~GenerationGUI_FillingDlg();

private:
    void Init();
    void enterEvent(QEvent* e);
    void MakeFillingSimulationAndDisplay();

    GenerationGUI* myGenerationGUI;

    Handle(GEOM_ShapeTypeFilter) myCompoundFilter;  /* Filter selection */

    GEOM::GEOM_Shape_var myGeomShape; /* is mySectionShape */
    TopoDS_Shape mySectionShape;
    Standard_Integer myMinDeg;
    Standard_Integer myMaxDeg;
    Standard_Real myTol3D;
    Standard_Real myTol2D;
    Standard_Integer myNbIter;
    bool myOkSectionShape;         /* to check when arguments is defined */

    DlgRef_1Sel5Spin* GroupPoints;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ValueChangedInSpinBox(double newValue);

};

#endif // DIALOGBOX_FILLING_H
