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
//  File   : RepairGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#include "RepairGUI.h"

#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include "RepairGUI_SewingDlg.h"        // Method SEWING
#include "RepairGUI_OrientationDlg.h"   // Method ORIENTATION
#include "RepairGUI_SuppressFacesDlg.h" // Method SUPPRESS FACES
#include "RepairGUI_SuppressHoleDlg.h"  // Method SUPPRESS HOLE

#include "utilities.h"

using namespace std;

//=======================================================================
// function : RepairGUI()
// purpose  : Constructor
//=======================================================================
RepairGUI::RepairGUI() :
  QObject()
{
  myGeomBase = new GEOMBase();
  myGeomGUI = GEOMContext::GetGeomGUI();
//   Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
//   myGeom = GEOM::GEOM_Gen::_narrow(comp);
  myGeom = myGeomGUI->myComponentGeom;
}


//=======================================================================
// function : ~RepairGUI()
// purpose  : Destructor
//=======================================================================
RepairGUI::~RepairGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool RepairGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  RepairGUI* myRepairGUI = new RepairGUI();
  myRepairGUI->myGeomGUI->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());

  switch (theCommandID)
    {
    case 601: // SEWING
      {
	RepairGUI_SewingDlg *aDlg = new RepairGUI_SewingDlg(parent, "", myRepairGUI, Sel);
	break;
      }
    case 602: // ORIENTATION
      {
	RepairGUI_OrientationDlg *aDlg = new RepairGUI_OrientationDlg(parent, "", myRepairGUI, Sel);
	break;
      }
    case 603: // SUPPRESS FACES : use ic
      {
	Handle(AIS_InteractiveContext) ic;
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	  ic = v3d->getAISContext();
	}
	RepairGUI_SuppressFacesDlg *aDlg = new RepairGUI_SuppressFacesDlg(parent, "", myRepairGUI, Sel, ic);
	break;
      }
    case 604: // SUPPRESS HOLES : use ic
      {
	Handle(AIS_InteractiveContext) ic;
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	  ic = v3d->getAISContext();
	}
	RepairGUI_SuppressHoleDlg *aDlg = new RepairGUI_SuppressHoleDlg(parent, "", myRepairGUI, Sel, ic);
	break;
      }
    default:
      {
	parent->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
	break;
      }
    }
  return true;
}


