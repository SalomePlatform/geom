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
//  File   : RepairGUI_SuppressHoleDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef GEOMETRYGUI_SUPPRESSHOLE_H
#define GEOMETRYGUI_SUPPRESSHOLE_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel1Check_QTD.h"
#include "DlgRef_1Sel3Check_QTD.h"

#include "RepairGUI.h"

//=================================================================================
// class    : RepairGUI_SuppressHoleDlg
// purpose  :
//=================================================================================
class RepairGUI_SuppressHoleDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT
      
public:
    RepairGUI_SuppressHoleDlg(QWidget* parent = 0, const char* name = 0, RepairGUI* theRepairGUI = 0, SALOME_Selection* Sel = 0, Handle(AIS_InteractiveContext) ic = 0, bool modal = FALSE, WFlags fl = 0);
    ~RepairGUI_SuppressHoleDlg();

private :
    void Init(Handle(AIS_InteractiveContext) ic);
    void enterEvent(QEvent* e);
    void closeEvent(QCloseEvent* e);

    void ResetStateOfDialog();
    void ResetPartial();

    RepairGUI* myRepairGUI;

    int myConstructorId;   /* Current constructor id = radio button id */
    
    /* Define a list of indices of sub shapes selected in a local context */
    bool GetIndexSubShapeSelected(const TopoDS_Shape& ShapeTopo, const int SubShapeType,
				  GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID,
				  Standard_Integer& aLocalContextId, bool& myUseLocalContext);

    /* Return the face selected by user from the main shape and index in a ListOfSub */
    TopoDS_Shape FaceFromList(const TopoDS_Shape& aShape,
			      const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfSub);

    /* Interactive and local context management see also : bool myUseLocalContext() */
    Handle(AIS_InteractiveContext) myIC;   /* Interactive context */ 
    Standard_Integer myLocalContextId;   /* identify a local context used by this method */
    bool myUseLocalContext;   /* true when this method as opened a local context */

    TopoDS_Shape myShape;   /* Main shape selected */
    TopoDS_Shape myFace;    /* Face selected */

    char* myShapeIOR;
    bool myOkShape;

    bool myOkSelectFace;   /* true = sub mode GEOM::FACE selection done */
    bool myOkSelectWire;   /* true = sub mode GEOM::WIRE selection done (first wire)  */  

    /* After selection contains index of face into myShape, wire into myFace, end face into myShape*/
    GEOM::GEOM_Shape::ListOfSubShapeID_var myListOfIdFace;
    GEOM::GEOM_Shape::ListOfSubShapeID_var myListOfIdWire;
    GEOM::GEOM_Shape::ListOfSubShapeID_var myListOfIdEndFace;
  
    DlgRef_1Sel3Check_QTD* Group1;
    DlgRef_1Sel1Check_QTD* Group2;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void ClickOnCancel();
    void ActivateThisDialog();
    void DeactivateActiveDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ConstructorsClicked(int constructorId);
    void ActivateUserFaceSelection();
    void ActivateUserWireSelection();
    void ActivateUserEndFaceSelection();
    
    /* For the second constructor */
    void ActivateUserWiresOnFaceShellSelection() ;

};

#endif // GEOMETRYGUI_SUPPRESSHOLE_H
