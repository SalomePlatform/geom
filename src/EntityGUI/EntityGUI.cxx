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
//  File   : EntityGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#include "EntityGUI.h"

#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"
#include "VTKViewer_ViewFrame.h"
#include "GEOM_AssemblyBuilder.h"
#include "SALOMEGUI_ImportOperation.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>

#include "DisplayGUI.h"

#include "EntityGUI_SketcherDlg.h" // Sketcher
#include "EntityGUI_SubShapeDlg.h" // Method SUBSHAPE

#include "utilities.h"

using namespace std;

//=======================================================================
// function : EntityGUI()
// purpose  : Constructor
//=======================================================================
EntityGUI::EntityGUI() :
  QObject()
{
  myGeomBase = new GEOMBase();
  myGeomGUI = GEOMContext::GetGeomGUI();
  myGeom = myGeomGUI->myComponentGeom;

  mySimulationShape1 = new AIS_Shape(TopoDS_Shape());
  mySimulationShape2 = new AIS_Shape(TopoDS_Shape());
}


//=======================================================================
// function : ~EntityGUI()
// purpose  : Destructor
//=======================================================================
EntityGUI::~EntityGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool EntityGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  EntityGUI* myEntityGUI = new EntityGUI();
  myEntityGUI->myGeomGUI->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());

  switch (theCommandID)
    {
    case 404: // SKETCHER
      {
	((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->onViewTop(); // DCQ : 28/02/2002
	EntityGUI_SketcherDlg* aDlg = new EntityGUI_SketcherDlg(parent, "", myEntityGUI, Sel);
	break;
      }
    case 407: // EXPLODE : use ic
      {
	Handle(AIS_InteractiveContext) ic;
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	  ic = v3d->getAISContext();
	}
	EntityGUI_SubShapeDlg *aDlg = new EntityGUI_SubShapeDlg(parent, "", myEntityGUI, Sel, ic);
	break ;
      }
    default:
      {
	parent->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
	break;
      }
    }
  return true;
}


