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
//  File   : GeometryGUI.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI.h"

// SALOME Includes
# include "Utils_ORB_INIT.hxx"
# include "Utils_SINGLETON.hxx"

#include "QAD.h"
#include "QAD_Tools.h"
#include "QAD_Config.h"
#include "QAD_Settings.h"
#include "QAD_RightFrame.h"
#include "QAD_MessageBox.h"
#include "QAD_Resource.h"
#include "QAD_FileDlg.h"

#include "OCCViewer_ViewPort.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_Viewer3d.h"

#include "SALOME_NamingService.hxx"
#include "SALOME_ListIteratorOfListIO.hxx"
#include "SALOME_InteractiveObject.hxx"

#include "SALOMEGUI_ImportOperation.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"
#include "SALOMEGUI_NameDlg.h"            
#include "utilities.h"

// Open CASCADE Includes
#include <AIS_Shape.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_Trihedron.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>

#include <BRep_Builder.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepCheck_Analyzer.hxx>

#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <gp_Mat.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS_Iterator.hxx>

//VRV: OCC 4.0 migration
#include <IGESControl_Writer.hxx>
#include <IGESControl_Controller.hxx>
#include <STEPControl_Writer.hxx>
//#include <STEPControlStd_StepModelType.hxx>
//VRV: OCC 4.0 migration

#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <Precision.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>

#include <IFSelect_ReturnStatus.hxx>
#include <Interface_Static.hxx>

// QT Includes
#define	 INCLUDE_MENUITEM_DEF
#include <qapplication.h>
#include <qmenudata.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qfont.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qmessagebox.h>
#include <qspinbox.h>
#include <qlist.h>
#include <qwidget.h> 
#include <qevent.h> 
#include <qlineedit.h>

#include "VTKViewer_RenderWindowInteractor.h"
#include "VTKViewer_ViewFrame.h"
#include "GEOM_Actor.h"
#include "GEOM_Client.hxx"
#include "GEOM_AISShape.hxx"
#include "GEOM_AssemblyBuilder.h"
#include "GEOM_InteractiveObject.hxx"


#include "GeometryGUI_aParameterDlg.h"

#include "GeometryGUI_PointDlg.h"         // Method POINT
#include "GeometryGUI_BoxDlg.h"           // Method BOX
#include "GeometryGUI_VectorDlg.h"        // Method VECTOR
#include "GeometryGUI_PlaneDlg.h"         // Method PLANE
#include "GeometryGUI_PrismDlg.h"         // Method PRISM
#include "GeometryGUI_FuseDlg.h"          // Method FUSE
#include "GeometryGUI_CommonDlg.h"        // Method COMMON
#include "GeometryGUI_CutDlg.h"           // Method CUT
#include "GeometryGUI_SectionDlg.h"       // Method SECTION

#include "GeometryGUI_LineDlg.h"          // Method LINE
#include "GeometryGUI_ScaleDlg.h"         // Method SCALE
#include "GeometryGUI_MirrorDlg.h"        // Method MIRROR
#include "GeometryGUI_SphereDlg.h"        // Method SPHERE
#include "GeometryGUI_CircleDlg.h"        // Method CIRCLE
#include "GeometryGUI_EllipseDlg.h"       // Method ELLIPSE
#include "GeometryGUI_RevolDlg.h"         // Method REVOL
#include "GeometryGUI_RotationDlg.h"      // Method ROTATION
#include "GeometryGUI_TranslationDlg.h"   // Method TRANSLATION
#include "GeometryGUI_MultiTranslationDlg.h"   // Method MULTI TRANSLATION
#include "GeometryGUI_MultiRotationDlg.h" // Method MULTI ROTATION
#include "GeometryGUI_ArcDlg.h"           // Method ARC
#include "GeometryGUI_PipeDlg.h"          // Method PIPE
#include "GeometryGUI_CylinderDlg.h"      // Method CYLINDER
#include "GeometryGUI_ConeDlg.h"          // Method CONE
#include "GeometryGUI_TorusDlg.h"         // Method TORUS
#include "GeometryGUI_FillingDlg.h"       // Method FILLING
#include "GeometryGUI_SewingDlg.h"        // Method SEWING
#include "GeometryGUI_CompoundDlg.h"      // Method GEOM::COMPOUND
#include "GeometryGUI_EdgeDlg.h"          // Method GEOM::EDGE
#include "GeometryGUI_OrientationDlg.h"   // Method ORIENTATION
#include "GeometryGUI_PartitionDlg.h"     // Method PARTITION
#include "GeometryGUI_SubShapeDlg.h"      // Method EXPLODE
#include "GeometryGUI_WireDlg.h"          // Method GEOM::WIRE
#include "GeometryGUI_WorkingPlaneDlg.h"  // Method WORKING PLANE
#include "GeometryGUI_PropertiesDlg.h"    // Method PROPERTIES
#include "GeometryGUI_CenterMassDlg.h"    // Method CENTER MASS
#include "GeometryGUI_InertiaDlg.h"       // Method INERTIA
#include "GeometryGUI_FaceDlg.h"          // Method GEOM::FACE
#include "GeometryGUI_FilletDlg.h"        // Method FILLET
#include "GeometryGUI_ChamferDlg.h"       // Method CHAMFER
#include "GeometryGUI_FillingHoleDlg.h"   // Method FILLING HOLE
#include "GeometryGUI_SuppressFacesDlg.h" // Method SUPPRESS FACES
#include "GeometryGUI_CheckShape.h"       // Method CHECKSHAPE
#include "GeometryGUI_ArchimedeDlg.h"     // Method ARCHIMEDE
#include "GeometryGUI_TransparencyDlg.h"  // Method TRANSPARENCY adjustement
#include "GeometryGUI_NbIsosDlg.h"        // Method ISOS adjustement
#include "GeometryGUI_BndBoxDlg.h"        // Method BNDBOX
#include "GeometryGUI_MaxToleranceDlg.h"  // Method MAXTOLERANCE
#include "GeometryGUI_WhatisDlg.h"        // Method WHATIS
#include "GeometryGUI_DistanceDlg.h"      // Method DISTANCE
#include "GeometryGUI_SuppressHoleDlg.h"  // Method SUPPRESS HOLE


static Handle(AIS_Shape) theConstructionShape = new AIS_Shape(TopoDS_Shape());
static Handle(GEOM_AISShape) theSelectedShape = new GEOM_AISShape(TopoDS_Shape(), "");

static AIS_ListOfInteractive ListDisplayedObject;

static bool Settings_AddInStudy = false;
static bool Settings_Copy = false;
static Standard_CString Fatherior = "";
static GEOM_Client ShapeReader;

/* The object itself created in the static method 'GetOrCreateGeometryGUI()' */
static GeometryGUI* GeomGUI = 0;



//=======================================================================
// class   : CustomItem
// purpose : Set Font to a text.
//=======================================================================
class CustomItem : public QCustomMenuItem
{
public:
  CustomItem( const QString& s, const QFont& f )
    : string( s ), font( f ){};
  ~CustomItem(){}
  
  void paint( QPainter* p, const QColorGroup& /*cg*/, bool /*act*/, bool /*enabled*/, int x, int y, int w, int h )
  {
    p->setFont ( font );
    p->drawText( x, y, w, h, AlignHCenter | AlignVCenter | ShowPrefix | DontClip, string );
  }
  
  QSize sizeHint()
  {
    return QFontMetrics( font ).size( AlignHCenter | AlignVCenter | ShowPrefix | DontClip,  string );
  }
private:
  QString string;
  QFont font;
};




//=======================================================================
// function : GeometryGUI()
// purpose  : Constructor
//=======================================================================
GeometryGUI::GeometryGUI() :
  QObject()
{
}

//=======================================================================
// function : ~GeometryGUI()
// purpose  : Destructor
//=======================================================================
GeometryGUI::~GeometryGUI()
{
}


//=======================================================================
// function : GetGeometryGUI() static
// purpose  : Returns current 'GeomGUI' a static pointer
//=======================================================================
GeometryGUI* GeometryGUI::GetGeometryGUI( )
{
  return GeomGUI;
}


//=====================================================================================
// function : GetIndex()
// purpose  : Get the index of a sub shape in a main shape : index start at 1
//=====================================================================================
int  GeometryGUI::GetIndex(const TopoDS_Shape& subshape, 
      const TopoDS_Shape& shape, 
      int /*ShapeType*/) 
{ 
  if( shape.IsNull() || subshape.IsNull() ) 
    return -1 ; 
  
  int index = 1; 
  if (subshape.ShapeType() == TopAbs_COMPOUND) 
  { 
    TopoDS_Iterator it; 
    TopTools_ListOfShape CL; 
    CL.Append( shape ); 
    TopTools_ListIteratorOfListOfShape itC; 
    for (itC.Initialize( CL ); itC.More(); itC.Next()) 
      { 
	for (it.Initialize( itC.Value() );  it.More(); it.Next()) 
	  { 
	    if ( it.Value().ShapeType() == TopAbs_COMPOUND) 
	      {
		if (it.Value().IsSame(subshape)) 
		  return index; 
		else 
		  index++; 
		CL.Append( it.Value() ); 
	      }
	  } 
      } 
  } 
  else 
    { 
      TopExp_Explorer Exp ( shape,  subshape.ShapeType() ); 
      TopTools_MapOfShape M; 
      while ( Exp.More() ) 
	{
	  if ( M.Add(Exp.Current()) ) 
	    { 
	      if ( Exp.Current().IsSame(subshape) ) 
		return index; 
	      index++; 
	    } 
	  Exp.Next(); 
	} 
    } 
  return -1; 
} 

//=======================================================================
// function : GetOrCreateGeometryGUI()
// purpose  : Gets or create an object 'GeometryGUI' with initialisations
//          : Returns 'GeomGUI' as a pointer
//=======================================================================
GeometryGUI* GeometryGUI::GetOrCreateGeometryGUI( QAD_Desktop* desktop )
{
  if( GeomGUI == 0 ) {
    GeomGUI = new GeometryGUI;
    GeomGUI->myActiveDialogBox = 0 ;
    GeomGUI->mySimulationShape = new AIS_Shape(TopoDS_Shape());
    GeomGUI->myState = -1 ;
    GeomGUI->myDesktop = desktop ; 
    GeomGUI->myActiveStudy = desktop->getActiveStudy();
    GeomGUI->mySimulationActor = vtkActorCollection::New() ;

    GeomGUI->myShadingColor = Quantity_Color( Quantity_NOC_GOLDENROD );

    Engines::Component_var comp = desktop->getEngine("FactoryServer", "GEOM");
    GeomGUI->myComponentGeom = GEOM::GEOM_Gen::_narrow(comp);
    
    /* GetCurrentStudy */
    int studyId = GeomGUI->myActiveStudy->getStudyId();
    GeomGUI->myComponentGeom->GetCurrentStudy(studyId);

    GeomGUI->myNbGeom = GeomGUI->myComponentGeom->NbLabels();
  } else {
    /* study may have changed */
    GeomGUI->myActiveStudy = desktop->getActiveStudy();
  }
  return GeomGUI;
}


//=================================================================================
// function : VertexToPoint()
// purpose  : If S can be converted in a gp_Pnt returns true and the result is P
//=================================================================================
bool GeometryGUI::VertexToPoint( const TopoDS_Shape& S, gp_Pnt& P )
{
  if( S.IsNull() || S.ShapeType() != TopAbs_VERTEX )
    return false ;
  P = BRep_Tool::Pnt(TopoDS::Vertex( S ));
  return true ;
}


//=================================================================================
// function : LinearEdgeExtremities()
// purpose  : If S can be converted in a linear edge and if initial an final points
//          : distance is sufficient, returns true else returns false.
//          : Resulting points are respectively P1 and P2
//=================================================================================
bool GeometryGUI::LinearEdgeExtremities( const TopoDS_Shape& S,  gp_Pnt& P1, gp_Pnt& P2 )
{
  if( S.IsNull() || S.ShapeType() != TopAbs_EDGE )
    return false ;
  BRepAdaptor_Curve curv(TopoDS::Edge(S));
  if (curv.GetType() != GeomAbs_Line)
    return false ;

  curv.D0( curv.FirstParameter(), P1 );
  curv.D0( curv.LastParameter(), P2 );

  if( P1.Distance(P2) <= Precision::Confusion() )
    return false ;
  
  return true ;
}

//=================================================================================
// function : GetBipointDxDyDz()
// purpose  : 
//=================================================================================
void GeometryGUI::GetBipointDxDyDz( gp_Pnt P1, gp_Pnt P2, double& dx, double& dy, double& dz )
{
  dx = P2.X() - P1.X() ;
  dy = P2.Y() - P1.Y() ;
  dz = P2.Z() - P1.Z() ;
  return ;
}

//=======================================================================
// function : GetTopoFromSelection()
// purpose  : Define tds from a single selection and retuen true
//=======================================================================
bool GeometryGUI::GetTopoFromSelection(SALOME_Selection *Sel, TopoDS_Shape& tds) 
{
  if(Sel->IObjectCount() != 1)
    return false ;
  
  Handle(SALOME_InteractiveObject) IO = Sel->firstIObject();
  /* case SObject */
  if ( IO->hasEntry() ) {
    SALOMEDS::Study_var aStudy = GeomGUI->myActiveStudy->getStudyDocument();
    SALOMEDS::SObject_var obj = aStudy->FindObjectID( IO->getEntry() );
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeIOR_var     anIOR;
    if ( !obj->_is_nil() ) {
       if (obj->FindAttribute(anAttr, "AttributeIOR")) {
         anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	 tds = this->GetShapeFromIOR(anIOR->Value());
	 if(tds.IsNull() )
	   return false ;
	 else
	   return true ;
       }
    }
  }

  if ( IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject)) ) {
    Standard_CString ior = "";
    Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast( IO );
    ior = GIObject->getIOR();
    tds = this->GetShapeFromIOR(ior);
    if(tds.IsNull() )
      return false ;
    else
      return true ;
  }
  
  return false;
}



//=====================================================================================
// function : PrepareSubShapeSelection()
// purpose  : ( localContextId of the method is opened and defined here)
//=====================================================================================
bool GeometryGUI::PrepareSubShapeSelection(const int SubShapeType, Standard_Integer& returnLocalContextId)
{
  //* Test the type of viewer */
  if ( this->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
    return false;
  
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  
  /* local context opening */
  SetDisplayedObjectList() ;
  OnDisplayOnly();

  returnLocalContextId = ic->OpenLocalContext(Standard_False, Standard_True, Standard_False, Standard_False) ;
  SALOME_Selection* Sel = SALOME_Selection::Selection( this->myActiveStudy->getSelection() );
  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
  for(;It.More();It.Next()) {
    Handle(SALOME_InteractiveObject) IObject = It.Value();
    Standard_Boolean found;
    Handle(GEOM_AISShape) Shape = ConvertIOinGEOMAISShape(IObject, found);
    if ( found && SubShapeType >= Shape->Shape().ShapeType()) {
      ic->Load(Shape, (8 - SubShapeType), Standard_True);
      ic->HilightWithColor(Shape, Quantity_NOC_RED);
    }
  }
  myDesktop->putInfo (tr("GEOM_PRP_SELECT_SUBSHAPES"));
  return true ;
}



//=====================================================================================
// function : PrepareSubShapeSelectionArgumentShape()
// purpose  : ( localContextId of the method is opened and defined here )
//=====================================================================================
bool GeometryGUI::PrepareSubShapeSelectionArgumentShape( const TopoDS_Shape& aShape,
							 const int SubShapeType,
							 Standard_Integer& returnLocalContextId )
{
  //* Test the type of viewer */
  if ( this->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
    return false;
  
  if( aShape.IsNull() )
    return false ;
  
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  
  /* local context opening */
  SetDisplayedObjectList() ;
  OnDisplayOnly();
  
  returnLocalContextId = ic->OpenLocalContext(Standard_False, Standard_True, Standard_False, Standard_False) ;
  
  Handle(GEOM_AISShape) Shape = new GEOM_AISShape(aShape, "") ;  
  ic->Display(Shape, 0, (8 - SubShapeType));

  //  Not Load(...) but Display(...)
  //  ic->Load(Shape, (8 - SubShapeType), Standard_True);
  ic->HilightWithColor(Shape, Quantity_NOC_RED);
  
  myDesktop->putInfo (tr("GEOM_PRP_SELECT_SUBSHAPES"));
  return true ;
}



//=======================================================================
// function : GetNameOfSelectedIObjects()
// purpose  : Define the name geom++ or other name of mono or multi sel.
//=======================================================================
int GeometryGUI::GetNameOfSelectedIObjects( SALOME_Selection* Sel,
					    QString& aName )
{
  int nbSel = Sel->IObjectCount() ;
  if ( nbSel == 1 ) {
    Handle(SALOME_InteractiveObject) IObject = Sel->firstIObject();
    aName = IObject->getName();
  } else {
    aName = tr( "%1_objects" ).arg( nbSel );
  }
  return nbSel;
}


//=======================================================================
// function : ConvertIOinGEOMAISShape()
// purpose  :
//=======================================================================
Handle(GEOM_AISShape) GeometryGUI::ConvertIOinGEOMAISShape( const Handle(SALOME_InteractiveObject)& IO,
							    Standard_Boolean& testResult,
							    bool onlyInActiveView )
{
  Handle(GEOM_AISShape) res;
  int nbSf = myActiveStudy->getStudyFramesCount();
  for ( int i = 0; i < nbSf; i++ ) {
    QAD_StudyFrame* sf = myActiveStudy->getStudyFrame(i);
    if ( sf->getTypeView() == VIEW_OCC ) {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getViewer();
      Handle (AIS_InteractiveContext) ic = v3d->getAISContext();

      AIS_ListOfInteractive List;
      ic->DisplayedObjects(List);
      AIS_ListOfInteractive List1;
      ic->ObjectsInCollector(List1);
      List.Append(List1);
      
      AIS_ListIteratorOfListOfInteractive ite(List);
      while (ite.More()) {
	if (ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	  Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	  if ( aSh->hasIO() ) {
	    Handle(GEOM_InteractiveObject) GIO = Handle(GEOM_InteractiveObject)::DownCast(aSh->getIO());
	    if ( GIO->isSame( IO ) ) {
	      if ( onlyInActiveView ) {
		if ( sf == myActiveStudy->getActiveStudyFrame() ) {
		  testResult = true;
		  return aSh;
		}
	      } else {
		testResult = true;
		return aSh;
	      }
	    }
	  }
	}
	ite.Next();
      }  
    }
  }
  testResult = false;
  return res;
}

//=======================================================================
// function : ConvertIORinGEOMAISShape()
// purpose  :
//=======================================================================
Handle(GEOM_AISShape) GeometryGUI::ConvertIORinGEOMAISShape( const char * IOR,
							     Standard_Boolean& testResult,
							     bool onlyInActiveView )
{
Handle(GEOM_AISShape) resultShape;
testResult = false;
  int nbSf = myActiveStudy->getStudyFramesCount();
  for ( int i = 0; i < nbSf; i++ ) 
    {
      QAD_StudyFrame* sf = myActiveStudy->getStudyFrame(i);
      if ( sf->getTypeView() == VIEW_OCC ) 
	{
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getViewer();
	  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();

	  AIS_ListOfInteractive List;
	  ic->DisplayedObjects(List);
	  AIS_ListOfInteractive List1;
	  ic->ObjectsInCollector(List1);
	  List.Append(List1);
      
	  AIS_ListIteratorOfListOfInteractive ite(List);
	  while (ite.More()) 
	    {
	      if (ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) 
		{
		  Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
		  if (  aSh->hasIO() ) 
		    {
		      Handle(GEOM_InteractiveObject) GIO = Handle(GEOM_InteractiveObject)::DownCast(aSh->getIO());
		      Standard_CString theIOR = GIO->getIOR();
		      if ( strcmp( IOR, theIOR ) == 0 ) 
			{
			  if ( onlyInActiveView ) 
			    {
			      if ( sf == myActiveStudy->getActiveStudyFrame() ) 
				{
				  testResult = true;
				  resultShape = aSh;
				  return resultShape; 
				}
			    } 
			  else 
			    {
			      testResult = true;
			      resultShape = aSh;
			      return resultShape; 
			    }
			}
		    }
		}
	      ite.Next();
	    }  
	}
    }
return  resultShape;
}
//=======================================================================
// function : ConvertIORinGEOMActor()
// purpose  :
//=======================================================================
GEOM_Actor* GeometryGUI::ConvertIORinGEOMActor( const char * IOR,
						Standard_Boolean& testResult,
						bool onlyInActiveView )
{
  int nbSf = GeomGUI->myActiveStudy->getStudyFramesCount();
  for ( int i = 0; i < nbSf; i++ ) {
    QAD_StudyFrame* sf = myActiveStudy->getStudyFrame(i);
    if ( sf->getTypeView() == VIEW_VTK ) {
      vtkRenderer* Renderer = ((VTKViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getRenderer();
      vtkActorCollection* theActors = Renderer->GetActors();
      theActors->InitTraversal();
      vtkActor *ac = theActors->GetNextActor();
      while(!(ac==NULL)) {
	if ( ac->IsA("GEOM_Actor") ) {
	  GEOM_Actor* anActor = GEOM_Actor::SafeDownCast( ac );
	  if ( anActor->hasIO() ) {
	    Handle(GEOM_InteractiveObject) GIO = Handle(GEOM_InteractiveObject)::DownCast(anActor->getIO());
	    Standard_CString theIOR = GIO->getIOR();
	    if ( strcmp( IOR, theIOR ) == 0 ) {
	      if ( onlyInActiveView ) {
		if ( sf == GeomGUI->myActiveStudy->getActiveStudyFrame() ) {
		  testResult = true;
		  return anActor;
		}
	      } else {
		testResult = true;
		return anActor;
	      }
	    }
	  }
	}
	ac = theActors->GetNextActor();
      }
    }
  }
  testResult = false;
  return GEOM_Actor::New();
}

//=======================================================================
// function : ConvertIOinGEOMShape()
// purpose  :
//=======================================================================
GEOM::GEOM_Shape_ptr GeometryGUI::ConvertIOinGEOMShape( const Handle(SALOME_InteractiveObject)& IO,
						  Standard_Boolean& testResult )
{
  GEOM::GEOM_Shape_var aShape ;
  testResult = false ;
  
  /* case SObject */
  if ( IO->hasEntry() ) {
    SALOMEDS::Study_var aStudy = GeomGUI->myActiveStudy->getStudyDocument();
    SALOMEDS::SObject_var obj = aStudy->FindObjectID( IO->getEntry() );
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeIOR_var     anIOR;
    if ( !obj->_is_nil() ) {
      if (obj->FindAttribute(anAttr, "AttributeIOR")) {
	anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	aShape = myComponentGeom->GetIORFromString(anIOR->Value()) ;
	if(!CORBA::is_nil(aShape)) testResult = true ;
	return aShape._retn();
      }
    }
  }
  /* case Graphical Object */
  if ( IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject)) ) {
    Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast( IO );
    Standard_CString ior = GIObject->getIOR();
    aShape = myComponentGeom->GetIORFromString(ior) ;
    if(!CORBA::is_nil(aShape)) testResult = true ;
    return aShape._retn();
  }
  return aShape._retn();
}


//=======================================================================
// function : ConvertListOfIOInListOfIOR()
// purpose  : 
//=======================================================================
void GeometryGUI::ConvertListOfIOInListOfIOR( const SALOME_ListIO& aList,
					      GEOM::GEOM_Gen::ListOfIOR& listIOR )
{
  int nbSel = aList.Extent();  
  listIOR.length(nbSel) ;
  int j=0;
  SALOME_ListIteratorOfListIO It( aList );
  for ( int i=0; It.More(); It.Next(), i++ ) 
    {
      Handle(SALOME_InteractiveObject) IObject = It.Value();
      
      if ( IObject->hasEntry() ) 
	{
	  SALOMEDS::Study_var aStudy = GeomGUI->myActiveStudy->getStudyDocument();
	  SALOMEDS::SObject_var obj = aStudy->FindObjectID( IObject->getEntry() );
          SALOMEDS::GenericAttribute_var anAttr;
          SALOMEDS::AttributeIOR_var     anIOR;
	  if ( !obj->_is_nil() && obj->FindAttribute(anAttr, "AttributeIOR") )
	    {
	      anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	      ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
	      CORBA::ORB_var& _orb = init.orb() ;
	      CORBA::String_var theValue = anIOR->Value();
	      CORBA::Object_var theObj = _orb->string_to_object(theValue);
	      if (theObj->_is_a("IDL:GEOM/GEOM_Shape:1.0"))
		{
		  listIOR[j] = strdup(theValue) ;
		  j++;
		}
	    }
	} 
      else if ( IObject->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject)) ) {
      Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast( IObject );
      Standard_CString ior = GIObject->getIOR();
      listIOR[j] = strdup(ior) ;
      j++;
    }
  }
  listIOR.length(j) ;
}


//=================================================================================
// function : GetShapeTypeString()
// purpose  : for a single shape
//=================================================================================
bool GeometryGUI::GetShapeTypeString( const TopoDS_Shape& aShape, Standard_CString& aTypeString )
{
  if( aShape.IsNull() ) {
    aTypeString = "aNullShape" ;
    return false ;
  }
  switch (aShape.ShapeType() )
  {
  case TopAbs_COMPOUND:
    { aTypeString = strdup(tr("GEOM_COMPOUND")) ; return true ; }
  case  TopAbs_COMPSOLID:
    { aTypeString = strdup(tr("GEOM_COMPOUNDSOLID")) ; return true ; }
  case TopAbs_SOLID:
    { aTypeString = strdup(tr("GEOM_SOLID")) ; return true ; }
  case TopAbs_SHELL:
    { aTypeString = strdup(tr("GEOM_SHELL")) ; return true ; }
  case TopAbs_FACE:
    { 
      BRepAdaptor_Surface surf(TopoDS::Face(aShape));
      if ( surf.GetType() == GeomAbs_Plane ) {
	aTypeString = strdup(tr("GEOM_PLANE")) ;
	return true ;
      } else if ( surf.GetType() == GeomAbs_Cylinder ) {
	aTypeString = strdup(tr("GEOM_SURFCYLINDER")) ;
	return true ;
      } else if ( surf.GetType() == GeomAbs_Sphere ) {
	aTypeString = strdup(tr("GEOM_SURFSPHERE")) ;
	return true ;
      } else if ( surf.GetType() == GeomAbs_Torus ) {
	aTypeString = strdup(tr("GEOM_SURFTORUS")) ;
	return true ;
      } else if ( surf.GetType() == GeomAbs_Cone ) {
	aTypeString = strdup(tr("GEOM_SURFCONE")) ;
	return true ;
      } else {
	aTypeString = strdup(tr("GEOM_FACE")) ;
	return true ;
      }
    }
  case TopAbs_WIRE:
    { aTypeString = strdup(tr("GEOM_WIRE")) ; return true ; }
  case TopAbs_EDGE:
    { 
      BRepAdaptor_Curve curv(TopoDS::Edge(aShape));
      if ( curv.GetType() == GeomAbs_Line ) {
	if ( (Abs(curv.FirstParameter()) >= 1E6 ) || 
	     (Abs(curv.LastParameter()) >= 1E6 )) {
	  aTypeString = strdup(tr("GEOM_LINE")) ;
	} else
	  aTypeString = strdup(tr("GEOM_EDGE")) ;
	return true ;
      } else if ( curv.GetType() == GeomAbs_Circle ) {
	if ( curv.IsClosed() )
	  aTypeString = strdup(tr("GEOM_CIRCLE")) ;
	else
	  aTypeString = strdup(tr("GEOM_ARC")) ;
	return true ;
      } else {
	aTypeString = strdup(tr("GEOM_EDGE")) ;
	return true ;
      }
    }
  case TopAbs_VERTEX:
    { aTypeString = strdup(tr("GEOM_VERTEX")) ; return true ; }
  case TopAbs_SHAPE:
    { aTypeString = strdup(tr("GEOM_SHAPE")) ; return true ; }
  }
  return false ;
}


//=================================================================================
// function : CreateArrowForLinearEdge()
// purpose  : Create a cone topology to be used to display an arrow in the middle
//          : of an edge showing its orientation. (For simulation and Viewer OCC only)
//=================================================================================
bool GeometryGUI::CreateArrowForLinearEdge( const TopoDS_Shape& tds, TopoDS_Shape& ArrowCone )
{
  if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() != VIEW_OCC || tds.ShapeType() != TopAbs_EDGE )
    return false;
  
  OCCViewer_ViewPort* vp = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewPort();
  Handle( V3d_View) view3d = ((OCCViewer_ViewPort3d*)vp)->getView();
  Standard_Real Width, Height ;
  view3d->Size(Width, Height) ;
  const Standard_Real aHeight = (Width + Height) / 50.0 ;
  
  try {
    Standard_Real first, last ;
    Handle(Geom_Curve) curv = BRep_Tool::Curve(TopoDS::Edge(tds), first, last);
    if( !curv->IsCN(1) )
      return false ;
    
    const Standard_Real param = (first+last) / 2.0 ;
    gp_Pnt middleParamPoint ;
    gp_Vec V1 ;
    curv->D1( param, middleParamPoint, V1) ;
    if ( V1.Magnitude() < Precision::Confusion() )
      return false ;
    
    /* Topology orientation not geom orientation */
    if( tds.Orientation() == TopAbs_REVERSED )
      V1 *= -1.0 ;
    
    gp_Ax2 anAxis( middleParamPoint, gp_Dir(V1) ) ;
    const Standard_Real radius1 = aHeight / 5.0 ;
    if( radius1 > 10.0 * Precision::Confusion() && aHeight > 10.0 * Precision::Confusion() ) {
      ArrowCone = BRepPrimAPI_MakeCone( anAxis, radius1, 0.0, aHeight ).Shape() ;
      return true ;
    }
  }
  catch(Standard_Failure) {
    // OCC failures are hard to catch in GUI.
    // This  because of the position for  #include <Standard_ErrorHandler.hxx> that is very critic to find 
    // in SALOME environment : compilation error !
  }
  return false ;
}


