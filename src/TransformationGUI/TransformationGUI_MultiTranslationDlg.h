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
//  File   : TransformationGUI_MultiTranslationDlg.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_MULTITRANSLATION_H
#define DIALOGBOX_MULTITRANSLATION_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel2Spin1Check.h"
#include "DlgRef_3Sel4Spin2Check.h"

#include "TransformationGUI.h"

#include "GEOM_ShapeTypeFilter.hxx"
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>

//=================================================================================
// class    : TransformationGUI_MultiTranslationDlg
// purpose  :
//=================================================================================
class TransformationGUI_MultiTranslationDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    TransformationGUI_MultiTranslationDlg(QWidget* parent = 0, const char* name = 0, TransformationGUI* theTransformationGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~TransformationGUI_MultiTranslationDlg();

private :
    void Init();
    void enterEvent(QEvent* e);
    void MakeMultiTranslationSimulationAndDisplay();

    TransformationGUI* myTransformationGUI;

    double step;
    int myConstructorId;   /* Current constructor id = radio button id */ 
    Handle(GEOM_ShapeTypeFilter) myEdgeFilter;   /* Filters selection */

    TopoDS_Shape myBase;
    GEOM::GEOM_Shape_var myGeomShape;   /* is myBase */
    gp_Vec myVec;
    int myNbTimes1;
    int myNbTimes2;
    Standard_Real myStep1;
    Standard_Real myStep2;
    gp_Dir myDir1;
    gp_Dir myDir2;
    bool myOkBase; 
    bool myOkDir1;
    bool myOkDir2;

    DlgRef_2Sel2Spin1Check* GroupPoints;
    DlgRef_3Sel4Spin2Check* GroupDimensions;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ReverseAngle1(int state);
    void ReverseAngle2(int state);
    void ValueChangedInSpinBox(double newValue);
    void ConstructorsClicked(int constructorId);

};

#endif // DIALOGBOX_MULTITRANSLATION_H