//=====================================================================================
// function : MakeSewingAndDisplay()
// purpose  :
//=====================================================================================
void RepairGUI::MakeSewingAndDisplay(GEOM::GEOM_Gen::ListOfIOR& listShapesIOR,
				     const Standard_Real precision)
{
  try {
    GEOM::GEOM_Shape_var result = myGeom->MakeSewing(listShapesIOR, precision);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NULLSHAPE"));
      return;
    }

    TopoDS_Shape S = myGeomGUI->GetShapeReader().GetShape(myGeom, result);
    Standard_CString type;
    myGeomBase->GetShapeTypeString(S,type);
    result->NameType(type);

    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
    return;
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeOrientationChangeAndDisplay()
// purpose  :
//=====================================================================================
void RepairGUI::MakeOrientationChangeAndDisplay(GEOM::GEOM_Shape_ptr Shape)
{
  try {
    GEOM::GEOM_Shape_var result = myGeom->OrientationChange(Shape);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NULLSHAPE"));
      return;
    }
    result->NameType(Shape->NameType());
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE")); 
    return;
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : OnSuppressHole() 
// purpose  : To suppress an hole on a shape 'ShapeTopo'.
//          : 'ListOfIdEndFace' may be an empty list.
//          : This means that hole do not traverse ShapeTopo.
//          : Warning : the hole to be suppressed must be defined by one or two single closed wires !
//=====================================================================================
bool RepairGUI::OnSuppressHole(const char* ShapeTopoIOR,
			       const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdFace,
			       const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdWire,
			       const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdEndFace)
{
  /* Test the type of viewer */
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
    return false;
  
  try {
    GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(ShapeTopoIOR);    
    GEOM::GEOM_Shape_var aResult = myGeom->SuppressHole(aShape, ListOfIdFace, ListOfIdWire, ListOfIdEndFace);
  
    TopoDS_Shape S = myGeomGUI->GetShapeReader().GetShape(myGeom, aResult);
    Standard_CString type;
    myGeomBase->GetShapeTypeString(S,type);
    aResult->NameType(type);

    if(myGeomBase->Display(aResult))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return true;
}


//=====================================================================================
// function : OnSuppressHolesInFaceOrShell() 
// purpose  : To suppress one or more holes on a face
//          : 'ListOfIdWires' contains indices or wires/holes.
//=====================================================================================
bool RepairGUI::OnSuppressHolesInFaceOrShell(const char* ShapeTopoIOR,
					     const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdWires)
{
  /* Test the type of viewer */
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return false;
  
  try {
    GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(ShapeTopoIOR);    
    GEOM::GEOM_Shape_var aResult = myGeom->SuppressHolesInFaceOrShell(aShape, ListOfIdWires);
    
    TopoDS_Shape S = myGeomGUI->GetShapeReader().GetShape(myGeom, aResult);
    Standard_CString type;
    myGeomBase->GetShapeTypeString(S,type);
    aResult->NameType(type);
    
    if(myGeomBase->Display(aResult))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return true;
}


//=====================================================================================
// function : OnSuppressFaces() 
// purpose  : To suppress faces from a shape
//          : The result is one or more shells/faces as main shapes !
//=====================================================================================
bool RepairGUI::OnSuppressFaces(const TopoDS_Shape& ShapeTopo, const char* ShapeTopoIOR,
				const Standard_Integer& aLocalContextId, bool& myUseLocalContext)
{
  /* Test the type of viewer */
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return false;
  
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::SObject_var theObj = aStudy->FindObjectIOR(ShapeTopoIOR);
  if(theObj->_is_nil()) {
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY"));
    return false;
  }
  
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
  
  if(myUseLocalContext == false) {
    /* local context is from DialogBox */
    MESSAGE("Error : No local context opened for suppress faces method" << endl);
    return false;
  }
  
  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ic->InitSelected();
  int nbSelected = ic->NbSelected();
  ListOfID->length(nbSelected);

  /* Create a list of indices of faces to be suppressed */
  int i = 0;
  const int SubShapeType = 4; /* GEOM::FACE type */
  ic->InitSelected();          /* to repositioning at beginning */
  while(ic->MoreSelected()) {
    int index = myGeomBase->GetIndex(ic->SelectedShape(), ShapeTopo, SubShapeType);
    ListOfID[i] = index;
    i++;
    ic->NextSelected();
  }
  
  /* Close local context opened in DialogBox */
  ic->CloseLocalContext(aLocalContextId);
  myUseLocalContext = false;
  
  /* Here is the main shape */
  GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(ShapeTopoIOR);  
  GEOM::GEOM_Gen::ListOfGeomShapes_var listGeomShapes = new GEOM::GEOM_Gen::ListOfGeomShapes;
  
  /* Call geom method that return a list of shells/faces as result of suppress */
  try {
    listGeomShapes = myGeom->SuppressFaces(aShape, ListOfID);
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  
  /* Test list of shells/faces */
  if(listGeomShapes->length() < 1) {
    return false;
  }

  /* Loop on each object created */
  for(int i=0; i<listGeomShapes->length(); i++) {
    GEOM::GEOM_Shape_var aShellOrFace = listGeomShapes[i];
    TopoDS_Shape S = myGeomGUI->GetShapeReader().GetShape(myGeom, aShellOrFace);
    
    if(S.IsNull())
      return false;
    
    char* nameG = (char *)malloc(20);
    Standard_CString Type;
    if(myGeomBase->GetShapeTypeString(S, Type)) {
      aShellOrFace->NameType(Type);
      sprintf (nameG, "%s_%d", Type, myGeomGUI->myNbGeom++);
    }
    else {
      aShellOrFace->NameType(tr("GEOM_SHAPE"));
      sprintf (nameG, "%s_%d", tr("GEOM_SHAPE").latin1(), myGeomGUI->myNbGeom++);
    }
    
    /* Display with name */
    if(!myGeomBase->Display(aShellOrFace, nameG)) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return false;
    }
  }
  
  QAD_Application::getDesktop()->putInfo (tr("GEOM_PRP_READY"));
  return true;
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  bool OnGUIEvent(int theCommandID, QAD_Desktop* parent)
  {return RepairGUI::OnGUIEvent(theCommandID, parent);}
}