//=======================================================================
// function : SelectionByNameInDialogs()
// purpose  : Called when user has entered a name of object in a LineEdit.
//          : The selection is changed. Dialog box will receive the 
//          : corresponding signal to manage this event.
//=======================================================================
bool GeometryGUI::SelectionByNameInDialogs( QWidget* aWidget, const QString& objectUserName, SALOME_Selection* Sel )
{
  
  /* Find SObject with name in component GEOM */
  SALOMEDS::Study_var ST = this->myActiveStudy->getStudyDocument() ;
  SALOMEDS::Study::ListOfSObject_var listSO = new SALOMEDS::Study::ListOfSObject ;
  listSO = ST->FindObjectByName( objectUserName, "GEOM" ) ;
  
  if ( listSO->length() < 1 ) {
    const QString caption  = tr("GEOM_WRN_WARNING") ;
    const QString text = tr("GEOM_NAME_INCORRECT") ;
    const QString button0  = tr("GEOM_BUT_OK") ;
    QMessageBox::warning( aWidget, caption, text, button0 ) ;
    return false ;
  }
  /* More than one object with same name */
  if ( listSO->length() > 1 ) {
    const QString caption  = tr("GEOM_WRN_WARNING") ;
    const QString text = tr("GEOM_IDENTICAL_NAMES_SELECT_BY_MOUSE") ;
    const QString button0  = tr("GEOM_BUT_OK") ;
    QMessageBox::warning( aWidget, caption, text, button0 ) ;
    return false ;
  }

  SALOMEDS::SObject_var theObj = listSO[0] ;
  /* Create a SALOME_InteractiveObject with a SALOME::SObject */
  Standard_CString anEntry = theObj->GetID() ;
  Handle(SALOME_InteractiveObject) SI = new SALOME_InteractiveObject( anEntry, "GEOM", strdup(objectUserName) ) ;

  /* Add as a selected object       */
  /* Clear any previous selection : */
  /* Warning the LineEdit is purged because of signal currentSelectionChanged ! */ 
  Sel->ClearIObjects() ; 
  Sel->AddIObject( SI ) ;
  return true ;
}


