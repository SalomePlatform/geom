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
//  File   : OperationGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

using namespace std;
#include "OperationGUI.h"

#include "DisplayGUI.h"

#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"
#include "OCCViewer_ViewFrame.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <Precision.hxx>

#include "OperationGUI_PartitionDlg.h"   // Method PARTITION
#include "OperationGUI_ArchimedeDlg.h"   // Method ARCHIMEDE
#include "OperationGUI_FilletDlg.h"      // Method FILLET
#include "OperationGUI_ChamferDlg.h"     // Method CHAMFER

//=======================================================================
// function : OperationGUI()
// purpose  : Constructor
//=======================================================================
OperationGUI::OperationGUI() :
  QObject()
{
  myGeomBase = new GEOMBase();
  myGeomGUI = GEOMContext::GetGeomGUI();
  myGeom = myGeomGUI->myComponentGeom;
}


//=======================================================================
// function : ~OperationGUI()
// purpose  : Destructor
//=======================================================================
OperationGUI::~OperationGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool OperationGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  OperationGUI* myOperationGUI = new OperationGUI();
  myOperationGUI->myGeomGUI->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());

  switch (theCommandID)
    {
    case 503: // PARTITION
      {
  	OperationGUI_PartitionDlg *aDlg = new OperationGUI_PartitionDlg(parent, "", myOperationGUI, Sel);	
	break;
      }
    case 504: // ARCHIMEDE
      {
	OperationGUI_ArchimedeDlg *aDlg = new OperationGUI_ArchimedeDlg(parent, "", myOperationGUI, Sel);	
	break;
      }
    case 505: // FILLET
      {
	Handle(AIS_InteractiveContext) ic;
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	  ic = v3d->getAISContext();
	}
  	OperationGUI_FilletDlg *aDlg = new OperationGUI_FilletDlg(parent, "", myOperationGUI, Sel, ic);	
	break;
      }
    case 506: // CHAMFER
      {
	Handle(AIS_InteractiveContext) ic;
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	  ic = v3d->getAISContext();
	}
	OperationGUI_ChamferDlg *aDlg = new OperationGUI_ChamferDlg(parent, "", myOperationGUI, Sel, ic);	
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
// function : MakePartitionAndDisplay()
// purpose  :
//=====================================================================================
void OperationGUI::MakePartitionAndDisplay(const GEOM::GEOM_Gen::ListOfIOR& listShapesIOR,
					   const GEOM::GEOM_Gen::ListOfIOR& listToolsIOR,
					   const GEOM::GEOM_Gen::ListOfIOR& listKeepInsIOR,
					   const GEOM::GEOM_Gen::ListOfIOR& listRemoveInsIOR,
					   const GEOM::shape_type limit)
{
  try {
    GEOM::GEOM_Shape_var result = myGeom->Partition(listShapesIOR, listToolsIOR,
						    listKeepInsIOR, listRemoveInsIOR,
						    (CORBA::Short) limit);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NULLSHAPE"));
      return;
    }
    result->NameType(tr("GEOM_PARTITION"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
    return;
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
}


//=======================================================================
// function : Archimede()
// purpose  : 
//=======================================================================
void OperationGUI::Archimede(const Handle(SALOME_InteractiveObject)& IO, const double aWeight,
			     const double aWaterDensity, const double aMeshingDeflection)
{
  try {
    if(IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject))) {
      Handle(GEOM_InteractiveObject) GIO = Handle(GEOM_InteractiveObject)::DownCast(IO);
      GEOM::GEOM_Shape_var Shape = myGeom->GetIORFromString(GIO->getIOR());
      
      GEOM::GEOM_Shape_var Result = myGeom->Archimede(Shape, aWeight, aWaterDensity, aMeshingDeflection);
      Result->NameType(tr("GEOM_PLANE"));
      if(myGeomBase->Display(Result)) {
	QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
      }
      return;
    }
    if(IO->hasEntry()) {
      SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
      SALOMEDS::SObject_var obj = aStudy->FindObjectID( IO->getEntry());
      SALOMEDS::GenericAttribute_var anAttr;
      SALOMEDS::AttributeIOR_var anIOR;
      if(!obj->_is_nil()) {
	if(obj->FindAttribute(anAttr, "AttributeIOR")) {
          anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	  GEOM::GEOM_Shape_var Shape = myGeom->GetIORFromString(anIOR->Value());
	  GEOM::GEOM_Shape_var Result = myGeom->Archimede(Shape, aWeight, aWaterDensity, aMeshingDeflection);
	  Result->NameType(tr("GEOM_PLANE"));
	  if (myGeomBase->Display(Result)) {
	    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
	  }
	}
      }
    }
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
}


//=====================================================================================
// function : OnFilletGetAll()
// purpose  :
//=====================================================================================
bool OperationGUI::OnFilletGetAll(const TopoDS_Shape& ShapeTopo, const double Radius,
				  const int SubShapeType, const char* ShapeTopoIOR)
{
  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ListOfID->length(0);

  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::SObject_var theObj = aStudy->FindObjectIOR(ShapeTopoIOR);
  if(theObj->_is_nil()) {
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY"));
    return false;
  }
  
  try {
    if(Radius <= Precision::Confusion())
      return false;

    GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(ShapeTopoIOR);
    GEOM::GEOM_Shape_var result = myGeom->MakeFillet(aShape, Radius, SubShapeType, ListOfID);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return false;
    }
    TopoDS_Shape S = myGeomGUI->GetShapeReader().GetShape(myGeom, result);
    Standard_CString type;
    myGeomBase->GetShapeTypeString(S,type);
    result->NameType(type);

    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return true;
}


//=====================================================================================
// function : OnFilletGetSelected()
// purpose  :
//=====================================================================================
bool OperationGUI::OnFilletGetSelected(const TopoDS_Shape& ShapeTopo, const char* ShapeTopoIOR,
				       const double Radius, const int SubShapeType,
				       Standard_Integer& aLocalContextId, bool& myUseLocalContext)
{
  if (QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return false;

  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
  
  if(myUseLocalContext == false) {
    /*  No local context opened for fillet method */
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
    return false;
  }
  
  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ic->InitSelected();
  int nbSelected = ic->NbSelected();
  int i = 0;
  ic->InitSelected();

  while(ic->MoreSelected()) {
    TopoDS_Shape s = ic->SelectedShape();
    if(s.ShapeType() == TopAbs_FACE) {
      TopExp_Explorer Exp(s, TopAbs_EDGE);
      TopTools_MapOfShape M;
      while(Exp.More()) {
	if(M.Add(Exp.Current())) { /* if a new edge : do not add doublons indices */
	  ListOfID->length(i + 1);
	  ListOfID[i] = myGeomBase->GetIndex(Exp.Current(), ShapeTopo, (int)TopAbs_EDGE);
	  i++;
	}
	Exp.Next(); 
      }	 
    }
    else {
      ListOfID->length(i + 1);
      ListOfID[i] = myGeomBase->GetIndex(ic->SelectedShape(), ShapeTopo, SubShapeType);
      i++;
    }
    ic->NextSelected();
  }

  GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(ShapeTopoIOR);
  GEOM::GEOM_Shape_var aResult;
  try {
    aResult = myGeom->MakeFillet(aShape, Radius, 6, ListOfID);

    /* local context from DialogBox */
    ic->CloseLocalContext(aLocalContextId);
    myUseLocalContext = false;

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

  if(myUseLocalContext) {
    /* local context from DialogBox */
    ic->CloseLocalContext(aLocalContextId);
    myUseLocalContext = false;
  }
  
  DisplayGUI* myDisplayGUI = new DisplayGUI();
  myDisplayGUI->OnDisplayAll(true) ;
  QAD_Application::getDesktop()->getActiveStudy()->updateObjBrowser();
  QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_READY"));

  return true;  
}


//=====================================================================================
// function : OnChamferGetAll()
// purpose  :
//=====================================================================================
bool OperationGUI::OnChamferGetAll(const TopoDS_Shape& ShapeTopo, const double D1, const double D2, const int SubShapeType, const char* ShapeTopoIOR)
{
  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ListOfID->length(0);

  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::SObject_var theObj = aStudy->FindObjectIOR(ShapeTopoIOR);
  if(theObj->_is_nil()) {
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY"));
    return false;
  }
  
  try {
    if(D1 <= Precision::Confusion())
      return false;
    if(D2 <= Precision::Confusion())
      return false;

    GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(ShapeTopoIOR);
    GEOM::GEOM_Shape_var result = myGeom->MakeChamfer(aShape, D1, D2, SubShapeType, ListOfID);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return false;
    }

    TopoDS_Shape S = myGeomGUI->GetShapeReader().GetShape(myGeom, result);
    Standard_CString type;
    myGeomBase->GetShapeTypeString(S,type);
    result->NameType(type);

    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return true;
}


//=====================================================================================
// function : OnChamferGetSelected()
// purpose  :
//=====================================================================================
bool OperationGUI::OnChamferGetSelected(const TopoDS_Shape& ShapeTopo, const char* ShapeTopoIOR,
					const double D1, const double D2, const int SubShapeType,
					Standard_Integer& aLocalContextId, bool& myUseLocalContext)
{
  if (QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return false;
  
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  
  if(myUseLocalContext == false) {
    /* No local context opened for chamfer method */
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
    return false;
  }
  
  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ic->InitSelected();
  int nbSelected = ic->NbSelected();
  int i = 0;
  ic->InitSelected();

  while(ic->MoreSelected()) {
    TopoDS_Shape s = ic->SelectedShape();
    if (s.ShapeType() == TopAbs_FACE) {
      TopExp_Explorer Exp(s, TopAbs_EDGE);
      TopTools_MapOfShape M;
      while(Exp.More()) {
	if(M.Add(Exp.Current())) { /* if a new edge : do not add doublons indices */
	  ListOfID->length(i + 1);
	  ListOfID[i] = myGeomBase->GetIndex(Exp.Current(), ShapeTopo, (int)TopAbs_EDGE);
	  i++;
	}
	Exp.Next(); 
      }	 
    }
    else {
      ListOfID->length(i + 1);
      ListOfID[i] = myGeomBase->GetIndex(ic->SelectedShape(), ShapeTopo, SubShapeType);
      i++;
    }
    ic->NextSelected();
  }
  
  GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(ShapeTopoIOR);
  GEOM::GEOM_Shape_var aResult;
  try {
    aResult = myGeom->MakeChamfer(aShape, D1, D2, 6, ListOfID);

    /* local context from DialogBox */
    ic->CloseLocalContext(aLocalContextId);
    myUseLocalContext = false;

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

  if (myUseLocalContext) {
    /* local context from DialogBox */
    ic->CloseLocalContext(aLocalContextId);
    myUseLocalContext = false;
  }

  DisplayGUI* myDisplayGUI = new DisplayGUI();
  myDisplayGUI->OnDisplayAll(true) ;
  QAD_Application::getDesktop()->getActiveStudy()->updateObjBrowser();
  QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_READY"));

  return true;  
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  bool OnGUIEvent(int theCommandID, QAD_Desktop* parent)
  {return OperationGUI::OnGUIEvent(theCommandID, parent);}
}
