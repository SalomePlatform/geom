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

#include "GEOMBase_Helper.h"

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
#include "GeometryGUI.h"

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
class EntityGUI_SketcherDlg : public EntityGUI_Skeleton_QTD, public GEOMBase_Helper
{ 
    Q_OBJECT

public:
    EntityGUI_SketcherDlg(QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~EntityGUI_SketcherDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

    void closeEvent( QCloseEvent* e );

private :
    void Init();
    void enterEvent(QEvent* e);
    void InitClick();
    void setEnabledUndo(bool value);
    void setEnabledRedo(bool value);

    QString GetNewCommand();

    int myConstructorId;
    int myConstructorDirId;
    int mySketchType;
    int mySketchState;

    bool myIsAllAdded;
    
    QLineEdit* myEditCurrentArgument;   /* Current LineEdit */
    SALOME_Selection* mySelection;      /* User shape selection */
    
    QStringList myCommand;
    QStringList myUndoCommand;

    Standard_Real myX, myY, myDX, myDY;
    Standard_Real myLength, myAngle, myRadius;
    Standard_Real myLastX1, myLastY1;
    Standard_Real myLastX2, myLastY2;

    EntityGUI_Point_QTD* GroupPt;
    EntityGUI_Dir1_QTD* GroupD1;
    EntityGUI_Dir2_QTD* GroupD2;

    EntityGUI_1Sel_QTD* Group1Sel;
    EntityGUI_1Spin* Group1Spin;
    EntityGUI_2Spin* Group2Spin;
    EntityGUI_3Spin* Group3Spin;
    EntityGUI_4Spin* Group4Spin;

    enum SketchState {FIRST_POINT, NEXT_POINT};

    enum SketchType {PT_ABS, PT_RELATIVE, PT_SEL,
		     DIR_ANGLE_LENGTH, DIR_ANGLE_X, DIR_ANGLE_Y,
		     DIR_PER_LENGTH, DIR_PER_X, DIR_PER_Y,
		     DIR_TAN_LENGTH, DIR_TAN_X, DIR_TAN_Y,
		     DIR_DXDY_LENGTH, DIR_DXDY_X, DIR_DXDY_Y};

  virtual void displayPreview  ( GEOM::GEOM_Object_ptr obj,
                                 const bool            append = false,
                                 const bool            activate = false,
                                 const bool            update = true,
                                 const double          lineWidth = -1 );

  bool createShapes( GEOM::GEOM_Object_ptr theObject,
                     TopoDS_Shape&         theApplyedWire,
                     TopoDS_Shape&         theLastSegment );
private slots:
    void ClickOnEnd();
    void ClickOnCancel();
    bool ClickOnApply();
    void ClickOnUndo();
    void ClickOnRedo();
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
