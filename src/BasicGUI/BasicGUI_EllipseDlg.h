//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE 
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
//  File   : BasicGUI_EllipseDlg.h
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#ifndef BASICGUI_ELLIPSE_H
#define BASICGUI_ELLIPSE_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel2Spin.h"

#include "BasicGUI.h"

#include "GEOM_ShapeTypeFilter.hxx"
#include <gp_Dir.hxx>

//=================================================================================
// class    : BasicGUI_EllipseDlg
// purpose  :
//=================================================================================
class BasicGUI_EllipseDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BasicGUI_EllipseDlg(QWidget* parent = 0, const char* name = 0, BasicGUI* theBasicGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~BasicGUI_EllipseDlg();

private :
    void Init();
    void enterEvent(QEvent* e);
    void MakeEllipseSimulationAndDisplay();

    BasicGUI* myBasicGUI;

    double step;
    Handle(GEOM_ShapeTypeFilter) myVertexFilter;
    Handle(GEOM_ShapeTypeFilter) myEdgeFilter;  /* Filter selection */

    gp_Pnt myPoint;   /* Central point of ellipse */   
    bool myOkPoint;   /* true when myPoint is defined */
    gp_Dir myDir;     /* to set normal axis of ellipse */
    bool myOkDir;     /* true when myPoint is defined */
    
    Standard_Real myMajorRadius;
    Standard_Real myMinorRadius;

    DlgRef_2Sel2Spin* GroupPoints;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ValueChangedInSpinBox(double newValue);

};

#endif // BASICGUI_ELLIPSE_H
