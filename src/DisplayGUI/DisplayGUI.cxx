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
//  File   : DisplayGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

using namespace std;
#include "DisplayGUI.h"

#include "QAD_RightFrame.h"
#include "GEOM_AssemblyBuilder.h"
#include "VTKViewer_ViewFrame.h"
#include "OCCViewer_ViewFrame.h"
#include "OCCViewer_Viewer3d.h"
#include "SALOMEGUI_ImportOperation.h"
#include "SALOME_ListIteratorOfListIO.hxx"

#include <AIS_ListIteratorOfListOfInteractive.hxx>

//=======================================================================
// function : DisplayGUI()
// purpose  : Constructor
//=======================================================================
DisplayGUI::DisplayGUI() :
  QObject()
{
  myGeomBase = new GEOMBase();
  myGeomGUI = GEOMContext::GetGeomGUI();
  myGeom = myGeomGUI->myComponentGeom;
}


//=======================================================================
// function : ~DisplayGUI()
// purpose  : Destructor
//=======================================================================
DisplayGUI::~DisplayGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool DisplayGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  DisplayGUI* myDisplayGUI = new DisplayGUI();

  switch (theCommandID)
    {
    case 211: // MENU VIEW - WIREFRAME/SHADING
      {
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
	  QApplication::setOverrideCursor(waitCursor);

	  VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();

	  int themode = myRenderInter->GetDisplayMode();
	  if(themode==0) {
	    myRenderInter->SetDisplayMode(1);
	    QAD_Application::getDesktop()->menuBar()->changeItem(211, tr("GEOM_MEN_WIREFRAME"));
	  } 
	  else {
	    myRenderInter->SetDisplayMode(0);
	    QAD_Application::getDesktop()->menuBar()->changeItem(211, tr("GEOM_MEN_SHADING"));
	  }

	  QApplication::restoreOverrideCursor(); 	  
	} 
	else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  QApplication::setOverrideCursor(Qt::waitCursor);

	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	  Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
	  AIS_DisplayMode mode = (AIS_DisplayMode)ic->DisplayMode();
	  AIS_DisplayMode newmode = (mode == AIS_WireFrame ? AIS_Shaded : AIS_WireFrame);

	  AIS_ListOfInteractive List;
	  ic->DisplayedObjects(List);
	  AIS_ListOfInteractive List1;
	  ic->ObjectsInCollector(List1);
	  List.Append(List1);
	  
	  AIS_ListIteratorOfListOfInteractive ite(List);
	  while(ite.More()) {
	    if(ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	      Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	      ic->SetDisplayMode(aSh,Standard_Integer(newmode),true);
	    }
	    ite.Next();
	  }

	  ic->SetDisplayMode(newmode, Standard_False);
	  if(newmode == 1)
	    QAD_Application::getDesktop()->menuBar()->changeItem(211, tr("GEOM_MEN_WIREFRAME"));
	  else
	    QAD_Application::getDesktop()->menuBar()->changeItem(211, tr("GEOM_MEN_SHADING"));
	  
	  QApplication::restoreOverrideCursor();
	}
	break;
      }
    case 212: // MENU VIEW - DISPLAY ALL
      {
	myDisplayGUI->OnDisplayAll();
	break;
      }
    case 213: // MENU VIEW - DISPLAY ONLY
      {
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK)
	  myDisplayGUI->OnVTKDisplayOnly();
	else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC)
	  myDisplayGUI->OnDisplayOnly();	
	break;
      }
    case 214: // MENU VIEW - ERASE ALL
      {
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK)
	  ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor()->EraseAll();
	else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	  Handle(AIS_InteractiveContext) ic = v3d->getAISContext();

	  ic->EraseAll(Standard_True, Standard_False);
	  ic->Display(v3d->getTrihedron());
	}
      	break;
      }
    case 215: // MENU VIEW - ERASE ONLY
      {
	myDisplayGUI->OnErase();
	break;
      }
    case 8031: // POPUP VIEWER - WIREFRAME/SHADING
      {
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
	  // VTK
	  VTKViewer_RenderWindowInteractor* myRenderInter = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
	  
	  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
	  SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
	  
	  QApplication::setOverrideCursor(Qt::waitCursor);
	  for(;It.More();It.Next()) {
	    Handle(SALOME_InteractiveObject) IOS = It.Value();
	    myRenderInter->SwitchRepresentation(IOS, false);
	  }
	  myRenderInter->Render();
	  QApplication::restoreOverrideCursor();
	}
	else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  QApplication::setOverrideCursor(Qt::waitCursor);
	  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	  Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
	  SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
	  for(;It.More();It.Next()) {
	    Handle(SALOME_InteractiveObject) IObject = It.Value();
	    Standard_Boolean found;
	    Handle(GEOM_AISShape) Shape = myDisplayGUI->myGeomBase->ConvertIOinGEOMAISShape(IObject, found, true);
            if(!Shape.IsNull()) {
  	      AIS_DisplayMode mode = (AIS_DisplayMode)Shape->DisplayMode();
	      if(mode == -1)
	        mode = (AIS_DisplayMode)ic->DisplayMode();

	      ic->SetDisplayMode(Shape, mode == AIS_WireFrame ? AIS_Shaded : AIS_WireFrame, false);
	    }
	  }
	  ic->UpdateCurrentViewer();
	}
	QApplication::restoreOverrideCursor();
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


