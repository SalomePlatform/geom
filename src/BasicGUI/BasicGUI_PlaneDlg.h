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
//  File   : BasicGUI_PlaneDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#ifndef DIALOGBOX_PLANE_H
#define DIALOGBOX_PLANE_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel1Spin.h"
#include "DlgRef_2Sel1Spin.h"
#include "DlgRef_3Sel1Spin.h"

#if defined WNT && defined WIN32 && defined SALOME_WNT_EXPORTS
#define BASICGUI_WNT_EXPORT __declspec( dllexport )
#else
#define BASICGUI_WNT_EXPORT
#endif

//=================================================================================
// class    : BasicGUI_PlaneDlg
// purpose  :
//=================================================================================
class BasicGUI_PlaneDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BasicGUI_PlaneDlg( GeometryGUI* theGeometryGUI, QWidget* parent = 0,
		       const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~BasicGUI_PlaneDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

    virtual void closeEvent( QCloseEvent* e );

private :
    void Init();
    void enterEvent(QEvent* e);
    double getSize() const;

    GEOM::GEOM_Object_var myPoint, myDir, myPoint1, myPoint2, myPoint3, myFace;

    DlgRef_2Sel1Spin* GroupPntDir;
    DlgRef_3Sel1Spin* Group3Pnts;
    DlgRef_1Sel1Spin* GroupFace;

private slots:
    void ClickOnOk();
    void ClickOnCancel();
    bool ClickOnApply();
    
    void ActivateThisDialog();
    void DeactivateActiveDialog();
    
    void SelectionIntoArgument();
    void LineEditReturnPressed();
    void SetEditCurrentArgument();
    void ConstructorsClicked(int constructorId);
    void ValueChangedInSpinBox(double newValue);

};

#endif // DIALOGBOX_PLANE_H
