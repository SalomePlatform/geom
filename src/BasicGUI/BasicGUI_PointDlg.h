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
//  File   : BasicGUI_PointDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_POINT_H
#define DIALOGBOX_POINT_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel1Spin.h"
#include "DlgRef_3Spin.h"

#include "BasicGUI.h"

#include "GEOM_ShapeTypeFilter.hxx"
#include <TopoDS_Edge.hxx>

//=================================================================================
// class    : BasicGUI_PointDlg
// purpose  :
//=================================================================================
class BasicGUI_PointDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BasicGUI_PointDlg(QWidget* parent = 0, const char* name = 0, BasicGUI* theBasicGUI = 0, SALOME_Selection* Sel = 0, const Handle(AIS_InteractiveContext)& ic = 0, bool modal = FALSE, WFlags fl = 0);

    ~BasicGUI_PointDlg();
    
private :
    void Init(const Handle(AIS_InteractiveContext)& ic);
    void enterEvent(QEvent* e);
    void closeEvent(QCloseEvent* e);

    BasicGUI* myBasicGUI;

    double step;
    int myConstructorId;
    Handle(GEOM_ShapeTypeFilter) myEdgeFilter;   /* filter for selection */

    gp_Pnt myPoint;       /* Is 'mySimulationTopoDs'  */
    bool myOkEdge;        /* true when an edge is selected by user */
    double myParameter;   /* Parameter used to create a vertex on edge (point on curve) */

    /* Interactive and local context management see also : bool UseLocalContext() */
    Handle(AIS_InteractiveContext) myIC;   /* Interactive context from IAPP */
    Standard_Integer myLocalContextId;     /* identify a local context for this method */
    TopAbs_ShapeEnum myLocalContextMode;   /* identify a selection mode into local context */
    bool myUseLocalContext;                /* true when method as opened a local context  */

    DlgRef_1Sel1Spin* GroupDimensions;
    DlgRef_3Spin* GroupPoints;

private slots:
    void ClickOnOk();
    void ClickOnCancel();
    void ClickOnApply();
    void ActivateThisDialog();
    void DeactivateActiveDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ConstructorsClicked(int constructorId);
    void ValueChangedInSpinBox(double newValue);

    bool CalculateVertexOnCurve(const TopoDS_Edge& anEdge,
				const Standard_Real aParameter,
				TopoDS_Shape& resultVertex);

public:   
    void PointIntoCoordinates(gp_Pnt P, bool displayPoint);
    /* return true if method has opened a local context */
    bool UseLocalContext(){return myUseLocalContext;};

};

#endif // DIALOGBOX_POINT_H
