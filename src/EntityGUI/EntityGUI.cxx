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

using namespace std;
#include "EntityGUI.h"

#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"
#include "VTKViewer_ViewFrame.h"
#include "GEOM_AssemblyBuilder.h"
#include "SALOMEGUI_ImportOperation.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include <BRepTools_WireExplorer.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <Geom_Circle.hxx>
#include <Precision.hxx>

#include "DisplayGUI.h"
#include "EntityGUI_SubShapeDlg.h"   // Method SUBSHAPE

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

  QMenuBar* Mb = QAD_Application::getDesktop()->getMainMenuBar();

  switch (theCommandID)
    {
    case 404: // SKETCHER
      {
	((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->onViewTop(); // DCQ : 28/02/2002
	
	OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();

	myEntityGUI->myGeomGUI->GetSketcher() = Sketch(v3d->getViewer3d());
	myEntityGUI->myGeomGUI->myState = 2;

	Mb->setItemChecked(4052, false);
	Mb->setItemChecked(4053, false);

	myEntityGUI->myGeomGUI->GetSketcher().SetParameterVisibility(LENGTH_PARAMETER, Mb->isItemChecked(4061));
	myEntityGUI->myGeomGUI->GetSketcher().SetParameterVisibility(ANGLE_PARAMETER, Mb->isItemChecked(4062));
	myEntityGUI->myGeomGUI->GetSketcher().SetParameterVisibility(RADIUS_PARAMETER, Mb->isItemChecked(4063));
	myEntityGUI->myGeomGUI->GetSketcher().SetParameterVisibility(XVALUE_PARAMETER, Mb->isItemChecked(4064));
	myEntityGUI->myGeomGUI->GetSketcher().SetParameterVisibility(YVALUE_PARAMETER, Mb->isItemChecked(4065));
	
	myEntityGUI->myGeomGUI->GetSketcher().SetTransitionStatus(NOCONSTRAINT);
	break;
      }
    case 4041: // SKETCH Segment
      {
	myEntityGUI->myGeomGUI->GetSketcher().ChangeMode(SEGMENT);
	break;
      }
    case 4042: // SKETCH Arc
      {
	myEntityGUI->myGeomGUI->GetSketcher().ChangeMode(ARC_CHORD);
	break;
      }
    case 4043: // SKETCH Set Angle
      {
	myEntityGUI->OnSketchSetAngle();
	break;
      }
    case 4044: // SKETCH Set X
      {
	myEntityGUI->OnSketchSetx();
	break;
      }
    case 4045: // SKETCH Set Y
      {
	myEntityGUI->OnSketchSety();
	break;
      }
    case 4046: // SKETCH Delete
      {
	myEntityGUI->OnSketchDelete();
	break;
      }
    case 4047: // SKETCH End
      {
	myEntityGUI->OnSketchEnd();
	break;
      }
    case 4048: // SKETCH Close
      {
	myEntityGUI->OnSketchClose();
	break;
      }
    case 4051: // sketcher Set Plane
      {
	//TO DO
	break;
      }
    case 4052: // sketcher TANGENT
      { 
	Mb->setItemChecked(theCommandID, !Mb->isItemChecked(theCommandID));
	if(Mb->isItemChecked(theCommandID) == true) 
	  myEntityGUI->myGeomGUI->GetSketcher().SetTransitionStatus(TANGENT);
	else
	  myEntityGUI->myGeomGUI->GetSketcher().SetTransitionStatus(NOCONSTRAINT);

	Mb->setItemChecked(4053, false);
	break;
      }
    case 4053: // sketcher PERPENDICULAR
      {
	Mb->setItemChecked(theCommandID, !Mb->isItemChecked(theCommandID));
	if(Mb->isItemChecked(theCommandID) == true) 
	  myEntityGUI->myGeomGUI->GetSketcher().SetTransitionStatus(PERPENDICULAR);
	else 
	  myEntityGUI->myGeomGUI->GetSketcher().SetTransitionStatus(NOCONSTRAINT);
	
	Mb->setItemChecked(4052, false);
	break;
      }
    case 4061: // SKETCH OptionsOnofflengthdimension
      {
	Mb->setItemChecked(theCommandID, !Mb->isItemChecked(theCommandID));
	myEntityGUI->myGeomGUI->GetSketcher().SetParameterVisibility(LENGTH_PARAMETER, Mb->isItemChecked(theCommandID));
	break;
      }
    case 4062: // SKETCH OptionsOnoffangledimension
      {
	Mb->setItemChecked(theCommandID, !Mb->isItemChecked(theCommandID));
	myEntityGUI->myGeomGUI->GetSketcher().SetParameterVisibility(ANGLE_PARAMETER, Mb->isItemChecked(theCommandID));
	break;
      }
    case 4063: // SKETCH OptionsOnoffradiusdimension
      {
	Mb->setItemChecked(theCommandID, !Mb->isItemChecked(theCommandID));
	myEntityGUI->myGeomGUI->GetSketcher().SetParameterVisibility(RADIUS_PARAMETER, Mb->isItemChecked(theCommandID));
	break;
      }
    case 4064: // SKETCH OptionsOnoffxdimension
      {
	Mb->setItemChecked(theCommandID, !Mb->isItemChecked(theCommandID));
	myEntityGUI->myGeomGUI->GetSketcher().SetParameterVisibility(XVALUE_PARAMETER, Mb->isItemChecked(theCommandID));
	break;
      }
    case 4065: // SKETCH OptionsOnoffydimension
      {
	Mb->setItemChecked(theCommandID, !Mb->isItemChecked(theCommandID));
	myEntityGUI->myGeomGUI->GetSketcher().SetParameterVisibility(YVALUE_PARAMETER, Mb->isItemChecked(theCommandID));
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
// function : OnSketchSetAngle()
// purpose  : 
//=======================================================================
void EntityGUI::OnSketchSetAngle()
{
  Standard_Real anAngle = myGeomGUI->GetSketcher().GetSegmentAngle()/PI180;
  Sketch::fitInResol(anAngle); 
  Standard_Boolean res = false;
  QString Value = QString("%1").arg(anAngle);
  anAngle = myGeomBase->Parameter(res, Value, tr("GEOM_MEN_ANGLE"), tr("GEOM_MEN_ENTER_ANGLE"),
		      -180.0, +180.0, 6) * PI180;

  if(res) {
    myGeomGUI->GetSketcher().SetSegmentAngle(anAngle);
    QMenuBar* Mb = QAD_Application::getDesktop()->getMainMenuBar();
    Mb->setItemChecked(4052, false);
    Mb->setItemChecked(4053, false);
  }
  return;
}


//=======================================================================
// function : OnSketchSetx()
// purpose  : 
//=======================================================================
void EntityGUI::OnSketchSetx()
{
  Standard_Boolean res = false;
  double X = myGeomBase->Parameter(res, "0.", tr("GEOM_MEN_X"), tr("GEOM_MEN_SKETCHER_X"),
		       2.0 * Precision::Confusion(), 1E6, 6);
  if(res)
    myGeomGUI->GetSketcher().SetXDimension(X);
  QMenuBar* Mb = QAD_Application::getDesktop()->getMainMenuBar();
  Mb->setItemChecked(4052, false);
  Mb->setItemChecked(4053, false);
  return;
}


//=======================================================================
// function : OnSketchSety()
// purpose  : 
//=======================================================================
void EntityGUI::OnSketchSety()
{
  Standard_Boolean res = false;
  double Y = myGeomBase->Parameter(res, "0.", tr("GEOM_MEN_Y"), tr("GEOM_MEN_SKETCHER_Y"), 2.0 * Precision::Confusion(), 1E6, 6);
  if(res)
    myGeomGUI->GetSketcher().SetYDimension(Y);
  QMenuBar* Mb = QAD_Application::getDesktop()->getMainMenuBar();
  Mb->setItemChecked(4052, false);
  Mb->setItemChecked(4053, false);
  return;
}


//=======================================================================
// function : OnSketchDelete()
// purpose  : 
//=======================================================================
void EntityGUI::OnSketchDelete()
{
  if(myGeomGUI->GetSketcher().GetmyEdgesNumber() == 1) {
    QMenuBar* Mb = QAD_Application::getDesktop()->getMainMenuBar();
    Mb->setItemEnabled(405, false);  // SKETCH CONTRAINTS
    myGeomGUI->GetSketcher().SetTransitionStatus(NOCONSTRAINT);
  }

  if(myGeomGUI->GetSketcher().Delete())
    myGeomGUI->myState = -1;
  return;
}


//=======================================================================
// function : OnSketchClose()
// purpose  : 
//=======================================================================
void EntityGUI::OnSketchClose()
{
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle(AIS_InteractiveContext) myContext = v3d->getAISContext();

  TopoDS_Wire W = myGeomGUI->GetSketcher().Close();
  if(!W.IsNull()) {
    GEOM::GEOM_Gen::ListOfIOR_var listShapes = new GEOM::GEOM_Gen::ListOfIOR;
    listShapes->length(0);
    unsigned int i = 0;

    BRepTools_WireExplorer Ex(W);
    while(Ex.More()) {
      TopoDS_Edge E = Ex.Current();
      gp_Pnt pt1, pt2;

      pt1 = BRep_Tool::Pnt(TopExp::FirstVertex(E));
      pt2 = BRep_Tool::Pnt(TopExp::LastVertex(E));

      gp_Pnt CenterPoint;
      Handle(Geom_Curve) Curve;
      Handle(Geom_Circle) Circle;
      gp_Circ Circ;
      Standard_Real First,Last;

      Curve = BRep_Tool::Curve(E,First,Last);
      if(Curve->IsKind(STANDARD_TYPE(Geom_Circle))) {
	Circle = Handle(Geom_Circle)::DownCast(Curve); // pointer on geom_circ
	Circ = Circle->Circ();                         // gp_Circ
	
	Curve->D0((First + Last) / 2., CenterPoint);

	GEOM::PointStruct pI = myGeom->MakePointStruct(pt1.X(), pt1.Y(), pt1.Z());
	GEOM::PointStruct pC = myGeom->MakePointStruct(CenterPoint.X(), CenterPoint.Y(), CenterPoint.Z());
	GEOM::PointStruct pE = myGeom->MakePointStruct(pt2.X(), pt2.Y(), pt2.Z());

	GEOM::GEOM_Shape_var arc;

	try {
	  arc = myGeom->MakeArc(pI, pC, pE);
	}
	catch (const SALOME::SALOME_Exception& S_ex) {
	  QtCatchCorbaException(S_ex);
	}

 	listShapes->length(i+1);
	listShapes[i] = strdup(arc->Name());
	i++;   
      }
      else {
	GEOM::PointStruct pI = myGeom->MakePointStruct(pt1.X(), pt1.Y(), pt1.Z());
	GEOM::PointStruct pE = myGeom->MakePointStruct(pt2.X(), pt2.Y(), pt2.Z());
	GEOM::GEOM_Shape_var segment;

	try {
	  segment = myGeom->MakeEdge(pI,pE);
	}	
	catch (const SALOME::SALOME_Exception& S_ex) {
	  QtCatchCorbaException(S_ex);
	}

	listShapes->length(i+1);
	listShapes[i] = strdup(segment->Name());
	i++;
      }
      Ex.Next();
    }
    GEOM::GEOM_Shape_var Wire = myGeom->MakeWire(listShapes);
    TopoDS_Shape S = myGeomGUI->GetShapeReader().GetShape(myGeom, Wire);
    Standard_CString type;
    myGeomBase->GetShapeTypeString(S,type);
    Wire->NameType(type);

    if(myGeomBase->Display(Wire))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  myGeomGUI->myState = -1;
  QMenuBar* Mb = QAD_Application::getDesktop()->getMainMenuBar();
  Mb->setItemEnabled(405, false);  // SKETCH CONTRAINTS
  myGeomGUI->GetSketcher().SetTransitionStatus(NOCONSTRAINT);
  return;
}


//=======================================================================
// function : OnSketchEnd()
// purpose  : 
//=======================================================================
void EntityGUI::OnSketchEnd()
{
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) myContext = v3d->getAISContext();

  TopoDS_Wire W = myGeomGUI->GetSketcher().End();
  if(!W.IsNull()) {
    GEOM::GEOM_Gen::ListOfIOR_var listShapes = new GEOM::GEOM_Gen::ListOfIOR;
    listShapes->length(0);
    unsigned int i = 0;

    BRepTools_WireExplorer Ex(W);
    while(Ex.More()) {
      TopoDS_Edge E = TopoDS::Edge(Ex.Current());
      
      gp_Pnt pt1, pt2;
      pt1 = BRep_Tool::Pnt(TopExp::FirstVertex(E));
      pt2 = BRep_Tool::Pnt(TopExp::LastVertex(E));

      gp_Pnt CenterPoint;
      Handle(Geom_Curve) Curve;
      Handle(Geom_Circle) Circle;
      gp_Circ Circ;
      Standard_Real First,Last;
      
      Curve = BRep_Tool::Curve(E,First,Last);
      if(Curve->IsKind(STANDARD_TYPE(Geom_Circle))) {
	Circle = Handle(Geom_Circle)::DownCast(Curve); // pointer on geom_circ
	Circ = Circle->Circ();                         // gp_Circ
	
	Curve->D0((First + Last) / 2., CenterPoint);

	GEOM::PointStruct pI = myGeom->MakePointStruct(pt1.X(), pt1.Y(), pt1.Z());
	GEOM::PointStruct pC = myGeom->MakePointStruct(CenterPoint.X(), CenterPoint.Y(), CenterPoint.Z());
	GEOM::PointStruct pE = myGeom->MakePointStruct(pt2.X(), pt2.Y(), pt2.Z());

	GEOM::GEOM_Shape_var arc;

	try {
	  arc = myGeom->MakeArc(pI, pC, pE);
	}
	catch (const SALOME::SALOME_Exception& S_ex) {
	  QtCatchCorbaException(S_ex);
	}
		
 	listShapes->length(i+1);
	listShapes[i] = strdup(arc->Name());
	i++;   
      } else {
	GEOM::PointStruct pI = myGeom->MakePointStruct(pt1.X(), pt1.Y(), pt1.Z());
	GEOM::PointStruct pE = myGeom->MakePointStruct(pt2.X(), pt2.Y(), pt2.Z());
	GEOM::GEOM_Shape_var segment;
	
	try {
	  segment = myGeom->MakeEdge(pI,pE);
	}
	catch (const SALOME::SALOME_Exception& S_ex) {
	  QtCatchCorbaException(S_ex);
	}
	
	listShapes->length(i+1);
	listShapes[i] = strdup(segment->Name());
	i++;
      }
      Ex.Next();
    }

    GEOM::GEOM_Shape_var Wire = myGeom->MakeWire(listShapes);
    TopoDS_Shape S = myGeomGUI->GetShapeReader().GetShape(myGeom, Wire);
    Standard_CString type;
    myGeomBase->GetShapeTypeString(S,type);
    Wire->NameType(type);

    if(myGeomBase->Display(Wire))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  myGeomGUI->myState = -1;
  QMenuBar* Mb = QAD_Application::getDesktop()->getMainMenuBar();
  Mb->setItemEnabled(405, false);  // SKETCH CONTRAINTS
  myGeomGUI->GetSketcher().SetTransitionStatus(NOCONSTRAINT);
  return;
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
      sprintf(nameG, "%s_%d", tr("GEOM_SHAPE").latin1(), myGeomGUI->GetNbGeom()++);
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
	sprintf (nameG, "%s_%d", tr("GEOM_SHAPE").latin1(), myGeomGUI->GetNbGeom()++);
      }
      result = new GEOM_AISShape(Exp.Current(), nameG);
      IO = new GEOM_InteractiveObject(aResult->Name(), myGeomGUI->GetFatherior(), "GEOM");
    }
  }
  else {
    if ( myGeomBase->GetShapeTypeString(compound,Type)) {
      aResult->NameType(Type);
      sprintf (nameG, "%s_%d", Type, myGeomGUI->GetNbGeom()++);
    } else {
      aResult->NameType(tr("GEOM_SHAPE"));
      sprintf (nameG, "%s_%d", tr("GEOM_SHAPE").latin1(), myGeomGUI->GetNbGeom()++);
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
