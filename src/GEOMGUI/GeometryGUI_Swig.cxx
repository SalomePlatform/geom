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
//  File   : GeometryGUI_Swig.cxx
//  Author : Nicolas REJNERI, Paul RASCLE
//  Module : GEOM
//  $Header$

#include "GeometryGUI_Swig.hxx"
#include "SALOMEGUI_Swig.hxx"

#include "GeometryGUI.h"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_ImportOperation.h"

#include "OB_Browser.h"

#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewManager.h"
#include "SOCC_ViewModel.h"

#include "SVTK_ViewModel.h"
#include "SVTK_ViewWindow.h"
#include "SVTK_View.h"
#include "SVTK_Renderer.h"

#include "GEOM_Actor.h"
#include "GEOM_Client.hxx"
#include "GEOM_AISShape.hxx"
#include "GEOM_AssemblyBuilder.h"
#include "GEOM_InteractiveObject.hxx"

#include "SALOME_Event.hxx"

#include "utilities.h"

#include "SALOMEDSClient.hxx"

// OCCT Includes
#include <TopAbs.hxx>
#include <TopoDS_Shape.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

using namespace std;


static GEOM_Client ShapeReader;

inline OCCViewer_Viewer* GetOCCViewer(SUIT_Application* theApp){
  SUIT_ViewWindow* window = theApp->desktop()->activeWindow();
  if(window && window->getViewManager()->getType() == OCCViewer_Viewer::Type()){
    OCCViewer_ViewWindow* vw = dynamic_cast<OCCViewer_ViewWindow*>( window );
    if ( vw ) {
      OCCViewer_ViewManager* vm = dynamic_cast<OCCViewer_ViewManager*>( vw->getViewManager() );
      if ( vm )
	return vm->getOCCViewer();
    }
  }

  return 0;
}

inline SVTK_ViewWindow* GetSVTKViewWindow(SUIT_Application* theApp){
  SUIT_ViewWindow* window = theApp->desktop()->activeWindow();
  if(window && window->getViewManager()->getType() == SVTK_Viewer::Type())
    return dynamic_cast<SVTK_ViewWindow*>( window );

  return 0;
}

GEOM_Swig::GEOM_Swig()
{
  // MESSAGE("Constructeur");
}

GEOM_Swig::~GEOM_Swig()
{
  // MESSAGE("Destructeur");
}

