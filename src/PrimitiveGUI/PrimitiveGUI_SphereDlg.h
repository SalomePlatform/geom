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
//  File   : PrimitiveGUI_SphereDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_SPHERE_H
#define DIALOGBOX_SPHERE_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel1Spin.h"
#include "DlgRef_1Spin.h"

#include "PrimitiveGUI.h"

#include "GEOM_ShapeTypeFilter.hxx"
#include <gp_Pnt.hxx>

//=================================================================================
// class    : PrimitiveGUI_SphereDlg
// purpose  :
//=================================================================================
class PrimitiveGUI_SphereDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    PrimitiveGUI_SphereDlg(QWidget* parent = 0, const char* name = 0, PrimitiveGUI* thePrimitiveGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~PrimitiveGUI_SphereDlg();

private :
    void Init();
    void enterEvent(QEvent* e);

    PrimitiveGUI* myPrimitiveGUI;

    double step;
    int myConstructorId;
    Handle(GEOM_ShapeTypeFilter) myVertexFilter;  /* filter for selection */

    gp_Pnt myPoint1;   /* Points containing the vector */
    Standard_Real myRadius;
    bool myOkPoint1;

    DlgRef_1Sel1Spin* GroupPoints;
    DlgRef_1Spin* GroupDimensions;

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

#endif // DIALOGBOX_SPHERE_H