//=======================================================================
// function : MakePointAndDisplay
// purpose  : 
//=======================================================================
void GeometryGUI::MakePointAndDisplay( const double x, const double y, const double z ) 
{
  try {
    GEOM::GEOM_Shape_var P = myComponentGeom->MakeVertex( x, y, z );
    P->NameType( tr("GEOM_VERTEX") );
    if ( Display( P, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }  
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return ;
}



//=======================================================================
// function : MakeVectorAndDisplay()
// purpose  : 
//=======================================================================
void GeometryGUI::MakeVectorAndDisplay( const gp_Pnt P1, const gp_Pnt P2 )
{
  try {
    GEOM::PointStruct pstruct1 = myComponentGeom->MakePointStruct( P1.X(), P1.Y(), P1.Z() ) ;
    GEOM::PointStruct pstruct2 = myComponentGeom->MakePointStruct( P2.X(), P2.Y(), P2.Z() ) ;
    GEOM::GEOM_Shape_var Vector = myComponentGeom->MakeVector( pstruct1, pstruct2 );
    Vector->NameType(tr("GEOM_VECTOR"));
    if ( Display( Vector, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
}


//=======================================================================
// function : MakeBoxAndDisplay()
// purpose  : 
//=======================================================================
void GeometryGUI::MakeBoxAndDisplay( const gp_Pnt P1, const gp_Pnt P2 )
{
  try {
    GEOM::GEOM_Shape_var box = myComponentGeom->MakeBox( P1.X(), P1.Y(), P1.Z(), P2.X(), P2.Y(), P2.Z() );
    box->NameType(tr("GEOM_BOX"));
    if ( Display( box, "") ) {
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
    }
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
}




//=======================================================================
// function : MakePlaneAndDisplay()
// purpose  : Plane point is P1 and dx, dy, dz define a normal vector
//=======================================================================
void GeometryGUI::MakePlaneAndDisplay( const gp_Pnt P1,        const Standard_Real dx,
				       const Standard_Real dy, const Standard_Real dz,
				       const Standard_Real TrimSize )
{
  try {
    gp_Dir aDirection;
    aDirection.SetCoord(dx, dy, dz) ;
    gp_Ax2 Axis( P1, aDirection );
    
    GEOM::PointStruct pstruct = myComponentGeom->MakePointStruct(P1.X(), P1.Y(), P1.Z() ) ;
    GEOM::PointStruct d = myComponentGeom->MakePointStruct(aDirection.X(), aDirection.Y(), aDirection.Z()) ;
    GEOM::DirStruct dstruct = myComponentGeom->MakeDirection(d) ;
    GEOM::GEOM_Shape_ptr plane = myComponentGeom->MakePlane(pstruct, dstruct, TrimSize) ;
    plane->NameType(tr("GEOM_PLANE"));
    if ( Display( plane, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
}


//=======================================================================
// function : MakeWorkingPlane()
// purpose  : Change the point of view3d
//=======================================================================
void GeometryGUI::MakeWorkingPlane( const gp_Pnt P, const gp_Dir D)
{
  if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() != VIEW_OCC) {
    GeomGUI->myDesktop->putInfo( tr("GEOM_PRP_NOT_FOR_VTK_VIEWER") ) ;
    return ;
  }
  OCCViewer_ViewPort* vp = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewPort();
  Handle( V3d_View) view3d = ((OCCViewer_ViewPort3d*)vp)->getView();
  view3d->SetAt( P.X(), P.Y(), P.Z() );
  view3d->SetProj( D.X(), D.Y(), D.Z() );
  myDesktop->putInfo(tr("GEOM_PRP_DONE"));
}


//=======================================================================
// function : MakePrismAndDisplay()
// purpose  : P1 and P2 is to define a vector for prism
//=======================================================================
void GeometryGUI::MakePrismAndDisplay( GEOM::GEOM_Shape_ptr BaseShape, const gp_Pnt P1, const gp_Pnt P2 )
{
  try {
    GEOM::PointStruct PS1 = myComponentGeom->MakePointStruct(P1.X(), P1.Y(), P1.Z() ) ;
    GEOM::PointStruct PS2 = myComponentGeom->MakePointStruct(P2.X(), P2.Y(), P2.Z() ) ;
    if( BaseShape->_is_nil() ) {
      myDesktop->putInfo (tr("GEOM_PRP_NULLSHAPE")) ; 
      return ;
    }
    
    GEOM::GEOM_Shape_ptr result = myComponentGeom->MakePrism( BaseShape, PS1, PS2 ) ;
    if (result->_is_nil()) {
      myDesktop->putInfo (tr("GEOM_PRP_NULLSHAPE")) ; 
      return ;
    }  

    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, result);
    Standard_CString type;
    GetShapeTypeString(S,type);
    result->NameType( type );

    if ( Display( result, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return ;
}


//=====================================================================================
// function : MakeLineAndDisplay()
// purpose  : Create an infinite oriented line (linear edge in fact) 
//=====================================================================================
void GeometryGUI::MakeLineAndDisplay( const gp_Pnt InitPoint, const gp_Pnt LastPoint )
{
  gp_Pnt P1,P2;  
  double dx, dy, dz ;
  GetBipointDxDyDz( InitPoint, LastPoint, dx, dy, dz ) ;
  Standard_Real length = InitPoint.Distance(LastPoint) ;
  if( length <= Precision::Confusion() ) {
    myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
    return ;
  }
  
  Standard_Real coeff = 1E6 / length ;
  
  /* To create a line with length = 1E6	    */
  /* Precision::Infinite() is 1E100 in OCC  */
  P1.SetX( InitPoint.X() - (coeff * dx) ) ;
  P1.SetY( InitPoint.Y() - (coeff * dy) ) ;
  P1.SetZ( InitPoint.Z() - (coeff * dz) ) ;
  
  P2.SetX( LastPoint.X() + (coeff * dx) ) ;
  P2.SetY( LastPoint.Y() + (coeff * dy) ) ;
  P2.SetZ( LastPoint.Z() + (coeff * dz) ) ;
  
  try {
    GEOM::PointStruct pstruct = myComponentGeom->MakePointStruct(P1.X(), P1.Y(), P1.Z()) ;
    GEOM::PointStruct d = myComponentGeom->MakePointStruct(P2.X(), P2.Y(), P2.Z()) ;
    GEOM::DirStruct dstruct = myComponentGeom->MakeDirection(d) ;
    
    GEOM::GEOM_Shape_ptr result = myComponentGeom->MakeLine(pstruct, dstruct) ;
    if(result->_is_nil()) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
      return ;
    }
    result->NameType(tr("GEOM_LINE"));
    
    if ( Display( result, "") )
      myDesktop->putInfo(tr("GEOM_PRP_READY"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return ;
}



//=======================================================================
// function : MakeBooleanAndDisplay()
// purpose  : 
//=======================================================================
void GeometryGUI::MakeBooleanAndDisplay( GEOM::GEOM_Shape_ptr Shape1, GEOM::GEOM_Shape_ptr Shape2, const short operation )
{
  try {
    GEOM::GEOM_Shape_ptr result = myComponentGeom->MakeBoolean( Shape1, Shape2, operation ) ;
    if (result->_is_nil()) {
      myDesktop->putInfo (tr("GEOM_PRP_NULLSHAPE")) ; 
      return ;
    }  

    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, result);
    Standard_CString type;
    GetShapeTypeString(S,type);
    result->NameType( type );

    if ( Display( result, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return ;
}


//=====================================================================================
// function : MakeSphere()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeSphereAndDisplay( const gp_Pnt aCenterPoint, const double aRadius )
{
  try {
    if( aRadius <= Precision::Confusion() ) {
      QAD_MessageBox::warn1 ( QAD_Application::getDesktop(), tr ("GEOM_WRN_WARNING"),
			      tr ("GEOM_WRN_RADIUS_NULL"),   tr ("GEOM_BUT_YES") );
      return ;
    }
    GEOM::GEOM_Shape_ptr result = myComponentGeom->MakeSphere(aCenterPoint.X(),aCenterPoint.Y(),aCenterPoint.Z(), aRadius);
    result->NameType(tr("GEOM_SPHERE"));
    if ( Display( result, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return ;
}


//=====================================================================================
// function : MakeConeAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeConeAndDisplay( const gp_Pnt BasePoint,
				      const gp_Dir aDir,
				      const double Radius1,
				      const double Radius2,
				      const double aHeight )
{
  try {
    if( ( Radius1 <= Precision::Confusion() && Radius2 <= Precision::Confusion() ) || aHeight <= Precision::Confusion() )
      return ;
    GEOM::PointStruct pstruct = myComponentGeom->MakePointStruct( BasePoint.X(), BasePoint.Y(), BasePoint.Z() ) ;
    GEOM::PointStruct d = myComponentGeom->MakePointStruct( aDir.X(), aDir.Y(), aDir.Z() ) ;
    GEOM::DirStruct dstruct = myComponentGeom->MakeDirection(d) ;
    
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeCone( pstruct, dstruct, Radius1, Radius2, aHeight ) ;  
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
      return  ;
    }
    result->NameType(tr("GEOM_CONE"));
    if ( Display( result, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  } 
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return  ;
}


//=====================================================================================
// function : MakeCylinderAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeCylinderAndDisplay( const gp_Pnt BasePoint,
					  const gp_Dir aDir,
					  const double Radius,
					  const double aHeight )
{
  try {
    if( Radius <= Precision::Confusion() || aHeight <= Precision::Confusion() )
      return ;
    GEOM::PointStruct pstruct = myComponentGeom->MakePointStruct( BasePoint.X(), BasePoint.Y(), BasePoint.Z() ) ;
    GEOM::PointStruct d = myComponentGeom ->MakePointStruct( aDir.X(), aDir.Y(), aDir.Z() ) ;
    GEOM::DirStruct dstruct = myComponentGeom->MakeDirection(d) ;
    
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeCylinder(pstruct, dstruct, Radius, aHeight) ;
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
      return  ;
    }
    result->NameType(tr("GEOM_CYLINDER"));
    if ( Display( result, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return  ;
}


//=====================================================================================
// function : MakeTorusAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeTorusAndDisplay( const gp_Pnt BasePoint,
				       const gp_Dir aDir,
				       const double Radius1,
				       const double Radius2 )
{
  try {
    if( Radius1 <= Precision::Confusion() || Radius2 <= Precision::Confusion() )
      return ;
    
    GEOM::PointStruct pstruct = myComponentGeom->MakePointStruct( BasePoint.X(), BasePoint.Y(), BasePoint.Z() ) ;
    GEOM::PointStruct d = myComponentGeom ->MakePointStruct( aDir.X(), aDir.Y(), aDir.Z() ) ;
    GEOM::DirStruct dstruct = myComponentGeom->MakeDirection(d) ;
    
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeTorus(pstruct, dstruct, Radius1, Radius2) ;
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
      return  ;
    }
    result->NameType(tr("GEOM_TORUS"));
    if ( Display( result, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return  ;
}


//=====================================================================================
// function : MakePipeAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakePipeAndDisplay( GEOM::GEOM_Shape_ptr aPath, GEOM::GEOM_Shape_ptr aBase ) 
{
  try {
    GEOM::GEOM_Shape_ptr result = myComponentGeom->MakePipe(aPath, aBase);
    if ( !result->_is_nil() && Display( result, "") ) {

      TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, result);
      Standard_CString type;
      GetShapeTypeString(S,type);
      result->NameType( type );

      myDesktop->putInfo( tr("GEOM_PRP_DONE") );
    }
    else {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
    }
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return ;
}


//=====================================================================================
// function : MakeFillingAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeFillingAndDisplay( GEOM::GEOM_Shape_ptr SectionShape, 
					 const short mindeg,
					 const short maxdeg,
					 const double tol3d, 
					 const double tol2d, 
					 const short nbiter )
{  
  try {
    GEOM::GEOM_Shape_ptr result = myComponentGeom->MakeFilling( SectionShape,
							  mindeg, maxdeg,
							  tol3d, tol2d, nbiter );
    if (result->_is_nil()) {
      myDesktop->putInfo (tr("GEOM_PRP_NULLSHAPE")) ; 
      return ;
    }  

    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, result);
    Standard_CString type;
    GetShapeTypeString(S,type);
    result->NameType( type );

    if ( Display( result, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
    return ;  
  } 
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
}



//=====================================================================================
// function : MakeMirrorAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeMirrorAndDisplay( GEOM::GEOM_Shape_ptr Shape1, GEOM::GEOM_Shape_ptr Shape2 )
{
  try {
    GEOM::GEOM_Shape_var result  = myComponentGeom->MakeMirrorByPlane( Shape1, Shape2 );
    if (result->_is_nil()) {
      myDesktop->putInfo (tr("GEOM_PRP_NULLSHAPE")) ; 
      return ;
    }  
    result->NameType( Shape1->NameType() );
    if ( Display( result, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return ;
}


//=====================================================================================
// function : MakeSewingAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeSewingAndDisplay( GEOM::GEOM_Gen::ListOfIOR& listShapesIOR,
					const Standard_Real precision )
{
  try {
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeSewing(listShapesIOR, precision) ;
    if( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_NULLSHAPE")) ;
      return ;
    }

    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, result);
    Standard_CString type;
    GetShapeTypeString(S,type);
    result->NameType( type );

    if( Display( result, "") )
      myDesktop ->putInfo(tr("GEOM_PRP_DONE")) ;
    return ;
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
}


//=====================================================================================
// function : MakeCompoundAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeCompoundAndDisplay( GEOM::GEOM_Gen::ListOfIOR& listShapesIOR )
{
  try {
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeCompound(listShapesIOR) ;
    if( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_NULLSHAPE")) ;
      return ;
    }
    result->NameType(tr("GEOM_COMPOUND"));
    if( Display( result, "" ))
      myDesktop->putInfo(tr("GEOM_PRP_DONE")) ;
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return ;
}


//=====================================================================================
// function : MakeFaceAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeFaceAndDisplay( GEOM::GEOM_Shape_ptr aWire, const Standard_Boolean wantPlanar )
{
  try {
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeFace( aWire, wantPlanar) ;
    if( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_NULLSHAPE")) ;
      return ;
    }
    if ( wantPlanar )
      result->NameType(tr("GEOM_PLANE"));
    else
      result->NameType(tr("GEOM_FACE"));
    if( Display( result, "") )
      myDesktop ->putInfo(tr("GEOM_PRP_DONE")) ;
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return ;
}



//=====================================================================================
// function : MakeLinearEdgeAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeLinearEdgeAndDisplay( const gp_Pnt P1, const gp_Pnt P2 )
{
  try {
    GEOM::PointStruct ps1 = myComponentGeom->MakePointStruct( P1.X(), P1.Y(), P1.Z() ) ;
    GEOM::PointStruct ps2 = myComponentGeom->MakePointStruct( P2.X(), P2.Y(), P2.Z() ) ;
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeEdge( ps1, ps2 ) ;
    if( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_NULLSHAPE")) ;
      return ;
    }
    result->NameType(tr("GEOM_EDGE"));
    if( Display( result, "") )
      myDesktop ->putInfo(tr("GEOM_PRP_DONE")) ;
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return ;
}


//=====================================================================================
// function : MakeOrientationChangeAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeOrientationChangeAndDisplay( GEOM::GEOM_Shape_ptr Shape )
{
  try {
    GEOM::GEOM_Shape_var result = myComponentGeom->OrientationChange( Shape ) ;
    if( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_NULLSHAPE")) ;
      return ;
    }
    result->NameType( Shape->NameType() );
    if ( Display( result, "" ))
      myDesktop->putInfo(tr("GEOM_PRP_DONE")); 
    return ;
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
}


//=====================================================================================
// function : MakeScaleAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeScaleAndDisplay( GEOM::GEOM_Shape_ptr Shape, const gp_Pnt centralPoint, const Standard_Real factor )
{
  try {
    GEOM::PointStruct P = myComponentGeom->MakePointStruct(centralPoint.X(), centralPoint.Y(), centralPoint.Z() ) ;
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeScaleTransform(Shape, P, factor) ;
    //    result->NameType(tr("GEOM_SCALE"));
    result->NameType( Shape->NameType() );
    if ( Display( result, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
    else 
      myDesktop->putInfo (tr("GEOM_PRP_NULLSHAPE")) ; 
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return ;
}


//=======================================================================================
// function : MakeRevolutionAndDisplay()
// purpose  :
//=======================================================================================
void GeometryGUI::MakeRevolutionAndDisplay( GEOM::GEOM_Shape_ptr Shape, const gp_Pnt loc, const gp_Dir dir, const Standard_Real revolAngle )
{
  try {
    GEOM::AxisStruct axis = myComponentGeom->MakeAxisStruct(loc.X(), loc.Y(), loc.Z(), dir.X(), dir.Y(), dir.Z() ) ;
    GEOM::GEOM_Shape_ptr result = myComponentGeom->MakeRevolution(Shape, axis, revolAngle) ;
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
      return  ;
    }

    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, result);
    Standard_CString type;
    GetShapeTypeString(S,type);
    result->NameType( type );

    if ( Display( result ) )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return  ;
}


//=======================================================================================
// function : MakeRotationAndDisplay()
// purpose  :
//=======================================================================================
void GeometryGUI::MakeRotationAndDisplay( GEOM::GEOM_Shape_ptr Shape,
					  const gp_Pnt loc,
					  const gp_Dir dir,
					  const Standard_Real angle )
{
  try {
    const GEOM::AxisStruct axis = myComponentGeom->MakeAxisStruct(loc.X(), loc.Y(), loc.Z(), dir.X(), dir.Y(), dir.Z() ) ;
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeRotation(Shape, axis, angle);
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT") );
      return ;
    }
    result->NameType( Shape->NameType() );
    if ( Display( result ) )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }  
  return  ;
}


//=================================================================================
// function : MakeTranslationAndDisplay()
// purpose  : Translate a shape
//=================================================================================
void GeometryGUI::MakeTranslationAndDisplay( GEOM::GEOM_Shape_ptr Shape, gp_Vec V )
{
  try {
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeTranslation( Shape, V.X(), V.Y(), V.Z() ) ;
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT") );
      return ;
    }
    result->NameType( Shape->NameType() );
    if ( Display( result ) )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }  
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return  ;
}

//=================================================================================
// function : MakeMultiTranslation1DAndDisplay()
// purpose  : Multi-Translate a shape
//=================================================================================
void GeometryGUI::MakeMultiTranslation1DAndDisplay( GEOM::GEOM_Shape_ptr Shape, const gp_Dir Dir, const double Step, const short NbTimes ) 
{
  try {
    GEOM::PointStruct d = myComponentGeom->MakePointStruct( Dir.X(), Dir.Y(), Dir.Z() ) ;
    GEOM::DirStruct dstruct = myComponentGeom->MakeDirection(d) ;

    GEOM::GEOM_Shape_var result = myComponentGeom->MakeMultiTranslation1D( Shape, dstruct, Step, NbTimes );
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT") );
      return ;
    }
    result->NameType( tr("GEOM_COMPOUND") );
    if ( Display( result ) )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }  
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return  ;
}


//=================================================================================
// function : MakeMultiTranslation2DAndDisplay()
// purpose  : Multi-Translate a shape
//=================================================================================
void GeometryGUI::MakeMultiTranslation2DAndDisplay( GEOM::GEOM_Shape_ptr Shape, const gp_Dir Dir1, const double Step1, const short NbTimes1, const gp_Dir Dir2, const double Step2, const short NbTimes2 ) 
{
  try {
    GEOM::PointStruct d1 = myComponentGeom->MakePointStruct( Dir1.X(), Dir1.Y(), Dir1.Z() ) ;
    GEOM::DirStruct dstruct1 = myComponentGeom->MakeDirection(d1) ;
    GEOM::PointStruct d2 = myComponentGeom->MakePointStruct( Dir2.X(), Dir2.Y(), Dir2.Z() ) ;
    GEOM::DirStruct dstruct2 = myComponentGeom->MakeDirection(d2) ;

    GEOM::GEOM_Shape_var result = myComponentGeom->MakeMultiTranslation2D( Shape, dstruct1, Step1, NbTimes1, dstruct2, Step2, NbTimes2 );
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT") );
      return ;
    }
    result->NameType( tr("GEOM_COMPOUND") );
    if ( Display( result ) )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }  
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return  ;
}


//=================================================================================
// function : MakeMultiRotation1DAndDisplay()
// purpose  : Multi-Rotate a shape
//=================================================================================
void GeometryGUI::MakeMultiRotation1DAndDisplay( GEOM::GEOM_Shape_ptr Shape, const gp_Dir Dir, const gp_Pnt Loc, const short NbTimes ) 
{
  try {
    GEOM::PointStruct d = myComponentGeom->MakePointStruct( Dir.X(), Dir.Y(), Dir.Z() ) ;
    GEOM::DirStruct dstruct = myComponentGeom->MakeDirection(d) ;
    GEOM::PointStruct pstruct = myComponentGeom->MakePointStruct( Loc.X(), Loc.Y(), Loc.Z() ) ;

    GEOM::GEOM_Shape_var result = myComponentGeom->MakeMultiRotation1D( Shape, dstruct, pstruct, NbTimes );
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT") );
      return ;
    }
    result->NameType( tr("GEOM_COMPOUND") );
    if ( Display( result ) )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }  
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return  ;
}


//=================================================================================
// function : MakeMultiRotation2DAndDisplay()
// purpose  : Multi-Rotate a shape
//=================================================================================
void GeometryGUI::MakeMultiRotation2DAndDisplay( GEOM::GEOM_Shape_ptr Shape, const gp_Dir Dir, const gp_Pnt Loc, const double Ang, const short NbTimes1, const double Step, const short NbTimes2 ) 
{
  try {
    GEOM::PointStruct d = myComponentGeom->MakePointStruct( Dir.X(), Dir.Y(), Dir.Z() ) ;
    GEOM::DirStruct dstruct = myComponentGeom->MakeDirection(d) ;
    GEOM::PointStruct pstruct = myComponentGeom->MakePointStruct( Loc.X(), Loc.Y(), Loc.Z() ) ;

    GEOM::GEOM_Shape_var result = myComponentGeom->MakeMultiRotation2D( Shape, dstruct, pstruct, Ang, NbTimes1, Step, NbTimes2 );
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT") );
      return ;
    }
    result->NameType( tr("GEOM_COMPOUND") );
    if ( Display( result ) )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }  
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return  ;
}



//=======================================================================================
// function : MakeArcAndDisplay()
// purpose  : Make an arc of circle from InitPoint to CirclePoint and passing on EndPoint
//=======================================================================================
void GeometryGUI::MakeArcAndDisplay( gp_Pnt InitPoint, gp_Pnt CirclePoint, gp_Pnt EndPoint )
{
  gp_Vec v1( CirclePoint, InitPoint ) ;
  gp_Vec v2( CirclePoint, EndPoint ) ;
  if( v1.IsParallel(v2, Precision::Angular() ) )
    return ;
  
  try { 
    GEOM::PointStruct pI = myComponentGeom->MakePointStruct( InitPoint.X(), InitPoint.Y(), InitPoint.Z() );
    GEOM::PointStruct pC = myComponentGeom->MakePointStruct( CirclePoint.X(), CirclePoint.Y(), CirclePoint.Z() );
    GEOM::PointStruct pE = myComponentGeom->MakePointStruct( EndPoint.X(), EndPoint.Y(), EndPoint.Z() );
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeArc(pI, pC, pE) ;
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
      return  ;
    }
    result->NameType(tr("GEOM_ARC"));
    if ( Display(  result ) )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }    
  return ;
}


//=====================================================================================
// function : MakeCircleAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeCircleAndDisplay( const gp_Pnt CenterPoint, const gp_Dir dir, const Standard_Real Radius )
{
  try {
    GEOM::PointStruct pstruct = myComponentGeom->MakePointStruct( CenterPoint.X(), CenterPoint.Y(),  CenterPoint.Z() ) ;
    GEOM::PointStruct d = myComponentGeom->MakePointStruct( dir.X(), dir.Y(), dir.Z() ) ;
    GEOM::DirStruct dstruct = myComponentGeom->MakeDirection(d) ;
    
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeCircle(pstruct, dstruct, Radius) ;
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT") );
      return ;
    }
    result->NameType(tr("GEOM_CIRCLE"));
    if ( Display( result ) )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return  ;
}


//=====================================================================================
// function : MakeEllipseAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeEllipseAndDisplay( const gp_Pnt CenterPoint,
					 const gp_Dir dir,
					 const Standard_Real Major_Radius,
					 const Standard_Real Minor_Radius )
{
  try {
    GEOM::PointStruct pstruct = myComponentGeom->MakePointStruct( CenterPoint.X(), CenterPoint.Y(),  CenterPoint.Z() ) ;
    GEOM::PointStruct d = myComponentGeom->MakePointStruct( dir.X(), dir.Y(), dir.Z() ) ;
    GEOM::DirStruct dstruct = myComponentGeom->MakeDirection(d) ;
    
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeEllipse(pstruct, dstruct, Major_Radius, Minor_Radius) ;
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT") );
      return ;
    }
    result->NameType(tr("GEOM_ELLIPSE"));
    if ( Display( result ) )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return  ;
}


//=====================================================================================
// function : MakeWireAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeWireAndDisplay( GEOM::GEOM_Gen::ListOfIOR& listShapesIOR )
{
  try {
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeWire(listShapesIOR) ;
    if( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_NULLSHAPE")) ;
      return ;
    }
    result->NameType(tr("GEOM_WIRE"));
    if( Display( result, "") )
      myDesktop ->putInfo(tr("GEOM_PRP_DONE")) ;
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return ;
}


//=====================================================================================
// function : MakePartitionAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakePartitionAndDisplay (const GEOM::GEOM_Gen::ListOfIOR& listShapesIOR,
                                           const GEOM::GEOM_Gen::ListOfIOR& listToolsIOR,
                                           const GEOM::GEOM_Gen::ListOfIOR& listKeepInsIOR,
                                           const GEOM::GEOM_Gen::ListOfIOR& listRemoveInsIOR,
                                           const GEOM::shape_type           limit)
{
  try {
    GEOM::GEOM_Shape_var result = myComponentGeom->Partition(listShapesIOR,
                                                             listToolsIOR,
                                                             listKeepInsIOR,
                                                             listRemoveInsIOR,
                                                             (CORBA::Short) limit);
    if( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_NULLSHAPE")) ;
      return ;
    }
    result->NameType(tr("GEOM_PARTITION"));
    if( Display( result, "") )
      myDesktop ->putInfo(tr("GEOM_PRP_DONE")) ;
    return ;
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
}

//=======================================================================
// function : SetState()
// purpose  : Sets myState = aState a private field indicating which methode is active
//=======================================================================
void GeometryGUI::SetState(int aState )
{  this->myState = aState ;  return ; }


//=======================================================================
// function : ResetState()
// purpose  : Sets myState = -1 a private field indicating which methode is active
//=======================================================================
void GeometryGUI::ResetState()
{  this->myState = -1 ;  return ; }


//=======================================================================
// function : EmitSignalDeactivateDialog()
// purpose  : Emit a signal to deactivate the active dialog Box
//=======================================================================
void GeometryGUI::EmitSignalDeactivateDialog()
{
  emit this->SignalDeactivateActiveDialog() ;
  return ;
}

//=======================================================================
// function : EmitSignalCloseAllDialogs()
// purpose  : Emit a signal to deactivate all non modal dialogs box
//=======================================================================
void GeometryGUI::EmitSignalCloseAllDialogs()
{
  emit this->SignalCloseAllDialogs() ;
  return ;
}


//=======================================================================
// function : GetActiveDialogBox()
// purpose  : 
//=======================================================================
QDialog* GeometryGUI::GetActiveDialogBox()
{
  return this->myActiveDialogBox ;
}


//=======================================================================
// function : SetActiveDialogBox()
// purpose  : 
//=======================================================================
void GeometryGUI::SetActiveDialogBox(QDialog* aDlg)
{
  this->myActiveDialogBox = (QDialog*)aDlg ;
  return ;
}


//=======================================================================
// function : GetActiveStudy()
// purpose  : 
//=======================================================================
QAD_Study* GeometryGUI::GetActiveStudy()
{
  return this->myActiveStudy ;
}


//=======================================================================
// function : GetActiveDesktop()
// purpose  : 
//=======================================================================
QAD_Desktop* GeometryGUI::GetDesktop()
{
  return this->myDesktop ;
}


//=====================================================================================
// function : OnSubShapeGetAll()
// purpose  : Explode a shape in all sub shapes with a SubShapeType
//=====================================================================================
bool GeometryGUI::OnSubShapeGetAll(const TopoDS_Shape& ShapeTopo, const char* ShapeTopoIOR, const int SubShapeType)
{
  SALOMEDS::Study_var   aStudy = myActiveStudy->getStudyDocument();
  SALOMEDS::SObject_var theObj = aStudy->FindObjectIOR( ShapeTopoIOR );
  if ( theObj->_is_nil() ) {
    myDesktop->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY"));
    return false ;
  }

  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::SObject_var fatherSF = aStudy->FindObjectID(myActiveStudy->getActiveStudyFrame()->entry());
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var    aName;
  SALOMEDS::AttributeIOR_var     anIOR;
  SALOMEDS::AttributePixMap_var  aPixmap;

  /* We create a sub object for each sub shape as attribute of the main object  */
  /* Each sub object contains list (length=1) containing its index in the main shape */
  GEOM::GEOM_Shape_var aShape  = myComponentGeom->GetIORFromString( ShapeTopoIOR );
  GEOM::GEOM_Gen::ListOfGeomShapes_var listGeomShapes = new GEOM::GEOM_Gen::ListOfGeomShapes;
  GEOM::GEOM_Shape_var aResult ;
  
  try {
    listGeomShapes = myComponentGeom->SubShapeAll( aShape, SubShapeType );
    if( listGeomShapes->length() < 1 ) {
      myDesktop->putInfo (tr("GEOM_PRP_ABORT"));
      return false ;
    }
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  
  /* open transaction */
  QAD_Operation* op = new SALOMEGUI_ImportOperation( myActiveStudy );
  op->start();
  
  TopoDS_Shape mainTopo = ShapeReader.GetShape(myComponentGeom, aShape);
  TopoDS_Shape mainShape;
  bool main = false;
  while ( !main ) {
    if ( aShape->IsMainShape() ) {
      mainShape = ShapeReader.GetShape(myComponentGeom, aShape);
      main = true;
    } else
      aShape = myComponentGeom->GetIORFromString( aShape->MainName() );
  }
  
  /* Loop on each sub shape created */
  /* int i = 1 ; /* index for the nameType */  
  for( int j=0; j<listGeomShapes->length(); j++) {
    
    /* Get each sub shape extracted CORBA and OCC */
    aResult = listGeomShapes[j] ;    
    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, aResult);
    
    if ( S.IsNull() ) {
      myDesktop->putInfo (tr("GEOM_PRP_ABORT"));
      return false;
    }

// NRI : Following lines are commented according to bugID SAL695 (see history)
//      BRepCheck_Analyzer anAnalyzer(S);
//      if(!anAnalyzer.IsValid()) {
//        myDesktop->putInfo (tr("GEOM_PRP_ABORT"));
//        MESSAGE("#### An exploded shape number "<< j << " is not valid");
//        op->abort();
//        return false;
//      }
    
    /* Set the nameType of sub shape */
    char* nameG = (char *)malloc(20);
    Standard_CString Type;
    if ( GetShapeTypeString(S, Type) ) {
      aResult->NameType( Type );
      sprintf (nameG, "%s_%d", Type, GetIndex( S, mainShape, SubShapeType ) );
    } 
    else {
      aResult->NameType( tr("GEOM_SHAPE") );
      sprintf (nameG, "%s_%d", tr("GEOM_SHAPE").latin1(), this->myNbGeom++ );
    }
    SALOMEDS::SObject_var   SO = aStudy->FindObjectIOR( aResult->Name() );

    bool allreadyexist = false;
          
    if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
      Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
      
      Handle(GEOM_AISShape) result = new GEOM_AISShape( S, nameG );      
      Handle(GEOM_InteractiveObject) IO = new GEOM_InteractiveObject(aResult->Name(), Fatherior, "GEOM");
      
      MESSAGE ( "SO->_is_nil() " << SO->_is_nil() )

      if ( SO->_is_nil() ) {
	SALOMEDS::SObject_var newObj = aStudyBuilder->NewObject( theObj );
        anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
        aName->SetValue(nameG);
        anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
        anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        anIOR->SetValue(aResult->Name());

	anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributePixMap");
	aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
	MESSAGE( " Type " << S.ShapeType() )
	if ( S.ShapeType() == TopAbs_COMPOUND ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_COMPOUND" );
	} else if ( S.ShapeType() == TopAbs_COMPSOLID ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_COMPSOLID" );
	} else if ( S.ShapeType() == TopAbs_SOLID ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_SOLID" );
	} else if ( S.ShapeType() == TopAbs_SHELL ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_SHELL" );
	} else if ( S.ShapeType() == TopAbs_FACE ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_FACE" );
	} else if ( S.ShapeType() == TopAbs_WIRE ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_WIRE" );
	} else if ( S.ShapeType() == TopAbs_EDGE ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_EDGE" );
	} else if ( S.ShapeType() == TopAbs_VERTEX ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_VERTEX" );
	}

	MESSAGE ( " aPixmap->GetPixMap " << aPixmap->GetPixMap() )

	SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
	aStudyBuilder->Addreference(newObj1, newObj);
	IO->setEntry(newObj->GetID());

	aResult->StudyShapeId( newObj->GetID() );
      } else {
	allreadyexist = true;
 	if ( ! SObjectExist(theObj, aResult->Name()) ) {
	  SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(theObj);
	  aStudyBuilder->Addreference(newObj1, SO);
	  IO->setEntry(SO->GetID());
	  aResult->StudyShapeId( SO->GetID() );
	}
      }
      
      result->setIO( IO );
      result->setName( nameG );
      if ( !allreadyexist )
	ic->Display(result);
      
    } else if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
      VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
      
      int themode = myRenderInter->GetDisplayMode();
      vtkRenderer *theRenderer = ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
      vtkRenderWindow *renWin = theRenderer->GetRenderWindow();
      
      Handle(GEOM_InteractiveObject) IO = new GEOM_InteractiveObject(aResult->Name(), Fatherior,"GEOM");
      
      if ( SO->_is_nil() ) {
	SALOMEDS::SObject_var newObj = aStudyBuilder->NewObject( theObj );
        anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
        anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
        anIOR->SetValue(aResult->Name());
        anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeName");
        aName = SALOMEDS::AttributeName::_narrow(anAttr);
        aName->SetValue(nameG);
	
	anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributePixMap");
	aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
	if ( S.ShapeType() == TopAbs_COMPOUND ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_COMPOUND" );
	} else if ( S.ShapeType() == TopAbs_COMPSOLID ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_COMPSOLID" );
	} else if ( S.ShapeType() == TopAbs_SOLID ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_SOLID" );
	} else if ( S.ShapeType() == TopAbs_SHELL ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_SHELL" );
	} else if ( S.ShapeType() == TopAbs_FACE ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_FACE" );
	} else if ( S.ShapeType() == TopAbs_WIRE ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_WIRE" );
	} else if ( S.ShapeType() == TopAbs_EDGE ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_EDGE" );
	} else if ( S.ShapeType() == TopAbs_VERTEX ) {
	  aPixmap->SetPixMap( "ICON_OBJBROWSER_VERTEX" );
	}

	SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
	aStudyBuilder->Addreference(newObj1, newObj);
	IO->setEntry(newObj->GetID());
      } else {
	allreadyexist = true;
	if ( ! SObjectExist(theObj, aResult->Name()) ) {
	  SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(theObj);
	  aStudyBuilder->Addreference(newObj1, SO);
	  IO->setEntry(SO->GetID());
	}
      }
      
      if ( !allreadyexist ) {
	vtkActorCollection* theActors = 
	  GEOM_AssemblyBuilder::BuildActors(S,0,themode,Standard_True);
	theActors->InitTraversal();
	vtkActor* anActor = (vtkActor*)theActors->GetNextActor();
	while(!(anActor==NULL)) {
	  GEOM_Actor* GActor = GEOM_Actor::SafeDownCast( anActor );
	  GActor->setIO( IO );
	  GActor->setName( nameG );
	  theRenderer->AddActor(GActor);
	  renWin->Render();
	  anActor = (vtkActor*)theActors->GetNextActor();
	}
      }
    }
  }

  /* commit transaction */
  op->finish();
  
  myActiveStudy->updateObjBrowser();
  myDesktop->putInfo (tr("GEOM_PRP_READY"));
  return true ;
}



//=====================================================================================
// function : GetIndexSubShapeSelected()
// purpose  : Define a ListOfID of sub shapes selected in ShapeTopo with SubShapeType
//          : Method used by Dialogs
//=====================================================================================
bool GeometryGUI::GetIndexSubShapeSelected( const TopoDS_Shape& ShapeTopo,
					    const int SubShapeType,
					    GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID,
					    Standard_Integer& aLocalContextId,
					    bool& myUseLocalContext )
{
  //* Test the type of viewer */
  if ( this->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC ) {
    return false;
  }
  
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)this->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();

  ic->InitSelected();
  int nbSelected = ic->NbSelected();
  ListOfID.length(nbSelected);
  

  //***************** DEFINE INDEX OF EACH SELECTION *********************//
  int i = 0 ;
  ic->InitSelected(); /* to restart */
  while( ic->MoreSelected() ) {

    /* Find index of sub shape into main shape */
    TopExp_Explorer Exp ( ShapeTopo, TopAbs_ShapeEnum(SubShapeType) );
    int index = 1;
    bool found = false ;
    while ( Exp.More() ) {
      if ( (Exp.Current()).IsSame( ic->SelectedShape()) ) {
	found = true ;
	break;
      }
      index++;
      Exp.Next();
    }
    if( !found ) {
      /* Manage local context from DialogBox */
      ic->CloseLocalContext(aLocalContextId) ;
      myUseLocalContext = false ;
      return false ;
    }
    ListOfID[i] = index ;
    i++;
    ic->NextSelected();
  }
    //***************** END  *********************//

  /* Manage local context from DialogBox */
  ic->CloseLocalContext(aLocalContextId) ;
  myUseLocalContext = false ;
  
  return true ;
}


//=====================================================================================
// function : OnSubShapeGetSelected()
// purpose  :
//=====================================================================================
bool GeometryGUI::OnSubShapeGetSelected( const TopoDS_Shape& ShapeTopo,
					 const char* ShapeTopoIOR,
					 const int SubShapeType,
					 Standard_Integer& aLocalContextId,
					 bool& myUseLocalContext )
{
  //* Test the type of viewer */
  if ( this->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC ) {
    return false;
  }

  SALOMEDS::Study_var   aStudy = myActiveStudy->getStudyDocument();
  SALOMEDS::SObject_var theObj = aStudy->FindObjectIOR( ShapeTopoIOR );
  if ( theObj->_is_nil() ) {
    myDesktop->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY"));
    return false ;
  }


  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  
  if( myUseLocalContext == false ) {
    /* local context is from DialogBox */
    MESSAGE("Error : No local context opened for sub shapes method" << endl ) ;
    return false ;
  }

  GEOM::GEOM_Shape_var aShape  = myComponentGeom->GetIORFromString( ShapeTopoIOR );
  TopoDS_Shape mainTopo = ShapeReader.GetShape(myComponentGeom, aShape);

  TopoDS_Shape mainShape;
  bool main = false;
  while ( !main ) {
    if ( aShape->IsMainShape() ) {
      mainShape = ShapeReader.GetShape(myComponentGeom, aShape);
      main = true;
    } else
      aShape = myComponentGeom->GetIORFromString( aShape->MainName() );
  }

  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ic->InitSelected();
  int nbSelected = ic->NbSelected();
  ListOfID->length(nbSelected);
  
  TopoDS_Compound compound;
  ic->InitSelected(); /* to init again */ 
  BRep_Builder B;
  B.MakeCompound( compound );

  int i = 0;
  /* We create a unique compound containing all the sub shapes selected by user as attribute of the main shape */
  /* the compound is homogenous by selection */
  while(ic->MoreSelected()) {
    int index = GetIndex( ic->SelectedShape(), mainShape, SubShapeType );
    ListOfID[i] = index ;
    B.Add( compound, ic->SelectedShape() );
    i++;
    ic->NextSelected();
  }
 
  /* Test if user has selected sub shapes */
  if( ListOfID->length() < 1 )
    return false ;

  GEOM::GEOM_Shape_var aResult ;
  try {
    aResult = myComponentGeom->SubShape( aShape, SubShapeType, ListOfID );
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }

  /* local context from DialogBox */
  ic->CloseLocalContext(aLocalContextId) ;
  myUseLocalContext = false ;

  char* nameG =  (char *)malloc(20);
  Standard_CString Type;

  Handle(GEOM_AISShape) result; 
  Handle(GEOM_InteractiveObject) IO ;

  //  if ( ! SObjectExist(theObj, aResult->Name()) ) {
  if ( nbSelected == 1 ) {
    TopExp_Explorer Exp ( compound, TopAbs_ShapeEnum(SubShapeType) );
    if ( Exp.More() ) {
      if ( GetShapeTypeString(Exp.Current(),Type) ) {
	aResult->NameType( Type );
	sprintf (nameG, "%s_%d", Type, GetIndex( Exp.Current(), mainTopo, SubShapeType ) );
      } else {
	aResult->NameType( tr("GEOM_SHAPE") );
	sprintf (nameG, "%s_%d", tr("GEOM_SHAPE").latin1(), this->myNbGeom++ );
      }
      result = new GEOM_AISShape( Exp.Current(), nameG );
      IO = new GEOM_InteractiveObject(aResult->Name(), Fatherior, "GEOM");
    }
  }
  else {
    if ( GetShapeTypeString(compound,Type) ) {
      aResult->NameType( Type );
      sprintf (nameG, "%s_%d", Type, this->myNbGeom++ );
    } else {
      aResult->NameType( tr("GEOM_SHAPE") );
      sprintf (nameG, "%s_%d", tr("GEOM_SHAPE").latin1(), this->myNbGeom++ );
    }
    result = new GEOM_AISShape( compound, nameG );
    IO = new GEOM_InteractiveObject(aResult->Name(), Fatherior, "GEOM");
  }
  
  SALOMEDS::SObject_var SO = aStudy->FindObjectIOR( aResult->Name() );
  
  /* open transaction */
  QAD_Operation* op = new SALOMEGUI_ImportOperation( myActiveStudy );
  op->start();
  
  SALOMEDS::StudyBuilder_var     aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::SObject_var          fatherSF = aStudy->FindObjectID(myActiveStudy->getActiveStudyFrame()->entry());
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var    aName;
  SALOMEDS::AttributeIOR_var     anIOR;
  SALOMEDS::AttributePixMap_var  aPixmap;
 
  bool allreadyexist = false;

  if ( SO->_is_nil() ) {
    SALOMEDS::SObject_var newObj = aStudyBuilder->NewObject( theObj );
    anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
    anIOR->SetValue(aResult->Name());
    anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeName");
    aName = SALOMEDS::AttributeName::_narrow(anAttr);
    aName->SetValue(result->getName());
    
    anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributePixMap");
    aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
    if ( result->Shape().ShapeType() == TopAbs_COMPOUND ) {
      aPixmap->SetPixMap( "ICON_OBJBROWSER_COMPOUND" );
    } else if ( result->Shape().ShapeType() == TopAbs_COMPSOLID ) {
      aPixmap->SetPixMap( "ICON_OBJBROWSER_COMPSOLID" );
    } else if ( result->Shape().ShapeType() == TopAbs_SOLID ) {
      aPixmap->SetPixMap( "ICON_OBJBROWSER_SOLID" );
    } else if ( result->Shape().ShapeType() == TopAbs_SHELL ) {
      aPixmap->SetPixMap( "ICON_OBJBROWSER_SHELL" );
    } else if ( result->Shape().ShapeType() == TopAbs_FACE ) {
      aPixmap->SetPixMap( "ICON_OBJBROWSER_FACE" );
    } else if ( result->Shape().ShapeType() == TopAbs_WIRE ) {
      aPixmap->SetPixMap( "ICON_OBJBROWSER_WIRE" );
    } else if ( result->Shape().ShapeType() == TopAbs_EDGE ) {
      aPixmap->SetPixMap( "ICON_OBJBROWSER_EDGE" );
    } else if ( result->Shape().ShapeType() == TopAbs_VERTEX ) {
      aPixmap->SetPixMap( "ICON_OBJBROWSER_VERTEX" );
    }

    SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
    aStudyBuilder->Addreference(newObj1, newObj);
    
    IO->setEntry(newObj->GetID());
    aResult->StudyShapeId(newObj->GetID());
  } else {
    allreadyexist = true;
    if ( ! SObjectExist(theObj, aResult->Name()) ) {
      SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(theObj);
      aStudyBuilder->Addreference(newObj1, SO);
    
      IO->setEntry(SO->GetID());
      aResult->StudyShapeId(SO->GetID());
    }
  }
  
  /* commit transaction */
  op->finish();
  
  result->setIO( IO );
  result->setName( nameG );

  if ( !allreadyexist )
    ic->Display(result);

  this->OnDisplayAll(true);
  myActiveStudy->updateObjBrowser();
  myDesktop->putInfo (tr("GEOM_PRP_READY"));

  return true ;
}


//=====================================================================================
// function : OnSuppressFaces() 
// purpose  : To suppress faces from a shape
//          : The result is one or more shells/faces as main shapes !
//=====================================================================================
bool GeometryGUI::OnSuppressFaces( const TopoDS_Shape& ShapeTopo,
				   const char* ShapeTopoIOR,
				   const Standard_Integer& aLocalContextId,
				   bool& myUseLocalContext )
{
  /* Test the type of viewer */
  if ( this->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC ) {
    return false;
  }
  
  SALOMEDS::Study_var   aStudy = myActiveStudy->getStudyDocument();
  SALOMEDS::SObject_var theObj = aStudy->FindObjectIOR( ShapeTopoIOR );
  if ( theObj->_is_nil() ) {
    myDesktop->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY"));
    return false ;
  }
  
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  
  if( myUseLocalContext == false ) {
    /* local context is from DialogBox */
    MESSAGE("Error : No local context opened for suppress faces method" << endl ) ;
    return false ;
  }
  
  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ic->InitSelected();
  int nbSelected = ic->NbSelected();
  ListOfID->length(nbSelected);

  /* Create a list of indices of faces to be suppressed */
  int i = 0;
  const int SubShapeType = 4 ; /* GEOM::FACE type */
  ic->InitSelected();          /* to repositioning at beginning */
  while( ic->MoreSelected() ) {
    int index = GetIndex( ic->SelectedShape(), ShapeTopo, SubShapeType );
    ListOfID[i] = index ;
    i++;
    ic->NextSelected();
  }
  
  /* Close local context opened in DialogBox */
  ic->CloseLocalContext(aLocalContextId) ;
  myUseLocalContext = false ;
  
  /* Here is the main shape */
  GEOM::GEOM_Shape_var aShape = myComponentGeom->GetIORFromString( ShapeTopoIOR );  
  GEOM::GEOM_Gen::ListOfGeomShapes_var listGeomShapes = new GEOM::GEOM_Gen::ListOfGeomShapes;
  
  /* Call geom method that return a list of shells/faces as result of suppress */
  try {
    listGeomShapes = myComponentGeom->SuppressFaces( aShape, ListOfID );
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  
  /* Test list of shells/faces */
  if( listGeomShapes->length() < 1 ) {
    return false ;
  }

  /* Loop on each object created */
  for( int i=0; i<listGeomShapes->length(); i++ ) {
    
    GEOM::GEOM_Shape_var aShellOrFace = listGeomShapes[i] ;
    TopoDS_Shape S = ShapeReader.GetShape( this->myComponentGeom, aShellOrFace );
    
    if( S.IsNull() ) {
      return false ;
    }
    
    char* nameG =  (char *)malloc(20);
    Standard_CString Type;
    if ( GetShapeTypeString(S, Type) ) {
      aShellOrFace->NameType( Type );
      sprintf (nameG, "%s_%d", Type, this->myNbGeom++);
    }
    else {
      aShellOrFace->NameType( tr("GEOM_SHAPE") );
      sprintf (nameG, "%s_%d", tr("GEOM_SHAPE").latin1(), this->myNbGeom++ );
    }
    
    /* Display with name */
    if( !Display( aShellOrFace, nameG) ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
      return false ;
    }
  }
  
  myDesktop->putInfo (tr("GEOM_PRP_READY"));
  return true ;
}


//=====================================================================================
// function : OnSuppressHole() 
// purpose  : To suppress an hole on a shape 'ShapeTopo'.
//          : 'ListOfIdEndFace' may be an empty list.
//          : This means that hole do not traverse ShapeTopo.
//          : Warning : the hole to be suppressed must be defined by one or two single closed wires !
//=====================================================================================
bool GeometryGUI::OnSuppressHole( const char* ShapeTopoIOR,
				  const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdFace,
				  const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdWire,
				  const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdEndFace )
{
  /* Test the type of viewer */
  if ( this->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC ) {
    return false;
  }
  
  try {
    GEOM::GEOM_Shape_var aShape  = myComponentGeom->GetIORFromString( ShapeTopoIOR );    
    GEOM::GEOM_Shape_var aResult = myComponentGeom->SuppressHole( aShape, ListOfIdFace, ListOfIdWire, ListOfIdEndFace ) ;
  
    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, aResult);
    Standard_CString type;
    GetShapeTypeString(S,type);
    aResult->NameType( type );

    if ( Display( aResult, "" ) )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return true ;
}


//=====================================================================================
// function : OnSuppressHolesInFaceOrShell() 
// purpose  : To suppress one or more holes on a face
//          : 'ListOfIdWires' contains indices or wires/holes.
//=====================================================================================
bool GeometryGUI::OnSuppressHolesInFaceOrShell( const char* ShapeTopoIOR,
						const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdWires )
{
  /* Test the type of viewer */
  if ( this->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC ) {
    return false;
  }
  
  try {
    GEOM::GEOM_Shape_var aShape  = myComponentGeom->GetIORFromString( ShapeTopoIOR );    
    GEOM::GEOM_Shape_var aResult = myComponentGeom->SuppressHolesInFaceOrShell( aShape, ListOfIdWires ) ;
    
    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, aResult);
    Standard_CString type;
    GetShapeTypeString(S,type);
    aResult->NameType( type );
    
    if ( Display( aResult, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return true ;
}



//=======================================================================
// function : activeStudyChanged()
// purpose  : static
//=======================================================================
void GeometryGUI::activeStudyChanged( QAD_Desktop* parent )
{
  MESSAGE ("GeometryGUI::activeStudyChanged init.")
  /* Create or retrieve an object GeomGUI */
  GeometryGUI::GetOrCreateGeometryGUI(parent); 
  if(GeomGUI != 0) {  

    if (GeomGUI->myState == CURRENT_SKETCH) {
      GeomGUI->mySketcher.Clear();
      GeomGUI->ResetState();
    }

    QMenuBar* Mb = GeomGUI->myDesktop->getMainMenuBar();
    bool ViewOCC = false;
    if ( GeomGUI->myDesktop->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) 
      ViewOCC = true;

    Mb->setItemEnabled( 312, ViewOCC); //Sketch
    Mb->setItemEnabled( 309, ViewOCC); //SuppressFace
    Mb->setItemEnabled( 314, ViewOCC); //SuppressHole
    
    Mb->setItemEnabled( 703, ViewOCC);// ShadingColor Settings
    Mb->setItemEnabled( 704, ViewOCC);// Isos Settings

    GeomGUI->EraseSimulationShape() ;
    GeomGUI->EmitSignalCloseAllDialogs() ;
    GeomGUI = 0 ;
  }

  //  GeomGUI->SetSettings( parent );
  // MESSAGE ("GeometryGUI::activeStudyChanged done.")
  return ;
}


//=======================================================================
// function : DefineDlgPosition()
// purpose  : Define x and y the default position for a dialog box
//=======================================================================
bool GeometryGUI::DefineDlgPosition(QWidget* aDlg, int& x, int& y)
{
  /* Here the position is on the bottom right corner - 10 */
  QAD_Desktop* PP = QAD_Application::getDesktop() ;
  x = abs ( PP->x() + PP->size().width()  - aDlg->size().width()  - 10 ) ;
  y = abs ( PP->y() + PP->size().height() - aDlg->size().height() - 10 ) ;
  return true ;  
}

//=======================================================================
// function : OnGUIEvent() [static]
// purpose  : manage all events on GUI
//=======================================================================
bool GeometryGUI::OnGUIEvent(int theCommandID,	QAD_Desktop* parent)
{
  /* Create or retrieve an object GeomGUI */
  GeometryGUI::GetOrCreateGeometryGUI(parent);

  SALOMEDS::Study_var aStudy = GeomGUI->myActiveStudy->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  
  OCCViewer_Viewer3d* v3d;
  Handle(AIS_InteractiveContext) ic;
  vtkRenderer* Renderer;

  QMenuBar* Mb = GeomGUI->myDesktop->getMainMenuBar();

  bool ViewOCC = false;

  if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    v3d = ((OCCViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    ic = v3d->getAISContext();
    ViewOCC = true;    
    Mb->setItemEnabled( 312, ViewOCC);//Sketch
    Mb->setItemEnabled( 309, ViewOCC);//SuppressFace
    Mb->setItemEnabled( 314, ViewOCC);//SuppressHole
    
    Mb->setItemEnabled( 703, ViewOCC);// ShadingColor Settings
    Mb->setItemEnabled( 704, ViewOCC);// Isos Settings

  } else if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
    Renderer = ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();

    // OnSuppressFaces, OnSuppressHole, SETTINGS - SHADING COLOR, SETTINGS - ISOS, SETTINGS : STEP VALUE FOR SPIN BOXES, SKETCHER, ISOS - POPUP VIEWER, 
    Mb->setItemEnabled( 312, ViewOCC);//Sketch
    Mb->setItemEnabled( 309, ViewOCC);//SuppressFace
    Mb->setItemEnabled( 314, ViewOCC);//SuppressHole
    
    Mb->setItemEnabled( 703, ViewOCC);// ShadingColor Settings
    Mb->setItemEnabled( 704, ViewOCC);// Isos Settings
  } else {
    return 0;
  }

  if (GeomGUI->myState == CURRENT_SKETCH && theCommandID != 10000 && theCommandID != 10001 && theCommandID != 10002 && theCommandID != 10003 && theCommandID != 10004 && theCommandID != 10006 && theCommandID != 10007 && theCommandID != 10008 && theCommandID != 10010 && theCommandID != 10011 && theCommandID != 10012 && theCommandID != 10013 && theCommandID != 10014 && theCommandID != 3133 && theCommandID != 3134)
    return false;

  switch (theCommandID)
    { 
    case 31: // COPY
      GeomGUI->OnEditCopy();
      break;
    
    case 33: // DELETE
      GeomGUI->OnEditDelete();
      break;

    case 111:
      {
	GeomGUI->SetState(111);
	GeomGUI->Import();
	GeomGUI->ResetState();
	break;
      }
    case 112:
      {
	GeomGUI->SetState(112);
	GeomGUI->Import();
	GeomGUI->ResetState();
	break;
      }
    case 113: // IMPORT STEP
      {
	GeomGUI->SetState(113);
	GeomGUI->Import();
	GeomGUI->ResetState();
	break;
      }

    case 121: // EXPORT BREP
      {
	GeomGUI->SetState(121);
	GeomGUI->Export();
	GeomGUI->ResetState();
	break;
      }

    case 122: // EXPORT IGES
      {
	GeomGUI->SetState(122);
	GeomGUI->Export();
	GeomGUI->ResetState();
	break;
      }

    case 123: // EXPORT STEP
      {
	GeomGUI->SetState(123);
	GeomGUI->Export();
	GeomGUI->ResetState();
	break;
      }
      
    case 303: // EXPLODE : use ic
      { 	
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_SubShapeDlg *aDlg = new GeometryGUI_SubShapeDlg( parent, "", Sel, ic ) ;
	break ;
      }
      
    case 304: // GEOM::EDGE
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_EdgeDlg *aDlg = new GeometryGUI_EdgeDlg( parent, "", Sel ) ;
	break ;
      }
      
    case 305: // GEOM::WIRE
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_WireDlg *aDlg = new GeometryGUI_WireDlg( parent, "", Sel ) ;
	break ;
      }
      
    case 306: // GEOM::FACE
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_FaceDlg *aDlg = new GeometryGUI_FaceDlg ( parent, "", Sel ) ;
	break ;
      }
      
    case 308: // GEOM::COMPOUND
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_CompoundDlg *aDlg = new GeometryGUI_CompoundDlg ( parent, "", Sel ) ;
	break ;
      }
      
    case 309: // SUPPRESS FACES : use ic
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_SuppressFacesDlg *aDlg = new GeometryGUI_SuppressFacesDlg( parent, "", Sel, ic ) ;
	break ;
      }
      
    case 314: // SUPPRESS HOLES : use ic
      {
	if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
	  GeomGUI->EmitSignalDeactivateDialog() ;
	  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	  GeometryGUI_SuppressHoleDlg *aDlg = new GeometryGUI_SuppressHoleDlg( parent, "", Sel, ic ) ;
	}
	else { 
	  GeomGUI->myDesktop->putInfo( tr("GEOM_PRP_NOT_FOR_VTK_VIEWER") ) ;
	}
	break ;
      }
    case 501: // SEWING
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_SewingDlg *aDlg = new GeometryGUI_SewingDlg ( parent, "", Sel ) ;
	break ;
      }
      
    case 502: // ORIENTATION
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_OrientationDlg *aDlg = new GeometryGUI_OrientationDlg( parent, "", Sel ) ;
	break ;
      }
      
    case 601: // PROPERTIES (Length, surface, volume)
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_PropertiesDlg *aDlg = new GeometryGUI_PropertiesDlg( parent, "", Sel ) ;
	break ;
      }
      
    case 604: // CDG : Center of mass
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_CenterMassDlg *aDlg = new GeometryGUI_CenterMassDlg ( parent, "", Sel ) ;
	break ;
      }
      
    case 605: // INERTIA
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_InertiaDlg *aDlg = new GeometryGUI_InertiaDlg( parent, "", Sel ) ;
	break ;
      }
      
    case 607: // MAXTOLERANCE
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_MaxToleranceDlg *aDlg = new GeometryGUI_MaxToleranceDlg( parent, "", Sel ) ;
	break ;
      }
     
    case 608: // WHATIS
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_WhatisDlg *aDlg = new GeometryGUI_WhatisDlg( parent, "", Sel ) ;
	break ;
      }
      
    case 609: // CHECKSHAPE
     {
       GeomGUI->EmitSignalDeactivateDialog() ;
       SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
       GeometryGUI_CheckShape *aDlg = new GeometryGUI_CheckShape( parent, "", Sel ) ;
       break ;
     }
     
    case 701: // SETTINGS - COPY
      {
	QMenuData* pp;
	QMenuItem* item = parent->menuBar()->findItem(701,&pp);
	bool check = !pp->isItemChecked(701);
	pp->setItemChecked(701,check);
	Settings_Copy = check;	
	QAD_CONFIG->addSetting( "Geometry:SettingsCopy", Settings_Copy );
	break;
      }

    case 702: // SETTINGS - ADD IN STUDY
      {
	QMenuData* pp;
	QMenuItem* item = parent->menuBar()->findItem(702,&pp);
	bool check = !pp->isItemChecked(702);
	pp->setItemChecked(702,check);
	Settings_AddInStudy = check;
	
	QAD_CONFIG->addSetting( "Geometry:SettingsAddInStudy", Settings_AddInStudy );
	break;
      }

    case 703: // SETTINGS - SHADING COLOR
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
	  break;
	
	QString SCr = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorRed");
	QString SCg = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorGreen");
	QString SCb = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorBlue");
	QColor color;
	if( !SCr.isEmpty() && !SCg.isEmpty() && !SCb.isEmpty() ) {	  
	  color = QColor (SCr.toInt(),
			  SCg.toInt(),
			  SCb.toInt());
	} else {
	  Quantity_Color Default = Quantity_Color();
	  color = QColor ( (int)Default.Red()  * 255.0,
			   (int)Default.Green()* 255.0,
			   (int)Default.Blue() * 255.0 );
	}
	
	QColor c = QColorDialog::getColor( color, QAD_Application::getDesktop() );
	if ( c.isValid() ) {
	  GeomGUI->myShadingColor = Quantity_Color(c.red()  / 255.0,
						   c.green()/ 255.0,
						   c.blue() / 255.0,
						   Quantity_TOC_RGB);
	  
	  AIS_ListOfInteractive List;
	  ic->DisplayedObjects(List);
	  AIS_ListOfInteractive List1;
	  ic->ObjectsInCollector(List1);
	  List.Append(List1);
	  
	  AIS_ListIteratorOfListOfInteractive ite(List);
	  while (ite.More()) {
	    if (ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	      Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	      aSh->SetShadingColor( GeomGUI->myShadingColor );
	      ic->Redisplay( aSh, Standard_True, Standard_True);
	    }
	    ite.Next();
	  }
	  
	  ic->UpdateCurrentViewer();
		
	  QAD_CONFIG->addSetting("Geometry:SettingsShadingColorRed",   c.red()   );
	  QAD_CONFIG->addSetting("Geometry:SettingsShadingColorGreen", c.green() );
	  QAD_CONFIG->addSetting("Geometry:SettingsShadingColorBlue",  c.blue()  );	
	}
	break;
      }

     
    case 704: // SETTINGS - ISOS
      {
	if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
	  break;

	QString IsoU = QAD_CONFIG->getSetting("Geometry:SettingsIsoU");
	QString IsoV = QAD_CONFIG->getSetting("Geometry:SettingsIsoV");
	if ( !IsoU.isEmpty() )
	  ic->DefaultDrawer()->UIsoAspect()->SetNumber(IsoU.toInt());
	else
	  IsoU = "1";
	if ( !IsoV.isEmpty() )
	  ic->DefaultDrawer()->VIsoAspect()->SetNumber(IsoV.toInt());
	else
	  IsoV = "1";
	
	GeometryGUI_NbIsosDlg * NbIsosDlg = new GeometryGUI_NbIsosDlg( QAD_Application::getDesktop(),
								       tr("GEOM_MEN_ISOS"), TRUE );	
	int UIso = IsoU.toInt();
	int VIso = IsoV.toInt();
	
	NbIsosDlg->SpinBoxU->setValue(UIso) ;
	NbIsosDlg->SpinBoxV->setValue(VIso) ;

	if ( NbIsosDlg->exec() ) {
	  UIso = NbIsosDlg->SpinBoxU->text().toInt() ;
	  VIso = NbIsosDlg->SpinBoxV->text().toInt() ;
	  
	  ic->DefaultDrawer()->UIsoAspect()->SetNumber(UIso);
	  ic->DefaultDrawer()->VIsoAspect()->SetNumber(VIso);
	  QAD_CONFIG->addSetting( "Geometry:SettingsIsoU", NbIsosDlg->SpinBoxU->text() ); /* text format */
	  QAD_CONFIG->addSetting( "Geometry:SettingsIsoV", NbIsosDlg->SpinBoxV->text() ); /* text format */
	}

	AIS_ListOfInteractive List;
	ic->DisplayedObjects(List);
	AIS_ListOfInteractive List1;
	ic->ObjectsInCollector(List1);
	List.Append(List1);
	
	AIS_ListIteratorOfListOfInteractive ite(List);
	ic->InitCurrent();
	if ( ic->NbCurrents() )
	  while (ite.More()) {
	    if (ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	      
	      Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	      Handle (AIS_Drawer) CurDrawer;
	      CurDrawer = aSh->Attributes();
	      CurDrawer->UIsoAspect()->SetNumber( UIso );
	      CurDrawer->VIsoAspect()->SetNumber( VIso );
	      ic->SetLocalAttributes(aSh, CurDrawer);
	      ic->Redisplay(aSh);	
	    }
	    ite.Next();
	  }
	ic->UpdateCurrentViewer();
	break;
      }
 
    case 705: // SETTINGS : STEP VALUE FOR SPIN BOXES
      {
	//NRI	if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
	//NRI	  break;
	
	QString step = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" );
	if( step.isEmpty() )
	  step = "100.0" ;

	Standard_Boolean res = false ;
	double dd = Parameter( res, QString("%1").arg(step), tr("GEOM_MEN_STEP_LABEL"), tr("GEOM_STEP_TITLE"), 0.001, 10000.0, 3 ) ;
	if( res ) {
	  QAD_CONFIG->addSetting( "Geometry:SettingsGeomStep",  QString("%1").arg(dd) ) ;

	  /* Emit signal to GeometryGUI_SpinBoxes */
	  GeomGUI->SignalDefaultStepValueChanged( dd ) ;
	}
	else
	  parent->putInfo(tr("GEOM_PRP_ABORT"));

	break ;
      }
      
    case 801: // ADD IN STUDY - POPUP VIEWER
      {
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	const SALOME_ListIO& ListSelected = Sel->StoredIObjects();
	SALOME_ListIteratorOfListIO It( ListSelected );
	for ( ; It.More(); It.Next() ) {
	  Handle(SALOME_InteractiveObject) IObject = It.Value();
	  GeomGUI->AddInStudy(true, IObject);
	}
	/* Is set on the dialog box */
	QApplication::restoreOverrideCursor();
	break;
      }

    case 901: // RENAME
      {
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	for ( ; It.More(); It.Next() ) {
	  Handle(SALOME_InteractiveObject) IObject = It.Value();

	  SALOMEDS::SObject_var obj = aStudy->FindObjectID( IObject->getEntry() );
          SALOMEDS::GenericAttribute_var anAttr;
          SALOMEDS::AttributeName_var    aName;
	  if ( !obj->_is_nil() ) {
	    if (obj->FindAttribute(anAttr, "AttributeName")) {
              aName = SALOMEDS::AttributeName::_narrow(anAttr);

	      QString nm = QString( aName->Value() );
	      nm = SALOMEGUI_NameDlg::getName( QAD_Application::getDesktop(), nm );
	      if ( !nm.isEmpty() ) {
		QApplication::setOverrideCursor( Qt::waitCursor );
		GeomGUI->myActiveStudy->renameIObject( IObject, nm );
		QApplication::restoreOverrideCursor();
	      }
	    }
	  }
	}
	break;
      }  
 
    case 903: // DISPLAY OBJECT BROWSER
      {
	if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
	  // VTK
	  QApplication::setOverrideCursor( Qt::waitCursor );
	  
	  SALOMEDS::SObject_var fatherSF = 
	    aStudy->FindObjectID(GeomGUI->myActiveStudy->getActiveStudyFrame()->entry());

	  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	  
	  for(;It.More();It.Next()) {
	    Handle(SALOME_InteractiveObject) IObject = It.Value();
	    SALOMEDS::SObject_var obj = aStudy->FindObjectID( IObject->getEntry() );

	    VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();

            SALOMEDS::GenericAttribute_var anAttr;
            SALOMEDS::AttributeName_var    aName;
            SALOMEDS::AttributeIOR_var     anIOR;
	 
	    if(myRenderInter->isInViewer(IObject)) {
	      myRenderInter->Display(IObject);
	    }
	    else {
	      // Create new actor
	      if ( !obj->_is_nil() ) {
		if ( obj->FindAttribute(anAttr, "AttributeIOR")) {
		  // this SObject may be GEOM module root SObject
		  SALOMEDS::ChildIterator_var anIter = GeomGUI->myActiveStudy->getStudyDocument()->NewChildIterator(obj);
		  bool useSubItems = false;
		  while (anIter->More() && !useSubItems) {
		    SALOMEDS::SObject_var subobj = anIter->Value();
		    SALOMEDS::GenericAttribute_var aTmpAttr;
		    if (subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		      anAttr = aTmpAttr;
		      obj = subobj;
		      useSubItems = true;
		    } else anIter->Next();
		  }
		  
		  while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
		    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
		    GEOM::GEOM_Shape_var aShape = GeomGUI->myComponentGeom->GetIORFromString(anIOR->Value());
		    TopoDS_Shape Shape = ShapeReader.GetShape(GeomGUI->myComponentGeom,aShape);

		    if (obj->FindAttribute(anAttr, "AttributeName")) {
		      aName = SALOMEDS::AttributeName::_narrow(anAttr);

		      vtkRenderer* theRenderer = ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
		      vtkActorCollection* theAllActors = theRenderer->GetActors();
		      theAllActors->InitTraversal();
		      vtkActor* actor = (vtkActor*)theAllActors->GetNextActor();
		      Handle(SALOME_InteractiveObject) anIObj;
		      // don't create new study object if it already exists
		      bool isDisplayed = false;
		      while(!(actor==NULL)) {
			SALOME_Actor* Gactor = SALOME_Actor::SafeDownCast(actor);
			if (Gactor!=NULL) {
			  if (Gactor->hasIO()) {
			    if (strcmp(Gactor->getIO()->getEntry(),obj->GetID())==0) {
			      isDisplayed = true;
			      anIObj = Gactor->getIO();
			      if (!anIObj.IsNull()) myRenderInter->Display(anIObj);
			    }
			  }
			}
			actor=(vtkActor*)(theAllActors->GetNextActor());
		      }
		      if (!isDisplayed) {
			// open transaction
			QAD_Operation* op = new SALOMEGUI_ImportOperation( GeomGUI->myActiveStudy );
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
			  GEOM_Actor* GActor = GEOM_Actor::SafeDownCast( anActor );
			  Handle(GEOM_InteractiveObject) IO = new GEOM_InteractiveObject(anIOR->Value(), Fatherior,"GEOM");
			  IO->setEntry(obj->GetID());
			  GActor->setIO( IO );
			  GActor->setName( IObject->getName() );
			  
			  theRenderer->AddActor(GActor);
			  renWin->Render();
			  anActor = (vtkActor*)theActors->GetNextActor();
			}
		      }
		    }
		    // next item iteration
		    if (useSubItems) {
		      anIter->Next();
		      anAttr = SALOMEDS::GenericAttribute::_nil();
		      while (anIter->More() && anAttr->_is_nil()) {
			SALOMEDS::SObject_var subobject = anIter->Value();
			SALOMEDS::GenericAttribute_var aTmpAttribute;
			if (subobject->FindAttribute(aTmpAttribute, "AttributeIOR")) {
			  anAttr = aTmpAttribute;
			  obj = subobject;
			} else anIter->Next();
		      }
		    } else anAttr = SALOMEDS::GenericAttribute::_nil();
		  }
		}
	      }
	    }
	  }
	  GeomGUI->myActiveStudy->updateObjBrowser( true );
	  QApplication::restoreOverrideCursor();

	} else if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
	  QApplication::setOverrideCursor( Qt::waitCursor );
	  
	  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	  for(;It.More();It.Next()) {
	    Handle(SALOME_InteractiveObject) IObject = It.Value();

	    SALOMEDS::SObject_var fatherSF = 
	      aStudy->FindObjectID( GeomGUI->myActiveStudy->getActiveStudyFrame()->entry());
            SALOMEDS::GenericAttribute_var anAttr;
            SALOMEDS::AttributeName_var    aName;
            SALOMEDS::AttributeIOR_var     anIOR;

	    if ( v3d->isInViewer( IObject, true ) ) {
	      Standard_Boolean found;
	      Handle(GEOM_AISShape) aSh = GeomGUI->ConvertIOinGEOMAISShape( IObject, found, true );
	      if ( found ) {
		ic->Display(aSh);
		ic->AddOrRemoveCurrentObject(aSh, true);
	      }
	      
	    } else {
	      SALOMEDS::SObject_var obj = aStudy->FindObjectID( IObject->getEntry() );	      
	      if ( !obj->_is_nil() ) {
		if ( obj->FindAttribute(anAttr, "AttributeIOR")) {
		  // this SObject may be GEOM module root SObject
		  SALOMEDS::ChildIterator_var anIter = GeomGUI->myActiveStudy->getStudyDocument()->NewChildIterator(obj);
		  bool useSubItems = false;
		  while (anIter->More() && !useSubItems) {
		    SALOMEDS::SObject_var subobj = anIter->Value();
		    SALOMEDS::GenericAttribute_var aTmpAttr;
		    if (subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		      anAttr = aTmpAttr;
		      obj = subobj;
		      useSubItems = true;
		    } else anIter->Next();
		  }
		  while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
		    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
		    GEOM::GEOM_Shape_var aShape = GeomGUI->myComponentGeom->GetIORFromString(anIOR->Value());
		    TopoDS_Shape Shape = ShapeReader.GetShape(GeomGUI->myComponentGeom,aShape);
		    if (obj->FindAttribute(anAttr, "AttributeName")) {
		      aName = SALOMEDS::AttributeName::_narrow(anAttr);
		      // searchin for already displayed objects with the same shape
		      AIS_ListOfInteractive aDisplayed;
		      ic->DisplayedObjects(aDisplayed);
		      AIS_ListIteratorOfListOfInteractive anIObjects(aDisplayed);
		      Handle(AIS_Shape) anAISShape;
		      for(;anIObjects.More();anIObjects.Next()) {
			anAISShape = Handle(AIS_Shape)::DownCast(anIObjects.Value());
			if (!anAISShape.IsNull()) {
			  if (anAISShape->Shape().IsSame(Shape)) break;
			  anAISShape.Nullify();
			}
		      }
		      if (!anAISShape.IsNull()) {
			if (!ic->IsDisplayed(anAISShape)) ic->Display(anAISShape);
		      } else {
			if (!useSubItems) {
			  // open transaction
			  QAD_Operation* op = new SALOMEGUI_ImportOperation( GeomGUI->myActiveStudy );
			  op->start();
			  
			  SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
			  aStudyBuilder->Addreference(newObj1, obj);
			  // commit transaction
			  op->finish();
			}

			Handle(GEOM_AISShape) aSh = new GEOM_AISShape(Shape, aName->Value());
			aSh->SetShadingColor( GeomGUI->myShadingColor );
			Handle(GEOM_InteractiveObject) IO = new GEOM_InteractiveObject(anIOR->Value(),
										       Fatherior,
										       "GEOM");
			IO->setEntry(obj->GetID());
			aSh->setIO( IO );
			aSh->setName( aName->Value() );
			ic->Display (aSh);
			if (!useSubItems) ic->AddOrRemoveCurrentObject(aSh, true);
		      }
		    }
		    // next item iteration
		    if (useSubItems) {
		      anIter->Next();
		      anAttr=SALOMEDS::GenericAttribute::_nil();
		      while (anIter->More() && anAttr->_is_nil()) {
			SALOMEDS::SObject_var subobject = anIter->Value();
			SALOMEDS::GenericAttribute_var aTmpAttribute;
			if (subobject->FindAttribute(aTmpAttribute, "AttributeIOR")) {
			  anAttr = aTmpAttribute;
			  obj = subobject;
			} else anIter->Next();
		      }
		    } else anAttr = SALOMEDS::GenericAttribute::_nil();
		  }
		}
	      }
	    }
	  }
	  GeomGUI->myActiveStudy->updateObjBrowser( true );
	  QApplication::restoreOverrideCursor();
	}
	break;
      }
      
    case 3011: // POINT
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_PointDlg *aDlg = new GeometryGUI_PointDlg( parent, "", Sel, ic ) ; 
	break;
      }
      
    case 3012:  // LINE
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_LineDlg *aDlg = new GeometryGUI_LineDlg( parent, "", Sel ) ;
	break ;
      }
      
    case 3013:  // CIRCLE
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_CircleDlg *aDlg = new GeometryGUI_CircleDlg( parent, "", Sel ) ;
	break ;
      }

    case 3014:  // ELLIPSE
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_EllipseDlg *aDlg = new GeometryGUI_EllipseDlg( parent, "", Sel ) ;
	break ;
      }

    case 3015:  // ARC
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_ArcDlg *aDlg = new GeometryGUI_ArcDlg( parent, "", Sel ) ;
	break ;
      }
      
    case 3016: // VECTOR
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_VectorDlg *aDlg = new GeometryGUI_VectorDlg( parent, "", Sel ) ;
	break;
      }
      
    case 3017: // PLANE
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_PlaneDlg *aDlg = new GeometryGUI_PlaneDlg( parent, "", Sel ) ;
	break;
      }
      
    case 3018: // WORKING PLANE
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_WorkingPlaneDlg *aDlg = new GeometryGUI_WorkingPlaneDlg( parent, "", Sel ) ;
	break;
      }
      
    case 312: // SKETCHER
      {
	if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
	  break;

	GeomGUI->EmitSignalDeactivateDialog() ;

	((OCCViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->onViewTop(); // DCQ : 28/02/2002
	
	GeomGUI->mySketcher = Sketch( v3d->getViewer3d() );
	GeomGUI->SetState(CURRENT_SKETCH);
	
	QMenuBar* Mb = GeomGUI->myDesktop->getMainMenuBar();
	QMenuData* pp;

        QMenuItem* item = Mb->findItem(10010,&pp);
	GeomGUI->mySketcher.SetParameterVisibility(LENGTH_PARAMETER,pp->isItemChecked(10010));
	item = Mb->findItem(10011,&pp);
	GeomGUI->mySketcher.SetParameterVisibility(ANGLE_PARAMETER,pp->isItemChecked(10011));
	item = Mb->findItem(10012,&pp);
	GeomGUI->mySketcher.SetParameterVisibility(RADIUS_PARAMETER,pp->isItemChecked(10012));
	item = Mb->findItem(10013,&pp);
	GeomGUI->mySketcher.SetParameterVisibility(XVALUE_PARAMETER,pp->isItemChecked(10013));
	item = Mb->findItem(10014,&pp);
	GeomGUI->mySketcher.SetParameterVisibility(YVALUE_PARAMETER,pp->isItemChecked(10014));
	
	GeomGUI->mySketcher.SetTransitionStatus(NOCONSTRAINT);
	item = Mb->findItem(3133,&pp);
	pp->setItemChecked(3133,false);
	item = Mb->findItem(3134,&pp);
	pp->setItemChecked(3134,false);
	break;
      }

    case 3021: // BOX
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
 	GeometryGUI_BoxDlg *aDlg = new GeometryGUI_BoxDlg( parent, "", Sel ) ;
	break;
      }

    case 3022: // CYLINDER
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
 	GeometryGUI_CylinderDlg *aDlg = new GeometryGUI_CylinderDlg( parent, "", Sel ) ;	
	break;
      }

    case 3023: // SPHERE
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_SphereDlg *aDlg = new GeometryGUI_SphereDlg( parent, "", Sel ) ;
	break;
      }
      
    case 3024: // TORUS
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
 	GeometryGUI_TorusDlg *aDlg = new GeometryGUI_TorusDlg( parent, "", Sel ) ;
	break;
      }

    case 3025: // CONE
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
 	GeometryGUI_ConeDlg *aDlg = new GeometryGUI_ConeDlg( parent, "", Sel ) ;
	break;
      }

    case 3131:
      {
	break;
      }

    case 3133: // sketcher
      { 
	QMenuBar* Mb = GeomGUI->myDesktop->getMainMenuBar();
        QMenuData* pp;
	QMenuItem* item = Mb->findItem(3133,&pp);
	pp->setItemChecked(3133,!pp->isItemChecked(3133));
	if (pp->isItemChecked(3133) == true) 
	  GeomGUI->mySketcher.SetTransitionStatus(TANGENT);
	else
	  GeomGUI->mySketcher.SetTransitionStatus(NOCONSTRAINT);

	pp->setItemChecked(3134,false);
	break;
      }

    case 3134: // sketcher
      {
	QMenuBar* Mb = GeomGUI->myDesktop->getMainMenuBar();
        QMenuData* pp;
	QMenuItem* item = Mb->findItem(3134,&pp);
	pp->setItemChecked(3134,!pp->isItemChecked(3134));
	if (pp->isItemChecked(3134) == true) 
	  GeomGUI->mySketcher.SetTransitionStatus(PERPENDICULAR);
	else 
	  GeomGUI->mySketcher.SetTransitionStatus(NOCONSTRAINT);
	
	pp->setItemChecked(3133,false);
	break;
      }

    case 4011: // FUSE
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
 	GeometryGUI_FuseDlg *aDlg = new GeometryGUI_FuseDlg( parent, "", Sel ) ;
	break;
      }

    case 4012: // COMMON
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_CommonDlg *aDlg = new GeometryGUI_CommonDlg( parent, "", Sel ) ;
	break;
      }

    case 4013: // CUT
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_CutDlg *aDlg = new GeometryGUI_CutDlg( parent, "", Sel ) ;
	break;
      }

    case 4014: // SECTION
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_SectionDlg *aDlg = new GeometryGUI_SectionDlg( parent, "", Sel ) ;
	break;
      }
      
    case 4021: // TRANSLATION
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_TranslationDlg *aDlg = new GeometryGUI_TranslationDlg( parent, "", Sel ) ;
	break;
      }

    case 4022: // ROTATION
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_RotationDlg *aDlg = new GeometryGUI_RotationDlg( parent, "", Sel ) ;
	break;
      }

   case 4030: // MULTI TRANSLATION
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_MultiTranslationDlg *aDlg = new GeometryGUI_MultiTranslationDlg( parent, "", Sel ) ;
	break;
      }

    case 4040: // MULTI ROTATION
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_MultiRotationDlg *aDlg = new GeometryGUI_MultiRotationDlg( parent, "", Sel ) ;
	break;
      }

    case 4023: // MIRROR
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_MirrorDlg *aDlg = new GeometryGUI_MirrorDlg( parent, "", Sel ) ;
	break;
      }
      
    case 4024: // SCALE
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_ScaleDlg *aDlg = new GeometryGUI_ScaleDlg( parent, "", Sel ) ;
	break;
      }
      
    case 4025: // PARTITION
      {		
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_PartitionDlg *aDlg = new GeometryGUI_PartitionDlg( parent, "", Sel ) ;	
	break;
      }

    case 4026: // ARCHIMEDE
      {		
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_ArchimedeDlg *aDlg = new GeometryGUI_ArchimedeDlg( parent, "", Sel ) ;	
	break;
      }

    case 4027: // FILLET
      {		
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_FilletDlg *aDlg = new GeometryGUI_FilletDlg( parent, "", Sel, ic ) ;	
	break;
      }

    case 4028: // CHAMFER
      {		
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_ChamferDlg *aDlg = new GeometryGUI_ChamferDlg( parent, "", Sel, ic ) ;	
	break;
      }

    case 4031: // PRISM
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_PrismDlg *aDlg = new GeometryGUI_PrismDlg( parent, "", Sel ) ;
	break;
      }

    case 4032: // REVOL
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_RevolDlg *aDlg = new GeometryGUI_RevolDlg( parent, "", Sel ) ;
	break;
      }

    case 4033: // FILLING
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_FillingDlg *aDlg = new GeometryGUI_FillingDlg( parent, "", Sel ) ;
	break;
      }

    case 4034: // PIPE
      {	
	GeomGUI->EmitSignalDeactivateDialog() ;
  	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  	GeometryGUI_PipeDlg *aDlg = new  GeometryGUI_PipeDlg(parent, "", Sel ) ;
	break;
      }
      
    case 5001: // CHECK GEOMETRY
      {
	QAD_PyEditor* PyEditor = GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getPyEditor();
	PyEditor->setText("from GEOM_usinggeom import *\n");
	PyEditor->setText(">>> ");
	PyEditor->handleReturn();
	break;
      }

    case 6021: // SHADING - WIREFRAME
      {
	if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
	  VTKViewer_RenderWindowInteractor* myRenderInter= 
	    ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();

	  QApplication::setOverrideCursor(waitCursor); 
	  
	  int themode = myRenderInter->GetDisplayMode();
	  if( themode==0 ) {
	    myRenderInter->SetDisplayMode(1);
	    GeomGUI->GetDesktop()->menuBar()->changeItem(6021, tr("GEOM_MEN_WIREFRAME") );
	  } else {
	    myRenderInter->SetDisplayMode(0);
	    GeomGUI->GetDesktop()->menuBar()->changeItem(6021, tr("GEOM_MEN_SHADING") );
	  }

	  QApplication::restoreOverrideCursor(); 	  
	  
	} else if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  AIS_DisplayMode mode = ( AIS_DisplayMode )ic->DisplayMode();
	  QApplication::setOverrideCursor( Qt::waitCursor );
	  AIS_DisplayMode newmode = (mode == AIS_WireFrame ? AIS_Shaded : AIS_WireFrame);

	  AIS_ListOfInteractive List;
	  ic->DisplayedObjects(List);
	  AIS_ListOfInteractive List1;
	  ic->ObjectsInCollector(List1);
	  List.Append(List1);
	  
	  AIS_ListIteratorOfListOfInteractive ite(List);
	  while (ite.More()) {
	    if (ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	      Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	      ic->SetDisplayMode(aSh,Standard_Integer(newmode),true);
	    }
	    ite.Next();
	  }

	  ic->SetDisplayMode( newmode, Standard_False);
	  if ( newmode == 1 )
	    GeomGUI->GetDesktop()->menuBar()->changeItem(6021, tr("GEOM_MEN_WIREFRAME") );
	  else
	    GeomGUI->GetDesktop()->menuBar()->changeItem(6021, tr("GEOM_MEN_SHADING") );
	  
	  QApplication::restoreOverrideCursor();
	}
	break;
      }
      
    case 6022: // DISPLAY ALL
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK )
	  //VTK
	  ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor()->DisplayAll();
	else if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC )
	  GeomGUI->OnDisplayAll();
	break;
      }
      
    case 6023: // DISPLAY ONLY
    case 8023: // DISPLAY ONLY - POPUP VIEWER
      {
	if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
	  // VTK
	  GeomGUI->OnVTKDisplayOnly();
	}
	else if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC)
	  GeomGUI->OnDisplayOnly();
	break;
      }
      
    case 6024: // ERASE ALL
      {
	if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
	  //VTK
	  ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor()->EraseAll();
	}
	else if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC)  {
	  ic->EraseAll(Standard_True, Standard_False);
	  ic->Display(v3d->getTrihedron());
	}

	//NRI	SALOME_Selection* Sel = SALOME_Selection::Selection( myActiveStudy->getSelection() );
	//NRI	Sel->ClearInteractiveObjects();
      	break;
      }

    case 6025 : // ERASE ONLY
    case 8022 : // ERASE - POPUP VIEWER
      {
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
	  // VTK
	  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	  Handle(SALOME_InteractiveObject) anIObject;
	  for(;It.More();It.Next()) {
	    anIObject = It.Value();
	    VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
	    
	    if(myRenderInter->isInViewer(anIObject)) {
	      myRenderInter->Erase(anIObject);
	    } else {
	      SALOMEDS::SObject_var obj = aStudy->FindObjectID( anIObject->getEntry() );
	      SALOMEDS::GenericAttribute_var anAttr;
	      SALOMEDS::AttributeIOR_var     anIOR;
	      if ( !obj->_is_nil() ) {
		if ( obj->FindAttribute(anAttr, "AttributeIOR")) {
		  // this SObject may be GEOM module root SObject
		  SALOMEDS::ChildIterator_var anIter = GeomGUI->myActiveStudy->getStudyDocument()->NewChildIterator(obj);
		  bool useSubItems = false;
		  while (anIter->More() && !useSubItems) {
		    SALOMEDS::SObject_var subobj = anIter->Value();
		    SALOMEDS::GenericAttribute_var aTmpAttr;
		    if (subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		      anAttr = aTmpAttr;
		      obj = subobj;
		      useSubItems = true;
		    } else anIter->Next();
		  }
		  
		  while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
		    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
		    GEOM::GEOM_Shape_var aShape = GeomGUI->myComponentGeom->GetIORFromString(anIOR->Value());
		    TopoDS_Shape Shape = ShapeReader.GetShape(GeomGUI->myComponentGeom,aShape);
		    if (obj->FindAttribute(anAttr, "AttributeName")) {
		      // searchin for already displayed objects with the same shape
		      vtkRenderer* theRenderer = ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
		      vtkActorCollection* theAllActors = theRenderer->GetActors();
		      theAllActors->InitTraversal();
		      vtkActor* actor = (vtkActor*)theAllActors->GetNextActor();
		      Handle(SALOME_InteractiveObject) anIObj;
		      // don't create new study object if it already exists
		      bool isDisplayed = false;
		      while(!(actor==NULL)) {
			SALOME_Actor* Gactor = SALOME_Actor::SafeDownCast(actor);
			if (Gactor!=NULL) {
			  if (Gactor->hasIO()) {
			    if (strcmp(Gactor->getIO()->getEntry(),obj->GetID())==0) {
			      isDisplayed = true;
			      anIObj = Gactor->getIO();
			      if (!anIObj.IsNull()) myRenderInter->Erase(anIObj);
			    }
			  }
			}
			actor=(vtkActor*)(theAllActors->GetNextActor());
		      }
		    }
		    if (useSubItems) {
		      anIter->Next();
		      anAttr=SALOMEDS::GenericAttribute::_nil();
		      while (anIter->More() && anAttr->_is_nil()) {
			SALOMEDS::SObject_var subobject = anIter->Value();
			SALOMEDS::GenericAttribute_var aTmpAttribute;
			if (subobject->FindAttribute(aTmpAttribute, "AttributeIOR")) {
			  anAttr = aTmpAttribute;
			  obj = subobject;
			} else anIter->Next();
		      }
		    } else anAttr = SALOMEDS::GenericAttribute::_nil();
		  }
		}
	      }
	    }
	  }
