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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : BasicGUI_WorkingPlaneDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#ifndef DIALOGBOX_WORKINGPLANE_H
#define DIALOGBOX_WORKINGPLANE_H

#include "GEOM_BasicGUI.hxx"

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel_QTD.h"
#include "DlgRef_2Sel_QTD.h"
#include "DlgRef_3Check_QTD.h"

#include <gp_Ax3.hxx>

class QCheckBox;

//=================================================================================
// class    : BasicGUI_WorkingPlaneDlg
// purpose  :
//=================================================================================
class GEOM_BASICGUI_EXPORT BasicGUI_WorkingPlaneDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BasicGUI_WorkingPlaneDlg( GeometryGUI* theGeometryGUI, QWidget* parent = 0,
			      const char* name = 0, bool modal = FALSE, Qt::WindowFlags fl = 0 );
    ~BasicGUI_WorkingPlaneDlg();
    virtual void closeEvent( QCloseEvent* e );

private:
    void Init();
    void enterEvent(QEvent* e);

    bool updateWPlane(const bool showPreview = true);

    GEOM::GEOM_Object_var myFace;
    GEOM::GEOM_Object_var myVectX;
    GEOM::GEOM_Object_var myVectZ;

    int aOriginType;

    Ui::DlgRef_1Sel_QTD* Group1;
    Ui::DlgRef_2Sel_QTD* Group2;
    Ui::DlgRef_3Check_QTD* Group3;

    QCheckBox* myReverseCB;

    gp_Ax3 myWPlane;

private slots:
    void ClickOnOk();
    void ClickOnCancel();
    bool ClickOnApply();

    void ConstructorsClicked(int constructorId);
    void GroupClicked(int groupId);
    void SetEditCurrentArgument();
    void SelectionIntoArgument();
    void LineEditReturnPressed();
    void onReverse();

    void ActivateThisDialog();
    void DeactivateActiveDialog();

};

#endif // DIALOGBOX_WORKINGPLANE_H