//=================================================================================
// function : BuildPresentation()
// purpose  : static
//=================================================================================
void DisplayGUI::BuildPresentation(const Handle(SALOME_InteractiveObject)& theIO)
{
  DisplayGUI* myDisplayGUI = new DisplayGUI();

  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  
  OCCViewer_Viewer3d* v3d;
  Handle(AIS_InteractiveContext) ic;
  vtkRenderer* Renderer;

  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    ic = v3d->getAISContext();
  } 
  else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
    Renderer = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
  } 
  else
    return;

  if(theIO.IsNull())
    MESSAGE("BuildPresentation(): null SALOME_InteractiveObject passed")

  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
    // VTK
	  
    SALOMEDS::SObject_var fatherSF = aStudy->FindObjectID(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->entry());
	    
    SALOMEDS::SObject_var obj = aStudy->FindObjectID(theIO->getEntry());

    VTKViewer_RenderWindowInteractor* myRenderInter = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();

    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeName_var aName;
    SALOMEDS::AttributeIOR_var anIOR;
	 
    if(myRenderInter->isInViewer(theIO)) {
      myRenderInter->Display(theIO, false);
    }
    else {
      // Create new actor
      if(!obj->_is_nil()) {
	if(obj->FindAttribute(anAttr, "AttributeIOR")) {
	  // this SObject may be GEOM module root SObject

	  bool useSubItems = false;
	  SALOMEDS::ChildIterator_var anIter = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument()->NewChildIterator(obj);
	  if(myDisplayGUI->myGeom->GetIORFromString(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value())->_is_nil()) {
	    while (anIter->More() && !useSubItems) {
	      SALOMEDS::SObject_var subobj = anIter->Value();
	      SALOMEDS::GenericAttribute_var aTmpAttr;
	      if (subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		if (!myDisplayGUI->myGeom->GetIORFromString(SALOMEDS::AttributeIOR::_narrow(aTmpAttr)->Value())->_is_nil()) {
		  anAttr = aTmpAttr;
		  obj = subobj;
		  useSubItems = true;
		} 
		else 
		  anIter->Next();
	      } 
	      else 
		anIter->Next();
	    }
	  }

	  while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
	    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	    GEOM::GEOM_Shape_var aShape = myDisplayGUI->myGeom->GetIORFromString(anIOR->Value());
	    TopoDS_Shape Shape = myDisplayGUI->myGeomGUI->GetShapeReader().GetShape(myDisplayGUI->myGeom,aShape);
	    
	    if(obj->FindAttribute(anAttr, "AttributeName")) {
	      aName = SALOMEDS::AttributeName::_narrow(anAttr);
	      
	      vtkRenderer* theRenderer = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
	      vtkActorCollection* theAllActors = theRenderer->GetActors();
	      theAllActors->InitTraversal();
	      vtkActor* actor = (vtkActor*)theAllActors->GetNextActor();
	      Handle(SALOME_InteractiveObject) anIObj;
	      // don't create new study object if it already exists
	      bool isDisplayed = false;
	      while(!(actor==NULL)) {
		SALOME_Actor* Gactor = SALOME_Actor::SafeDownCast(actor);
		if(Gactor!=NULL) {
		  if(Gactor->hasIO()) {
		    if(strcmp(Gactor->getIO()->getEntry(),obj->GetID())==0) {
		      isDisplayed = true;
		      anIObj = Gactor->getIO();
		      if(!anIObj.IsNull()) myRenderInter->Display(anIObj, false);
		    }
		  }
		}
		actor=(vtkActor*)(theAllActors->GetNextActor());
	      }
	      if(!isDisplayed) {
		// open transaction
		QAD_Operation* op = new SALOMEGUI_ImportOperation( QAD_Application::getDesktop()->getActiveStudy() );
		op->start();
		
		SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
		aStudyBuilder->Addreference(newObj1, obj);
		// commit transaction
		op->finish();
		
		vtkRenderWindow *renWin = theRenderer->GetRenderWindow();
		int themode = myRenderInter->GetDisplayMode();
		
		vtkActorCollection* theActors = 
		  GEOM_AssemblyBuilder::BuildActors(Shape,0,themode,Standard_True);
		theActors->InitTraversal();
		vtkActor* anActor = (vtkActor*)theActors->GetNextActor();
		while(!(anActor==NULL)) {
		  GEOM_Actor* GActor = GEOM_Actor::SafeDownCast(anActor);
		  Handle(GEOM_InteractiveObject) IO = new GEOM_InteractiveObject(anIOR->Value(), myDisplayGUI->myGeomGUI->GetFatherior(),"GEOM");
		  IO->setEntry(obj->GetID());
		  GActor->setIO(IO);
		  GActor->setName(theIO->getName());
		  
		  theRenderer->AddActor(GActor);
		  //		  renWin->Render();
		  anActor = (vtkActor*)theActors->GetNextActor();
		}
	      }
	    }
	    // next item iteration
	    if(useSubItems) {
	      anIter->Next();
	      anAttr = SALOMEDS::GenericAttribute::_nil();
	      while(anIter->More() && anAttr->_is_nil()) {
		SALOMEDS::SObject_var subobject = anIter->Value();
		SALOMEDS::GenericAttribute_var aTmpAttribute;
		if (subobject->FindAttribute(aTmpAttribute, "AttributeIOR")) {
		  anAttr = aTmpAttribute;
		  obj = subobject;
		} 
		else
		  anIter->Next();
	      }
	    } 
	    else
	      anAttr = SALOMEDS::GenericAttribute::_nil();
	  }
	}
      }
    }
    // No viewer update should be done here!
    //myRenderInter->Render();
    //QAD_Application::getDesktop()->getActiveStudy()->updateObjBrowser( true );
  } 
  else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    SALOMEDS::SObject_var fatherSF = aStudy->FindObjectID(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->entry());
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeName_var aName;
    SALOMEDS::AttributeIOR_var anIOR;

    if(v3d->isInViewer(theIO, true)) {
      Standard_Boolean found;
      Handle(GEOM_AISShape) aSh = myDisplayGUI->myGeomBase->ConvertIOinGEOMAISShape(theIO, found, true);
      if(found) {
	ic->Display(aSh, false);
	ic->AddOrRemoveCurrentObject(aSh, false);
      }
      
    } 
    else {
      SALOMEDS::SObject_var obj = aStudy->FindObjectID(theIO->getEntry());	      
      if(!obj->_is_nil()) {
	MESSAGE("BuildPresentation(): SObject not null")
	if(obj->FindAttribute(anAttr, "AttributeIOR")) {
	  MESSAGE("BuildPresentation(): SObject has IOR")
	  // this SObject may be GEOM module root SObject

	  bool useSubItems = false;
	  SALOMEDS::ChildIterator_var anIter = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument()->NewChildIterator(obj);
	  if(myDisplayGUI->myGeom->GetIORFromString(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value())->_is_nil()) {
	    while(anIter->More() && !useSubItems) {
	      SALOMEDS::SObject_var subobj = anIter->Value();
	      SALOMEDS::GenericAttribute_var aTmpAttr;
	      if(subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		if(!myDisplayGUI->myGeom->GetIORFromString(SALOMEDS::AttributeIOR::_narrow(aTmpAttr)->Value())->_is_nil()) {
		  anAttr = aTmpAttr;
		  obj = subobj;
		  useSubItems = true;
		} 
		else
		  anIter->Next();
	      } 
	      else 
		anIter->Next();
	    }
	  }

	  while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
	    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	    GEOM::GEOM_Shape_var aShape = myDisplayGUI->myGeom->GetIORFromString(anIOR->Value());
	    TopoDS_Shape Shape = myDisplayGUI->myGeomGUI->GetShapeReader().GetShape(myDisplayGUI->myGeom,aShape);
	    if(Shape.IsNull())
	      MESSAGE("BuildPresentation(): TopoDS_Shape is null!")
	    if(obj->FindAttribute(anAttr, "AttributeName")) {
	      MESSAGE("BuildPresentation(): SObject has Name")
	      aName = SALOMEDS::AttributeName::_narrow(anAttr);
	      // searchin for already displayed objects with the same shape
	      AIS_ListOfInteractive aDisplayed;
	      ic->DisplayedObjects(aDisplayed);
	      AIS_ListIteratorOfListOfInteractive anIObjects(aDisplayed);
	      Handle(AIS_Shape) anAISShape;
	      for(;anIObjects.More();anIObjects.Next()) {
		anAISShape = Handle(AIS_Shape)::DownCast(anIObjects.Value());
		if(!anAISShape.IsNull()) {
		  if(anAISShape->Shape().IsSame(Shape)) 
		    break;
		  anAISShape.Nullify();
		}
	      }
	      if(!anAISShape.IsNull()) {
		if(!ic->IsDisplayed(anAISShape)) 
		  ic->Display(anAISShape, false);
	      } 
	      else {
		if(!useSubItems) {
		  // open transaction
		  QAD_Operation* op = new SALOMEGUI_ImportOperation( QAD_Application::getDesktop()->getActiveStudy() );
		  op->start();
		  if (fatherSF->_is_nil())
		    MESSAGE("BuildPresentation(): fatherSF is nil!")
		  SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
		  aStudyBuilder->Addreference(newObj1, obj);
		  // commit transaction
		  op->finish();
		}
		Handle(GEOM_AISShape) aSh = new GEOM_AISShape(Shape, aName->Value());
		aSh->SetShadingColor(myDisplayGUI->myGeomBase->myShadingColor);
		Handle(GEOM_InteractiveObject) IO = new GEOM_InteractiveObject(anIOR->Value(), myDisplayGUI->myGeomGUI->GetFatherior(), "GEOM");
		IO->setEntry(obj->GetID());
		aSh->setIO(IO);
		aSh->setName(aName->Value());
		ic->Display(aSh, false);
		if(!useSubItems) 
		  ic->AddOrRemoveCurrentObject(aSh, false);
	      }
	    }
	    // next item iteration
	    if(useSubItems) {
	      anIter->Next();
	      anAttr=SALOMEDS::GenericAttribute::_nil();
	      while (anIter->More() && anAttr->_is_nil()) {
		SALOMEDS::SObject_var subobject = anIter->Value();
		SALOMEDS::GenericAttribute_var aTmpAttribute;
		if(subobject->FindAttribute(aTmpAttribute, "AttributeIOR")) {
		  anAttr = aTmpAttribute;
		  obj = subobject;
		} 
		else 
		  anIter->Next();
	      }
	    } 
	    else 
	      anAttr = SALOMEDS::GenericAttribute::_nil();
	  }
	}
      }
    }
    // No viewer update should be done here!
    //QAD_Application::getDesktop()->getActiveStudy()->updateObjBrowser( true );
    //ic->UpdateCurrentViewer();
  }
}