//  	  SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
//  	  for(;It.More();It.Next()) {
//  	    Handle(SALOME_InteractiveObject) IOS = It.Value();
	  //  	    GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getVTKView()->getRWInteractor()->Erase(IOS);;
	}
	else if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  // OCC
	  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	  Handle(SALOME_InteractiveObject) anIObject;
	  for(;It.More();It.Next()) {
	    anIObject = It.Value();
	    if ( v3d->isInViewer( anIObject, true ) ) {
	      Standard_Boolean found;
	      Handle(GEOM_AISShape) aSh = GeomGUI->ConvertIOinGEOMAISShape( anIObject, found, true );
	      if ( found ) {
		ic->Erase(aSh);
		ic->AddOrRemoveCurrentObject(aSh, true);
	      }
	    } else {
	      SALOMEDS::SObject_var obj = aStudy->FindObjectID( anIObject->getEntry() );
	      SALOMEDS::GenericAttribute_var anAttr;
	      SALOMEDS::AttributeIOR_var     anIOR;
	      if ( !obj->_is_nil() ) {
		if ( obj->FindAttribute(anAttr, "AttributeIOR")) {
		  // this SObject may be GEOM module root SObject
		  SALOMEDS::ChildIterator_var anIter = GeomGUI->myActiveStudy->getStudyDocument()->NewChildIterator(obj);
		  bool useSubItems = false;
		  while (anIter->More() && !useSubItems) {
		    SALOMEDS::SObject_var subobj = anIter->Value();
		    SALOMEDS::GenericAttribute_var aTmpAttr;
		    if (subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		      anAttr = aTmpAttr;
		      obj = subobj;
		      useSubItems = true;
		    } else anIter->Next();
		  }
		  
		  while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
		    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
		    GEOM::GEOM_Shape_var aShape = GeomGUI->myComponentGeom->GetIORFromString(anIOR->Value());
		    TopoDS_Shape Shape = ShapeReader.GetShape(GeomGUI->myComponentGeom,aShape);
		    if (obj->FindAttribute(anAttr, "AttributeName")) {
		      // searchin for already displayed objects with the same shape
		      AIS_ListOfInteractive aDisplayed;
		      ic->DisplayedObjects(aDisplayed);
		      AIS_ListIteratorOfListOfInteractive anIObjects(aDisplayed);
		      Handle(AIS_Shape) anAISShape;
		      for(;anIObjects.More();anIObjects.Next()) {
			anAISShape = Handle(AIS_Shape)::DownCast(anIObjects.Value());
			if (!anAISShape.IsNull()) {
			  if (anAISShape->Shape().IsSame(Shape)) break;
			  anAISShape.Nullify();
			}
		      }
		      if (!anAISShape.IsNull()) {
			if (ic->IsDisplayed(anAISShape)) ic->Erase(anAISShape);
		      }
		    }
		    if (useSubItems) {
		      anIter->Next();
		      anAttr=SALOMEDS::GenericAttribute::_nil();
		      while (anIter->More() && anAttr->_is_nil()) {
			SALOMEDS::SObject_var subobject = anIter->Value();
			SALOMEDS::GenericAttribute_var aTmpAttribute;
			if (subobject->FindAttribute(aTmpAttribute, "AttributeIOR")) {
			  anAttr = aTmpAttribute;
			  obj = subobject;
			} else anIter->Next();
		      }
		    } else anAttr = SALOMEDS::GenericAttribute::_nil();
		  }
		}
	      }
	    }
	  }
	}
	Sel->ClearIObjects();
	break;
      }
      

    case 6060: // BOUNDING BOX
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_BndBoxDlg *aDlg = new  GeometryGUI_BndBoxDlg(parent, "", Sel ) ;
	break ;
      }

    case 6061: // MIN DISTANCE
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_DistanceDlg *aDlg = new GeometryGUI_DistanceDlg(parent, "", Sel ) ;
	break ;
      }

    case 8021: // WIREFRAME-SHADING
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
	  // VTK
	  VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
	  
	  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	  SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
	  
	  QApplication::setOverrideCursor( Qt::waitCursor );
	  for(;It.More();It.Next()) {
	    Handle(SALOME_InteractiveObject) IOS = It.Value();
	    myRenderInter->SwitchRepresentation(IOS, false);
	  }
	  myRenderInter->Render();
	  QApplication::restoreOverrideCursor();
	}
	else if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
	  QApplication::setOverrideCursor( Qt::waitCursor );
	  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	  for(;It.More();It.Next()) {
	    Handle(SALOME_InteractiveObject) IObject = It.Value();
	    Standard_Boolean found;
	    Handle(GEOM_AISShape) Shape = GeomGUI->ConvertIOinGEOMAISShape( IObject, found, true );
            if (!Shape.IsNull()) {
  	      AIS_DisplayMode mode = ( AIS_DisplayMode )Shape->DisplayMode();
	      if ( mode == -1 )
	        mode = ( AIS_DisplayMode )ic->DisplayMode();
	      
	      QApplication::setOverrideCursor( Qt::waitCursor );
	      ic->SetDisplayMode(Shape, mode == AIS_WireFrame ? AIS_Shaded : AIS_WireFrame, false);
	    }
	    QApplication::restoreOverrideCursor();
	  }
	  ic->UpdateCurrentViewer();
	}
	QApplication::restoreOverrideCursor();
	break;
      }
      
    case 8031: // COLOR - POPUP VIEWER
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
	  // VTK
	  VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
	  
	  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	  SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
	  
	  Handle(SALOME_InteractiveObject) FirstIOS =  Sel->firstIObject();
	  if(!FirstIOS.IsNull()) {
	    
	    QColor initcolor = myRenderInter->GetColor(FirstIOS);
	    
	    QColor c = QColorDialog::getColor( initcolor,
					       QAD_Application::getDesktop() );
	    
	    if ( c.isValid() ) {
	      QApplication::setOverrideCursor( Qt::waitCursor );
	      for(;It.More();It.Next()) {
		Handle(SALOME_InteractiveObject) IOS = It.Value();
		myRenderInter->SetColor(IOS,c);
	      }
	    }
	    QApplication::restoreOverrideCursor();
	  }
	}
	else if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
	  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	  Handle(SALOME_InteractiveObject) IO = Sel->firstIObject();
	  Standard_Boolean found;
	  Handle(GEOM_AISShape) Shape = GeomGUI->ConvertIOinGEOMAISShape(IO, found, true);
	  if ( found ) {
	    Quantity_Color CSFColor;
	    Shape->Color( CSFColor );
	    
	    QColor c = QColorDialog::getColor( QColor(CSFColor.Red()  * 255.0,
						      CSFColor.Green()* 255.0, 
						      CSFColor.Blue() * 255.0 ),
					       QAD_Application::getDesktop() );
	    
	    if ( c.isValid() ) {
	      CSFColor = Quantity_Color ( c.red()/255., c.green()/255., c.blue()/255., Quantity_TOC_RGB );
	      QApplication::setOverrideCursor( Qt::waitCursor );
	    
	      SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	      for(;It.More();It.Next()) {
		Handle(SALOME_InteractiveObject) IObject = It.Value();
		Standard_Boolean found;
		Handle(GEOM_AISShape) Shape = GeomGUI->ConvertIOinGEOMAISShape(IObject, found, true);
		if ( found ) {
		  Shape->SetColor ( CSFColor );
		  Shape->SetShadingColor ( CSFColor );
		}
	      }
	    }
	  }
	}
	QApplication::restoreOverrideCursor();
	break;
      }
      
    case 8032: // TRANSPARENCY - POPUP VIEWER
      {
	GeomGUI->EmitSignalDeactivateDialog() ;
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	GeometryGUI_TransparencyDlg *aDlg = new GeometryGUI_TransparencyDlg( parent, "", Sel, ic ) ;
	break ;
      }
      
    case 8033: // ISOS - POPUP VIEWER
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
	  break;
	else {
	  ic->InitCurrent();
	  if ( ic->MoreCurrent() ) {
	    Handle(GEOM_AISShape) CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());
	    QString IsoU = QAD_CONFIG->getSetting("Geometry:SettingsIsoU");
	    QString IsoV = QAD_CONFIG->getSetting("Geometry:SettingsIsoV");
	    
	    if ( !IsoU.isEmpty() )
	      ic->DefaultDrawer()->UIsoAspect()->SetNumber(IsoU.toInt());
	    else
	      IsoU = "1";
	    if ( !IsoV.isEmpty() )
	      ic->DefaultDrawer()->VIsoAspect()->SetNumber(IsoV.toInt());
	    else
	      IsoV = "1";
	    
	    GeometryGUI_NbIsosDlg * NbIsosDlg =
	      new GeometryGUI_NbIsosDlg(QAD_Application::getDesktop(), tr("GEOM_MEN_ISOS"), TRUE );
	    
	    NbIsosDlg->SpinBoxU->setValue(IsoU.toInt());
	    NbIsosDlg->SpinBoxV->setValue(IsoV.toInt());
	    
	    if ( NbIsosDlg->exec() ) {
	      QApplication::setOverrideCursor( Qt::waitCursor );
	      for ( ; ic->MoreCurrent (); ic->NextCurrent () ) {
		Handle (AIS_Drawer) CurDrawer;
		
		CurDrawer = ic->Current()->Attributes();
		CurDrawer->UIsoAspect()->SetNumber( NbIsosDlg->SpinBoxU->text().toInt() );
		CurDrawer->VIsoAspect()->SetNumber( NbIsosDlg->SpinBoxV->text().toInt() );
		
		ic->SetLocalAttributes(CurObject, CurDrawer);
		ic->Redisplay(CurObject);
	      }
	    }
	  }
	}
	QApplication::restoreOverrideCursor();
	break;
      }

    case 9022 : // ERASE - OBJBROSER POPUP
      {
	SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
	  // VTK
	  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	  Handle(SALOME_InteractiveObject) anIObject;
	  for(;It.More();It.Next()) {
	    anIObject = It.Value();
	  VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
	 
//	  Handle(SALOME_InteractiveObject) IObject;
	  if(myRenderInter->isInViewer(anIObject)) {
	    myRenderInter->Erase(anIObject);
	  } else {
	    SALOMEDS::SObject_var obj = aStudy->FindObjectID( anIObject->getEntry() );
	    SALOMEDS::GenericAttribute_var anAttr;
	    SALOMEDS::AttributeIOR_var     anIOR;
	    if ( !obj->_is_nil() ) {
	      if ( obj->FindAttribute(anAttr, "AttributeIOR")) {
		// this SObject may be GEOM module root SObject
		SALOMEDS::ChildIterator_var anIter = GeomGUI->myActiveStudy->getStudyDocument()->NewChildIterator(obj);
		bool useSubItems = false;
		while (anIter->More() && !useSubItems) {
		  SALOMEDS::SObject_var subobj = anIter->Value();
		  SALOMEDS::GenericAttribute_var aTmpAttr;
		  if (subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		    anAttr = aTmpAttr;
		    obj = subobj;
		    useSubItems = true;
		  } else anIter->Next();
		}
		
		while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
		  anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
		  GEOM::GEOM_Shape_var aShape = GeomGUI->myComponentGeom->GetIORFromString(anIOR->Value());
		  TopoDS_Shape Shape = ShapeReader.GetShape(GeomGUI->myComponentGeom,aShape);
		  if (obj->FindAttribute(anAttr, "AttributeName")) {
		    // searchin for already displayed objects with the same shape
		    vtkRenderer* theRenderer = ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
		    vtkActorCollection* theAllActors = theRenderer->GetActors();
		    theAllActors->InitTraversal();
		    vtkActor* actor = (vtkActor*)theAllActors->GetNextActor();
		    Handle(SALOME_InteractiveObject) anIObj;
		    // don't create new study object if it already exists
		    bool isDisplayed = false;
		    while(!(actor==NULL)) {
		      SALOME_Actor* Gactor = SALOME_Actor::SafeDownCast(actor);
		      if (Gactor!=NULL) {
			if (Gactor->hasIO()) {
			  if (strcmp(Gactor->getIO()->getEntry(),obj->GetID())==0) {
			    isDisplayed = true;
			    anIObj = Gactor->getIO();
			    if (!anIObj.IsNull()) myRenderInter->Erase(anIObj);
			  }
			}
		      }
		      actor=(vtkActor*)(theAllActors->GetNextActor());
		    }
		  }
		  if (useSubItems) {
		    anIter->Next();
		    anAttr=SALOMEDS::GenericAttribute::_nil();
		    while (anIter->More() && anAttr->_is_nil()) {
		      SALOMEDS::SObject_var subobject = anIter->Value();
		      SALOMEDS::GenericAttribute_var aTmpAttribute;
		      if (subobject->FindAttribute(aTmpAttribute, "AttributeIOR")) {
			anAttr = aTmpAttribute;
			obj = subobject;
		      } else anIter->Next();
		    }
		  } else anAttr = SALOMEDS::GenericAttribute::_nil();
		}      
	      }
	    }
	  }
//  	  SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
//  	  for(;It.More();It.Next()) {
//  	    Handle(SALOME_InteractiveObject) IOS = It.Value();
//  	    GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getVTKView()->getRWInteractor()->Erase(IOS);;
	}
