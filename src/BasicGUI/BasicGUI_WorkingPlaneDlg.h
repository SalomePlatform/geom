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
//  File   : BasicGUI_WorkingPlaneDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#ifndef DIALOGBOX_WORKINGPLANE_H
#define DIALOGBOX_WORKINGPLANE_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel_QTD.h"

#include "BasicGUI.h"

#include "GEOM_FaceFilter.hxx"

//=================================================================================
// class    : BasicGUI_WorkingPlaneDlg
// purpose  :
//=================================================================================
class BasicGUI_WorkingPlaneDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BasicGUI_WorkingPlaneDlg( QWidget* parent = 0, const char* name = 0, BasicGUI* theBasicGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~BasicGUI_WorkingPlaneDlg();

private:
    void Init();
    void enterEvent(QEvent* e);

    BasicGUI* myBasicGUI;

    Handle(GEOM_FaceFilter) myFaceFilter;   /* filter for selection */

    gp_Pnt myLoc;
    gp_Dir myDir;
    bool myOkPlane;   /* to check when arguments are defined */

    DlgRef_1Sel_QTD* GroupWPlane;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void SetEditCurrentArgument();
    void SelectionIntoArgument();
    void LineEditReturnPressed();
    void ActivateThisDialog();

};

#endif // DIALOGBOX_WORKINGPLANE_H