//=====================================================================================
// function : OnDisplayAll()
// purpose  :
//=====================================================================================
void DisplayGUI::OnDisplayAll(bool onlyPreviousDisplayedObject)
{
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    Handle (AIS_InteractiveContext) myContext = v3d->getAISContext();

    //myContext->Display(v3d->getTrihedron());

    if(!onlyPreviousDisplayedObject) {
      SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
      SALOMEDS::SComponent_var SC = aStudy->FindComponent("GEOM");
      SALOMEDS::ChildIterator_var it = aStudy->NewChildIterator(SC);
      for(; it->More();it->Next()) {
	SALOMEDS::SObject_var CSO = it->Value();
	SALOMEDS::AttributeIOR_var anIOR;
	SALOMEDS::GenericAttribute_var anAttr;
	if(CSO->FindAttribute(anAttr, "AttributeIOR")) {
	  anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	  Standard_Boolean testResult;
	  Handle(GEOM_AISShape) aSh = myGeomBase->ConvertIORinGEOMAISShape(anIOR->Value(), testResult);
	  if(testResult)
	    myContext->Display(aSh);
	  else {
	    GEOM::GEOM_Shape_ptr aShape = myGeom->GetIORFromString(anIOR->Value());
	    bool AddInSt = myGeomBase->mySettings_AddInStudy;
	    myGeomBase->mySettings_AddInStudy = false;
	    myGeomBase->Display(aShape);
	    myGeomBase->mySettings_AddInStudy = AddInSt;
	  }
	}
      }
    }
    else {
      AIS_ListOfInteractive aListDisplayedObject;
      myContext->DisplayedObjects(aListDisplayedObject);
      AIS_ListIteratorOfListOfInteractive ite(aListDisplayedObject);
      while(ite.More()) {
	if(ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	  Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	  if (aSh->hasIO()) {
	    Handle(GEOM_InteractiveObject) GIO = Handle(GEOM_InteractiveObject)::DownCast(aSh->getIO());
	    if (v3d->isInViewer(GIO,true))
	      myContext->Display(aSh);
	  }
	}
	ite.Next();
      }
    }
  }
  else if (QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
    SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
    SALOMEDS::SComponent_var SC = aStudy->FindComponent("GEOM");
    SALOMEDS::ChildIterator_var it = aStudy->NewChildIterator(SC);
    for(; it->More();it->Next()) {
      SALOMEDS::SObject_var CSO = it->Value();
      SALOMEDS::AttributeIOR_var anIOR;
      SALOMEDS::GenericAttribute_var anAttr;
      if(CSO->FindAttribute(anAttr, "AttributeIOR")) {
	anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	Standard_Boolean testResult;
	GEOM_Actor* aSh = myGeomBase->ConvertIORinGEOMActor(anIOR->Value(), testResult);
	if(testResult) {
	  Handle(SALOME_InteractiveObject) IObject = aSh->getIO();
	  ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor()->Display(IObject);
	}
	else {
	  GEOM::GEOM_Shape_ptr aShape = myGeom->GetIORFromString(anIOR->Value());
	  bool AddInSt = myGeomBase->mySettings_AddInStudy;
	  myGeomBase->mySettings_AddInStudy = false;
	  myGeomBase->Display(aShape);
	  myGeomBase->mySettings_AddInStudy = AddInSt;
	}
      }
    }
  }
  return;
}