//  	  SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
//  	  for(;It.More();It.Next()) {
//  	    Handle(SALOME_InteractiveObject) IOS = It.Value();
//  	    GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getVTKView()->getRWInteractor()->Erase(IOS);;
//  	  }
	}
	else if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
	  // OCC
	  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	  Handle(SALOME_InteractiveObject) anIObject;
	  for(;It.More();It.Next()) {
	    anIObject = It.Value();
	    if ( v3d->isInViewer( anIObject, true ) ) {
	      Standard_Boolean found;
	      Handle(GEOM_AISShape) aSh = GeomGUI->ConvertIOinGEOMAISShape( anIObject, found, true );
	      if ( found ) {
		ic->Erase(aSh);
		ic->AddOrRemoveCurrentObject(aSh, true);
	      }
	    } else {
	      SALOMEDS::SObject_var obj = aStudy->FindObjectID( anIObject->getEntry() );
	      SALOMEDS::GenericAttribute_var anAttr;
	      SALOMEDS::AttributeIOR_var     anIOR;
	      if ( !obj->_is_nil() ) {
		if ( obj->FindAttribute(anAttr, "AttributeIOR")) {
		  // this SObject may be GEOM module root SObject
		  SALOMEDS::ChildIterator_var anIter = GeomGUI->myActiveStudy->getStudyDocument()->NewChildIterator(obj);
		  bool useSubItems = false;
		  while (anIter->More() && !useSubItems) {
		    SALOMEDS::SObject_var subobj = anIter->Value();
		    SALOMEDS::GenericAttribute_var aTmpAttr;
		    if (subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		      anAttr = aTmpAttr;
		      obj = subobj;
		      useSubItems = true;
		    } else anIter->Next();
		  }
		  
		  while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
		    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
		    GEOM::GEOM_Shape_var aShape = GeomGUI->myComponentGeom->GetIORFromString(anIOR->Value());
		    TopoDS_Shape Shape = ShapeReader.GetShape(GeomGUI->myComponentGeom,aShape);
		    if (obj->FindAttribute(anAttr, "AttributeName")) {
		      // searchin for already displayed objects with the same shape
		      AIS_ListOfInteractive aDisplayed;
		      ic->DisplayedObjects(aDisplayed);
		      AIS_ListIteratorOfListOfInteractive anIObjects(aDisplayed);
		      Handle(AIS_Shape) anAISShape;
		      for(;anIObjects.More();anIObjects.Next()) {
			anAISShape = Handle(AIS_Shape)::DownCast(anIObjects.Value());
			if (!anAISShape.IsNull()) {
			  if (anAISShape->Shape().IsSame(Shape)) break;
			  anAISShape.Nullify();
			}
		      }
		      if (!anAISShape.IsNull()) {
			if (ic->IsDisplayed(anAISShape)) ic->Erase(anAISShape);
		      }
		    }
		    if (useSubItems) {
		      anIter->Next();
		      anAttr=SALOMEDS::GenericAttribute::_nil();
		      while (anIter->More() && anAttr->_is_nil()) {
			SALOMEDS::SObject_var subobject = anIter->Value();
			SALOMEDS::GenericAttribute_var aTmpAttribute;
			if (subobject->FindAttribute(aTmpAttribute, "AttributeIOR")) {
			  anAttr = aTmpAttribute;
			  obj = subobject;
			} else anIter->Next();
		      }
		    } else anAttr = SALOMEDS::GenericAttribute::_nil();
		  }
		}
	      }
	    }
	  }
//  	  QAD_Viewer3d* v3d = GeomGUI->myActiveStudy->getActiveStudyFrame()->getViewerOCC();
//  	  Handle (AIS_InteractiveContext) myContext = v3d->getAISContext();
//  	  myContext->EraseSelected();
	}

	Sel->ClearIObjects();
	break;
      }
      
    case 9023 : // DISPLAY ONLY - OBJBROSER POPUP
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK )
	  GeomGUI->OnVTKDisplayOnly();
	else if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC )
	  GeomGUI->OnDisplayOnly();
	break;
      }

    case 9024 : // OPEN - OBJBROSER POPUP
      {
	  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
	  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	  Handle(SALOME_InteractiveObject) anIObject;
	  for(;It.More();It.Next()) {
	    anIObject = It.Value();
	    SALOMEDS::SObject_var obj = aStudy->FindObjectID( anIObject->getEntry() );
	    SALOMEDS::AttributePersistentRef_var aPersist;
	    SALOMEDS::AttributeIOR_var anIOR;
	    if ( !obj->_is_nil() ) {
	      // this SObject may be GEOM module root SObject
	      SALOMEDS::ChildIterator_var anIter = GeomGUI->myActiveStudy->getStudyDocument()->NewChildIterator(obj);
	      SALOMEDS::GenericAttribute_var anAttr;
	      bool useSubItems = false;
	      while (anIter->More() && !useSubItems) {
		SALOMEDS::SObject_var subobj = anIter->Value();
		if (subobj->FindAttribute(anAttr, "AttributePersistentRef")) {
		  useSubItems = true;
		  obj = subobj;
		}
		else anIter->Next();
	      }
	      obj->FindAttribute(anAttr, "AttributePersistentRef");

	      while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
		if (!obj->FindAttribute(anAttr, "AttributeIOR") &&
		    obj->FindAttribute(anAttr, "AttributePersistentRef")) {
		  // load
		  Engines::Component_var comp = GeomGUI->myDesktop->getEngine("FactoryServer","GEOM");
		  if (!CORBA::is_nil(comp)) {
		    SALOMEDS::Driver_var   driver = SALOMEDS::Driver::_narrow(comp);


	            SALOMEDS::StudyBuilder_var aStudyBuilder = GeomGUI->myActiveStudy->getStudyDocument()->NewBuilder();
	            aStudyBuilder->LoadWith(GeomGUI->myActiveStudy->getStudyDocument()->FindComponent("GEOM"),driver);

		  } else {
		    MESSAGE("Component is null");
		  }
		}
	        if (useSubItems) {
		  anIter->Next();
		  obj = anIter->Value();
		} else anAttr = SALOMEDS::GenericAttribute::_nil();
              }
	    }
	  }
	break;
      }
      
    case 10000 : // SKETCH Segment
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
	  break;
	GeomGUI->OnSketchSegment();
	break;
      }
    case 10001 : // SKETCH Arc
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
	  break;
	GeomGUI->OnSketchArc();
	break;
      }
    case 10002 : // SKETCH Set Angle
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
	  break;
	GeomGUI->OnSketchSetAngle();
	break;
      }
    case 10003 : // SKETCH Set X
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
	  break;
	GeomGUI->OnSketchSetx();
	break;
      }
    case 10004 : // SKETCH Set Y
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
	  break;
	GeomGUI->OnSketchSety();
	break;
      }
    case 10006 : // SKETCH Delete
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
	  break;
	GeomGUI->OnSketchDelete();
	break;
      }
    case 10007 : // SKETCH End
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
	  break;
	GeomGUI->OnSketchEnd();
	break;
      }
    case 10008 : // SKETCH Close
      {
	if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
	  break;
	GeomGUI->OnSketchClose();
	break;
      }
    case 10010 : // SKETCH OptionsOnofflengthdimension
      {
	GeomGUI->OnSketchOptionsOnofflengthdimension();
	break;
      }
    case 10011 : // SKETCH OptionsOnoffangledimension
      {
	GeomGUI->OnSketchOptionsOnoffangledimension();
	break;
      }
    case 10012 : // SKETCH OptionsOnoffradiusdimension
      {
	GeomGUI->OnSketchOptionsOnoffradiusdimension();
	break;
      }
    case 10013 : // SKETCH OptionsOnoffxdimension
      {
	GeomGUI->OnSketchOptionsOnoffxdimension();
	break;
      }
    case 10014 : // SKETCH OptionsOnoffydimension
      {
	GeomGUI->OnSketchOptionsOnoffydimension();
	break;
      }

    default:
      {
	parent->putInfo( tr("GEOM_PRP_COMMAND").arg(theCommandID ) );
	break;
      }
    }
  
  return true ;
}

//=======================================================================
// function : ConvertClickToPoint()
// purpose  : Returns the point clicked in 3D view
//=======================================================================
gp_Pnt GeometryGUI::ConvertClickToPoint( Standard_Real x,
					 Standard_Real y,
					 Handle(V3d_View) aView )
{
  V3d_Coordinate XEye, YEye, ZEye, XAt, YAt, ZAt;
  aView->Eye(XEye, YEye, ZEye);

  aView->At(XAt, YAt, ZAt);
  gp_Pnt EyePoint(XEye, YEye, ZEye);
  gp_Pnt AtPoint(XAt, YAt, ZAt);

  gp_Vec EyeVector(EyePoint, AtPoint);
  gp_Dir EyeDir(EyeVector);

  gp_Pln PlaneOfTheView = gp_Pln(AtPoint,EyeDir);
  Standard_Real X, Y, Z;
  aView->Convert(x, y, X, Y, Z);
  gp_Pnt ConvertedPoint(X, Y, Z);

  gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView, ConvertedPoint);
  gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(),
				     ConvertedPointOnPlane.Y(),
				     PlaneOfTheView);
  return ResultPoint;
}




//==================================================================================
// function : 0nMousePress()
// purpose  : [static] manage mouse events
//==================================================================================
bool GeometryGUI::OnMousePress(QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame)
{
  MESSAGE ( "GeometryGUI::OnMousePress")
  GeometryGUI::GetOrCreateGeometryGUI(parent);

  if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
    return false;

  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  OCCViewer_ViewPort* vp = ((OCCViewer_ViewFrame*)studyFrame->getRightFrame()->getViewFrame())->getViewPort();
  
  /* Get the clicked or selected point */
  gp_Pnt thePoint;

  if ( GeomGUI->myState == CURRENT_SKETCH) {
    GeomGUI->mySketcher.ValidateEdge();
    if (GeomGUI->mySketcher.GetmyEdgesNumber() == 1 ) {
      QMenuBar* Mb = GeomGUI->myDesktop->getMainMenuBar();
      QMenuData* pp;
      QMenuItem* item = Mb->findItem(313,&pp);
      pp->setItemEnabled( 313, true);  // SKETCH CONTRAINTS
    }
  } else if( GeomGUI->myState == POINT_METHOD ) {

    GeomGUI->EraseSimulationShape() ;
    GeometryGUI_PointDlg *DialogPt = (GeometryGUI_PointDlg*)(GeomGUI->myActiveDialogBox) ;

    if ( DialogPt->UseLocalContext() ) {
      ic->InitSelected();
      if ( pe->state() == Qt::ShiftButton )
	v3d->getAISSelector()->shiftSelect();  /* Append selection */
      else
	v3d->getAISSelector()->select();       /* New selection    */
      
      if ( ic->MoreSelected() ) {
	thePoint = BRep_Tool::Pnt( TopoDS::Vertex(ic->SelectedShape()) );
      }
      else
	thePoint = ConvertClickToPoint(pe->x(), pe->y(), ( (OCCViewer_ViewPort3d*)vp)->getView() );
    } 
    else
      thePoint = ConvertClickToPoint(pe->x(), pe->y(), ( (OCCViewer_ViewPort3d*)vp)->getView() );
    
    if( DialogPt != 0 ) { 
      DialogPt->PointIntoCoordinates(thePoint, true) ;  /* display point */
    }
    else {
      // MESSAGE ("On Mouse Press : myActiveDialogBox is null"  << endl) ;
      GeomGUI->myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
    }
  }

  return false ;
}


//=======================================================================
// function : OnMouseMove()
// purpose  : [static] manage mouse events
//=======================================================================
bool GeometryGUI::OnMouseMove (QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame)
{
  GeometryGUI::GetOrCreateGeometryGUI(parent);

  if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return false;

  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  OCCViewer_ViewPort* vp = ((OCCViewer_ViewFrame*)studyFrame->getRightFrame()->getViewFrame())->getViewPort();

  if ( GeomGUI->myState == CURRENT_SKETCH) 
    GeomGUI->mySketcher.MakeCurrentEdge( pe->x(), pe->y(), ((OCCViewer_ViewPort3d*)vp)->getView() );
  
   return true;
}


//================================================================================
// function : SetDisplayedObjectList()
// purpose  :
//================================================================================
void GeometryGUI::SetDisplayedObjectList()
{
  if (myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return;

  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  ListDisplayedObject.Clear();

  Handle (AIS_InteractiveContext) aContext = v3d->getAISContext();
  aContext->DisplayedObjects( ListDisplayedObject );
}



//=====================================================================================
// function : OnDisplayAll()
// purpose  :
//=====================================================================================
void GeometryGUI::OnDisplayAll(bool onlyPreviousDisplayedObject)
{
  if (myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return;
  
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) myContext = v3d->getAISContext();
  
  myContext->Display(v3d->getTrihedron());

  if ( !onlyPreviousDisplayedObject ) {
    AIS_ListOfInteractive List1;
    myContext->ObjectsInCollector(List1);
    AIS_ListIteratorOfListOfInteractive ite1(List1);
    while (ite1.More()) {
      if (ite1.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite1.Value());
	if ( aSh->hasIO() ) {
	  Handle(GEOM_InteractiveObject) GIO = Handle(GEOM_InteractiveObject)::DownCast(aSh->getIO());
	  if ( v3d->isInViewer(GIO, true) ) {
	    myContext->Display(aSh);
	  }
	}
      }
      ite1.Next();
    }
  } else {
    AIS_ListIteratorOfListOfInteractive ite(ListDisplayedObject);
    while (ite.More()) {
      if (ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	if ( aSh->hasIO() ) {
	  Handle(GEOM_InteractiveObject) GIO = Handle(GEOM_InteractiveObject)::DownCast(aSh->getIO());
	  if ( v3d->isInViewer(GIO,true) ) {
	    myContext->Display(aSh);
	  }
	}
      }
      ite.Next();
    }
  }
}




