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

#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Study.h"
#include "QAD_StudyFrame.h"
#include "QAD_RightFrame.h"
#include "SALOMEGUI_ImportOperation.h"

#include "OCCViewer_Viewer3d.h"
#include "OCCViewer_ViewFrame.h"
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>

#include "SALOME_Event.hxx"

#include "VTKViewer_RenderWindowInteractor.h"
#include "VTKViewer_ViewFrame.h"

#include "GEOM_Actor.h"
#include "GEOM_Client.hxx"
#include "GEOM_AISShape.hxx"
#include "GEOM_AssemblyBuilder.h"
#include "GEOM_InteractiveObject.hxx"

#include "utilities.h"

using namespace std;

static GEOM_Client ShapeReader;


template<class TViewFrame> inline TViewFrame* GetFrame(QAD_Study* theStudy){
  return dynamic_cast<TViewFrame*>(SALOME::GetViewFrame(theStudy));
}


GEOM_Swig::GEOM_Swig()
{
  // MESSAGE("Constructeur");
}

GEOM_Swig::~GEOM_Swig()
{
  // MESSAGE("Destructeur");
}

void GEOM_Swig::createAndDisplayGO(const char* Entry)
{
  // MESSAGE("createAndDisplayGO");
  QAD_Study* ActiveStudy = QAD_Application::getDesktop()->getActiveStudy();
  SALOMEDS::Study_var aStudy = ActiveStudy->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();

  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  GEOM::GEOM_Gen_var Geom = GEOM::GEOM_Gen::_narrow(comp);

  CORBA::String_var aFatherIOR;
  SALOMEDS::SComponent_var father = aStudy->FindComponent("GEOM");
  aStudyBuilder->DefineComponentInstance( father, Geom );
  father->ComponentIOR(aFatherIOR);

  SALOMEDS::SObject_var fatherSF = aStudy->FindObjectID(ActiveStudy->getActiveStudyFrame()->entry());

  SALOMEDS::SObject_var obj = aStudy->FindObjectID(Entry);
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeIOR_var     anIOR;
  // Create new actor
  if ( !obj->FindAttribute(anAttr, "AttributeIOR")) 
    return;
  anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
  CORBA::String_var anIORValue = anIOR->Value();
  
  GEOM::GEOM_Object_var aShape = Geom->GetIORFromString(anIORValue);
  TopoDS_Shape Shape = ShapeReader.GetShape(Geom,aShape);
  
  if ( !obj->_is_nil() ) {
    if (obj->FindAttribute(anAttr, "AttributeName")) {
      SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(anAttr);
      CORBA::String_var aNameValue = aName->Value();
      // open transaction
      QAD_Operation* op = new SALOMEGUI_ImportOperation( ActiveStudy );
      op->start();
      
      SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
      aStudyBuilder->Addreference(newObj1, obj);
      // commit transaction
      op->finish();
		  
      Handle(GEOM_InteractiveObject) anIO = new GEOM_InteractiveObject(const_cast<char*>(anIORValue.in()),
								       const_cast<char*>(aFatherIOR.in()),
								       "GEOM",
								       const_cast<char*>(obj->GetID()));
      
      class TEvent: public SALOME_Event{
        QAD_Study* myStudy;
        TopoDS_Shape myShape;
        Handle(SALOME_InteractiveObject) myIO;
        const char* myName;
      public:
        TEvent(QAD_Study* theStudy, TopoDS_Shape theShape,
	       const Handle(SALOME_InteractiveObject)& theIO, 
	       const char* theName):
	  myStudy(theStudy), myShape(theShape),
				   myIO(theIO), myName(theName)
        {}
        virtual void Execute(){
	  if(VTKViewer_ViewFrame* aViewFrame = GetFrame<VTKViewer_ViewFrame>(myStudy)){
	    VTKViewer_RenderWindowInteractor* myRenderInter= aViewFrame->getRWInteractor();
	    int aMode = myRenderInter->GetDisplayMode();
	    
	    vtkActorCollection* theActors = GEOM_AssemblyBuilder::BuildActors(myShape,0,aMode,true);
	    theActors->InitTraversal();
	    while(vtkActor* anActor = theActors->GetNextActor()){
	      GEOM_Actor* GActor = GEOM_Actor::SafeDownCast(anActor);
	      GActor->setName(const_cast<char*>(myName));
	      GActor->setIO(myIO);
	      myRenderInter->Display(GActor);
	    }
	    myRenderInter->Update();
	  }else if(OCCViewer_ViewFrame* aViewFrame = GetFrame<OCCViewer_ViewFrame>(myStudy)){
	    Handle(AIS_InteractiveContext) ic = aViewFrame->getViewer()->getAISContext();
	    Handle(GEOM_AISShape) aSh = new GEOM_AISShape(myShape,const_cast<char*>(myName));
	    aSh->setName(const_cast<char*>(myName));
	    aSh->setIO(myIO);
	    
	    ic->Display(aSh);
	    ic->AddOrRemoveCurrentObject(aSh,true);
	  }
	  myStudy->updateObjBrowser(true);
        }
      };
      ProcessVoidEvent(new TEvent(ActiveStudy,Shape,anIO,aNameValue.in()));
    }
  }
}