//=====================================================================================
// function : OnVTKDisplayOnly()
// purpose  :
//=====================================================================================
void DisplayGUI::OnVTKDisplayOnly()
{
  QApplication::setOverrideCursor(Qt::waitCursor);

  // Erase all not selected actors
  vtkRenderer* aren = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
  vtkActorCollection* theActors = aren->GetActors();
  theActors->InitTraversal();
  vtkActor *ac = theActors->GetNextActor();
  while(!(ac==NULL)) {
    if(ac->IsA("SALOME_Actor")) {
      SALOME_Actor* anActor = SALOME_Actor::SafeDownCast(ac);
      if(!anActor->isHighlighted())
	anActor->VisibilityOff();
    }
    ac = theActors->GetNextActor();
  }

  // Display selection
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::SObject_var fatherSF = aStudy->FindObjectID(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->entry());
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var aName;
  SALOMEDS::AttributeIOR_var anIOR;

  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
	  
  for(;It.More();It.Next()) {
    Handle(SALOME_InteractiveObject) IObject = It.Value();
    SALOMEDS::SObject_var obj = aStudy->FindObjectID( IObject->getEntry() );

    VTKViewer_RenderWindowInteractor* myRenderInter = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();

    if(myRenderInter->isInViewer(IObject)) {
      myRenderInter->Display(IObject);
    }
    else {
      // Create new actor
      if(!obj->_is_nil()) {
	if(!obj->FindAttribute(anAttr, "AttributeIOR")) 
	  break;
	// If selected object contains displayable subobjects, then do nothing
	SALOMEDS::ChildIterator_var anIter = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument()->NewChildIterator(obj);
	SALOMEDS::GenericAttribute_var aTmpAttr;
	
	anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(anIOR->Value());
	if(CORBA::is_nil(aShape)) continue;
	TopoDS_Shape Shape = myGeomGUI->GetShapeReader().GetShape(myGeom, aShape);

	if(obj->FindAttribute(anAttr, "AttributeName")) {
          aName = SALOMEDS::AttributeName::_narrow(anAttr);
	  // open transaction
	  QAD_Operation* op = new SALOMEGUI_ImportOperation(QAD_Application::getDesktop()->getActiveStudy());
	  op->start();

	  SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
	  aStudyBuilder->Addreference(newObj1, obj);
	  // commit transaction
	  op->finish();
		  
	  vtkRenderer* theRenderer = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
	  int themode = myRenderInter->GetDisplayMode();
	  vtkActorCollection* theActors = GEOM_AssemblyBuilder::BuildActors(Shape, 0, themode, Standard_True);
	  theActors->InitTraversal();
	  vtkActor* anActor = (vtkActor*)theActors->GetNextActor();
	  while(!(anActor==NULL)) {
	    GEOM_Actor* GActor = GEOM_Actor::SafeDownCast(anActor);
	    GActor->setIO(IObject);
	    GActor->setName(IObject->getName());
		    
	    theRenderer->AddActor(GActor);
	    vtkRenderWindow *renWin = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer()->GetRenderWindow();
	    renWin->Render();
	    anActor = (vtkActor*)theActors->GetNextActor();
	  }
	}
      }
    }
  }
  QAD_Application::getDesktop()->getActiveStudy()->updateObjBrowser(true);

  QApplication::restoreOverrideCursor();
  return;		       
}    