//=====================================================================================
// function : OnVTKDisplayOnly()
// purpose  :
//=====================================================================================
void GeometryGUI::OnVTKDisplayOnly()
{

  // Erase all not selected actors
  
  QApplication::setOverrideCursor( Qt::waitCursor );

  vtkRenderer* aren = ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
  vtkActorCollection* theActors = aren->GetActors();
  theActors->InitTraversal();
  vtkActor *ac = theActors->GetNextActor();
  while(!(ac==NULL)) {
    if ( ac->IsA("SALOME_Actor") ) {
      SALOME_Actor* anActor = SALOME_Actor::SafeDownCast( ac );
      if(!anActor->isHighlighted()) anActor->VisibilityOff();
    }
    ac = theActors->GetNextActor();
  }

  // Display selection
  SALOMEDS::Study_var            aStudy = myActiveStudy->getStudyDocument();
  SALOMEDS::StudyBuilder_var     aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::SObject_var          fatherSF = aStudy->FindObjectID(GeomGUI->myActiveStudy->getActiveStudyFrame()->entry());
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var    aName;
  SALOMEDS::AttributeIOR_var     anIOR;

  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	  
  for(;It.More();It.Next()) {
    Handle(SALOME_InteractiveObject) IObject = It.Value();
    SALOMEDS::SObject_var obj = aStudy->FindObjectID( IObject->getEntry() );

    VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();

    if(myRenderInter->isInViewer(IObject)) {
      myRenderInter->Display(IObject);
    }
    else {
      // Create new actor
      if ( !obj->_is_nil() ) {
	if ( !obj->FindAttribute(anAttr, "AttributeIOR")) 
	  break;
	// If selected object contains displayable subobjects, then do nothing
	SALOMEDS::ChildIterator_var anIter = GeomGUI->myActiveStudy->getStudyDocument()->NewChildIterator(obj);
	SALOMEDS::GenericAttribute_var aTmpAttr;
	
	anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	GEOM::GEOM_Shape_var aShape = GeomGUI->myComponentGeom->GetIORFromString(anIOR->Value());
	if (CORBA::is_nil(aShape)) continue;
	TopoDS_Shape Shape = ShapeReader.GetShape(GeomGUI->myComponentGeom,aShape);

	if (obj->FindAttribute(anAttr, "AttributeName")) {
          aName = SALOMEDS::AttributeName::_narrow(anAttr);
	  // open transaction
	  QAD_Operation* op = new SALOMEGUI_ImportOperation( GeomGUI->myActiveStudy );
	  op->start();
		  
	  SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
	  aStudyBuilder->Addreference(newObj1, obj);
	  // commit transaction
	  op->finish();
		  
	  vtkRenderer* theRenderer = ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
	  int themode = myRenderInter->GetDisplayMode();
	  vtkActorCollection* theActors = 
	    GEOM_AssemblyBuilder::BuildActors(Shape,0,themode,Standard_True);
	  theActors->InitTraversal();
	  vtkActor* anActor = (vtkActor*)theActors->GetNextActor();
	  while(!(anActor==NULL)) {
	    GEOM_Actor* GActor = GEOM_Actor::SafeDownCast( anActor );
	    GActor->setIO( IObject );
	    GActor->setName( IObject->getName() );
		    
	    theRenderer->AddActor(GActor);
	    vtkRenderWindow *renWin
	      = ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer()->GetRenderWindow();
	    renWin->Render();
	    anActor = (vtkActor*)theActors->GetNextActor();
	  }
	}
      }
    }
  }
  GeomGUI->myActiveStudy->updateObjBrowser( true );
  QApplication::restoreOverrideCursor();
}    



//=====================================================================================
// function : OnDisplayOnly()
// purpose  :
//=====================================================================================
void GeometryGUI::OnDisplayOnly()
{
  if (myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return;

  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  SALOMEDS::Study_var aStudy = myActiveStudy->getStudyDocument();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();

  AIS_ListOfInteractive List;
  ic->DisplayedObjects(List);
  AIS_ListIteratorOfListOfInteractive ite(List);
  while (ite.More()) {
    if (ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
      Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
      if (!ic->IsSelected( aSh )) {
	ic->Erase( aSh, Standard_True, Standard_True );
      }
    }
    ite.Next();
  }

  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  SALOME_Selection* Sel = SALOME_Selection::Selection( myActiveStudy->getSelection() );

  SALOME_ListIteratorOfListIO It1( Sel->StoredIObjects() );
  for(;It1.More();It1.Next()) {
    Handle(SALOME_InteractiveObject) IObject = It1.Value();

    
    SALOMEDS::SObject_var fatherSF = 
      aStudy->FindObjectID(myActiveStudy->getActiveStudyFrame()->entry());
    if ( v3d->isInViewer( IObject, true ) ) {
      AIS_ListOfInteractive List1;
      ic->ObjectsInCollector(List1);
      AIS_ListIteratorOfListOfInteractive ite1(List1);
      while (ite1.More()) {
	if (ite1.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	  Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite1.Value());
	  if ( aSh->hasIO() ) {
	    Handle(GEOM_InteractiveObject) GIO = Handle(GEOM_InteractiveObject)::DownCast(aSh->getIO());
	    if ( IObject->isSame( GIO ) ) {
	      ic->Display(aSh);
	      ic->AddOrRemoveCurrentObject(aSh, true);
	      break;
	    }
	  }
	}
	ite1.Next();
      }
    } else {
      if ( IObject->hasEntry() ) {
	SALOMEDS::SObject_var          obj = aStudy->FindObjectID( IObject->getEntry() );
        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeName_var    aName;
        SALOMEDS::AttributeIOR_var     anIOR;
	if ( !obj->_is_nil() ) {
	  if (obj->FindAttribute(anAttr, "AttributeIOR")) {
	    // this SObject may be GEOM module root SObject

	    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	    GEOM::GEOM_Shape_var aShape = myComponentGeom->GetIORFromString( anIOR->Value() );
	    if (CORBA::is_nil(aShape)) continue;
	    TopoDS_Shape Shape = ShapeReader.GetShape(myComponentGeom, aShape);
	    
	    if (obj->FindAttribute(anAttr, "AttributeName")) {
	      aName = SALOMEDS::AttributeName::_narrow(anAttr);
	      // open transaction
	      QAD_Operation* op = new SALOMEGUI_ImportOperation( myActiveStudy );
	      op->start();
	      
	      SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
		  aStudyBuilder->Addreference(newObj1, obj);
	      // commit transaction
	      op->finish();
	    }
	    
	    Handle(GEOM_AISShape) aSh = new GEOM_AISShape(Shape, aName->Value());
	    Handle(GEOM_InteractiveObject) IO = new GEOM_InteractiveObject(anIOR->Value(), Fatherior,"GEOM");
		
	    IO->setEntry(obj->GetID());
	    aSh->setIO( IO );
	    aSh->setName( aName->Value() );
	    ic->Display (aSh);
	    ic->AddOrRemoveCurrentObject(aSh, true);
	  }
	}  
      }
    }
  }
}


//===============================================================================
// function : OnEditDelete()
// purpose  :
//===============================================================================
void GeometryGUI::OnEditDelete()
{
  if ( QAD_MessageBox::warn2 
       ( QAD_Application::getDesktop(),
	 tr ("GEOM_WRN_WARNING"),
	 tr ("GEOM_REALLY_DELETE"),
	 tr ("GEOM_BUT_YES"), tr ("GEOM_BUT_NO"), 1, 0, 0) != 1 )
    return;
       
  int nbSf = myActiveStudy->getStudyFramesCount();
    
  Standard_Boolean found;
  SALOMEDS::Study_var aStudy = myActiveStudy->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeIOR_var     anIOR;
  
  SALOME_Selection* Sel = SALOME_Selection::Selection( myActiveStudy->getSelection() );
  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
  for(;It.More();It.Next()) {
    Handle(SALOME_InteractiveObject) IObject = It.Value();
    if ( IObject->hasEntry() ) {
      SALOMEDS::Study_var aStudy = GeomGUI->myActiveStudy->getStudyDocument();
      SALOMEDS::SObject_var SO = aStudy->FindObjectID( IObject->getEntry() );
      
      /* Erase child graphical objects */
      SALOMEDS::ChildIterator_var it = aStudy->NewChildIterator(SO);
      for (; it->More();it->Next()) {
	SALOMEDS::SObject_var CSO= it->Value();
	if (CSO->FindAttribute(anAttr, "AttributeIOR") ) {
          anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	  /* Delete child(s) shape in Client : */
	  const TCollection_AsciiString ASCior(anIOR->Value()) ;
	  ShapeReader.RemoveShapeFromBuffer( ASCior ) ;

	  for ( int i = 0; i < nbSf; i++ ) {
	    QAD_StudyFrame* sf = myActiveStudy->getStudyFrame(i);
	    if ( sf->getTypeView() == VIEW_OCC ) {
	      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getViewer();
	      Handle (AIS_InteractiveContext) myContext = v3d->getAISContext();
	      Handle(GEOM_AISShape) Result = GeomGUI->ConvertIORinGEOMAISShape( anIOR->Value(), found );
	      if ( found )
		myContext->Erase( Result, true, false );
	    } else if ( sf->getTypeView() == VIEW_VTK ) {
	      //vtkRenderer* Renderer = ((VTKViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getRenderer();
	      VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getRWInteractor();
	      GEOM_Actor* ac = GeomGUI->ConvertIORinGEOMActor( anIOR->Value(), found );
	      if ( found ) {
		//Renderer->RemoveActor(ac);
		if ( ac->hasIO() ) 
		  myRenderInter->Remove( ac->getIO() );
	      }
	    }
	  }
	}
      }
      
      /* Erase main graphical object */
      for ( int i = 0; i < nbSf; i++ ) {
	QAD_StudyFrame* sf = myActiveStudy->getStudyFrame(i);
	if ( sf->getTypeView() == VIEW_OCC ) {
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getViewer();
	  Handle (AIS_InteractiveContext) myContext = v3d->getAISContext();
	  Handle(GEOM_AISShape) Result = GeomGUI->ConvertIOinGEOMAISShape( IObject, found );
	  if ( found )
	    myContext->Erase( Result, true, false );
	} else if ( sf->getTypeView() == VIEW_VTK ) {
	  VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getRWInteractor();
	  myRenderInter->Remove( IObject );
	}
      }
      
      /* Delete main shape in Client : */
      if (SO->FindAttribute(anAttr, "AttributeIOR") ) {
        anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	const TCollection_AsciiString ASCIor(anIOR->Value()) ;
	ShapeReader.RemoveShapeFromBuffer( ASCIor ) ;
      }

      /* Erase objects in Study */
      SALOMEDS::SObject_var obj = aStudy->FindObjectID( IObject->getEntry() );
      if ( !obj->_is_nil() ) {
	QAD_Operation* op = new SALOMEGUI_ImportOperation( myActiveStudy );
	op->start();
	aStudyBuilder->RemoveObject(obj);
	op->finish();
      }

    } /* IObject->hasEntry() */
  }   /* more/next           */

  /* Clear any previous selection */
  Sel->ClearIObjects() ; 
  myActiveStudy->updateObjBrowser();
}


//==============================================================================
// function : OnEditCopy()
// purpose  :
//==============================================================================
void GeometryGUI::OnEditCopy()
{
  SALOME_Selection* Sel = SALOME_Selection::Selection( myActiveStudy->getSelection() );
  GEOM::GEOM_Gen::ListOfIOR_var listIOR = new GEOM::GEOM_Gen::ListOfIOR;

  const SALOME_ListIO& List = Sel->StoredIObjects();

  GeomGUI->ConvertListOfIOInListOfIOR( List, listIOR);

  Sel->ClearIObjects();

  for (unsigned int ind = 0; ind < listIOR->length();ind++) {
    GEOM::GEOM_Shape_var aShapeInit = myComponentGeom->GetIORFromString(listIOR[ind]);
    try {
      GEOM::GEOM_Shape_var result = myComponentGeom->MakeCopy(aShapeInit) ;
      result->NameType( aShapeInit->NameType() );
      this->Display(result);
    }
    catch  (const SALOME::SALOME_Exception& S_ex) {
      QtCatchCorbaException(S_ex);
    }
  }
  
  myDesktop->putInfo(tr("GEOM_PRP_READY"));
}



//=====================================================================================
// function : OnKeyPress()
// purpose  : [static]
//=====================================================================================
bool GeometryGUI::OnKeyPress (QKeyEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame)
{
  GeometryGUI::GetOrCreateGeometryGUI(parent);

  if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return false;

  return true ;
}



//=====================================================================================
// function : DisplaySimulationShape() 
// purpose  : Displays 'this->mySimulationShape' a pure graphical shape from a TopoDS_Shape
//=====================================================================================
void GeometryGUI::DisplaySimulationShape(const TopoDS_Shape& S) 
{
  if( S.IsNull() )
    return ;
  	
  //NRI DEBUG : 14/02/2002
  if ( myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC )
    return ;
	
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  
  try {
    /* erase any previous */
    ic->Erase( this->mySimulationShape, Standard_True, Standard_False );
    ic->ClearPrs( this->mySimulationShape );
    this->mySimulationShape = new AIS_Shape( TopoDS_Shape() ) ;
    this->mySimulationShape->Set( S ) ;
    this->mySimulationShape->SetColor(Quantity_NOC_VIOLET) ;
    ic->Deactivate( this->mySimulationShape );
    ic->Display( this->mySimulationShape, Standard_False );
    ic->UpdateCurrentViewer();
  }
  catch(Standard_Failure) {
    MESSAGE( "Exception catched in GeometryGUI::DisplaySimulationShape " ) ;
  } 
  this->mySimulationShape->UnsetColor() ;
  return ;
}



//==================================================================================
// function : EraseSimulationShape()
// purpose  : Clears the display of 'mySimulationShape' a pure graphical shape
//==================================================================================
void GeometryGUI::EraseSimulationShape()
{
  int count = myActiveStudy->getStudyFramesCount();
  for ( int i = 0; i < count; i++ ) {
    if (myActiveStudy->getStudyFrame(i)->getTypeView() == VIEW_OCC ) {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getStudyFrame(i)->getRightFrame()->getViewFrame())->getViewer();
      Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
      ic->Erase( this->mySimulationShape, Standard_True, Standard_False );
      ic->ClearPrs( this->mySimulationShape );
      ic->UpdateCurrentViewer();
      
    } else if (myActiveStudy->getStudyFrame(i)->getTypeView() == VIEW_VTK ) { // VTK
    }
  }
  // MESSAGE ( " GeometryGUI::EraseSimulationShape done. " )
}



//=====================================================================================
// function : Import
// purpose  : BRep, Iges, Step
//=====================================================================================
bool GeometryGUI::Import( )
{
  SALOMEDS::Study_var aStudy = myActiveStudy->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  
  GEOM::GEOM_Shape_var aShape;
  QString file;
  QStringList filtersList ;
  
  switch ( GeomGUI->myState )
    {
    case 111 :	// Import BREP
      {
	filtersList.append( tr("GEOM_MEN_IMPORT_BREP") );
	filtersList.append( tr("GEOM_MEN_ALL_FILES") ) ;

	file = QAD_FileDlg::getFileName(myDesktop,
					"",
					filtersList,
					tr("GEOM_MEN_IMPORT"),
					true);
	if ( !file.isEmpty() ) {
	  QApplication::setOverrideCursor( Qt::waitCursor );
	  try {
	    aShape = myComponentGeom->ImportBREP( file.latin1() );
	  }
	  catch (const SALOME::SALOME_Exception& S_ex) {
	    QtCatchCorbaException(S_ex);
	  }
	}
	break;
      }
    case 112 :	// Import IGES
      {
	filtersList.append( tr("GEOM_MEN_IMPORT_IGES") ) ;
	filtersList.append( tr("GEOM_MEN_ALL_FILES") ) ;

	file = QAD_FileDlg::getFileName(myDesktop,
					"",
					filtersList,
					tr("GEOM_MEN_IMPORT"),
					true);
	if ( !file.isEmpty() ) {
	  QApplication::setOverrideCursor( Qt::waitCursor );
	  try {
	    aShape = myComponentGeom->ImportIGES( file.latin1() );
	  }
	  catch (const SALOME::SALOME_Exception& S_ex) {
	    QtCatchCorbaException(S_ex);
	  }
	}
	break;
      }
    case 113 : // Import STEP
      {
	filtersList.append( tr("GEOM_MEN_IMPORT_STEP") ) ;
	filtersList.append( tr("GEOM_MEN_ALL_FILES") ) ;

	file = QAD_FileDlg::getFileName(myDesktop,
					"",
					filtersList,
					tr("GEOM_MEN_IMPORT"),
					true);
	if ( !file.isEmpty() ) {
	  QApplication::setOverrideCursor( Qt::waitCursor );
	  try {
	    aShape = myComponentGeom->ImportSTEP( file.latin1() );
	  }
	  catch  (const SALOME::SALOME_Exception& S_ex) {
	    QtCatchCorbaException(S_ex);
	  }
	}
	break;
      }
    }
  
  if ( !file.isEmpty() ) {
    myDesktop->putInfo( tr("GEOM_PRP_LOADING").arg(QAD_Tools::getFileNameFromPath( file )) );

    SALOMEDS::SComponent_var father = aStudy->FindComponent("GEOM");
    SALOMEDS::SObject_var fatherSF = aStudy->FindObjectID( myActiveStudy->getActiveStudyFrame()->entry());
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeName_var    aName;
    SALOMEDS::AttributePixMap_var  aPixmap;
    int aLocked = false;
    if (father->_is_nil()) {
      QAD_Operation* op = new SALOMEGUI_ImportOperation( myActiveStudy );
      op->start();
      aLocked = aStudy->GetProperties()->IsLocked();
      if (aLocked) aStudy->GetProperties()->SetLocked(false);
      father = aStudyBuilder->NewComponent("GEOM");
      anAttr = aStudyBuilder->FindOrCreateAttribute(father, "AttributeName");
      aName = SALOMEDS::AttributeName::_narrow(anAttr);
      //      aName->SetValue( tr("GEOM_MEN_COMPONENT") );
      aName->SetValue( QAD_Application::getDesktop()->getComponentUserName( "GEOM" ) );
      anAttr = aStudyBuilder->FindOrCreateAttribute(father, "AttributePixMap");
      aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
      aPixmap->SetPixMap( "ICON_OBJBROWSER_Geometry" );
      if (aLocked) aStudy->GetProperties()->SetLocked(true);
      op->finish();
    }
//      if (aLocked) return false;
    aStudyBuilder->DefineComponentInstance( father, myComponentGeom );
    father->ComponentIOR( Fatherior );

    QString nameShape = QAD_Tools::getFileNameFromPath(file,false) +  QString("_%1").arg(this->myNbGeom++);

    if ( Display ( aShape, strdup(nameShape.latin1())) ) {
      myActiveStudy->setMessage( tr("GEOM_INF_LOADED").arg(QAD_Tools::getFileNameFromPath( file )) );
      myDesktop->putInfo( tr("GEOM_PRP_READY"));
    }
  }
  QApplication::restoreOverrideCursor();
  return true ;
}


//=====================================================================================
// function : Export
// purpose  : BRep, Iges, Step
//=====================================================================================
bool GeometryGUI::Export( )
{
  SALOMEDS::Study_var aStudy = myActiveStudy->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();

  GEOM::GEOM_Shape_var aShape;

  static QString filters[] = { tr("GEOM_MEN_IMPORT_BREP"),
			       tr("GEOM_MEN_IMPORT_IGES"),
			       tr("GEOM_MEN_IMPORT_STEP") };
  
  SALOME_Selection* Sel = SALOME_Selection::Selection( this->myActiveStudy->getSelection() );
  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );

  switch ( GeomGUI->myState )
    {
    case 121 :
      {
	for(;It.More();It.Next()) {
	  QApplication::restoreOverrideCursor();
	  Handle(SALOME_InteractiveObject) IObject = It.Value();
	  Standard_Boolean found;
	  GEOM::GEOM_Shape_var aShape = GeomGUI->ConvertIOinGEOMShape(IObject, found);
	  //	  Handle(GEOM_AISShape) Shape = ConvertIOinGEOMAISShape(IObject, found, true);
	  if ( found ) {
	    QString file = QAD_FileDlg::getFileName(myDesktop, 
						    QString( IObject->getName() ) + ".brep",
						    tr("GEOM_MEN_IMPORT_BREP"),
						    tr("GEOM_MEN_EXPORT"),
						    false);
	    if ( !file.isEmpty() && !aShape->_is_nil() ) {
	      QApplication::setOverrideCursor( Qt::waitCursor );
	      //      Standard_Boolean result = BRepTools::Write(Shape->Shape(), strdup(file.latin1()) );
	      try {
		GeomGUI->myComponentGeom->ExportBREP(strdup( file.latin1()), aShape);
	      }  
	      catch (const SALOME::SALOME_Exception& S_ex) {
		QtCatchCorbaException(S_ex);
	      }
	    }
	  }
	}
     	break;
      }
    case 122 :
      {
	for(;It.More();It.Next()) {
	  QApplication::restoreOverrideCursor();
	  Handle(SALOME_InteractiveObject) IObject = It.Value();
	  Standard_Boolean found;
	  GEOM::GEOM_Shape_var aShape = GeomGUI->ConvertIOinGEOMShape(IObject, found);
	  //	  Handle(GEOM_AISShape) Shape = ConvertIOinGEOMAISShape(IObject, found, true);
	  if ( found ) {
	    QString file = QAD_FileDlg::getFileName(myDesktop, 
						    QString( IObject->getName() ) + ".igs",
						    tr("GEOM_MEN_IMPORT_IGES"),
						    tr("GEOM_MEN_EXPORT"),
						    false);
	    if ( !file.isEmpty() && !aShape->_is_nil() ) {
	      QApplication::setOverrideCursor( Qt::waitCursor );
	      try {
		GeomGUI->myComponentGeom->ExportIGES(strdup( file.latin1()), aShape);
	      }  
	      catch (const SALOME::SALOME_Exception& S_ex) {
		QtCatchCorbaException(S_ex);
	      }
//  	      //VRV: OCC 4.0 migration
//  	      IGESControl_Controller::Init();
//  	      IGESControl_Writer ICW (Interface_Static::CVal("XSTEP.iges.unit"),
//  					 Interface_Static::IVal("XSTEP.iges.writebrep.mode"));
//  	      //VRV: OCC 4.0 migration

//  	      ICW.AddShape (Shape->Shape());
//  	      ICW.ComputeModel();
//  	      Standard_Boolean result = ICW.Write( strdup(file.latin1()) );
	    }
	  }
	}
	break;
      }

    case 123 :
      {
//  	bool test = false ;
//  	IFSelect_ReturnStatus status ;
//  	//VRV: OCC 4.0 migration
//  	STEPControl_Writer aWriter;
//  	//VRV: OCC 4.0 migration
	QString file;

	for( ; It.More(); It.Next() ) {
//  	  GEOM::GEOM_Shape_var aShape = GeomGUI->ConvertIOinGEOMShape(IObject, found);
  	  QApplication::restoreOverrideCursor();
	  Handle(SALOME_InteractiveObject) IObject = It.Value();
	  Standard_Boolean found;
	  GEOM::GEOM_Shape_var aShape = GeomGUI->ConvertIOinGEOMShape(IObject, found);
	  //	  Handle(GEOM_AISShape) Shape = ConvertIOinGEOMAISShape(IObject, found, true);
	  if ( found ) {
	    file = QAD_FileDlg::getFileName(myDesktop, 
					    QString( IObject->getName() ) + ".stp",
					    tr("GEOM_MEN_IMPORT_STEP"),
					    tr("GEOM_MEN_EXPORT"),
					    false);
	    if ( !file.isEmpty() && !aShape->_is_nil() ) {

	      QApplication::setOverrideCursor( Qt::waitCursor ) ;	
	      try {   
		GeomGUI->myComponentGeom->ExportSTEP(strdup( file.latin1()), aShape);   
	      }  
	      catch (const SALOME::SALOME_Exception& S_ex) {
		QtCatchCorbaException(S_ex);
	      }
//  	      //VRV: OCC 4.0 migration
//  	      status = aWriter.Transfer( Shape->Shape(), STEPControl_ManifoldSolidBrep ) ;
//  	      //VRV: OCC 4.0 migration
//  	      test = true ;
//  	      if ( status != IFSelect_RetDone ) {
//  		QApplication::restoreOverrideCursor() ;
//  		return false ;
//  	      }
	    }
	  }
	}
//  	if(test) {
//  	  status = aWriter.Write( strdup(file.latin1()) ) ;
//  	  QApplication::restoreOverrideCursor() ;
//  	  return status ;
//  	}
	break;
      }

    }
  QApplication::restoreOverrideCursor() ;
}


//=====================================================================================
// function : Display()
// purpose  : Displays a CORBA shape
//=====================================================================================
bool GeometryGUI::Display( GEOM::GEOM_Shape_ptr aShape, 
			   Standard_CString name)
{
  // MESSAGE ( "GeometryGUI::Display init ") 
  Handle(GEOM_InteractiveObject) IO;
  SALOME_Selection* Sel = SALOME_Selection::Selection( myActiveStudy->getSelection() );
  Sel->ClearIObjects();

  if( aShape->_is_nil() ) {
    QAD_MessageBox::warn1 ( QAD_Application::getDesktop(),
			    tr ("GEOM_WRN_WARNING"),
			    tr ("GEOM_PRP_ABORT"),
			    tr ("GEOM_BUT_OK") );
    return false ;
  }
  
  TopoDS_Shape shape = ShapeReader.GetShape(myComponentGeom, aShape);

  if ( shape.IsNull() )
    return false;

  SALOMEDS::Study_var            aStudy = myActiveStudy->getStudyDocument();
  SALOMEDS::StudyBuilder_var     aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var    aName;
  SALOMEDS::AttributePixMap_var    aPixmap;

  SALOMEDS::SComponent_var father = aStudy->FindComponent("GEOM");
  int aLocked = false;
  if (father->_is_nil()) {
    QAD_Operation* op = new SALOMEGUI_ImportOperation( myActiveStudy );
    op->start();
    aLocked = aStudy->GetProperties()->IsLocked();
    if (aLocked) aStudy->GetProperties()->SetLocked(false);
    father = aStudyBuilder->NewComponent("GEOM");
    anAttr = aStudyBuilder->FindOrCreateAttribute(father, "AttributeName");
    aName = SALOMEDS::AttributeName::_narrow(anAttr);
    //    aName->SetValue( tr("GEOM_MEN_COMPONENT") );
    aName->SetValue( QAD_Application::getDesktop()->getComponentUserName( "GEOM" ) );
    anAttr = aStudyBuilder->FindOrCreateAttribute(father, "AttributePixMap");
    aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
    aPixmap->SetPixMap( "ICON_OBJBROWSER_Geometry" );
    myActiveStudy->updateObjBrowser();
    if (aLocked) aStudy->GetProperties()->SetLocked(true);
    op->finish();
  }
//    if (aLocked) return false;

  aStudyBuilder->DefineComponentInstance( father, myComponentGeom );
  father->ComponentIOR( Fatherior );

  TCollection_AsciiString nameG("");
  Standard_CString Type;
  if ( TCollection_AsciiString(name).IsEqual(Standard_CString("")) ) {
    if ( TCollection_AsciiString(aShape->NameType()).IsEqual(Standard_CString("")) ) {
      Standard_CString type;
      GetShapeTypeString(shape,type);
      aShape->NameType( type );
      nameG += TCollection_AsciiString( type ) + TCollection_AsciiString("_") + 
	TCollection_AsciiString( GeomGUI->myNbGeom++ ) + TCollection_AsciiString("\0");
    } else
      nameG += TCollection_AsciiString( aShape->NameType()) + TCollection_AsciiString("_") + 
	TCollection_AsciiString( GeomGUI->myNbGeom++ ) + TCollection_AsciiString("\0");
  } else 
    nameG = TCollection_AsciiString(name);
  
  // VTK
  if ( myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
    VTKViewer_RenderWindowInteractor* myRenderInter = ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
    
    vtkRenderer *theRenderer = ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
    int themode = myRenderInter->GetDisplayMode();

    vtkActorCollection* theActors = 
      GEOM_AssemblyBuilder::BuildActors(shape,0,themode,Standard_True);
    theActors->InitTraversal();
    vtkActor* anActor = (vtkActor*)theActors->GetNextActor();

    IO = new GEOM_InteractiveObject(aShape->Name(),
				    Fatherior,
				    "GEOM");
    while(!(anActor==NULL)) {
      GEOM_Actor* GActor = GEOM_Actor::SafeDownCast( anActor );
      GActor->setIO( IO );
      GActor->setName( nameG.ToCString() );
      
      theRenderer->AddActor(GActor);
      anActor = (vtkActor*)theActors->GetNextActor();
    }
  } 
  // OCC
  else if ( myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
    Handle(GEOM_AISShape) theResult = new GEOM_AISShape( shape, nameG.ToCString() );
    theResult->SetShadingColor( myShadingColor );
    IO = new GEOM_InteractiveObject(aShape->Name(),
				    Fatherior,
				    "GEOM");
    theResult->setIO( IO );
    theResult->setName( nameG.ToCString() );
    
    /* Precaution : close any local context to permit the display */
    if ( ic->HasOpenedContext() ) {
      ic->CloseAllContexts();
    }
    
//     if(isInfinite)
//       theResult->SetInfiniteState() ;
    
    ic->Display(theResult);
  }

  Sel->AddIObject( IO, false );
  myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame()->Repaint();
  if ( Settings_AddInStudy )
    GeomGUI->AddInStudy( false, IO );
  
  return true;
}


