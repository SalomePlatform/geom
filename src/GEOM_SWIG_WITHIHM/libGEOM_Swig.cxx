// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : libGEOM_Swig.cxx
// Author : Nicolas REJNERI, Paul RASCLE
//
#include "libGEOM_Swig.h"

#include "GeometryGUI.h"
#include "GEOMToolsGUI.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"

#include "OB_Browser.h"

#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewManager.h"
#include "SOCC_ViewModel.h"
#include <SOCC_Prs.h>

#include "SVTK_ViewModel.h"
#include "SVTK_ViewWindow.h"
#include "SVTK_View.h"
#include "SVTK_Renderer.h"
#include <SVTK_Prs.h>

#include "GEOM_Actor.h"
#include "GEOM_Client.hxx"
#include "GEOM_AISShape.hxx"
#include "GEOM_InteractiveObject.hxx"
#include "GEOM_Displayer.h"
#include "GEOM_Constants.h"

#include "SALOME_Event.h"

// OCCT Includes
#include <TopAbs.hxx>
#include <TopoDS_Shape.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <BRepTools.hxx>

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

#include <vtkRenderer.h>

static SHAPE_READER(ShapeReader);

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

void GEOM_Swig::createAndDisplayGO (const char* Entry, bool isUpdated)
{
  class TEvent: public SALOME_Event
  {
    std::string myEntry;
    bool        myUpdateViewer;
  public:
    TEvent(const char* theEntry, bool toUpdateViewer):
      myEntry(theEntry),
      myUpdateViewer(toUpdateViewer)
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

      std::string aFatherIOR;
      _PTR(SComponent) father = aStudy->FindComponent("GEOM");
      if (!father)
        return;
      if (!father->ComponentIOR(aFatherIOR)) {
        CORBA::String_var objStr = SalomeApp_Application::orb()->object_to_string(Geom);
        aStudyBuilder->LoadWith(father, objStr.in());
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
      std::string anIORValue = anIOR->Value();

      GEOM::GEOM_Object_var aShape = Geom->GetIORFromString(anIORValue.c_str());
      TopoDS_Shape Shape = ShapeReader.GetShape(Geom,aShape);
      if (!Shape.IsNull()) {
        if (obj->FindAttribute(anAttr, "AttributeName")) {
          _PTR(AttributeName) aName (anAttr);
          std::string aNameValue = aName->Value();
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

          GEOM_Displayer(ActiveStudy).Display(anIO, myUpdateViewer);
          /*if (SVTK_ViewWindow* aViewWindow = GetSVTKViewWindow(app)) {
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
            }*/
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
  ProcessVoidEvent(new TEvent (Entry, isUpdated));

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

  if (isUpdated)
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

void GEOM_Swig::UpdateViewer()
{
  class TEventUpdateViewer: public SALOME_Event
  {
    public:
      TEventUpdateViewer() {}
      virtual void Execute() {
        SUIT_Application* app = SUIT_Session::session()->activeApplication();
        if (!app) return;
        SalomeApp_Study* ActiveStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
        if (!ActiveStudy) return;
        
        GEOM_Displayer(ActiveStudy).UpdateViewer();
      }
  };
  
  ProcessVoidEvent(new TEventUpdateViewer());
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

void GEOM_Swig::setDisplayMode(const char* theEntry, int theMode, bool isUpdated)
{
  class TEvent: public SALOME_Event {
    std::string myEntry;
    int myMode;
    bool myUpdateViewer;
  public:
    TEvent(const char* theEntryArg, int theModeArg, bool theUpdated):
      myEntry(theEntryArg), myMode(theModeArg), myUpdateViewer(theUpdated)
    {}
    virtual void Execute() {
      SUIT_Application* anApp = SUIT_Session::session()->activeApplication();
      if (!anApp) return;

      Handle(SALOME_InteractiveObject) anIO =
        new SALOME_InteractiveObject(myEntry.c_str(), "GEOM", "");

      if (SVTK_ViewWindow* aViewWindow = GetSVTKViewWindow(anApp)) {
        SVTK_View* aView = aViewWindow->getView();
        aView->SetDisplayMode(anIO, myMode);
        if (myUpdateViewer)
          aView->Repaint();
      }
      else if (OCCViewer_Viewer* occViewer = GetOCCViewer(anApp)) {
        SOCC_Viewer* soccViewer = dynamic_cast<SOCC_Viewer*>(occViewer);
        if (soccViewer)
          soccViewer->switchRepresentation(anIO, myMode, myUpdateViewer);
      }
    }
  };

  ProcessVoidEvent(new TEvent (theEntry, theMode, isUpdated));
}

void GEOM_Swig::setVectorsMode(const char* theEntry, bool isOn, bool isUpdated)
{
  class TEvent: public SALOME_Event {
    std::string myEntry;
    bool myOn;
    bool myUpdateViewer;
  public:
    TEvent(const char* theEntryArg, bool theOn, bool theUpdated):
      myEntry(theEntryArg), myOn(theOn), myUpdateViewer(theUpdated)
    {}
    virtual void Execute() {
      SUIT_Application* anApp = SUIT_Session::session()->activeApplication();
      if (!anApp) return;

      Handle(SALOME_InteractiveObject) anIO =
        new SALOME_InteractiveObject(myEntry.c_str(), "GEOM", "");

      if (SVTK_ViewWindow* aViewWindow = GetSVTKViewWindow(anApp)) {
        SVTK_View* aView = aViewWindow->getView();
        SVTK_Viewer* stvkViewer = dynamic_cast<SVTK_Viewer*>(aViewWindow->getViewManager()->getViewModel());
        SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( stvkViewer->CreatePrs( myEntry.c_str()  ) );
        vtkActorCollection* anActors = vtkPrs->GetObjects();
        anActors->InitTraversal();
        while (vtkActor* anAct = anActors->GetNextActor()) {
          GEOM_Actor* aGeomActor = GEOM_Actor::SafeDownCast(anAct);
          aGeomActor->SetVectorMode(!aGeomActor->GetVectorMode());
        }
        if (myUpdateViewer)
          aView->Repaint();
      }
      else if (OCCViewer_Viewer* occViewer = GetOCCViewer(anApp)) {
        Handle(AIS_InteractiveContext) ic = occViewer->getAISContext();
        SOCC_Viewer* soccViewer = dynamic_cast<SOCC_Viewer*>(occViewer);
        if (soccViewer) {
          SOCC_Prs* occPrs = dynamic_cast<SOCC_Prs*>( soccViewer->CreatePrs( myEntry.c_str() ) );
          if ( occPrs && !occPrs->IsNull() ) {
            AIS_ListOfInteractive shapes; occPrs->GetObjects( shapes );
            AIS_ListIteratorOfListOfInteractive interIter( shapes );
            for ( ; interIter.More(); interIter.Next() ) {
              Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast( interIter.Value() );
              aSh->SetDisplayVectors(myOn);
              ic->RecomputePrsOnly(interIter.Value());
            }
          }
        }
      }
    }
  };

  ProcessVoidEvent(new TEvent (theEntry, isOn, isUpdated));
}

void GEOM_Swig::setColor(const char* theEntry, int red, int green, int blue, bool isUpdated)
{
  class TEvent: public SALOME_Event {
    QString myEntry;
    int myRed;
    int myGreen;
    int myBlue;
    bool myUpdateViewer;
  public:
    TEvent(const char* theEntryArg, int theR, int theG, int theB, bool theUpdated):
      myEntry(theEntryArg), myRed(theR), myGreen(theG), myBlue(theB), myUpdateViewer(theUpdated)
    {}
    virtual void Execute() {
      SUIT_Application* anApp = SUIT_Session::session()->activeApplication();
      if (!anApp) return;
      GEOMToolsGUI::SetColor( myEntry, QColor( myRed, myGreen, myBlue), myUpdateViewer );
    }
  };
  ProcessVoidEvent(new TEvent(theEntry, red, green, blue, isUpdated));
}

void GEOM_Swig::setIsos(const char* Entry, int nbU, int nbV, bool isUpdated )
{
  class TEvent: public SALOME_Event {
    std::string myEntry;
    int myNbU, myNbV;
    bool myUpdateViewer;
  public:
    TEvent(const char* theEntry, int theNbU, int theNbV, bool theUpdated):
      myEntry(theEntry), myNbU(theNbU), myNbV(theNbV), myUpdateViewer(theUpdated)
    {}
    virtual void Execute() {
      SUIT_Application* app = SUIT_Session::session()->activeApplication();
      if (!app) return;
      SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
      if (!study) return;

      Handle(SALOME_InteractiveObject) anIO =
        new SALOME_InteractiveObject(myEntry.c_str(), "GEOM", "");

      if (SVTK_ViewWindow* aViewWindow = GetSVTKViewWindow(app)) {
        SVTK_Viewer* aView = dynamic_cast<SVTK_Viewer*>(aViewWindow->getViewManager()->getViewModel());
	SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( aView->CreatePrs( myEntry.c_str() ) );
	if ( vtkPrs ) {
	  vtkActorCollection* anActors = vtkPrs->GetObjects();
	  anActors->InitTraversal();
	  GEOM_Actor* anActor = GEOM_Actor::SafeDownCast( anActors->GetNextActor() );
	  if ( anActor ) {
	    int aIsos[2]={myNbU,myNbV};
	    anActor->SetNbIsos(aIsos);
	    anActor->StoreIsoNumbers();
	    QString anIsos = QString("%1%2%3").arg(myNbU).arg(DIGIT_SEPARATOR).arg(myNbV);
	    int aMgrId = aView->getViewManager()->getGlobalId();
	    study->setObjectProperty(aMgrId, myEntry.c_str(), ISOS_PROP, anIsos);
	  }
	}
	
	if (myUpdateViewer)
          aView->Repaint();
      }
      else if (OCCViewer_Viewer* occViewer = GetOCCViewer(app)) {
        Handle(AIS_InteractiveContext) ic = occViewer->getAISContext();
        SOCC_Viewer* soccViewer = dynamic_cast<SOCC_Viewer*>(occViewer);
        if (soccViewer) {
	  int aMgrId = soccViewer->getViewManager()->getGlobalId();
          SOCC_Prs* occPrs = dynamic_cast<SOCC_Prs*>( soccViewer->CreatePrs( myEntry.c_str() ) );
          if ( occPrs && !occPrs->IsNull() ) {
            AIS_ListOfInteractive shapes; occPrs->GetObjects( shapes );
            AIS_ListIteratorOfListOfInteractive interIter( shapes );
            for ( ; interIter.More(); interIter.Next() ) {
              Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast( interIter.Value() );
	      if ( !aSh.IsNull() ) {
		Handle(AIS_Drawer) drawer = aSh->Attributes();
		QVariant v = study->getObjectProperty( aMgrId, myEntry.c_str(), EDGE_WIDTH_PROP, QVariant() );
		int width = v.isValid() ? v.toInt() : 1;
		drawer->SetUIsoAspect( new Prs3d_IsoAspect(Quantity_NOC_GRAY75, Aspect_TOL_SOLID, width, myNbU) );
		drawer->SetVIsoAspect( new Prs3d_IsoAspect(Quantity_NOC_GRAY75, Aspect_TOL_SOLID, width, myNbV) );
		aSh->storeIsoNumbers();
                ic->SetLocalAttributes(aSh, drawer);
		ic->Redisplay(aSh);
		QString anIsos = QString("%1%2%3").arg(myNbU).arg(DIGIT_SEPARATOR).arg(myNbV);
		study->setObjectProperty(aMgrId, myEntry.c_str(), ISOS_PROP, anIsos);
	      }
            }
          }
        }
      }
    }
  };

  ProcessVoidEvent(new TEvent (Entry, nbU, nbV, isUpdated));
}

void GEOM_Swig::setTransparency(const char* theEntry, float transp, bool isUpdated)
{
  class TEvent: public SALOME_Event {
    std::string myEntry;
    float myParam;
    bool myUpdateViewer;
  public:
    TEvent(const char* theEntryArg, float theParam, bool theUpdated):
      myEntry(theEntryArg), myParam(theParam), myUpdateViewer(theUpdated)
    {}
    virtual void Execute() {
      SUIT_Application* anApp = SUIT_Session::session()->activeApplication();
      if (!anApp) return;

      Handle(SALOME_InteractiveObject) anIO =
        new SALOME_InteractiveObject(myEntry.c_str(), "GEOM", "");

      if (SVTK_ViewWindow* aViewWindow = GetSVTKViewWindow(anApp)) {
        SVTK_View* aView = aViewWindow->getView();
        aView->SetTransparency(anIO, myParam);
        if (myUpdateViewer)
          aView->Repaint();
      } else if (OCCViewer_Viewer* occViewer = GetOCCViewer(anApp)) {
        SOCC_Viewer* soccViewer = dynamic_cast<SOCC_Viewer*>(occViewer);
        if (soccViewer)
          soccViewer->setTransparency(anIO, myParam, myUpdateViewer);
      }
    }
  };

  ProcessVoidEvent(new TEvent (theEntry, transp, isUpdated));
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



void GEOM_Swig::eraseGO (const char* Entry, bool allWindows)
{
  class TEvent: public SALOME_Event
  {
    std::string myEntry;
    bool myFromAllWindows;
  public:
    TEvent(const char* theEntry, bool fromAllWindows):
      myEntry(theEntry), myFromAllWindows(fromAllWindows)
    {}
    virtual void Execute()
    {
      SUIT_Application* app = SUIT_Session::session()->activeApplication();
      if (!app) return;
      SalomeApp_Study* ActiveStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
      if (!ActiveStudy) return;

      Handle (SALOME_InteractiveObject) aIO = new SALOME_InteractiveObject(myEntry.c_str(), "GEOM", "");

      GEOM_Displayer(ActiveStudy).Erase(aIO, true);
      /*      if (myFromAllWindows) {
        QPtrList<SUIT_ViewWindow> aWindows = app->desktop()->windows();
        SUIT_ViewWindow* aWin = 0;
        for (aWin = aWindows.first(); aWin; aWin = aWindows.next()) {
          EraseObject(aWin, aIO);
        }
      } else {
        SUIT_ViewWindow* aWin = app->desktop()->activeWindow();
        if (aWin)
          EraseObject(aWin, aIO);
          }*/
    }

    /*  private:
    void EraseObject(SUIT_ViewWindow* theWin, Handle (SALOME_InteractiveObject) theIO)
    {
      if (theWin->getViewManager()->getType() == OCCViewer_Viewer::Type()){
        OCCViewer_ViewWindow* vw = dynamic_cast<OCCViewer_ViewWindow*>( theWin );
        if ( vw ) {
          OCCViewer_ViewManager* vm = dynamic_cast<OCCViewer_ViewManager*>( vw->getViewManager() );
          if ( vm ) {
            SOCC_Viewer* aViewer = dynamic_cast<SOCC_Viewer*>(vm->getOCCViewer());
            if (aViewer) {
              SALOME_Prs* aPrs = aViewer->CreatePrs(myEntry.c_str());
              if (aPrs) {
                SALOME_OCCPrs* aOccPrs =  dynamic_cast<SALOME_OCCPrs*>(aPrs);
                if (aOccPrs) {
                  aViewer->Erase(aOccPrs);
                  aViewer->Repaint();
                }
              }
            }
          }
        }
      } else if (theWin->getViewManager()->getType() == SVTK_Viewer::Type()){
        SVTK_ViewWindow* aViewWindow = dynamic_cast<SVTK_ViewWindow*>( theWin );
        if (aViewWindow) {
          aViewWindow->Erase(theIO);
        }
      }
      }*/

  };
  ProcessVoidEvent(new TEvent(Entry, allWindows));
}



void GEOM_Swig::setDeflection(const char* theEntry, float theDeflect)
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
        vtkActorCollection* aActors = aViewWindow->getRenderer()->GetActors();
        aActors->InitTraversal();
        while (vtkActor* aAct = aActors->GetNextActor()) {
          if (GEOM_Actor* aGeomActor = dynamic_cast<GEOM_Actor*>(aAct)) {
            if (aGeomActor->hasIO()) {
              Handle(SALOME_InteractiveObject) aNextIO = aGeomActor->getIO();
              if (aNextIO->isSame(anIO)) {
                aGeomActor->setDeflection(myParam);
                aViewWindow->Repaint();
                return;
              }
            }
          }
        }
        //      aView->SetTransparency(anIO, myParam);
        //aView->Repaint();
      } else if (OCCViewer_Viewer* occViewer = GetOCCViewer(anApp)) {
        Handle(AIS_InteractiveContext) aContext = occViewer->getAISContext();
        AIS_ListOfInteractive aAISList;
        aContext->DisplayedObjects(aAISList);
        AIS_ListIteratorOfListOfInteractive it(aAISList);
        for (; it.More(); it.Next()) {
          Handle(SALOME_InteractiveObject) aObj = 
            Handle(SALOME_InteractiveObject)::DownCast(it.Value()->GetOwner());
          if ((!aObj.IsNull()) && aObj->hasEntry() && aObj->isSame(anIO)) {
            Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(it.Value());
            if (!aShape.IsNull()) {
              TopoDS_Shape aSh = aShape->Shape();
              if (!aSh.IsNull())
                BRepTools::Clean(aSh);

              aShape->SetOwnDeviationCoefficient( myParam );
              aShape->SetOwnHLRDeviationAngle( 1.57 );
              aContext->Redisplay(aShape);
              return;
            }
          }
        }
      }
    }
  };

  ProcessVoidEvent(new TEvent (theEntry, theDeflect));
}