int  GEOM_Swig::getIndexTopology(const char* SubIOR, const char* IOR)
{
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  GEOM::GEOM_Gen_var  Geom = GEOM::GEOM_Gen::_narrow(comp);
 
  GEOM::GEOM_Object_var aMainShape = Geom->GetIORFromString(IOR);
  TopoDS_Shape shape = ShapeReader.GetShape(Geom, aMainShape);

  GEOM::GEOM_Object_var aSubShape = Geom->GetIORFromString(SubIOR);
  TopoDS_Shape subshape = ShapeReader.GetShape(Geom, aSubShape);

  int index = 1;
  if(subshape.ShapeType() == TopAbs_COMPOUND) { 
    TopoDS_Iterator it; 
    TopTools_ListOfShape CL; 
    CL.Append(shape); 
    TopTools_ListIteratorOfListOfShape itC; 
    for(itC.Initialize(CL); itC.More(); itC.Next()) {
      for(it.Initialize(itC.Value()); it.More(); it.Next()) { 
	if (it.Value().ShapeType() == TopAbs_COMPOUND) {
	  if (it.Value().IsSame(subshape))
	    return index; 
	  else 
	    index++; 
	  CL.Append(it.Value()); 
	}
      } 
    } 
  } 
  else { 
    TopExp_Explorer Exp(shape, subshape.ShapeType()); 
    TopTools_MapOfShape M; 
    while(Exp.More()) { 
      if(M.Add(Exp.Current())) {
	if(Exp.Current().IsSame(subshape)) 
	  return index; 
	index++; 
      } 
      Exp.Next(); 
    } 
  } 
  return -1;
}

const char* GEOM_Swig::getShapeTypeString(const char* IOR)
{
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  GEOM::GEOM_Gen_var  Geom = GEOM::GEOM_Gen::_narrow(comp);
 
  GEOM::GEOM_Object_var aShape = Geom->GetIORFromString(IOR);
  TopoDS_Shape shape    = ShapeReader.GetShape(Geom, aShape);

  if( shape.IsNull() ) {
    return "Null Shape" ;
  }

  switch (shape.ShapeType() )
  {
  case TopAbs_COMPOUND:
    { return "Compound" ;}
  case  TopAbs_COMPSOLID:
    { return "Compound Solid" ;}
  case TopAbs_SOLID:
    { return "Solid" ;}
  case TopAbs_SHELL:
    { return "Shell" ;}
  case TopAbs_FACE:
    { 
      BRepAdaptor_Surface surf(TopoDS::Face(shape));
      if ( surf.GetType() == GeomAbs_Plane ) {
	return "Plane" ;
      } else if ( surf.GetType() == GeomAbs_Cylinder ) {
	return "Cylindrical Face" ;
      } else if ( surf.GetType() == GeomAbs_Sphere ) {
	return "Spherical Face" ;
      } else if ( surf.GetType() == GeomAbs_Torus ) {
	return "Toroidal Face" ;
      } else if ( surf.GetType() == GeomAbs_Cone ) {
	return "Conical Face" ;
      } else {
	return "GEOM::FACE" ;
      }
    }
  case TopAbs_WIRE:
    { return "Wire" ;}
  case TopAbs_EDGE:
    { 
      BRepAdaptor_Curve curv(TopoDS::Edge(shape));
      if ( curv.GetType() == GeomAbs_Line ) {
	if ( (Abs(curv.FirstParameter()) >= 1E6 ) || 
	     (Abs(curv.LastParameter()) >= 1E6 )) {
	  return "Line" ;
	} else
	  return "Edge" ;
      } else if ( curv.GetType() == GeomAbs_Circle ) {
	if ( curv.IsClosed() )
	  return "Circle" ;
	else
	  return "Arc" ;
      } else {
	return "Edge" ;
      }
    }
  case TopAbs_VERTEX:
    { return "Vertex" ;}
  case TopAbs_SHAPE:
    { return "Shape" ;}
  }
  return 0;
}