//=====================================================================================
// function : OnDisplayOnly()
// purpose  :
//=====================================================================================
void DisplayGUI::OnDisplayOnly()
{
  if (QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return;

  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();

  AIS_ListOfInteractive List;
  ic->DisplayedObjects(List);
  AIS_ListIteratorOfListOfInteractive ite(List);
  while(ite.More()) {
    if(ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
      Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
      if (!ic->IsSelected( aSh )) {
	ic->Erase(aSh, Standard_True, Standard_True);
      }
    }
    ite.Next();
  }

  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());

  SALOME_ListIteratorOfListIO It1(Sel->StoredIObjects());
  for(;It1.More();It1.Next()) {
    Handle(SALOME_InteractiveObject) IObject = It1.Value();

    SALOMEDS::SObject_var fatherSF = 
      aStudy->FindObjectID(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->entry());
    if(v3d->isInViewer(IObject, true)) {
      AIS_ListOfInteractive List1;
      ic->ObjectsInCollector(List1);
      AIS_ListIteratorOfListOfInteractive ite1(List1);
      while(ite1.More()) {
	if(ite1.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	  Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite1.Value());
	  if (aSh->hasIO()) {
	    Handle(GEOM_InteractiveObject) GIO = Handle(GEOM_InteractiveObject)::DownCast(aSh->getIO());
	    if (IObject->isSame(GIO)) {
	      ic->Display(aSh);
	      ic->AddOrRemoveCurrentObject(aSh, true);
	      break;
	    }
	  }
	}
	ite1.Next();
      }
    }
    else {
      if (IObject->hasEntry()) {
	SALOMEDS::SObject_var obj = aStudy->FindObjectID(IObject->getEntry());
        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeName_var aName;
        SALOMEDS::AttributeIOR_var anIOR;
	if (!obj->_is_nil()) {
	  if (obj->FindAttribute(anAttr, "AttributeIOR")) {
	    // this SObject may be GEOM module root SObject

	    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	    GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(anIOR->Value());
	    if(CORBA::is_nil(aShape)) continue;
	    TopoDS_Shape Shape = myGeomGUI->GetShapeReader().GetShape(myGeom, aShape);
	    
	    if(obj->FindAttribute(anAttr, "AttributeName")) {
	      aName = SALOMEDS::AttributeName::_narrow(anAttr);
	      // open transaction
	      QAD_Operation* op = new SALOMEGUI_ImportOperation(QAD_Application::getDesktop()->getActiveStudy());
	      op->start();
	      
	      SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
	      aStudyBuilder->Addreference(newObj1, obj);
	      // commit transaction
	      op->finish();
	    }
	    
	    Handle(GEOM_AISShape) aSh = new GEOM_AISShape(Shape, aName->Value());
	    Handle(GEOM_InteractiveObject) IO = new GEOM_InteractiveObject(anIOR->Value(), myGeomGUI->GetFatherior(),"GEOM");
		
	    IO->setEntry(obj->GetID());
	    aSh->setIO(IO);
	    aSh->setName(aName->Value());
	    ic->Display(aSh);
	    ic->AddOrRemoveCurrentObject(aSh, true);
	  }
	}  
      }
    }
  }
}