//=====================================================================================
// function : AddInStudy() 
// purpose  : anIOShape or a selected shape
//=====================================================================================
bool GeometryGUI::AddInStudy( bool selection,
			      const Handle(SALOME_InteractiveObject)& anIO)
{
  SALOME_Selection* Sel = SALOME_Selection::Selection( myActiveStudy->getSelection() );
  if ( !( !Settings_AddInStudy || selection ) ) {
    Sel->ClearIObjects();
  }

  SALOMEDS::Study_var               aStudy = myActiveStudy->getStudyDocument();
  SALOMEDS::StudyBuilder_var        aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::GenericAttribute_var    anAttr;
  SALOMEDS::AttributeName_var       aName;
  SALOMEDS::AttributePixMap_var     aPixmap;
  SALOMEDS::AttributeIOR_var        anIOR;
  SALOMEDS::AttributeSelectable_var aSelAttr;
  
  SALOMEDS::SComponent_var father = aStudy->FindComponent("GEOM");
  int aLocked = false;
  if (father->_is_nil()) {
    QAD_Operation* op = new SALOMEGUI_ImportOperation( myActiveStudy );
    op->start();
    aLocked = aStudy->GetProperties()->IsLocked();
    if (aLocked) aStudy->GetProperties()->SetLocked(false);
    father = aStudyBuilder->NewComponent("GEOM");
    anAttr = aStudyBuilder->FindOrCreateAttribute(father, "AttributeName");
    aName = SALOMEDS::AttributeName::_narrow(anAttr);
    //    aName->SetValue( tr("GEOM_MEN_COMPONENT") );
    aName->SetValue( QAD_Application::getDesktop()->getComponentUserName( "GEOM" ) );
    anAttr = aStudyBuilder->FindOrCreateAttribute(father, "AttributePixMap");
    aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
    aPixmap->SetPixMap( "ICON_OBJBROWSER_Geometry" );
    if (aLocked) aStudy->GetProperties()->SetLocked(true);
    op->finish();
  }
//    if (aLocked) return false;

  aStudyBuilder->DefineComponentInstance( father, myComponentGeom );
  father->ComponentIOR( Fatherior );
  
  SALOMEDS::SObject_var fatherSF = aStudy->FindObjectID(myActiveStudy->getActiveStudyFrame()->entry());
  
  Handle(GEOM_AISShape) GAISShape;
  GEOM_Actor* GActor;
  Handle(GEOM_InteractiveObject) GIO;
  bool found = false;

  // VTK
  if ( myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
    vtkRenderer *Renderer = ((VTKViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();

    vtkActorCollection* theActors = Renderer->GetActors();
    theActors->InitTraversal();
    vtkActor *ac = theActors->GetNextActor();
    while(!(ac==NULL)) {
      if ( ac->IsA("GEOM_Actor") ) {
	GEOM_Actor* anActor = GEOM_Actor::SafeDownCast( ac );
	if ( anActor->hasIO() ) {
	  Handle(SALOME_InteractiveObject) IO = anActor->getIO();
	  if ( IO->IsKind(STANDARD_TYPE(GEOM_InteractiveObject)) ) {
	    GIO = Handle(GEOM_InteractiveObject)::DownCast( IO );
	    if ( anIO->isSame( GIO ) ) {
	      found = true;
	      GActor = anActor;
	      break;
	    }
	  }
	}
      }
      ac = theActors->GetNextActor();
    }
    
    if ( !found )
      return false;
  }
  // OCC
  else if ( myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC )  {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
    
    AIS_ListOfInteractive List;
    ic->DisplayedObjects(List);
    AIS_ListIteratorOfListOfInteractive ite(List);
    while (ite.More()) {
      if (ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	if ( aSh->hasIO() ) {
	  Handle(SALOME_InteractiveObject) IO = aSh->getIO();
	  if ( IO->IsKind(STANDARD_TYPE(GEOM_InteractiveObject)) ) {
	    GIO = Handle(GEOM_InteractiveObject)::DownCast( IO );
	    if ( anIO->isSame( GIO ) ) {
	      found = true;
	      GAISShape = aSh;
	      break;
	    }
	  }
	}
      }
      ite.Next();
    }

    if ( !found )
      return false;
  }

  if ( !Settings_AddInStudy || selection ) {
    QString Name = SALOMEGUI_NameDlg::getName( QAD_Application::getDesktop(), anIO->getName() );
    if ( !Name.isEmpty() ) {
      // VTK
      if ( myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
	GActor->setName( strdup(Name.latin1()) );
      }
      // OCC
      else if ( myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
	GAISShape->setName( strdup(Name.latin1()) );
      }
    } else {
      return false;
    }
  }

  // open transaction
  QAD_Operation* op = new SALOMEGUI_ImportOperation( myActiveStudy );
  op->start();

  SALOMEDS::SObject_var newObj = aStudyBuilder->NewObject(father);

  GEOM::GEOM_Shape_var aShape = myComponentGeom->GetIORFromString( GIO->getIOR() );

  /* For the shape inserted into the study we set its field 'studyshapeid'    */
  /* so the shape will contain its corresponding entry in the study Ocaf doc. */
  aShape->StudyShapeId(newObj->GetID()) ;

  GIO->setEntry(newObj->GetID());

  anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
  anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
  anIOR->SetValue(aShape->Name());

  anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeName");
  aName = SALOMEDS::AttributeName::_narrow(anAttr);

  anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributePixMap");
  aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
  if ( aShape->ShapeType() == GEOM::COMPOUND ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_COMPOUND" );
  } else if ( aShape->ShapeType() == GEOM::COMPSOLID ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_COMPSOLID" );
  } else if ( aShape->ShapeType() == GEOM::SOLID ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_SOLID" );
  } else if ( aShape->ShapeType() == GEOM::SHELL ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_SHELL" );
  } else if ( aShape->ShapeType() == GEOM::FACE ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_FACE" );
  } else if ( aShape->ShapeType() == GEOM::WIRE ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_WIRE" );
  } else if ( aShape->ShapeType() == GEOM::EDGE ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_EDGE" );
  } else if ( aShape->ShapeType() == GEOM::VERTEX ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_VERTEX" );
  }

  // VTK
  if ( myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
    GActor->setIO( GIO );
    aName->SetValue(GActor->getName());
  }
  // OCC
  else if ( myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    GAISShape->setIO( GIO );
    aName->SetValue(GAISShape->getName());
  }

  SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
  aStudyBuilder->Addreference(newObj1,newObj);

  GEOM::GEOM_Gen::ListOfIOR_var listIOR = new GEOM::GEOM_Gen::ListOfIOR;
  listIOR = myComponentGeom->GetReferencedObjects(aShape);

  if (listIOR->length()>0) {
    SALOMEDS::SObject_var Arguments = aStudyBuilder->NewObject(newObj);
    anAttr = aStudyBuilder->FindOrCreateAttribute(Arguments, "AttributeName");
    aName = SALOMEDS::AttributeName::_narrow(anAttr);
    aName->SetValue(tr("GEOM_ARGUMENTS"));
    anAttr = aStudyBuilder->FindOrCreateAttribute(Arguments, "AttributeSelectable");
    aSelAttr = SALOMEDS::AttributeSelectable::_narrow(anAttr);
    aSelAttr->SetSelectable(false);

    bool ObjectReferenced = false;
    for (unsigned int ind = 0; ind < listIOR->length();ind++) {
      SALOMEDS::SObject_var theObj = aStudy->FindObjectIOR(listIOR[ind]);

      if ( !theObj->_is_nil() ) {
	SALOMEDS::SObject_var RefObject = aStudyBuilder->NewObject(Arguments);
	aStudyBuilder->Addreference(RefObject, theObj);
	ObjectReferenced = true;
      }
    }

    if ( !ObjectReferenced )
      aStudyBuilder->RemoveObject(Arguments);
  }

  op->finish();


  if ( !Settings_AddInStudy || selection ) {
    myActiveStudy->updateObjBrowser();
  } else {
    myActiveStudy->updateObjBrowser(false);
    Sel->AddIObject( GIO );
  }

  // MESSAGE ( " highlihght done" )
  return true;
 
}


//=====================================================================================
// function : GetShapeFromIOR()
// purpose  : exist also as static method !
//=====================================================================================
TopoDS_Shape GeometryGUI::GetShapeFromIOR( QString IOR )
{
  TopoDS_Shape result;
  if( IOR.stripWhiteSpace().isEmpty() )
    return result;
  ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
  CORBA::ORB_var& _orb = init.orb() ;
  CORBA::Object_var obj = _orb->string_to_object( (char*)(IOR.latin1()) );
  if ( CORBA::is_nil( obj ) )
    return result;
  GEOM::GEOM_Shape_var aShape = myComponentGeom->GetIORFromString( IOR );
  if (!aShape->_is_nil()) {
    result = ShapeReader.GetShape( myComponentGeom, aShape );
  }
  return result;
}


//=====================================================================================
// function : SetSettings()
// purpose  : [static]
//=====================================================================================
bool GeometryGUI::SetSettings( QAD_Desktop* parent )
{
  /* Create or retrieve an object GeomGUI */
  GeometryGUI::GetOrCreateGeometryGUI(parent);

  //DCQ  parent->menuBar()->setItemEnabled( 504, false);  // CORRECTING
  //DCQ  parent->menuBar()->setItemEnabled( 6062, false); // RADIUS
  //VSR  parent->menuBar()->setItemEnabled( 701, false);  // AUTOMATIC COPY
  parent->menuBar()->setItemEnabled( 313, false);  // SKETCH CONTRAINTS
  parent->menuBar()->setItemEnabled( 3131, false); // SKETCH SET PLAN

  /* Shading Color */
  QString SCr = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorRed");
  QString SCg = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorGreen");
  QString SCb = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorBlue");
  if( !SCr.isEmpty() && !SCg.isEmpty() && !SCb.isEmpty() )   
    GeomGUI->myShadingColor = Quantity_Color ( SCr.toInt()/255., SCg.toInt()/255., SCb.toInt()/255.,
					       Quantity_TOC_RGB );

  /* Wireframe or Shading */
  int DisplayMode = 0;
  if ( parent->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)parent->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
    DisplayMode = ic->DisplayMode();
  } else if ( parent->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
    VTKViewer_RenderWindowInteractor* myRenderInter = ((VTKViewer_ViewFrame*)parent->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
    DisplayMode = myRenderInter->GetDisplayMode();
  }

  if ( DisplayMode == 1 )
    parent->menuBar()->changeItem(6021, tr("GEOM_MEN_WIREFRAME") );
  else
    parent->menuBar()->changeItem(6021, tr("GEOM_MEN_SHADING") );

  /* Copy */
//    QString Copy = QAD_CONFIG->getSetting("Geometry:SettingsCopy");
//    if ( !Copy.isEmpty() ) {
//      Settings_Copy = Copy.toInt();
//      QMenuData* pp;
//      parent->menuBar()->findItem(701,&pp);
//      pp->setItemChecked(701, Settings_Copy);
//    }
 QMenuData* pp;
 if ( parent->menuBar()->findItem(701,&pp) )
   pp->removeItem(701);

  /* Add in Study */
  QString AddInStudy = QAD_CONFIG->getSetting("Geometry:SettingsAddInStudy");
  if ( !AddInStudy.isEmpty() )
    Settings_AddInStudy	 = AddInStudy.toInt();
  else
    Settings_AddInStudy = 1;
  parent->menuBar()->findItem(702,&pp);
  pp->setItemChecked(702, Settings_AddInStudy);

  /* step value */
  QString S = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" );
  if( S.isEmpty() )
    QAD_CONFIG->addSetting( "Geometry:SettingsGeomStep", "100" );
  
  /* isos */
  QAD_Study* ActiveStudy = parent->getActiveStudy();
  int count = ActiveStudy->getStudyFramesCount();

  bool ViewOCC = false;
  for ( int i = 0; i < count; i++ ) {
    if ( ActiveStudy->getStudyFrame(i)->getTypeView() == VIEW_OCC ) {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)ActiveStudy->getStudyFrame(i)->getRightFrame()->getViewFrame())->getViewer();
      Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
      
      QString IsoU = QAD_CONFIG->getSetting("Geometry:SettingsIsoU");
      QString IsoV = QAD_CONFIG->getSetting("Geometry:SettingsIsoV");
      if ( !IsoU.isEmpty() )
	ic->DefaultDrawer()->UIsoAspect()->SetNumber(IsoU.toInt());
      if ( !IsoV.isEmpty() )
	ic->DefaultDrawer()->VIsoAspect()->SetNumber(IsoV.toInt());

      ViewOCC = true;
    }
  }

  QMenuBar* Mb = GeomGUI->myDesktop->getMainMenuBar();

  Mb->setItemEnabled( 312, ViewOCC); //Sketch
  Mb->setItemEnabled( 309, ViewOCC); //SuppressFace
  Mb->setItemEnabled( 314, ViewOCC); //SuppressHole
  
  Mb->setItemEnabled( 703, ViewOCC);// ShadingColor Settings
  Mb->setItemEnabled( 704, ViewOCC);// Isos Settings

  return true;
}


//=====================================================================================
// function : DefinePopup()
// purpose  : [static]
//=====================================================================================
void GeometryGUI::DefinePopup( QString & theContext, QString & theParent, QString & theObject )
{
  /* Create or retrieve an object GeomGUI */
  GeometryGUI::GetOrCreateGeometryGUI(QAD_Application::getDesktop());
  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );

  theObject = "";
  if ( Sel->IObjectCount() == 1 ) 
    {
      Handle(SALOME_InteractiveObject) IO = Sel->firstIObject();
      if (IO->hasEntry())
	{
	  SALOMEDS::SObject_var sobj = GeomGUI->myActiveStudy->getStudyDocument()->FindObjectID(IO->getEntry());
	  if (!sobj->_is_nil()) 
	    {
	      SALOMEDS::SComponent_var scomp = sobj->GetFatherComponent();
	      if (strcmp(scomp->GetID(), IO->getEntry()) == 0) 
		{ 
		  // component is selected
		  theObject = "Component";
		}
	    }
	}
    }

  if ( ( theParent.compare("Viewer")==0 ) ) 
    {
      if ( GeomGUI->myState == CURRENT_SKETCH ) 
	theContext = "Sketch";
      else 
	{
	  if ( Sel->IObjectCount() > 0 )
	    theContext = "";
	  else
	    theContext = "NothingSelected";
	}
    } 
  else 
    theContext = "";
  
}

//=====================================================================================
// function : CustomPopup()
// purpose  : [static]
//=====================================================================================
bool GeometryGUI::CustomPopup( QAD_Desktop* parent,
			       QPopupMenu* popup,
			       const QString& theContext,
			       const QString& theParent,
			       const QString& theObject )
{
  /* Create or retrieve an object GeomGUI */
  GeometryGUI::GetOrCreateGeometryGUI(parent);
  
  /* Deactivate any non modal dialog box to get the neutral point */
  GeomGUI->EmitSignalDeactivateDialog() ;
  
  SALOME_Selection* Sel = SALOME_Selection::Selection( GeomGUI->myActiveStudy->getSelection() );
  int nbSel = Sel->IObjectCount();
  
  if ( (nbSel == 0) && ( theContext.compare("Sketch")!=0 ) )
    return false;
  
    
  if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC)
    {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->
				 getRightFrame()->getViewFrame())->getViewer();
      Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
      if ( theParent.compare("Viewer")==0 )
	{
	  if ( theContext.compare("Sketch")==0 )
	    {
	      SketchStatus myCS = GeomGUI->mySketcher.GetCurrentStatus();
	      popup->setCheckable(TRUE);
	      if (myCS==SEGMENT) 
		{
		  popup->setItemChecked(10000,true);
		  popup->setItemChecked(10001,false);
		}
	      else if (myCS==ARC_CHORD)
		{
		  popup->setItemChecked(10000,false);
		  popup->setItemChecked(10001,true);
		}
	      
	      return true;
	    }
	  else
	    if (theObject.compare("Component") == 0)
	      {
		popup->removeItem(QAD_DisplayOnly_Popup_ID);
		return true;
	      }
	    else
	      {
		QFont f = QApplication::font();
		f.setBold( TRUE );
		if (nbSel==1)
		  {
		    Handle(SALOME_InteractiveObject) IObject = Sel->firstIObject();
		    popup->removeItem(QAD_TopLabel_Popup_ID);
		    popup->insertItem( new CustomItem ( QString(IObject->getName()), f ), QAD_TopLabel_Popup_ID, 0 );
		    if ( IObject->hasEntry() )
		      popup->setItemEnabled( 801, false );
		    else
		      popup->setItemEnabled( 801, true );
		    
		    if (IObject->IsKind(STANDARD_TYPE(GEOM_InteractiveObject)))
		      {
			Standard_Boolean found;
			Handle(GEOM_AISShape) Result = GeomGUI->ConvertIOinGEOMAISShape(IObject, found, true);

			if ( found )
			  {
			    if ( Result->DisplayMode() == 1 )
			      popup->changeItem(8021, tr("GEOM_MEN_WIREFRAME") );
			    else
			      popup->changeItem(8021, tr("GEOM_MEN_SHADING") );
			  }
		      }

		    if ( !(v3d->isInViewer( IObject ) && v3d->isVisible( IObject )) )
		      popup->removeItem(QAD_Erase_Popup_ID);
		    else
		      popup->removeItem(QAD_Display_Popup_ID);
		    
		  }
		else
		  {
		    popup->removeItem(QAD_DisplayOnly_Popup_ID);
		    popup->removeItem(QAD_TopLabel_Popup_ID);
		    popup->insertItem( new CustomItem ( tr("GEOM_MEN_POPUP_NAME").arg(nbSel), f ),
				      QAD_TopLabel_Popup_ID, 0 );
		    popup->setItemEnabled( 801, false );
		  }
	      }
	  return true;
	}
      else
	if ( theParent.compare("ObjectBrowser")==0 )
	  {
	    popup->removeItem(QAD_TopLabel_Popup_ID);
	    int id = popup->idAt(0); // separator
	    if (id < 0) popup->removeItem(id);
	    
	    // checking for GEOM label in the selected list
	    SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	    Handle(SALOME_InteractiveObject) anIObject;

	    bool useSubItems = false;
	    bool needOpen = false;
	    bool needDisplay = false;
	    bool needErase = false;
	    SALOMEDS::GenericAttribute_var aTmpAttr;
	    for(;It.More();It.Next())
	      {
		anIObject = It.Value();
		if (!anIObject->hasEntry())
		    continue;
		
		if (v3d->isInViewer(anIObject) && v3d->isVisible(anIObject)) 
		  needErase = true; 
		else 
		  needDisplay = true;
		SALOMEDS::SObject_var obj =
		  GeomGUI->myActiveStudy->getStudyDocument()->FindObjectID( anIObject->getEntry() );
		if (!obj->_is_nil())
		  {
		    GEOM::GEOM_Shape_var aShape;
		    if (obj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		      char *str = SALOMEDS::AttributeIOR::_narrow(aTmpAttr)->Value();
		      if (str && strlen(str))
			aShape = GeomGUI->myComponentGeom-> GetIORFromString(str);
		    }
		    else
		      if (obj->FindAttribute(aTmpAttr, "AttributePersistentRef"))
			needOpen = true;
		    if (aShape->_is_nil())
		      {
			SALOMEDS::ChildIterator_var anIter = GeomGUI->myActiveStudy->
			  getStudyDocument()->NewChildIterator(obj);
			while (anIter->More())
			  {
			    SALOMEDS::SObject_var subobj = anIter->Value();
			    if (subobj->FindAttribute(aTmpAttr, "AttributeIOR"))
			      {
				useSubItems = true;
				needErase = true;
				needDisplay = true;
			      }
			    else
			      if (subobj->FindAttribute(aTmpAttr, "AttributePersistentRef"))
				{
				  needOpen = true;
				  useSubItems = true;
				}
			    anIter->Next();
			  }
		      }
		  }
	      }
	    // remove useless popup items
	    if (nbSel != 1) popup->removeItem(901); // rename
	    if (useSubItems) popup->removeItem(QAD_DisplayOnly_Popup_ID); // display only
	    if (!needOpen)
	      {
		int index = popup->indexOf(9024);
		popup->removeItem(9024); // open
		popup->removeItemAt(index); // separator under Open

		if (!needDisplay) popup->removeItem(QAD_Display_Popup_ID);
		if (!needErase) popup->removeItem(QAD_Erase_Popup_ID);
		if (!needDisplay && !needErase)
		  {
		    int id = popup->idAt(popup->count()-1); // last item
		    popup->removeItem(id); // separator
		  }
	      }
	    else
	      {
		popup->removeItem(QAD_DisplayOnly_Popup_ID); // display only
		popup->removeItem(QAD_Display_Popup_ID);
		popup->removeItem(QAD_Erase_Popup_ID);
		if (nbSel!=1)
		  {
		    int id = popup->idAt(popup->count()-1); // last item
		    popup->removeItem(id); // separator
		  }
	      }
	    return true;
	  }
      // MESSAGE ( " CUSTOM POPUP VIEWER OCC done. ")
    }
  else
    if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK)
      {
	// MESSAGE ( " CUSTOM POPUP VIEWER VTK ")
	if ( ( theParent.compare("Viewer")==0 ) )
	  {
	    
	    popup->setItemEnabled( 8033, false );
	    if (theObject.compare("Component") == 0)
	      {
		popup->removeItem(QAD_DisplayOnly_Popup_ID);
		return true;
	      } 

	  //int id = popup->idAt(0);
	  QFont f = QApplication::font();
	  f.setBold( TRUE );
	  
	  if (nbSel==1)
	    {
	      Handle(SALOME_InteractiveObject) IObject = Sel->firstIObject();
	      popup->removeItem(QAD_TopLabel_Popup_ID);
	      popup->insertItem( new CustomItem ( QString(IObject->getName()), f ), QAD_TopLabel_Popup_ID, 0 );
	      if ( IObject->hasEntry() )
		{
		  popup->setItemEnabled( 801, false );
		  SALOMEDS::Study_var aStudy = GeomGUI->myActiveStudy->getStudyDocument();
		  SALOMEDS::SObject_var SO = aStudy->FindObjectID( IObject->getEntry() );
		  SALOMEDS::GenericAttribute_var anAttr;
		  SALOMEDS::AttributeIOR_var     anIOR;
		  if ( !SO->_is_nil() )
		    {
		      if (SO->FindAttribute(anAttr, "AttributeIOR") )
			{
			  anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
			  Standard_Boolean found;
			  GEOM_Actor* Result = GeomGUI->ConvertIORinGEOMActor(anIOR->Value(), found, true);
			  if ( found )
			    {
			      if ( Result->getDisplayMode() == 1 )
				popup->changeItem(8021, "Wireframe" );
			      else
				popup->changeItem(8021, "Shading" );
			      
			      if ( !Result->GetVisibility() )
				popup->removeItem(QAD_Erase_Popup_ID);
			      else
				popup->removeItem(QAD_Display_Popup_ID);		  
			    }
			  else
			    popup->removeItem(QAD_Erase_Popup_ID);
			}
		    }
		}
	      else
		popup->setItemEnabled( 801, true );
	    }
	  else
	    {
	      popup->removeItem(QAD_DisplayOnly_Popup_ID);
	      popup->removeItem(QAD_TopLabel_Popup_ID);
	      popup->insertItem( new CustomItem ( tr("GEOM_MEN_POPUP_NAME").arg(nbSel), f ), QAD_TopLabel_Popup_ID, 0 );
	      popup->setItemEnabled( 801, false );
	    }
	  return true;
	}
	else
	  if ( theParent.compare("ObjectBrowser")==0 )
	    {
	      popup->removeItem(QAD_TopLabel_Popup_ID);
	      int id = popup->idAt(0); // separator
	      if (id < 0) popup->removeItem(id);
      
	      // checking for GEOM label in the selected list
	      SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
	      Handle(SALOME_InteractiveObject) anIObject;
	      
	      bool useSubItems = false;
	      bool needOpen = false;
	      bool needDisplay = false;
	      bool needErase = false;
	      SALOMEDS::GenericAttribute_var aTmpAttr;
	      VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->
					       getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
	      for(;It.More();It.Next())
		{
		  anIObject = It.Value();
		  if ( !anIObject->hasEntry() )
		    continue;
		  
		  if (myRenderInter->isInViewer(anIObject) && myRenderInter->isVisible(anIObject)) 
		    needErase = true; 
		  else 
		    needDisplay = true;

		  SALOMEDS::SObject_var obj = GeomGUI->myActiveStudy->getStudyDocument()
		    ->FindObjectID( anIObject->getEntry() );
		  if (!obj->_is_nil()) {
		    GEOM::GEOM_Shape_var aShape;
		    if (obj->FindAttribute(aTmpAttr, "AttributeIOR")){
		      char *str = SALOMEDS::AttributeIOR::_narrow(aTmpAttr)->Value();
		      if (str && strlen(str))
			aShape = GeomGUI->myComponentGeom->GetIORFromString(str);
		    }
		    else
		      if (obj->FindAttribute(aTmpAttr, "AttributePersistentRef"))
			needOpen = true;
		    if (aShape->_is_nil())
		      {
			SALOMEDS::ChildIterator_var anIter =
			  GeomGUI->myActiveStudy->getStudyDocument()->NewChildIterator(obj);
			while (anIter->More())
			  {
			    SALOMEDS::SObject_var subobj = anIter->Value();
			    if (subobj->FindAttribute(aTmpAttr, "AttributeIOR"))
			      {
				useSubItems = true;
				needDisplay = true;
				needErase = true;
			      }
			    else
			      if (subobj->FindAttribute(aTmpAttr, "AttributePersistentRef"))
				{
				  needOpen = true;
				  useSubItems = true;
				}
			    anIter->Next();
			  }
		      }
		  }
		}
	      // remove useless popup items
	      if (nbSel != 1) popup->removeItem(901); // rename
	      if (useSubItems) popup->removeItem(QAD_DisplayOnly_Popup_ID); // display only
	      if (!needOpen)
		{
		  int index = popup->indexOf(9024);
		  popup->removeItem(9024); // open
		  popup->removeItemAt(index); // separator under Open
		  
		  if (!needDisplay) popup->removeItem(QAD_Display_Popup_ID);
		  if (!needErase) popup->removeItem(QAD_Erase_Popup_ID);
		  if (!needDisplay && !needErase)
		    {
		      int id = popup->idAt(popup->count()-1); // last item
		      popup->removeItem(id); // separator
		    }
		}
	      else
		{
		  popup->removeItem(QAD_DisplayOnly_Popup_ID); // display only
		  popup->removeItem(QAD_Display_Popup_ID);
		  popup->removeItem(QAD_Erase_Popup_ID);
		  if (nbSel!=1)
		    {
		      int id = popup->idAt(popup->count()-1); // last item
		      popup->removeItem(id); // separator
		    }
		}
	      return true;
	      // MESSAGE ( " CUSTOM POPUP VIEWER VTK done.")
	    }
      } else
	{ // other viewer types not supported.
	  while (1)
	    {
	      int id = popup->idAt(0);
	      if (id <= QAD_TopLabel_Popup_ID && id != -1)
		popup->removeItemAt(0);
	      else
		break;
	    }
	  popup->removeItem(QAD_DisplayOnly_Popup_ID);	
	  popup->removeItem(QAD_Display_Popup_ID);
	  popup->removeItem(QAD_Erase_Popup_ID);
	  int id = popup->idAt(popup->count()-1); // last item
	  if (id < 0 && id != -1) popup->removeItem(id); // separator
	  return false;
	}
  return false;
}

