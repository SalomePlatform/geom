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
//  File   : MeasureGUI_CenterMassDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#ifndef DIALOGBOX_CMASS_H
#define DIALOGBOX_CMASS_H

#include "GEOMBase_Skeleton.h"
#include "MeasureGUI_1Sel3LineEdit_QTD.h"

#include "MeasureGUI.h"

#include <gp_Pnt.hxx>

//=================================================================================
// class    : MeasureGUI_CenterMassDlg
// purpose  :
//=================================================================================
class MeasureGUI_CenterMassDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    MeasureGUI_CenterMassDlg(QWidget* parent = 0, const char* name = 0, MeasureGUI* theMeasureGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~MeasureGUI_CenterMassDlg();

private:
    void Init();
    void enterEvent(QEvent* e);
    bool CalculateAndDisplayCenterMass();

    MeasureGUI* myMeasureGUI;

    GEOM::GEOM_Shape_var myGeomShape;   /* is myBase */
    TopoDS_Shape myShape;  /* Shape argument */
    gp_Pnt myCenterMass;
    bool myOkCenterMass;   /* true after center of mass simulation calculation */

    MeasureGUI_1Sel3LineEdit_QTD* GroupC1;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void SetEditCurrentArgument();
    void SelectionIntoArgument();
    void LineEditReturnPressed();
    void ActivateThisDialog();

};

#endif // DIALOGBOX_CMASS_H
