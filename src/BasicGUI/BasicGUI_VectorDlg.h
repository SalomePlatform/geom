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
//  File   : BasicGUI_VectorDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_VECTOR_H
#define DIALOGBOX_VECTOR_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel_QTD.h"
#include "DlgRef_3Spin1Check.h"

#include "BasicGUI.h"
#include "GEOM_ShapeTypeFilter.hxx"

//=================================================================================
// class    : BasicGUI_VectorDlg
// purpose  :
//=================================================================================
class BasicGUI_VectorDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BasicGUI_VectorDlg(QWidget* parent = 0, const char* name = 0, BasicGUI* theBasicGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~BasicGUI_VectorDlg();

private :
    void Init();
    void enterEvent(QEvent* e);
    bool AddArrowToSimulation(TopoDS_Shape& modifiedShape);

    BasicGUI* myBasicGUI;

    double step;
    int myConstructorId;
    Handle(GEOM_ShapeTypeFilter) myVertexFilter;   /* Filter selection */

    gp_Pnt myPoint1;   /* Points containing the vector */   
    gp_Pnt myPoint2;
    bool myOkPoint1;   /* Are true when myPoint is defined */    
    bool myOkPoint2; 
    double myDx;
    double myDy;
    double myDz;

    DlgRef_2Sel_QTD* GroupPoints;
    DlgRef_3Spin1Check* GroupDimensions;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ConstructorsClicked(int constructorId);
    void ValueChangedInSpinBox(double newValue);
    void ReverseVector(int state);

};

#endif // DIALOGBOX_VECTOR_H
