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
//  File   : EntityGUI_SketcherDlg.h
//  Author : Damine COQUERET
//  Module : GEOM
//  $Header: 

#ifndef ENTITYGUI_SKETCHERDLG_H
#define ENTITYGUI_SKETCHERDLG_H

#include "EntityGUI_Skeleton_QTD.h"
#include "EntityGUI_Point_QTD.h"
#include "EntityGUI_Dir1_QTD.h"
#include "EntityGUI_Dir2_QTD.h"

#include "EntityGUI_1Sel_QTD.h"
#include "EntityGUI_1Spin.h"
#include "EntityGUI_2Spin.h"
#include "EntityGUI_3Spin.h"
#include "EntityGUI_4Spin.h"

#include "EntityGUI.h"

#include "GEOM_ShapeTypeFilter.hxx"

#include <gp_Dir.hxx>

#include <qwidget.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

//=================================================================================
// class    : EntityGUI_Dlg
// purpose  :
//=================================================================================
class EntityGUI_SketcherDlg : public EntityGUI_Skeleton_QTD
{ 
    Q_OBJECT

public:
    EntityGUI_SketcherDlg(QWidget* parent = 0, const char* name = 0, EntityGUI* theEntityGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~EntityGUI_SketcherDlg();

private :
    void Init();
    void enterEvent(QEvent* e);
    void closeEvent(QCloseEvent* e);
    void InitClick();
    void MakeSimulationAndDisplay();
    QString GetNewCommand();

    EntityGUI* myEntityGUI;

    double step;
    int myConstructorId;
    int myConstructorDirId;
    int mySketchType;
    int mySketchState;
    Handle(GEOM_ShapeTypeFilter) myVertexFilter;

    TopoDS_Shape mySimulationTopoDs1;    /* Shape used for simulation display */
    TopoDS_Shape mySimulationTopoDs2;    /* Shape used for simulation display */
    QLineEdit* myEditCurrentArgument;   /* Current LineEdit */
    SALOME_Selection* mySelection;      /* User shape selection */
    GEOM::GEOM_Gen_var myGeom;          /* Current GeomI object */
    GEOMBase* myGeomBase;
    GEOMContext* myGeomGUI;  /* Current GeomGUI object */

    int myLastOp;
    QString myCommand;
    QString myLastCommand;
    TopoDS_Shape myShape;
    gp_Dir myLastDir;

    Standard_Real myX;
    Standard_Real myY;
    Standard_Real myDX;
    Standard_Real myDY;
    Standard_Real myLastX1;
    Standard_Real myLastY1;
    Standard_Real myLastX2;
    Standard_Real myLastY2;
    Standard_Real myLength;
    Standard_Real myAngle;
    Standard_Real myRadius;

    EntityGUI_Point_QTD* GroupPt;
    EntityGUI_Dir1_QTD* GroupD1;
    EntityGUI_Dir2_QTD* GroupD2;

    EntityGUI_1Sel_QTD* Group1Sel;
    EntityGUI_1Spin* Group1Spin;
    EntityGUI_2Spin* Group2Spin;
    EntityGUI_3Spin* Group3Spin;
    EntityGUI_4Spin* Group4Spin;

    enum SketchState {FIRST_POINT, SECOND_POINT, NEXT_POINT};

    enum SketchType {PT_ABS, PT_RELATIVE, PT_SEL,
		     DIR_ANGLE_LENGTH, DIR_ANGLE_X, DIR_ANGLE_Y,
		     DIR_PER_LENGTH, DIR_PER_X, DIR_PER_Y,
		     DIR_TAN_LENGTH, DIR_TAN_X, DIR_TAN_Y,
		     DIR_DXDY_LENGTH, DIR_DXDY_X, DIR_DXDY_Y};

private slots:
    void ClickOnEnd();
    void ClickOnClose();
    void ClickOnCancel();
    void ClickOnApply();
    void ClickOnUndo();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void DeactivateActiveDialog();
    void ActivateThisDialog();
    void TypeClicked(int constructorId);
    void DestClicked(int constructorId);
    void PointClicked(int constructorId);
    void Dir1Clicked(int constructorId);
    void Dir2Clicked(int constructorId);
    void ValueChangedInSpinBox(double newValue);

};

#endif // ENTITYGUI_SKETCHERDLG_H