//=======================================================================
// function : OnSketchEnd()
// purpose  : 
//=======================================================================
void EntityGUI::OnSketchEnd(const char *Cmd)
{
  try {
    GEOM::GEOM_Shape_var result = myGeom->MakeSketcher(Cmd);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NULLSHAPE"));
      return;
    }
    result->NameType(tr("GEOM_WIRE"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : DisplaySimulationShape() 
// purpose  : Displays 'this->mySimulationShape' a pure graphical shape from a TopoDS_Shape
//=====================================================================================
void EntityGUI::DisplaySimulationShape(const TopoDS_Shape& S1, const TopoDS_Shape& S2) 
{
  //NRI DEBUG : 14/02/2002
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return;
	
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
  try {
    if(!S1.IsNull()) {
      /* erase any previous */
      ic->Erase(mySimulationShape1, Standard_True, Standard_False);
      ic->ClearPrs(mySimulationShape1);
      
      mySimulationShape1 = new AIS_Shape(TopoDS_Shape());
      mySimulationShape1->Set(S1);
      mySimulationShape1->SetColor(Quantity_NOC_RED);
    
      ic->Deactivate(mySimulationShape1);
      ic->Display(mySimulationShape1, Standard_False);
      mySimulationShape1->UnsetColor();
    }
    if(!S2.IsNull()) {
      ic->Erase(mySimulationShape2, Standard_True, Standard_False);
      ic->ClearPrs(mySimulationShape2);

      mySimulationShape2 = new AIS_Shape(TopoDS_Shape());
      mySimulationShape2->Set(S2);
      mySimulationShape2->SetColor(Quantity_NOC_VIOLET);

      ic->Deactivate(mySimulationShape2);
      ic->Display(mySimulationShape2, Standard_False);
      mySimulationShape2->UnsetColor();
    }
    ic->UpdateCurrentViewer();
  }
  catch(Standard_Failure) {
    MESSAGE("Exception catched in EntityGUI::DisplaySimulationShape ");
  } 
  return;
}


//==================================================================================
// function : EraseSimulationShape()
// purpose  : Clears the display of 'mySimulationShape' a pure graphical shape
//==================================================================================
void EntityGUI::EraseSimulationShape(int Sh)
{
  int count = QAD_Application::getDesktop()->getActiveStudy()->getStudyFramesCount();
  for(int i = 0; i < count; i++) {
    if(QAD_Application::getDesktop()->getActiveStudy()->getStudyFrame(i)->getTypeView() == VIEW_OCC) {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getStudyFrame(i)->getRightFrame()->getViewFrame())->getViewer();
      Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
      if(Sh < 1) {
	ic->Erase(mySimulationShape1, Standard_True, Standard_False);
	ic->ClearPrs(mySimulationShape1);
      }
      if(Sh <= 1) {
	ic->Erase(mySimulationShape2, Standard_True, Standard_False);
	ic->ClearPrs(mySimulationShape2);
      }
      ic->UpdateCurrentViewer();
    } 
  }
}


//=====================================================================================
// function : SObjectExist()
// purpose  :
//=====================================================================================
bool EntityGUI::SObjectExist(SALOMEDS::SObject_ptr theFatherObject, const char* IOR)
{
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::ChildIterator_var it = aStudy->NewChildIterator(theFatherObject);
  SALOMEDS::SObject_var RefSO;
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeIOR_var anIOR;
  for(; it->More();it->Next()) {
    SALOMEDS::SObject_var SO= it->Value();
    if(SO->FindAttribute(anAttr, "AttributeIOR")) {
      anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
      if(strcmp( anIOR->Value(), IOR ) == 0)
	return true;
    }
    if(SO->ReferencedObject(RefSO)) {
      if(RefSO->FindAttribute(anAttr, "AttributeIOR")) {
        anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	if(strcmp(anIOR->Value(), IOR) == 0)
	  return true;
      }
    }
  }
  return false;
}


//=====================================================================================
// function : OnSubShapeGetAll()
// purpose  : Explode a shape in all sub shapes with a SubShapeType
//=====================================================================================
bool EntityGUI::OnSubShapeGetAll(const TopoDS_Shape& ShapeTopo, const char* ShapeTopoIOR, const int SubShapeType)
{
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::SObject_var theObj = aStudy->FindObjectIOR(ShapeTopoIOR);
  if(theObj->_is_nil()) {
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY"));
    return false;
  }

  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::SObject_var fatherSF = aStudy->FindObjectID(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->entry());
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var aName;
  SALOMEDS::AttributeIOR_var anIOR;
  SALOMEDS::AttributePixMap_var aPixmap;

  /* We create a sub object for each sub shape as attribute of the main object  */
  /* Each sub object contains list (length=1) containing its index in the main shape */
  GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(ShapeTopoIOR);
  GEOM::GEOM_Gen::ListOfGeomShapes_var listGeomShapes = new GEOM::GEOM_Gen::ListOfGeomShapes;
  GEOM::GEOM_Shape_var aResult;
  
  try {
    listGeomShapes = myGeom->SubShapeAll(aShape, SubShapeType);
    if(listGeomShapes->length() < 1) {
      QAD_Application::getDesktop()->putInfo (tr("GEOM_PRP_ABORT"));
      return false;
    }
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  
  /* open transaction */
  QAD_Operation* op = new SALOMEGUI_ImportOperation(QAD_Application::getDesktop()->getActiveStudy());
  op->start();
  
  TopoDS_Shape mainTopo = myGeomGUI->GetShapeReader().GetShape(myGeom, aShape);
  TopoDS_Shape mainShape;
  bool main = false;
  while(!main) {
    if(aShape->IsMainShape()) {
      mainShape = myGeomGUI->GetShapeReader().GetShape(myGeom, aShape);
      main = true;
    } 
    else
      aShape = myGeom->GetIORFromString(aShape->MainName());
  }
  
  /* Loop on each sub shape created */
  /* int i = 1 ; /* index for the nameType */  
  for(int j=0; j<listGeomShapes->length(); j++) {
    /* Get each sub shape extracted CORBA and OCC */
    aResult = listGeomShapes[j] ;    
    TopoDS_Shape S = myGeomGUI->GetShapeReader().GetShape(myGeom, aResult);
    
    if (S.IsNull()) {
      QAD_Application::getDesktop()->putInfo (tr("GEOM_PRP_ABORT"));
      return false;
    }
    
    /* Set the nameType of sub shape */
    char* nameG = (char *)malloc(20);
    Standard_CString Type;
    if(myGeomBase->GetShapeTypeString(S, Type)) {
      aResult->NameType(Type);
      sprintf(nameG, "%s_%d", Type, myGeomBase->GetIndex(S, mainShape, SubShapeType));
    } 
    else {
      aResult->NameType(tr("GEOM_SHAPE"));
      sprintf(nameG, "%s_%d", tr("GEOM_SHAPE").latin1(), myGeomGUI->myNbGeom++);
    }
    SALOMEDS::SObject_var SO = aStudy->FindObjectIOR(aResult->Name());

    bool allreadyexist = false;
          
    if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
      Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
      
      Handle(GEOM_AISShape) result = new GEOM_AISShape(S, nameG);      
      Handle(GEOM_InteractiveObject) IO = new GEOM_InteractiveObject(aResult->Name(), myGeomGUI->GetFatherior(), "GEOM");
      
      MESSAGE ("SO->_is_nil() " << SO->_is_nil())

      if(SO->_is_nil()) {
	SALOMEDS::SObject_var newObj = aStudyBuilder->NewObject(theObj);
        anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
        aName->SetValue(nameG);
        anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
        anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        anIOR->SetValue(aResult->Name());

	anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributePixMap");
	aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
	MESSAGE(" Type " << S.ShapeType())
	if (S.ShapeType() == TopAbs_COMPOUND)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_COMPOUND");
	else if(S.ShapeType() == TopAbs_COMPSOLID)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_COMPSOLID");
	else if(S.ShapeType() == TopAbs_SOLID)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_SOLID");
	else if(S.ShapeType() == TopAbs_SHELL)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_SHELL");
	else if(S.ShapeType() == TopAbs_FACE)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_FACE");
	else if(S.ShapeType() == TopAbs_WIRE)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_WIRE");
	else if(S.ShapeType() == TopAbs_EDGE)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_EDGE");
	else if(S.ShapeType() == TopAbs_VERTEX)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_VERTEX");

	MESSAGE(" aPixmap->GetPixMap " << aPixmap->GetPixMap())

	SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
	aStudyBuilder->Addreference(newObj1, newObj);
	IO->setEntry(newObj->GetID());

	aResult->StudyShapeId(newObj->GetID());
      }
      else {
	allreadyexist = true;
 	if(!this->SObjectExist(theObj, aResult->Name())) {
	  SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(theObj);
	  aStudyBuilder->Addreference(newObj1, SO);
	  IO->setEntry(SO->GetID());
	  aResult->StudyShapeId(SO->GetID());
	}
      }
      
      result->setIO(IO);
      result->setName(nameG);
      if(!allreadyexist)
	ic->Display(result);
      
    }
    else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
      VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
      
      int themode = myRenderInter->GetDisplayMode();
      vtkRenderer *theRenderer = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
      vtkRenderWindow *renWin = theRenderer->GetRenderWindow();
      
      Handle(GEOM_InteractiveObject) IO = new GEOM_InteractiveObject(aResult->Name(), myGeomGUI->GetFatherior(), "GEOM");
      
      if(SO->_is_nil()) {
	SALOMEDS::SObject_var newObj = aStudyBuilder->NewObject(theObj);
        anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
        anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        anIOR->SetValue(aResult->Name());
        anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
        aName->SetValue(nameG);
	
	anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributePixMap");
	aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
	if(S.ShapeType() == TopAbs_COMPOUND)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_COMPOUND");
	else if(S.ShapeType() == TopAbs_COMPSOLID)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_COMPSOLID");
	else if(S.ShapeType() == TopAbs_SOLID)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_SOLID");
	else if(S.ShapeType() == TopAbs_SHELL)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_SHELL");
	else if(S.ShapeType() == TopAbs_FACE)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_FACE");
	else if(S.ShapeType() == TopAbs_WIRE)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_WIRE");
	else if(S.ShapeType() == TopAbs_EDGE)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_EDGE");
	else if(S.ShapeType() == TopAbs_VERTEX)
	  aPixmap->SetPixMap("ICON_OBJBROWSER_VERTEX");

	SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
	aStudyBuilder->Addreference(newObj1, newObj);
	IO->setEntry(newObj->GetID());
      }
      else {
	allreadyexist = true;
	if(!this->SObjectExist(theObj, aResult->Name())) {
	  SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(theObj);
	  aStudyBuilder->Addreference(newObj1, SO);
	  IO->setEntry(SO->GetID());
	}
      }
      
      if(!allreadyexist) {
	vtkActorCollection* theActors = GEOM_AssemblyBuilder::BuildActors(S,0,themode,Standard_True);
	theActors->InitTraversal();
	vtkActor* anActor = (vtkActor*)theActors->GetNextActor();
	while(!(anActor==NULL)) {
	  GEOM_Actor* GActor = GEOM_Actor::SafeDownCast(anActor);
	  GActor->setIO(IO);
	  GActor->setName(nameG);
	  theRenderer->AddActor(GActor);
	  renWin->Render();
	  anActor = (vtkActor*)theActors->GetNextActor();
	}
      }
    }
  }

  /* commit transaction */
  op->finish();

  QAD_Application::getDesktop()->getActiveStudy()->updateObjBrowser();
  QAD_Application::getDesktop()->putInfo (tr("GEOM_PRP_READY"));
  return true;
}


//=====================================================================================
// function : OnSubShapeGetSelected()
// purpose  :
//=====================================================================================
bool EntityGUI::OnSubShapeGetSelected(const TopoDS_Shape& ShapeTopo, const char* ShapeTopoIOR, const int SubShapeType, Standard_Integer& aLocalContextId, bool& myUseLocalContext)
{
  //* Test the type of viewer */
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
  
  if( myUseLocalContext == false ) {
    /* local context is from DialogBox */
    MESSAGE("Error : No local context opened for sub shapes method" << endl ) ;
    return false ;
  }

  GEOM::GEOM_Shape_var aShape  = myGeom->GetIORFromString( ShapeTopoIOR );
  TopoDS_Shape mainTopo = myGeomGUI->GetShapeReader().GetShape(myGeom, aShape);

  TopoDS_Shape mainShape;
  bool main = false;
  while(!main) {
    if(aShape->IsMainShape()) {
      mainShape = myGeomGUI->GetShapeReader().GetShape(myGeom, aShape);
      main = true;
    }
    else
      aShape = myGeom->GetIORFromString(aShape->MainName());
  }

  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ic->InitSelected();
  int nbSelected = ic->NbSelected();
  ListOfID->length(nbSelected);
  
  TopoDS_Compound compound;
  ic->InitSelected(); /* to init again */ 
  BRep_Builder B;
  B.MakeCompound(compound);

  int i = 0;
  /* We create a unique compound containing all the sub shapes selected by user as attribute of the main shape */
  /* the compound is homogenous by selection */
  while(ic->MoreSelected()) {
    int index = myGeomBase->GetIndex(ic->SelectedShape(), mainShape, SubShapeType);
    ListOfID[i] = index;
    B.Add(compound, ic->SelectedShape());
    i++;
    ic->NextSelected();
  }
 
  /* Test if user has selected sub shapes */
  if(ListOfID->length() < 1)
    return false;

  GEOM::GEOM_Shape_var aResult;
  try {
    aResult = myGeom->SubShape(aShape, SubShapeType, ListOfID);
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }

  /* local context from DialogBox */
  ic->CloseLocalContext(aLocalContextId);
  myUseLocalContext = false ;

  char* nameG = (char *)malloc(20);
  Standard_CString Type;

  Handle(GEOM_AISShape) result; 
  Handle(GEOM_InteractiveObject) IO;

  if(nbSelected == 1) {
    TopExp_Explorer Exp (compound, TopAbs_ShapeEnum(SubShapeType));
    if(Exp.More()) {
      if(myGeomBase->GetShapeTypeString(Exp.Current(),Type)) {
	aResult->NameType(Type);
	sprintf (nameG, "%s_%d", Type, myGeomBase->GetIndex( Exp.Current(), mainTopo, SubShapeType));
      } 
      else {
	aResult->NameType(tr("GEOM_SHAPE"));
	sprintf (nameG, "%s_%d", tr("GEOM_SHAPE").latin1(), myGeomGUI->myNbGeom++);
      }
      result = new GEOM_AISShape(Exp.Current(), nameG);
      IO = new GEOM_InteractiveObject(aResult->Name(), myGeomGUI->GetFatherior(), "GEOM");
    }
  }
  else {
    if ( myGeomBase->GetShapeTypeString(compound,Type)) {
      aResult->NameType(Type);
      sprintf (nameG, "%s_%d", Type, myGeomGUI->myNbGeom++);
    } else {
      aResult->NameType(tr("GEOM_SHAPE"));
      sprintf (nameG, "%s_%d", tr("GEOM_SHAPE").latin1(), myGeomGUI->myNbGeom++);
    }
    result = new GEOM_AISShape(compound, nameG);
    IO = new GEOM_InteractiveObject(aResult->Name(), myGeomGUI->GetFatherior(), "GEOM");
  }
  
  SALOMEDS::SObject_var SO = aStudy->FindObjectIOR(aResult->Name());
  
  /* open transaction */
  QAD_Operation* op = new SALOMEGUI_ImportOperation(QAD_Application::getDesktop()->getActiveStudy());
  op->start();
  
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::SObject_var fatherSF = aStudy->FindObjectID(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->entry());
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var aName;
  SALOMEDS::AttributeIOR_var anIOR;
  SALOMEDS::AttributePixMap_var aPixmap;
 
  bool allreadyexist = false;

  if(SO->_is_nil()) {
    SALOMEDS::SObject_var newObj = aStudyBuilder->NewObject(theObj);
    anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
    anIOR->SetValue(aResult->Name());
    anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeName");
    aName = SALOMEDS::AttributeName::_narrow(anAttr);
    aName->SetValue(result->getName());
    
    anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributePixMap");
    aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
    if(result->Shape().ShapeType() == TopAbs_COMPOUND)
      aPixmap->SetPixMap("ICON_OBJBROWSER_COMPOUND");
    else if(result->Shape().ShapeType() == TopAbs_COMPSOLID)
      aPixmap->SetPixMap("ICON_OBJBROWSER_COMPSOLID");
    else if(result->Shape().ShapeType() == TopAbs_SOLID)
      aPixmap->SetPixMap("ICON_OBJBROWSER_SOLID");
    else if(result->Shape().ShapeType() == TopAbs_SHELL)
      aPixmap->SetPixMap("ICON_OBJBROWSER_SHELL");
    else if(result->Shape().ShapeType() == TopAbs_FACE)
      aPixmap->SetPixMap("ICON_OBJBROWSER_FACE");
    else if(result->Shape().ShapeType() == TopAbs_WIRE)
      aPixmap->SetPixMap("ICON_OBJBROWSER_WIRE");
    else if(result->Shape().ShapeType() == TopAbs_EDGE)
      aPixmap->SetPixMap("ICON_OBJBROWSER_EDGE");
    else if(result->Shape().ShapeType() == TopAbs_VERTEX)
      aPixmap->SetPixMap("ICON_OBJBROWSER_VERTEX");

    SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
    aStudyBuilder->Addreference(newObj1, newObj);
    
    IO->setEntry(newObj->GetID());
    aResult->StudyShapeId(newObj->GetID());
  } 
  else {
    allreadyexist = true;
    if(!this->SObjectExist(theObj, aResult->Name())) {
      SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(theObj);
      aStudyBuilder->Addreference(newObj1, SO);
    
      IO->setEntry(SO->GetID());
      aResult->StudyShapeId(SO->GetID());
    }
  }
  
  /* commit transaction */
  op->finish();
  
  result->setIO(IO);
  result->setName(nameG);

  if(!allreadyexist)
    ic->Display(result);

  DisplayGUI* myDisplayGUI = new DisplayGUI();
  myDisplayGUI->OnDisplayAll(true);

  QAD_Application::getDesktop()->getActiveStudy()->updateObjBrowser();
  QAD_Application::getDesktop()->putInfo (tr("GEOM_PRP_READY"));
  return true;
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  bool OnGUIEvent(int theCommandID, QAD_Desktop* parent)
  {return EntityGUI::OnGUIEvent(theCommandID, parent);}
}