//=====================================================================================
// function : OnErase()
// purpose  :
//=====================================================================================
void DisplayGUI::OnErase()
{
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
    // VTK
    SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
    Handle(SALOME_InteractiveObject) anIObject;
    for(;It.More();It.Next()) {
      anIObject = It.Value();
      VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();

      if(myRenderInter->isInViewer(anIObject)) {
	myRenderInter->Erase(anIObject);
      } 
      else {
	SALOMEDS::SObject_var obj = aStudy->FindObjectID(anIObject->getEntry());
	SALOMEDS::GenericAttribute_var anAttr;
	SALOMEDS::AttributeIOR_var anIOR;
	if(!obj->_is_nil()) {
	  if(obj->FindAttribute(anAttr, "AttributeIOR")) {
	    // this SObject may be GEOM module root SObject
	    SALOMEDS::ChildIterator_var anIter = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument()->NewChildIterator(obj);
	    bool useSubItems = false;
	    while(anIter->More() && !useSubItems) {
	      SALOMEDS::SObject_var subobj = anIter->Value();
	      SALOMEDS::GenericAttribute_var aTmpAttr;
	      if(subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		anAttr = aTmpAttr;
		obj = subobj;
		useSubItems = true;
	      } 
	      else
		anIter->Next();
	    }

	    while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
	      anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	      GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(anIOR->Value());
	      TopoDS_Shape Shape = myGeomGUI->GetShapeReader().GetShape(myGeom, aShape);
	      if(obj->FindAttribute(anAttr, "AttributeName")) {
		// searchin for already displayed objects with the same shape
		vtkRenderer* theRenderer = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
		vtkActorCollection* theAllActors = theRenderer->GetActors();
		theAllActors->InitTraversal();
		vtkActor* actor = (vtkActor*)theAllActors->GetNextActor();
		Handle(SALOME_InteractiveObject) anIObj;
		// don't create new study object if it already exists
		bool isDisplayed = false;
		while(!(actor==NULL)) {
		  SALOME_Actor* Gactor = SALOME_Actor::SafeDownCast(actor);
		  if(Gactor!=NULL) {
		    if(Gactor->hasIO()) {
		      if(strcmp(Gactor->getIO()->getEntry(),obj->GetID())==0) {
			isDisplayed = true;
			anIObj = Gactor->getIO();
			if(!anIObj.IsNull())
			  myRenderInter->Erase(anIObj);
		      }
		    }
		  }
		  actor=(vtkActor*)(theAllActors->GetNextActor());
		}
	      }
	      if(useSubItems) {
		anIter->Next();
		anAttr=SALOMEDS::GenericAttribute::_nil();
		while(anIter->More() && anAttr->_is_nil()) {
		  SALOMEDS::SObject_var subobject = anIter->Value();
		  SALOMEDS::GenericAttribute_var aTmpAttribute;
		  if(subobject->FindAttribute(aTmpAttribute, "AttributeIOR")) {
		    anAttr = aTmpAttribute;
		    obj = subobject;
		  }
		  else
		    anIter->Next();
		}
	      } 
	      else
		anAttr = SALOMEDS::GenericAttribute::_nil();
	    } 
	  }
	}
      }
    }
  }
  else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    // OCC
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
    SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
    Handle(SALOME_InteractiveObject) anIObject;
    for(;It.More();It.Next()) {
      anIObject = It.Value();
      if(v3d->isInViewer(anIObject, true)) {
	Standard_Boolean found;
	Handle(GEOM_AISShape) aSh = myGeomBase->ConvertIOinGEOMAISShape(anIObject, found, true);
	if(found) {
	  ic->Erase(aSh);
	  ic->AddOrRemoveCurrentObject(aSh, true);
	}
      } 
      else {
	SALOMEDS::SObject_var obj = aStudy->FindObjectID(anIObject->getEntry());
	SALOMEDS::GenericAttribute_var anAttr;
	SALOMEDS::AttributeIOR_var anIOR;
	if(!obj->_is_nil()) {
	  if(obj->FindAttribute(anAttr, "AttributeIOR")) {
	    // this SObject may be GEOM module root SObject
	    SALOMEDS::ChildIterator_var anIter = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument()->NewChildIterator(obj);
	    bool useSubItems = false;
	    while(anIter->More() && !useSubItems) {
	      SALOMEDS::SObject_var subobj = anIter->Value();
	      SALOMEDS::GenericAttribute_var aTmpAttr;
	      if(subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		anAttr = aTmpAttr;
		obj = subobj;
		useSubItems = true;
	      } 
	      else
		anIter->Next();
	    }

	    while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
	      anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	      GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(anIOR->Value());
	      TopoDS_Shape Shape = myGeomGUI->GetShapeReader().GetShape(myGeom, aShape);
	      if(obj->FindAttribute(anAttr, "AttributeName")) {
		// searchin for already displayed objects with the same shape
		AIS_ListOfInteractive aDisplayed;
		ic->DisplayedObjects(aDisplayed);
		AIS_ListIteratorOfListOfInteractive anIObjects(aDisplayed);
		Handle(AIS_Shape) anAISShape;
		for(;anIObjects.More();anIObjects.Next()) {
		  anAISShape = Handle(AIS_Shape)::DownCast(anIObjects.Value());
		  if(!anAISShape.IsNull()) {
		    if(anAISShape->Shape().IsSame(Shape))
		      break;
		    anAISShape.Nullify();
		  }
		}
		if(!anAISShape.IsNull()) {
		  if(ic->IsDisplayed(anAISShape)) 
		    ic->Erase(anAISShape);
		}
	      }
	      if(useSubItems) {
		anIter->Next();
		anAttr=SALOMEDS::GenericAttribute::_nil();
		while(anIter->More() && anAttr->_is_nil()) {
		  SALOMEDS::SObject_var subobject = anIter->Value();
		  SALOMEDS::GenericAttribute_var aTmpAttribute;
		  if(subobject->FindAttribute(aTmpAttribute, "AttributeIOR")) {
		    anAttr = aTmpAttribute;
		    obj = subobject;
		  }
		  else
		    anIter->Next();
		}
	      }
	      else
		anAttr = SALOMEDS::GenericAttribute::_nil();
	    }
	  }
	}
      }
    }
  }
  Sel->ClearIObjects();
}