const char* GEOM_Swig::getShapeTypeIcon(const char* IOR)
{
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  GEOM::GEOM_Gen_var  Geom = GEOM::GEOM_Gen::_narrow(comp);
 
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
  QAD_Study* aStudy = QAD_Application::getDesktop()->getActiveStudy();
  Handle(SALOME_InteractiveObject) anIO = SALOME::FindIObject(aStudy,theEntry);

  class TEvent: public SALOME_Event{
    QAD_Study* myStudy;
    Handle(SALOME_InteractiveObject) myIO;
    int myParam;
  public:
    TEvent(QAD_Study* theStudy, const Handle(SALOME_InteractiveObject)& theIO, int theParam):
      myStudy(theStudy), myIO(theIO), myParam(theParam)
    {}
    virtual void Execute(){
      if(VTKViewer_ViewFrame* aViewFrame = GetFrame<VTKViewer_ViewFrame>(myStudy)){
	VTKViewer_RenderWindowInteractor* myRenderInter= aViewFrame->getRWInteractor();
	myRenderInter->SetDisplayMode(myIO,myParam);
	myRenderInter->Update();
      }else if(OCCViewer_ViewFrame* aViewFrame = GetFrame<OCCViewer_ViewFrame>(myStudy)){
	OCCViewer_Viewer3d* v3d = aViewFrame->getViewer();
	v3d->SwitchRepresentation(myIO,myParam);
      }
    }
  };

  ProcessVoidEvent(new TEvent(aStudy,anIO,theMode));
}

void GEOM_Swig::setColor(const char* theEntry, int red, int green, int blue)
{
  QAD_Study* aStudy = QAD_Application::getDesktop()->getActiveStudy();
  Handle(SALOME_InteractiveObject) anIO = SALOME::FindIObject(aStudy,theEntry);
  QColor aColor(red,green,blue);

  class TEvent: public SALOME_Event{
    QAD_Study* myStudy;
    Handle(SALOME_InteractiveObject) myIO;
    QColor myParam;
  public:
    TEvent(QAD_Study* theStudy, const Handle(SALOME_InteractiveObject)& theIO, const QColor& theParam):
      myStudy(theStudy), myIO(theIO), myParam(theParam)
    {}
    virtual void Execute(){
      if(VTKViewer_ViewFrame* aViewFrame = GetFrame<VTKViewer_ViewFrame>(myStudy)){
	VTKViewer_RenderWindowInteractor* myRenderInter= aViewFrame->getRWInteractor();
	myRenderInter->SetColor(myIO,myParam);
	myRenderInter->Update();
      }else if(OCCViewer_ViewFrame* aViewFrame = GetFrame<OCCViewer_ViewFrame>(myStudy)){
	OCCViewer_Viewer3d* v3d = aViewFrame->getViewer();
	v3d->SetColor(myIO,myParam);
      }
    }
  };

  ProcessVoidEvent(new TEvent(aStudy,anIO,aColor));
}

void GEOM_Swig::setTransparency(const char* theEntry, float transp)
{
  QAD_Study* aStudy = QAD_Application::getDesktop()->getActiveStudy();
  Handle(SALOME_InteractiveObject) anIO = SALOME::FindIObject(aStudy,theEntry);

  class TEvent: public SALOME_Event{
    QAD_Study* myStudy;
    Handle(SALOME_InteractiveObject) myIO;
    float myParam;
  public:
    TEvent(QAD_Study* theStudy, const Handle(SALOME_InteractiveObject)& theIO, float theParam):
      myStudy(theStudy), myIO(theIO), myParam(theParam)
    {}
    virtual void Execute(){
      if(VTKViewer_ViewFrame* aViewFrame = GetFrame<VTKViewer_ViewFrame>(myStudy)){
	VTKViewer_RenderWindowInteractor* myRenderInter= aViewFrame->getRWInteractor();
	myRenderInter->SetTransparency(myIO,myParam);
	myRenderInter->Update();
      }else if(OCCViewer_ViewFrame* aViewFrame = GetFrame<OCCViewer_ViewFrame>(myStudy)){
	OCCViewer_Viewer3d* v3d = aViewFrame->getViewer();
	v3d->SetTransparency(myIO,myParam);
      }
    }
  };

  ProcessVoidEvent(new TEvent(aStudy,anIO,transp));
}