void GEOM_Swig::createAndDisplayGO (const char* Entry)
{
  class TEvent: public SALOME_Event
  {
    std::string myEntry;
  public:
    TEvent(const char* theEntry):
      myEntry(theEntry)
    {}
    virtual void Execute()
    {
      SUIT_Application* app = SUIT_Session::session()->activeApplication();
      if (!app) return;

      SalomeApp_Study* ActiveStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
      if (!ActiveStudy) return;

      _PTR(Study) aStudy(ActiveStudy->studyDS());
      _PTR(StudyBuilder) aStudyBuilder = aStudy->NewBuilder();

      GEOM::GEOM_Gen_var Geom = GeometryGUI::GetGeomGen();
      if (CORBA::is_nil(Geom)) {
        GeometryGUI::InitGeomGen();
        Geom = GeometryGUI::GetGeomGen();
      }
      if (CORBA::is_nil(Geom))
        return;

      string aFatherIOR;
      _PTR(SComponent) father = aStudy->FindComponent("GEOM");
      if (!father)
        return;
      if (!father->ComponentIOR(aFatherIOR)) {
        aStudyBuilder->LoadWith(father, SalomeApp_Application::orb()->object_to_string(Geom));
        father->ComponentIOR(aFatherIOR);
      }

      _PTR(SObject) obj = aStudy->FindObjectID(myEntry);
      if (!obj)
        return;

      // Create new actor
      _PTR(GenericAttribute) anAttr;
      if (!obj->FindAttribute(anAttr, "AttributeIOR"))
        return;
      _PTR(AttributeIOR) anIOR(anAttr);
      string anIORValue = anIOR->Value();

      GEOM::GEOM_Object_var aShape = Geom->GetIORFromString(anIORValue.c_str());
      TopoDS_Shape Shape = ShapeReader.GetShape(Geom,aShape);
      if (!Shape.IsNull()) {
        if (obj->FindAttribute(anAttr, "AttributeName")) {
          _PTR(AttributeName) aName (anAttr);
          string aNameValue = aName->Value();
          // open transaction
          /*SUIT_Operation* op = new SalomeApp_ImportOperation (app);
          op->start();

          _PTR(SObject) newObj1 = aStudyBuilder->NewObject(father);
          aStudyBuilder->Addreference(newObj1, obj);
          // commit transaction
          op->commit();*/
          Handle(GEOM_InteractiveObject) anIO =
            new GEOM_InteractiveObject (const_cast<char*>(anIORValue.c_str()),
                                        const_cast<char*>(aFatherIOR.c_str()),
                                        "GEOM",
                                        const_cast<char*>( obj->GetID().c_str()));

	  if (SVTK_ViewWindow* aViewWindow = GetSVTKViewWindow(app)) {
	    SVTK_View* aView = aViewWindow->getView();
	    int aMode = aView->GetDisplayMode();

	    vtkActorCollection* theActors =
              GEOM_AssemblyBuilder::BuildActors(Shape,0,aMode,true);
	    theActors->InitTraversal();
	    while (vtkActor* anActor = theActors->GetNextActor()) {
	      GEOM_Actor* GActor = GEOM_Actor::SafeDownCast(anActor);
	      GActor->setName(const_cast<char*>(aNameValue.c_str()));
	      GActor->setIO(anIO);
	      aView->Display(GActor);
	    }
	    aView->Repaint();
	  } else if (OCCViewer_Viewer* occViewer = GetOCCViewer(app)) {
	    Handle(AIS_InteractiveContext) ic = occViewer->getAISContext();
	    Handle(GEOM_AISShape) aSh =
              new GEOM_AISShape (Shape,const_cast<char*>(aNameValue.c_str()));
	    aSh->setName(const_cast<char*>(aNameValue.c_str()));
	    aSh->setIO(anIO);
	    ic->Display(aSh);
	    ic->AddOrRemoveCurrentObject(aSh,true);
	  }
	  // update object browser
	  SalomeApp_Application* app = NULL; //dynamic_cast<SalomeApp_Application*>(app);
	  if (app) {
	    CAM_Module* module = app->module("Geometry");
	    SalomeApp_Module* appMod = dynamic_cast<SalomeApp_Module*>(module);
	    if (appMod)
	      appMod->updateObjBrowser(true);
	  }
	}
      }
    }
  };

  // MESSAGE("createAndDisplayGO");
  ProcessVoidEvent(new TEvent (Entry));

  class TEventUpdateBrowser: public SALOME_Event
    {
      public:
	TEventUpdateBrowser() {}
	virtual void Execute() {
          SalomeApp_Application* app =
            dynamic_cast<SalomeApp_Application*>(SUIT_Session::session()->activeApplication());
          if (app) {
	    CAM_Module* module = app->module("Geometry");
	    SalomeApp_Module* appMod = dynamic_cast<SalomeApp_Module*>(module);
	    if (appMod) appMod->updateObjBrowser(true);
	  }
        }
    };

  ProcessVoidEvent(new TEventUpdateBrowser ());
}

void GEOM_Swig::createAndDisplayFitAllGO (const char* Entry)
{
  class TEventFitAll: public SALOME_Event
  {
    public:
      TEventFitAll() {}
      virtual void Execute() {
	SUIT_Application* app = SUIT_Session::session()->activeApplication();
	if (!app) return;
	
	if (SVTK_ViewWindow* aViewWindow = GetSVTKViewWindow(app))
	  {
	    SVTK_View* aView = aViewWindow->getView();
	    aView->GetRenderer()->OnFitAll();
	  }
	else if (OCCViewer_Viewer* occViewer = GetOCCViewer(app))
	  {  
	    Handle(V3d_Viewer) aViewer3d = occViewer->getViewer3d();
	    aViewer3d->InitActiveViews();
	    
	    if (aViewer3d->MoreActiveViews())
	      aViewer3d->ActiveView()->FitAll();
	  }
      }
  };

  createAndDisplayGO(Entry);
  ProcessVoidEvent(new TEventFitAll());
}