//=====================================================================================
// function : PrepareSubShapeSelection()
// purpose  : (localContextId of the method is opened and defined here)
//=====================================================================================
bool DisplayGUI::PrepareSubShapeSelection(const int SubShapeType, Standard_Integer& returnLocalContextId)
{
  //* Test the type of viewer */
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return false;
  
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  
  /* local context opening */
  //myGeomBase->SetDisplayedObjectList();
  this->OnDisplayOnly();

  returnLocalContextId = ic->OpenLocalContext(Standard_False, Standard_True, Standard_False, Standard_False);
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
  for(;It.More();It.Next()) {
    Handle(SALOME_InteractiveObject) IObject = It.Value();
    Standard_Boolean found;
    Handle(GEOM_AISShape) Shape = myGeomBase->ConvertIOinGEOMAISShape(IObject, found);
    if(found && SubShapeType >= Shape->Shape().ShapeType()) {
      ic->Load(Shape, (8 - SubShapeType), Standard_True);
      ic->HilightWithColor(Shape, Quantity_NOC_RED);
    }
  }
  QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SELECT_SUBSHAPES"));
  return true;
}


//=====================================================================================
// function : PrepareSubShapeSelectionArgumentShape()
// purpose  : ( localContextId of the method is opened and defined here )
//=====================================================================================
bool DisplayGUI::PrepareSubShapeSelectionArgumentShape(const TopoDS_Shape& aShape,const int SubShapeType, Standard_Integer& returnLocalContextId)
{
  //* Test the type of viewer */
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return false;
  
  if(aShape.IsNull())
    return false ;
  
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  
  /* local context opening */
  //myGeomBase->SetDisplayedObjectList();
  this->OnDisplayOnly();
  
  returnLocalContextId = ic->OpenLocalContext(Standard_False, Standard_True, Standard_False, Standard_False) ;
  
  Handle(GEOM_AISShape) Shape = new GEOM_AISShape(aShape, "");  
  ic->Display(Shape, 0, (8 - SubShapeType));

  //  Not Load(...) but Display(...)
  //  ic->Load(Shape, (8 - SubShapeType), Standard_True);
  ic->HilightWithColor(Shape, Quantity_NOC_RED);
  
  QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SELECT_SUBSHAPES"));
  return true;
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  bool OnGUIEvent(int theCommandID, QAD_Desktop* parent)
  {return DisplayGUI::OnGUIEvent(theCommandID, parent);}
  
  void BuildPresentation(const Handle(SALOME_InteractiveObject)& theIO)
  {return DisplayGUI::BuildPresentation(theIO);}
}