void GeometryGUI::BuildPresentation(const Handle(SALOME_InteractiveObject)& theIO)
{
  /* Create or retrieve an object GeomGUI */
  GeometryGUI::GetOrCreateGeometryGUI(QAD_Application::getDesktop());
  
  SALOMEDS::Study_var aStudy = GeomGUI->myActiveStudy->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  
  OCCViewer_Viewer3d* v3d;
  Handle(AIS_InteractiveContext) ic;
  vtkRenderer* Renderer;

  if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    v3d = ((OCCViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    ic = v3d->getAISContext();
  } else if ( GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
    Renderer = ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
  } else
    return;

  if (theIO.IsNull())
    MESSAGE("BuildPresentation(): null SALOME_InteractiveObject passed")

  if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
    // VTK
	  
    SALOMEDS::SObject_var fatherSF = 
      aStudy->FindObjectID(GeomGUI->myActiveStudy->getActiveStudyFrame()->entry());
	    
    SALOMEDS::SObject_var obj = aStudy->FindObjectID( theIO->getEntry() );

    VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();

    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeName_var    aName;
    SALOMEDS::AttributeIOR_var     anIOR;
	 
    if(myRenderInter->isInViewer(theIO)) {
      myRenderInter->Display(theIO, false);
    }
    else {
      // Create new actor
      if ( !obj->_is_nil() ) {
	if ( obj->FindAttribute(anAttr, "AttributeIOR")) {
	  // this SObject may be GEOM module root SObject

	  bool useSubItems = false;
	  SALOMEDS::ChildIterator_var anIter = GeomGUI->myActiveStudy->getStudyDocument()->NewChildIterator(obj);
	  if (GeomGUI->myComponentGeom->GetIORFromString(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value())->_is_nil()) {
	    while (anIter->More() && !useSubItems) {
	      SALOMEDS::SObject_var subobj = anIter->Value();
	      SALOMEDS::GenericAttribute_var aTmpAttr;
	      if (subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		if (!GeomGUI->myComponentGeom->GetIORFromString(SALOMEDS::AttributeIOR::_narrow(aTmpAttr)->Value())->_is_nil()) {
		  anAttr = aTmpAttr;
		  obj = subobj;
		  useSubItems = true;
		} else anIter->Next();
	      } else anIter->Next();
	    }
	  }

	  while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
	    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	    GEOM::GEOM_Shape_var aShape = GeomGUI->myComponentGeom->GetIORFromString(anIOR->Value());
	    TopoDS_Shape Shape = ShapeReader.GetShape(GeomGUI->myComponentGeom,aShape);
	    
	    if (obj->FindAttribute(anAttr, "AttributeName")) {
	      aName = SALOMEDS::AttributeName::_narrow(anAttr);
	      
	      vtkRenderer* theRenderer = ((VTKViewer_ViewFrame*)GeomGUI->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
	      vtkActorCollection* theAllActors = theRenderer->GetActors();
	      theAllActors->InitTraversal();
	      vtkActor* actor = (vtkActor*)theAllActors->GetNextActor();
	      Handle(SALOME_InteractiveObject) anIObj;
	      // don't create new study object if it already exists
	      bool isDisplayed = false;
	      while(!(actor==NULL)) {
		SALOME_Actor* Gactor = SALOME_Actor::SafeDownCast(actor);
		if (Gactor!=NULL) {
		  if (Gactor->hasIO()) {
		    if (strcmp(Gactor->getIO()->getEntry(),obj->GetID())==0) {
		      isDisplayed = true;
		      anIObj = Gactor->getIO();
		      if (!anIObj.IsNull()) myRenderInter->Display(anIObj, false);
		    }
		  }
		}
		actor=(vtkActor*)(theAllActors->GetNextActor());
	      }
	      if (!isDisplayed) {
		// open transaction
		QAD_Operation* op = new SALOMEGUI_ImportOperation( GeomGUI->myActiveStudy );
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
		  GEOM_Actor* GActor = GEOM_Actor::SafeDownCast( anActor );
		  Handle(GEOM_InteractiveObject) IO = new GEOM_InteractiveObject(anIOR->Value(), Fatherior,"GEOM");
		  IO->setEntry(obj->GetID());
		  GActor->setIO( IO );
		  GActor->setName( theIO->getName() );
		  
		  theRenderer->AddActor(GActor);
		  //		  renWin->Render();
		  anActor = (vtkActor*)theActors->GetNextActor();
		}
	      }
	    }
	    // next item iteration
	    if (useSubItems) {
	      anIter->Next();
	      anAttr = SALOMEDS::GenericAttribute::_nil();
	      while (anIter->More() && anAttr->_is_nil()) {
		SALOMEDS::SObject_var subobject = anIter->Value();
		SALOMEDS::GenericAttribute_var aTmpAttribute;
		if (subobject->FindAttribute(aTmpAttribute, "AttributeIOR")) {
		  anAttr = aTmpAttribute;
		  obj = subobject;
		} else anIter->Next();
	      }
	    } else anAttr = SALOMEDS::GenericAttribute::_nil();
	  }
	}
      }
    }
    // No viewer update should be done here!
    //myRenderInter->Render();
    //GeomGUI->myActiveStudy->updateObjBrowser( true );
  } else if (GeomGUI->myActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    SALOMEDS::SObject_var fatherSF = 
      aStudy->FindObjectID( GeomGUI->myActiveStudy->getActiveStudyFrame()->entry());
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeName_var    aName;
    SALOMEDS::AttributeIOR_var     anIOR;

    if ( v3d->isInViewer( theIO, true ) ) {
      Standard_Boolean found;
      Handle(GEOM_AISShape) aSh = GeomGUI->ConvertIOinGEOMAISShape( theIO, found, true );
      if ( found ) {
	ic->Display(aSh, false);
	ic->AddOrRemoveCurrentObject(aSh, false);
      }
      
    } else {
      SALOMEDS::SObject_var obj = aStudy->FindObjectID( theIO->getEntry() );	      
      if ( !obj->_is_nil() ) {
	MESSAGE("BuildPresentation(): SObject not null")
	if ( obj->FindAttribute(anAttr, "AttributeIOR")) {
	  MESSAGE("BuildPresentation(): SObject has IOR")
	  // this SObject may be GEOM module root SObject

	  bool useSubItems = false;
	  SALOMEDS::ChildIterator_var anIter = GeomGUI->myActiveStudy->getStudyDocument()->NewChildIterator(obj);
	  if (GeomGUI->myComponentGeom->GetIORFromString(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value())->_is_nil()) {
	    while (anIter->More() && !useSubItems) {
	      SALOMEDS::SObject_var subobj = anIter->Value();
	      SALOMEDS::GenericAttribute_var aTmpAttr;
	      if (subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		if (!GeomGUI->myComponentGeom->GetIORFromString(SALOMEDS::AttributeIOR::_narrow(aTmpAttr)->Value())->_is_nil()) {
		  anAttr = aTmpAttr;
		  obj = subobj;
		  useSubItems = true;
		} else anIter->Next();
	      } else anIter->Next();
	    }
	  }

	  while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
	    anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	    GEOM::GEOM_Shape_var aShape = GeomGUI->myComponentGeom->GetIORFromString(anIOR->Value());
	    TopoDS_Shape Shape = ShapeReader.GetShape(GeomGUI->myComponentGeom,aShape);
	    if (Shape.IsNull())
	      MESSAGE("BuildPresentation(): TopoDS_Shape is null!")
	    if (obj->FindAttribute(anAttr, "AttributeName")) {
	      MESSAGE("BuildPresentation(): SObject has Name")
	      aName = SALOMEDS::AttributeName::_narrow(anAttr);
	      // searchin for already displayed objects with the same shape
	      AIS_ListOfInteractive aDisplayed;
	      ic->DisplayedObjects(aDisplayed);
	      AIS_ListIteratorOfListOfInteractive anIObjects(aDisplayed);
	      Handle(AIS_Shape) anAISShape;
	      for(;anIObjects.More();anIObjects.Next()) {
		anAISShape = Handle(AIS_Shape)::DownCast(anIObjects.Value());
		if (!anAISShape.IsNull()) {
		  if (anAISShape->Shape().IsSame(Shape)) break;
		  anAISShape.Nullify();
		}
	      }
	      if (!anAISShape.IsNull()) {
		if (!ic->IsDisplayed(anAISShape)) ic->Display(anAISShape, false);
	      } else {
		if (!useSubItems) {
		  // open transaction
		  QAD_Operation* op = new SALOMEGUI_ImportOperation( GeomGUI->myActiveStudy );
		  op->start();
		  if (fatherSF->_is_nil())
		    MESSAGE("BuildPresentation(): fatherSF is nil!")
		  SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
		  aStudyBuilder->Addreference(newObj1, obj);
		  // commit transaction
		  op->finish();
		}
		Handle(GEOM_AISShape) aSh = new GEOM_AISShape(Shape, aName->Value());
		aSh->SetShadingColor( GeomGUI->myShadingColor );
		Handle(GEOM_InteractiveObject) IO = new GEOM_InteractiveObject(anIOR->Value(),
									       Fatherior,
									       "GEOM");
		IO->setEntry(obj->GetID());
		aSh->setIO( IO );
		aSh->setName( aName->Value() );
		ic->Display (aSh, false);
		if (!useSubItems) ic->AddOrRemoveCurrentObject(aSh, false);
	      }
	    }
	    // next item iteration
	    if (useSubItems) {
	      anIter->Next();
	      anAttr=SALOMEDS::GenericAttribute::_nil();
	      while (anIter->More() && anAttr->_is_nil()) {
		SALOMEDS::SObject_var subobject = anIter->Value();
		SALOMEDS::GenericAttribute_var aTmpAttribute;
		if (subobject->FindAttribute(aTmpAttribute, "AttributeIOR")) {
		  anAttr = aTmpAttribute;
		  obj = subobject;
		} else anIter->Next();
	      }
	    } else 
	      anAttr = SALOMEDS::GenericAttribute::_nil();
	  }
	}
      }
    }
    // No viewer update should be done here!
    //GeomGUI->myActiveStudy->updateObjBrowser( true );
    //ic->UpdateCurrentViewer();
  }
}

//=======================================================================
// function : Parameter()
// purpose  : return a parameter (float) from a dialog box
//
//  avalue1    : is a float or integer used as a default value displayed
//  aTitle1    : is the title for aValue1
//  aTitle     : is the main title
//  bottom     : maximum value to be entered
//  top	       : minimum value to be entered
//  decimals   : number of decimals
//=======================================================================
double GeometryGUI::Parameter( Standard_Boolean& res,
			       const char* aValue1,
			       const char* aTitle1,
			       const char* aTitle,
			       const double bottom,
			       const double top,
			       const int decimals )
{
  GeometryGUI_aParameterDlg * Dialog =
    new GeometryGUI_aParameterDlg(aValue1,
				  aTitle1,
				  QAD_Application::getDesktop(),
				  aTitle,
				  TRUE,
				  0,
				  bottom,
				  top,
				  decimals) ;
  int r = Dialog->exec() ;
  float X = 0.0 ;
  if ( r == QDialog::Accepted ) {
    res = Standard_True;
    X = Dialog->getValue();
  } else
    res = Standard_False;
  delete Dialog;
  return X;
}


//=======================================================================
// function : OnSketchSegment()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchSegment()
{
  this->mySketcher.ChangeMode(SEGMENT);
}


//=======================================================================
// function : OnSketchArc()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchArc()
{
  this->mySketcher.ChangeMode(ARC_CHORD);
}

//=======================================================================
// function : OnSketchSetAngle()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchSetAngle()
{
  Standard_Real anAngle = this->mySketcher.GetSegmentAngle()/PI180;
  Sketch::fitInResol(anAngle); 
  Standard_Boolean res = false ;
  QString Value = QString("%1").arg( anAngle );
  anAngle = Parameter(res, Value, tr("GEOM_MEN_ANGLE"), tr("GEOM_MEN_ENTER_ANGLE"), -180.0, +180.0, 6 )*PI180 ;

  if( res ) {
    this->mySketcher.SetSegmentAngle(anAngle);
    QMenuBar* Mb = this->myDesktop->getMainMenuBar();
    QMenuData* pp;
    QMenuItem* item = Mb->findItem(3133,&pp);
    pp->setItemChecked(3133,false);
    item = Mb->findItem(3134,&pp);
    pp->setItemChecked(3134,false);
  }
  
}


//=======================================================================
// function : OnSketchSetx()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchSetx()
{
  Standard_Boolean res = false;
  double X = Parameter( res,
  			"0.",
  			tr("GEOM_MEN_X"),
  			tr("GEOM_MEN_SKETCHER_X"),
  			2.0 * Precision::Confusion(),
  			1E6,
  			6 ) ;
  if (res)
    this->mySketcher.SetXDimension(X);
  QMenuBar* Mb = this->myDesktop->getMainMenuBar();
  QMenuData* pp;
  QMenuItem* item = Mb->findItem(3133,&pp);
  pp->setItemChecked(3133,false);
  item = Mb->findItem(3134,&pp);
  pp->setItemChecked(3134,false);
}

//=======================================================================
// function : OnSketchSety()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchSety()
{
  Standard_Boolean res = false;
  double Y = Parameter( res,
  			"0.",
  			tr("GEOM_MEN_Y"),
  			tr("GEOM_MEN_SKETCHER_Y"),
  			2.0 * Precision::Confusion(),
  			1E6,
  			6 ) ;
  if (res)
    this->mySketcher.SetYDimension(Y);
  QMenuBar* Mb = this->myDesktop->getMainMenuBar();
  QMenuData* pp;
  QMenuItem* item = Mb->findItem(3133,&pp);
  pp->setItemChecked(3133,false);
  item = Mb->findItem(3134,&pp);
  pp->setItemChecked(3134,false);
}


//=======================================================================
// function : OnSketchDelete()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchDelete()
{
  if (GeomGUI->mySketcher.GetmyEdgesNumber() == 1 ) {
    QMenuBar* Mb = GeomGUI->myDesktop->getMainMenuBar();
    QMenuData* pp;
    QMenuItem* item = Mb->findItem(313,&pp);
    pp->setItemEnabled( 313, false);  // SKETCH CONTRAINTS
    GeomGUI->mySketcher.SetTransitionStatus(NOCONSTRAINT);
  }

  if (this->mySketcher.Delete())
    GeomGUI->ResetState();
}


//=======================================================================
// function : OnSketchClose()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchClose()
{
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)this->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) myContext = v3d->getAISContext();

  TopoDS_Wire W = this->mySketcher.Close();
  if ( !W.IsNull() ) {
    //
    GEOM::GEOM_Gen::ListOfIOR_var listShapes = new GEOM::GEOM_Gen::ListOfIOR;
    listShapes->length(0);
    unsigned int i = 0;

    BRepTools_WireExplorer Ex(W);
    while ( Ex.More() ) {
      TopoDS_Edge E = Ex.Current();
      gp_Pnt pt1 , pt2 ;

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
	Circ   = Circle->Circ();                       // gp_Circ
	
	Curve->D0( (First + Last) / 2. , CenterPoint );

	GEOM::PointStruct pI = myComponentGeom->MakePointStruct( pt1.X(), pt1.Y(), pt1.Z() );
	GEOM::PointStruct pC = myComponentGeom->MakePointStruct( CenterPoint.X(), CenterPoint.Y(), CenterPoint.Z() );
	GEOM::PointStruct pE = myComponentGeom->MakePointStruct( pt2.X(), pt2.Y(), pt2.Z() );

	GEOM::GEOM_Shape_var arc;

	try {
	  arc = myComponentGeom->MakeArc(pI, pC, pE) ;
	}
	catch (const SALOME::SALOME_Exception& S_ex) {
	  QtCatchCorbaException(S_ex);
	}

 	listShapes->length(i+1);
	listShapes[i] = strdup(arc->Name());
	i++;   
     } else {
	GEOM::PointStruct pI = myComponentGeom->MakePointStruct( pt1.X(), pt1.Y(), pt1.Z() );
	GEOM::PointStruct pE = myComponentGeom->MakePointStruct( pt2.X(), pt2.Y(), pt2.Z() );
	GEOM::GEOM_Shape_var segment;

	try {
	  segment = myComponentGeom->MakeEdge(pI,pE) ;
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
    GEOM::GEOM_Shape_var Wire = myComponentGeom->MakeWire(listShapes) ;
    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, Wire);
    Standard_CString type;
    GetShapeTypeString(S,type);
    Wire->NameType( type );

    if ( Display(Wire, "" )) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
    } 
  }
  GeomGUI->ResetState();
  QMenuBar* Mb = this->myDesktop->getMainMenuBar();
  QMenuData* pp;
  QMenuItem* item = Mb->findItem(313,&pp);
  pp->setItemEnabled( 313, false);  // SKETCH CONTRAINTS
  GeomGUI->mySketcher.SetTransitionStatus(NOCONSTRAINT);
}



//=======================================================================
// function : OnSketchEnd()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchEnd()
{
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)this->myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) myContext = v3d->getAISContext();

  TopoDS_Wire W = this->mySketcher.End();
  if (!W.IsNull()) {
    //
    GEOM::GEOM_Gen::ListOfIOR_var listShapes = new GEOM::GEOM_Gen::ListOfIOR;
    listShapes->length(0);
    unsigned int i = 0;

    BRepTools_WireExplorer Ex(W);
    while ( Ex.More() ) {
      TopoDS_Edge E = TopoDS::Edge( Ex.Current() );
      
      gp_Pnt pt1 , pt2 ;
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
	Circ   = Circle->Circ();                       // gp_Circ
	
	Curve->D0( (First + Last) / 2. , CenterPoint );

	GEOM::PointStruct pI = myComponentGeom->MakePointStruct( pt1.X(), pt1.Y(), pt1.Z() );
	GEOM::PointStruct pC = myComponentGeom->MakePointStruct( CenterPoint.X(), CenterPoint.Y(), CenterPoint.Z() );
	GEOM::PointStruct pE = myComponentGeom->MakePointStruct( pt2.X(), pt2.Y(), pt2.Z() );

	GEOM::GEOM_Shape_var arc;

	try {
	  arc = myComponentGeom->MakeArc(pI, pC, pE) ;
	}
	catch (const SALOME::SALOME_Exception& S_ex) {
	  QtCatchCorbaException(S_ex);
	}
		
 	listShapes->length(i+1);
	listShapes[i] = strdup(arc->Name());
	i++;   
      } else {
	GEOM::PointStruct pI = myComponentGeom->MakePointStruct( pt1.X(), pt1.Y(), pt1.Z() );
	GEOM::PointStruct pE = myComponentGeom->MakePointStruct( pt2.X(), pt2.Y(), pt2.Z() );
	GEOM::GEOM_Shape_var segment;
	
	try {
	  segment = myComponentGeom->MakeEdge(pI,pE) ;
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

    GEOM::GEOM_Shape_var Wire = myComponentGeom->MakeWire(listShapes) ;
    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, Wire);
    Standard_CString type;
    GetShapeTypeString(S,type);
    Wire->NameType( type );

    if ( Display(Wire, "") ) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
    }
  }
  GeomGUI->ResetState();
  QMenuBar* Mb = this->myDesktop->getMainMenuBar();
  QMenuData* pp;
  QMenuItem* item = Mb->findItem(313,&pp);
  pp->setItemEnabled( 313, false);  // SKETCH CONTRAINTS
  GeomGUI->mySketcher.SetTransitionStatus(NOCONSTRAINT);
}

//=======================================================================
// function : OnSettingsNoconstraint()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSettingsNoconstraint() 
{
    this->mySketcher.SetTransitionStatus(NOCONSTRAINT);
}

//=======================================================================
// function : OnSettingsPerpendicular()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSettingsPerpendicular() 
{
    this->mySketcher.SetTransitionStatus(PERPENDICULAR);
}

//=======================================================================
// function : OnSettingsTangent()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSettingsTangent() 
{
  this->mySketcher.SetTransitionStatus(TANGENT);
}

//=======================================================================
// function : OnSketchOptionsOnoffangledimension()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchOptionsOnoffangledimension()
{
  QMenuBar* Mb = this->myDesktop->getMainMenuBar();
  QMenuData* pp;
  QMenuItem* item = Mb->findItem(10011,&pp);
  pp->setItemChecked(10011,!pp->isItemChecked(10011));
  this->mySketcher.SetParameterVisibility(ANGLE_PARAMETER,pp->isItemChecked(10011));
}

//=======================================================================
// function : OnSketchOptionsOnofflengthdimension()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchOptionsOnofflengthdimension()
{
  QMenuBar* Mb = this->myDesktop->getMainMenuBar();
  QMenuData* pp;
  QMenuItem* item = Mb->findItem(10010,&pp);
  pp->setItemChecked(10010,!pp->isItemChecked(10010));
  this->mySketcher.SetParameterVisibility(LENGTH_PARAMETER,pp->isItemChecked(10010));
}

//=======================================================================
// function : OnSketchOptionsOnoffradiusdimension()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchOptionsOnoffradiusdimension()
{
  QMenuBar* Mb = this->myDesktop->getMainMenuBar();
  QMenuData* pp;
  QMenuItem* item = Mb->findItem(10012,&pp);
  pp->setItemChecked(10012,!pp->isItemChecked(10012));
  this->mySketcher.SetParameterVisibility(RADIUS_PARAMETER,pp->isItemChecked(10012));
}


//=======================================================================
// function : OnSketchOptionsOnoffxdimension()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchOptionsOnoffxdimension()
{
  QMenuBar* Mb = this->myDesktop->getMainMenuBar();
  QMenuData* pp;
  QMenuItem* item = Mb->findItem(10013,&pp);
  pp->setItemChecked(10013,!pp->isItemChecked(10013));
  this->mySketcher.SetParameterVisibility(XVALUE_PARAMETER,pp->isItemChecked(10013));
}

//=======================================================================
// function : OnSketchOptionsOnoffydimension()
// purpose  : 
//=======================================================================
void GeometryGUI::OnSketchOptionsOnoffydimension()
{
  QMenuBar* Mb = this->myDesktop->getMainMenuBar();
  QMenuData* pp;
  QMenuItem* item = Mb->findItem(10014,&pp);
  pp->setItemChecked(10014,!pp->isItemChecked(10014));
  this->mySketcher.SetParameterVisibility(YVALUE_PARAMETER,pp->isItemChecked(10014));
}


//=======================================================================
// function : Archimede()
// purpose  : 
//=======================================================================
void GeometryGUI::Archimede( const Handle(SALOME_InteractiveObject)& IO, 
			     const double aWeight,
			     const double aWaterDensity,
			     const double aMeshingDeflection )
{
  try {
    if ( IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject)) ) {
      Handle(GEOM_InteractiveObject) GIO =
	Handle(GEOM_InteractiveObject)::DownCast( IO );
      GEOM::GEOM_Shape_var Shape = GeomGUI->myComponentGeom->GetIORFromString( GIO->getIOR() );
      
      GEOM::GEOM_Shape_var Result = GeomGUI->myComponentGeom->Archimede( Shape, aWeight, aWaterDensity, aMeshingDeflection);
      Result->NameType(tr("GEOM_PLANE"));
      if ( Display(Result, "") ) {
	QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
      }
      return;
    }
    if ( IO->hasEntry() ) {
      SALOMEDS::Study_var aStudy = GeomGUI->myActiveStudy->getStudyDocument();
      SALOMEDS::SObject_var obj = aStudy->FindObjectID( IO->getEntry() );
      SALOMEDS::GenericAttribute_var anAttr;
      SALOMEDS::AttributeIOR_var     anIOR;
      if ( !obj->_is_nil() ) {
	if (obj->FindAttribute(anAttr, "AttributeIOR")) {
          anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	  GEOM::GEOM_Shape_var Shape = GeomGUI->myComponentGeom->GetIORFromString( anIOR->Value() );
	  GEOM::GEOM_Shape_var Result = GeomGUI->myComponentGeom->Archimede( Shape, aWeight, aWaterDensity, aMeshingDeflection);
	  Result->NameType(tr("GEOM_PLANE"));
	  if ( Display(Result, "") ) {
	    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
	  }
	}
      }
    }

  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  bool OnGUIEvent(int theCommandID, QAD_Desktop* parent)
  {
    return GeometryGUI::OnGUIEvent(theCommandID, parent);
  }

  bool OnKeyPress (QKeyEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame)
  {
    return GeometryGUI::OnKeyPress (pe, parent, studyFrame);
  }

  bool OnMousePress (QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame)
  {
    return GeometryGUI::OnMousePress (pe, parent, studyFrame);
  }

  bool OnMouseMove (QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame)
  {
    return GeometryGUI::OnMouseMove (pe, parent, studyFrame);
  }

  bool SetSettings ( QAD_Desktop* parent )
  {
    return GeometryGUI::SetSettings( parent );
  }

  bool customPopup ( QAD_Desktop* parent, QPopupMenu* popup, const QString & theContext,
		     const QString & theParent, const QString & theObject )
  {
     return GeometryGUI::CustomPopup( parent, popup, theContext, theParent, theObject );
  }

  void definePopup ( QString & theContext, QString & theParent, QString & theObject )
  {
    GeometryGUI::DefinePopup( theContext, theParent, theObject );
  }
  
  bool activeStudyChanged ( QAD_Desktop* parent )
  {
    GeometryGUI::activeStudyChanged( parent );
  }

  void buildPresentation ( const Handle(SALOME_InteractiveObject)& theIO )
  {
    GeometryGUI::BuildPresentation(theIO);
  }

  void supportedViewType(int* buffer, int bufferSize)
  {
    if (!buffer || !bufferSize) return;
    buffer[0] = (int)VIEW_OCC;
    if (--bufferSize) buffer[1] = (int)VIEW_VTK;
  }
}

//=====================================================================================
// function : OnFilletGetAll()
// purpose  :
//=====================================================================================
bool GeometryGUI::OnFilletGetAll(const TopoDS_Shape& ShapeTopo, const double Radius, const int SubShapeType, const char* ShapeTopoIOR)
{
  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ListOfID->length(0);

  SALOMEDS::Study_var   aStudy = myActiveStudy->getStudyDocument();
  SALOMEDS::SObject_var theObj = aStudy->FindObjectIOR( ShapeTopoIOR );
  if ( theObj->_is_nil() ) {
    myDesktop->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY"));
    return false ;
  }
  
  try {
    if( Radius <= Precision::Confusion() )
      return false;

    GEOM::GEOM_Shape_var aShape  = myComponentGeom->GetIORFromString( ShapeTopoIOR );
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeFillet(aShape, Radius, SubShapeType, ListOfID) ;
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
      return  false;
    }
    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, result);
    Standard_CString type;
    GetShapeTypeString(S,type);
    result->NameType( type );

    if ( Display( result, "" ))
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
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
bool GeometryGUI::OnFilletGetSelected(const TopoDS_Shape& ShapeTopo,
				      const char* ShapeTopoIOR,
				      const double Radius,
				      const int SubShapeType,
				      Standard_Integer& aLocalContextId,
				      bool& myUseLocalContext )
{
  if ( this->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC ) {
    return false;
  }

  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  
  if( myUseLocalContext == false ) {
    /*  No local context opened for fillet method */
    myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
    return false ;
  }
  
  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ic->InitSelected();
  int nbSelected = ic->NbSelected();
  int i = 0;
  ic->InitSelected();

  while(ic->MoreSelected()) {
    TopoDS_Shape s = ic->SelectedShape();
    if ( s.ShapeType() == TopAbs_FACE ) {
      TopExp_Explorer Exp( s, TopAbs_EDGE );
      TopTools_MapOfShape M ;
      while ( Exp.More() ) {
	if( M.Add(Exp.Current() )) { /* if a new edge : do not add doublons indices */
	  ListOfID->length( i + 1 );
	  ListOfID[i] = GetIndex( Exp.Current(), ShapeTopo, (int)TopAbs_EDGE ) ;
	  i++ ;
	}
	Exp.Next(); 
      }	 
    }
    else {
      ListOfID->length( i + 1 );
      ListOfID[i] = GetIndex( ic->SelectedShape(), ShapeTopo, SubShapeType );
      i++;
    }
    ic->NextSelected();
  }

  GEOM::GEOM_Shape_var aShape  = myComponentGeom->GetIORFromString( ShapeTopoIOR );
  GEOM::GEOM_Shape_var aResult ;
  try {
    aResult = myComponentGeom->MakeFillet( aShape, Radius, 6, ListOfID );

    /* local context from DialogBox */
    ic->CloseLocalContext(aLocalContextId) ;
    myUseLocalContext = false ;

    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, aResult);
    Standard_CString type;
    GetShapeTypeString(S,type);
    aResult->NameType( type );

    if ( Display( aResult, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }

  if ( myUseLocalContext ) {
    /* local context from DialogBox */
    ic->CloseLocalContext(aLocalContextId) ;
    myUseLocalContext = false ;
  }

  this->OnDisplayAll(true);
  myActiveStudy->updateObjBrowser();
  myDesktop->putInfo (tr("GEOM_PRP_READY"));

  return true ;  
}

//=====================================================================================
// function : OnChamferGetAll()
// purpose  :
//=====================================================================================
bool GeometryGUI::OnChamferGetAll(const TopoDS_Shape& ShapeTopo, const double D1, const double D2, const int SubShapeType, const char* ShapeTopoIOR)
{
 GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ListOfID->length(0);

  SALOMEDS::Study_var   aStudy = myActiveStudy->getStudyDocument();
  SALOMEDS::SObject_var theObj = aStudy->FindObjectIOR( ShapeTopoIOR );
  if ( theObj->_is_nil() ) {
    myDesktop->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY"));
    return false ;
  }
  
  try {
    if( D1 <= Precision::Confusion() )
      return false;
    if( D2 <= Precision::Confusion() )
      return false;

    GEOM::GEOM_Shape_var aShape  = myComponentGeom->GetIORFromString( ShapeTopoIOR );
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeChamfer(aShape, D1, D2, SubShapeType, ListOfID) ;
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
      return  false;
    }

    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, result);
    Standard_CString type;
    GetShapeTypeString(S,type);
    result->NameType( type );

    if ( Display( result, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
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
bool GeometryGUI::OnChamferGetSelected(const TopoDS_Shape& ShapeTopo,
				       const char* ShapeTopoIOR,
				       const double D1, const double D2, const int SubShapeType,
				       Standard_Integer& aLocalContextId,
				       bool& myUseLocalContext)
{
  if ( this->myActiveStudy->getActiveStudyFrame()->getTypeView() > VIEW_OCC ) {
    return false;
  }
  
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  
  if( myUseLocalContext == false ) {
    /* No local context opened for chamfer method */
    myDesktop->putInfo(tr("GEOM_PRP_ABORT"));
    return false ;
  }
  
  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ic->InitSelected();
  int nbSelected = ic->NbSelected();
  int i = 0;
  ic->InitSelected();

  while(ic->MoreSelected()) {
    TopoDS_Shape s = ic->SelectedShape();
    if ( s.ShapeType() == TopAbs_FACE ) {
      TopExp_Explorer Exp( s, TopAbs_EDGE );
      TopTools_MapOfShape M ;
      while ( Exp.More() ) {
	if( M.Add(Exp.Current() )) { /* if a new edge : do not add doublons indices */
	  ListOfID->length( i + 1 );
	  ListOfID[i] = GetIndex( Exp.Current(), ShapeTopo, (int)TopAbs_EDGE ) ;
	  i++ ;
	}
	Exp.Next(); 
      }	 
    }
    else {
      ListOfID->length( i + 1 );
      ListOfID[i] = GetIndex( ic->SelectedShape(), ShapeTopo, SubShapeType ) ;
      i++;
    }
    ic->NextSelected();
  }
  
  GEOM::GEOM_Shape_var aShape  = myComponentGeom->GetIORFromString( ShapeTopoIOR );
  GEOM::GEOM_Shape_var aResult ;
  try {
    aResult = myComponentGeom->MakeChamfer( aShape, D1, D2, 6, ListOfID );

    /* local context from DialogBox */
    ic->CloseLocalContext(aLocalContextId) ;
    myUseLocalContext = false ;

    TopoDS_Shape S = ShapeReader.GetShape(myComponentGeom, aResult);
    Standard_CString type;
    GetShapeTypeString(S,type);
    aResult->NameType( type );

    if ( Display( aResult, "") )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }

  if ( myUseLocalContext ) {
    /* local context from DialogBox */
    ic->CloseLocalContext(aLocalContextId) ;
    myUseLocalContext = false ;
  }

  this->OnDisplayAll(true);
  myActiveStudy->updateObjBrowser();
  myDesktop->putInfo (tr("GEOM_PRP_READY"));

  return true ;  
}

//=====================================================================================
// function : MakeCDGAndDisplay()
// purpose  :
//=====================================================================================
void GeometryGUI::MakeCDGAndDisplay(GEOM::GEOM_Shape_ptr Shape)
{
  try {
    GEOM::GEOM_Shape_var result = myComponentGeom->MakeCDG( Shape );
    if ( result->_is_nil() ) {
      myDesktop->putInfo(tr("GEOM_PRP_ABORT") );
      return ;
    }
    result->NameType( tr("GEOM_POINT") );
    if ( Display( result ) )
      myDesktop->putInfo(tr("GEOM_PRP_DONE"));
  }  
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return  ;
}

bool GeometryGUI::SObjectExist(SALOMEDS::SObject_ptr theFatherObject, const char* IOR) {
  SALOMEDS::Study_var aStudy = GeomGUI->myActiveStudy->getStudyDocument();
  SALOMEDS::ChildIterator_var it = aStudy->NewChildIterator(theFatherObject);
  SALOMEDS::SObject_var RefSO;
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeIOR_var     anIOR;
  for (; it->More();it->Next()) {
    SALOMEDS::SObject_var SO= it->Value();
    if (SO->FindAttribute(anAttr, "AttributeIOR") ) {
      anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
      if ( strcmp( anIOR->Value(), IOR ) == 0 )
	return true;
    }
    if ( SO->ReferencedObject( RefSO ) ) {
      if (RefSO->FindAttribute(anAttr, "AttributeIOR") ) {
        anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	if ( strcmp( anIOR->Value(), IOR ) == 0 )
	  return true;
      }
    }
  }
  return false;
}