int GEOM_Swig::getIndexTopology(const char* SubIOR, const char* IOR)
{
  GEOM::GEOM_Gen_var aGeomGen = GeometryGUI::GetGeomGen();
  if (CORBA::is_nil(aGeomGen))
    return -1;

  GEOM::GEOM_Object_var aMainShape = aGeomGen->GetIORFromString(IOR);
  GEOM::GEOM_Object_var aSubShape  = aGeomGen->GetIORFromString(SubIOR);
  if (CORBA::is_nil(aMainShape) || CORBA::is_nil(aSubShape))
    return -1;

  GEOM::GEOM_IShapesOperations_var anIShapesOperations =
    aGeomGen->GetIShapesOperations(aMainShape->GetStudyID());
  if (CORBA::is_nil(anIShapesOperations))
    return -1;

  return anIShapesOperations->GetTopologyIndex(aMainShape, aSubShape);
}

const char* GEOM_Swig::getShapeTypeString(const char* IOR)
{
  TCollection_AsciiString aTypeName ("Shape of unknown type");

  GEOM::GEOM_Gen_var aGeomGen = GeometryGUI::GetGeomGen();
  if (!CORBA::is_nil(aGeomGen))
  {
    GEOM::GEOM_Object_var aShape = aGeomGen->GetIORFromString(IOR);
    if (!CORBA::is_nil(aShape))
    {
      GEOM::GEOM_IShapesOperations_var anIShapesOperations =
        aGeomGen->GetIShapesOperations(aShape->GetStudyID());
      if (!CORBA::is_nil(anIShapesOperations))
      {
        aTypeName = anIShapesOperations->GetShapeTypeString(aShape);
      }
    }
  }

  return CORBA::string_dup(aTypeName.ToCString());
}


const char* GEOM_Swig::getShapeTypeIcon(const char* IOR)
{
  GEOM::GEOM_Gen_var Geom = GeometryGUI::GetGeomGen();
  if ( CORBA::is_nil( Geom ) )
    return "None";

  GEOM::GEOM_Object_var aShape = Geom->GetIORFromString(IOR);
  TopoDS_Shape shape = ShapeReader.GetShape(Geom, aShape);

  if( shape.IsNull() ) {
    return "None" ;
  }

  switch (shape.ShapeType() )
    {
    case TopAbs_COMPOUND:
      { return "ICON_OBJBROWSER_COMPOUND" ;}
    case  TopAbs_COMPSOLID:
      { return "ICON_OBJBROWSER_COMPSOLID" ;}
    case TopAbs_SOLID:
      { return "ICON_OBJBROWSER_SOLID" ;}
    case TopAbs_SHELL:
      { return "ICON_OBJBROWSER_SHELL" ;}
    case TopAbs_FACE:
      { return "ICON_OBJBROWSER_FACE" ;}
    case TopAbs_WIRE:
      { return "ICON_OBJBROWSER_WIRE" ;}
    case TopAbs_EDGE:
      { return "ICON_OBJBROWSER_EDGE" ;}
    case TopAbs_VERTEX:
      { return "ICON_OBJBROWSER_VERTEX" ;}
    }

  return "None";
}

void GEOM_Swig::setDisplayMode(const char* theEntry, int theMode)
{
  class TEvent: public SALOME_Event {
    std::string myEntry;
    int myMode;
  public:
    TEvent(const char* theEntryArg, int theModeArg):
      myEntry(theEntryArg), myMode(theModeArg)
    {}
    virtual void Execute() {
      SUIT_Application* anApp = SUIT_Session::session()->activeApplication();
      if (!anApp) return;

      Handle(SALOME_InteractiveObject) anIO =
        new SALOME_InteractiveObject(myEntry.c_str(), "GEOM", "");

      if (SVTK_ViewWindow* aViewWindow = GetSVTKViewWindow(anApp)) {
	SVTK_View* aView = aViewWindow->getView();
	aView->SetDisplayMode(anIO, myMode);
	aView->Repaint();
      }
      else if (OCCViewer_Viewer* occViewer = GetOCCViewer(anApp)) {
	SOCC_Viewer* soccViewer = dynamic_cast<SOCC_Viewer*>(occViewer);
	if (soccViewer)
	  soccViewer->switchRepresentation(anIO, myMode);
      }
    }
  };

  ProcessVoidEvent(new TEvent (theEntry, theMode));
}

void GEOM_Swig::setColor(const char* theEntry, int red, int green, int blue)
{
  class TEvent: public SALOME_Event {
    std::string myEntry;
    int myRed;
    int myGreen;
    int myBlue;
  public:
    TEvent(const char* theEntryArg, int theR, int theG, int theB):
      myEntry(theEntryArg), myRed(theR), myGreen(theG), myBlue(theB)
    {}
    virtual void Execute() {
      SUIT_Application* anApp = SUIT_Session::session()->activeApplication();
      if (!anApp) return;

      Handle(SALOME_InteractiveObject) anIO =
        new SALOME_InteractiveObject(myEntry.c_str(), "GEOM", "");

      if (SVTK_ViewWindow* aViewWindow = GetSVTKViewWindow(anApp)){
	SVTK_View* aView = aViewWindow->getView();
        QColor aColor (myRed, myGreen, myBlue);
        aView->SetColor(anIO, aColor);
	aView->Repaint();
      } else if (OCCViewer_Viewer* occViewer = GetOCCViewer(anApp)) {
	Handle(AIS_InteractiveContext) ic = occViewer->getAISContext();
	AIS_ListOfInteractive List;
	ic->DisplayedObjects(List);
	AIS_ListIteratorOfListOfInteractive ite (List);
	for (; ite.More(); ite.Next()) {
	  Handle(SALOME_InteractiveObject) anObj =
	    Handle(SALOME_InteractiveObject)::DownCast(ite.Value()->GetOwner());
	  if (!anObj.IsNull() && anObj->hasEntry() && anObj->isSame(anIO)) {
	    Quantity_Color CSFColor =
              Quantity_Color(myRed/255., myGreen/255., myBlue/255., Quantity_TOC_RGB);
	    ite.Value()->SetColor(CSFColor);
	    if (ite.Value()->IsKind(STANDARD_TYPE(GEOM_AISShape)))
	      Handle(GEOM_AISShape)::DownCast(ite.Value())->SetShadingColor(CSFColor);
	    ite.Value()->Redisplay(Standard_True);
	    occViewer->update();
	    break;
	  }
	}
      }
    }
  };
  ProcessVoidEvent(new TEvent(theEntry, red, green, blue));
}

void GEOM_Swig::setTransparency(const char* theEntry, float transp)
{
  class TEvent: public SALOME_Event {
    std::string myEntry;
    float myParam;
  public:
    TEvent(const char* theEntryArg, float theParam):
      myEntry(theEntryArg), myParam(theParam)
    {}
    virtual void Execute() {
      SUIT_Application* anApp = SUIT_Session::session()->activeApplication();
      if (!anApp) return;

      Handle(SALOME_InteractiveObject) anIO =
        new SALOME_InteractiveObject(myEntry.c_str(), "GEOM", "");

      if (SVTK_ViewWindow* aViewWindow = GetSVTKViewWindow(anApp)) {
	SVTK_View* aView = aViewWindow->getView();
	aView->SetTransparency(anIO, myParam);
	aView->Repaint();
      } else if (OCCViewer_Viewer* occViewer = GetOCCViewer(anApp)) {
	SOCC_Viewer* soccViewer = dynamic_cast<SOCC_Viewer*>(occViewer);
	if (soccViewer)
	  soccViewer->setTransparency(anIO, myParam);
      }
    }
  };

  ProcessVoidEvent(new TEvent (theEntry, transp));
}


class TInitGeomGenEvent: public SALOME_Event {
public:
  typedef bool TResult;
  TResult myResult;
  TInitGeomGenEvent() : myResult(false) {}
  virtual void Execute() {
    myResult = GeometryGUI::InitGeomGen();
  }
};
bool GEOM_Swig::initGeomGen()
{
  return ProcessEvent(new TInitGeomGenEvent());
}
