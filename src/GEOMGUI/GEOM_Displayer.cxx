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
//  File   : GEOM_Displayer.cxx
//  Author : Vadim SANDLER
//  Module : GEOM
//  $Header$

#include "GEOM_Displayer.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GEOM_TypeFilter.hxx"
#include "QAD_Desktop.h"
#include "QAD_Application.h"
#include "QAD_Study.h"
#include "QAD_StudyFrame.h"
#include "QAD_RightFrame.h"
#include "QAD_ViewFrame.h"
#include "QAD_Config.h"
#include "SALOME_ListIteratorOfListIO.hxx"
#include "OCCViewer_Prs.h"
#include "OCCViewer_ViewFrame.h"
#include "OCCViewer_Viewer3d.h"
#include "VTKViewer_Prs.h"
#include "VTKViewer_ViewFrame.h"
#include "GEOMImpl_Types.hxx"
#include "TColStd_MapOfInteger.hxx"
#include "TColStd_MapIteratorOfMapOfInteger.hxx"
#include "GEOM_EdgeFilter.hxx"
#include "GEOM_FaceFilter.hxx"
#include "GEOM_PreviewFilter.hxx"
#include "GEOM_LogicalFilter.hxx"
#include "GEOM_AssemblyBuilder.h"
#include "GEOM_Actor.h"
#include "GEOM_AISTrihedron.hxx"
#include "GEOM_VTKTrihedron.hxx"

#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)

#include "GeometryGUI.h"
#include "GEOM_AISShape.hxx"
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_Drawer.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Geom_Plane.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pln.hxx>
#include <Geom_Axis2Placement.hxx>

#include <vtkActorCollection.h>

#include <cstring>

using namespace std;

//================================================================
// Function : getActiveStudy
// Purpose  : Get active study, returns 0 if no open study frame
//================================================================
static inline QAD_Study* getActiveStudy()
{
  QAD_Desktop* desktop = QAD_Application::getDesktop();
  if ( desktop ) {
    QAD_Application* app = desktop->getActiveApp();
    if ( app )
      return app->getActiveStudy();
  }
  return 0;
}

//================================================================
// Function : getTopAbsMode
// Purpose  : Get TopAbs_ShapeEnum value corresponding to the
//            one from GEOMImpl_Types.h
//================================================================
static inline int getTopAbsMode( const int implType )
{
  switch ( implType )
  {
    case GEOM_COMPOUND  : return TopAbs_COMPOUND;
    case GEOM_SOLID     : return TopAbs_SOLID;
    case GEOM_SHELL     : return TopAbs_SHELL;
    case GEOM_FACE      : return TopAbs_FACE;
    case GEOM_WIRE      : return TopAbs_WIRE;
    case GEOM_EDGE      : return TopAbs_EDGE;
    case GEOM_POINT     : return TopAbs_VERTEX;
    default             : return -1;
  }
}

//================================================================
// Function : getTopAbsMode
// Purpose  : Get filter corresponding to the type of object
//            from GEOMImpl_Types.h
//================================================================
static inline Handle(SALOME_Filter) getFilter( const int theMode )
{
  Handle(SALOME_Filter) aFilter;
  
  int aTopAbsMode = getTopAbsMode( theMode );
  if ( aTopAbsMode != -1 )
    aFilter = new GEOM_ShapeTypeFilter( ( TopAbs_ShapeEnum )aTopAbsMode );
  else
    switch ( theMode )
    {
      case GEOM_LINE      : aFilter = new GEOM_EdgeFilter( StdSelect_Line     ); break;
      case GEOM_CIRCLE    : aFilter = new GEOM_EdgeFilter( StdSelect_Circle   ); break;

      case GEOM_PLANE     : aFilter = new GEOM_FaceFilter( StdSelect_Plane    ); break;
      case GEOM_CYLINDER  : aFilter = new GEOM_FaceFilter( StdSelect_Cylinder ); break;
      case GEOM_SPHERE    : aFilter = new GEOM_FaceFilter( StdSelect_Sphere   ); break;
      case GEOM_TORUS     : aFilter = new GEOM_FaceFilter( StdSelect_Torus    ); break;
      case GEOM_REVOLUTION: aFilter = new GEOM_FaceFilter( StdSelect_Revol    ); break;
      case GEOM_CONE      : aFilter = new GEOM_FaceFilter( StdSelect_Cone     ); break;
      
      case GEOM_PREVIEW   : aFilter = new GEOM_PreviewFilter(); break;
      
      case GEOM_ALLSHAPES : aFilter = new GEOM_ShapeTypeFilter( -1, true ); break;
      case GEOM_ALLGEOM   : aFilter = new SALOME_TypeFilter( "GEOM" ); break;

      default             : aFilter = new GEOM_TypeFilter( theMode ); break;
    }

  return aFilter;
}

//================================================================
// Function : getEntry
// Purpose  :
//================================================================
static char* getEntry( GEOM::GEOM_Object_ptr object )
{
  QAD_Study* study = getActiveStudy();
  if ( study )  {
    string IOR = GeometryGUI::GetORB()->object_to_string( object );
    if ( IOR != "" ) {
      SALOMEDS::SObject_var SO = study->getStudyDocument()->FindObjectIOR( IOR.c_str() );
      if ( !SO->_is_nil() ) {
	return SO->GetID();
      }
    }
  }
  return "";
}

//================================================================
// Function : getName
// Purpose  :
//================================================================
static const char* getName( GEOM::GEOM_Object_ptr theObj )
{
  QAD_Study* aStudy = QAD_Application::getDesktop()->getActiveStudy();

  if ( aStudy )
  {
    string anIOR = GeometryGUI::GetORB()->object_to_string( theObj );
    if ( anIOR != "" )
    {
      SALOMEDS::SObject_var aSObj = aStudy->getStudyDocument()->FindObjectIOR( anIOR.c_str() );
      SALOMEDS::GenericAttribute_var anAttr;

      if ( !aSObj->_is_nil() && aSObj->FindAttribute( anAttr, "AttributeName") )
      {
        SALOMEDS::AttributeName_var aNameAttr = SALOMEDS::AttributeName::_narrow( anAttr );
        return aNameAttr->Value();
      }
    }
  }

  return "";
}

//=================================================================
/*!
 *  GEOM_Displayer::GEOM_Displayer
 *  Constructor
 */
//=================================================================
GEOM_Displayer::GEOM_Displayer()
{
  /* Shading Color */
  QString SCr = QAD_CONFIG->getSetting( "Geometry:SettingsShadingColorRed" );
  QString SCg = QAD_CONFIG->getSetting( "Geometry:SettingsShadingColorGreen" );
  QString SCb = QAD_CONFIG->getSetting( "Geometry:SettingsShadingColorBlue" );
  if ( !SCr.isEmpty() && !SCg.isEmpty() && !SCb.isEmpty() )
    myShadingColor = Quantity_Color(
      SCr.toInt()/255., SCg.toInt()/255., SCb.toInt()/255., Quantity_TOC_RGB );
  else
    myShadingColor = Quantity_Color( Quantity_NOC_GOLDENROD );

  myColor = -1;
  // This color is used for shape displaying. If it is equal -1 then
  // default color is used.

  myWidth = -1;
  myType = -1;

  myToActivate = true;
  // This parameter is used for activisation/deactivisation of objects to be displayed

  myViewFrame = 0;
}

//=================================================================
/*!
 *  GEOM_Displayer::~GEOM_Displayer
 *  Destructor
 */
//=================================================================
GEOM_Displayer::~GEOM_Displayer()
{
}

//=================================================================
/*!
 *  GetActiveView
 *  Get active study frame, returns 0 if no open study frame
 */
//=================================================================
QAD_ViewFrame* GEOM_Displayer::GetActiveView()
{
  QAD_Study* activeStudy = getActiveStudy();
  if ( activeStudy )
  {
    QAD_StudyFrame* sf = activeStudy->getActiveStudyFrame();
    if ( sf )
    {
      return sf->getRightFrame()->getViewFrame();
    }
  }
  return 0;
}

//=================================================================
/*!
 *  GEOM_Displayer::Display
 *  Display interactive object in the current viewer
 */
//=================================================================
void GEOM_Displayer::Display( const Handle(SALOME_InteractiveObject)& theIO, const bool updateViewer,
			      QAD_ViewFrame* theViewFrame )
{
  QAD_ViewFrame* vf = theViewFrame;
  if ( !vf )
    vf = GetActiveView();
  
  if ( vf )
  {
      if ( vf->inherits( "VTKViewer_ViewFrame" ) )
      {
        SALOME_Prs* prs = vf->CreatePrs( !theIO.IsNull() ? theIO->getEntry() : 0 );
        ((SALOME_View*)vf)->Erase( prs, true );
        delete prs;
      }
    
    SALOME_Prs* prs = buildPresentation( theIO, vf );
    
    if ( prs )
    {
      vf->BeforeDisplay( this );
      ((SALOME_View*)vf)->Display( prs );
      vf->AfterDisplay( this );
      
      if ( updateViewer )
        vf->Repaint();
      
      delete prs;  // delete presentation because displayer is its owner
    }
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Display
 *  This overloaded Display() method can be useful for operations
 *  not using dialog boxes.
 */
//=================================================================
void GEOM_Displayer::Display( GEOM::GEOM_Object_ptr theObj, const bool updateViewer )
{
  if ( theObj->_is_nil() )
    return;

  string entry = getEntry( theObj );
  if ( entry != "" ) {
    Display( new SALOME_InteractiveObject(
      entry.c_str(), "GEOM", strdup( getName( theObj ) ) ), updateViewer );
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Erase
 *  Erase interactive object in the current viewer
 */
//=================================================================
void GEOM_Displayer::Erase( const Handle(SALOME_InteractiveObject)& theIO,
                            const bool forced,
                            const bool updateViewer,
			    QAD_ViewFrame* theViewFrame )
{
  if ( theIO.IsNull() )
    return;

  QAD_ViewFrame* vf = theViewFrame;
  if ( !vf )
    vf = GetActiveView();
  
  if ( vf ) {
    SALOME_Prs* prs = vf->CreatePrs( theIO->getEntry() );
    if ( prs ) {
      ((SALOME_View*)vf)->Erase( prs, forced );
      if ( updateViewer )
	vf->Repaint();
      delete prs;  // delete presentation because displayer is its owner
    }
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Erase
 *  Erase geometry object in the current viewer
 */
//=================================================================
void GEOM_Displayer::Erase( GEOM::GEOM_Object_ptr theObj,
                            const bool forced,
                            const bool updateViewer )
{
  string entry = getEntry( theObj );
  if ( entry != "" )
  {
    Erase( new SALOME_InteractiveObject(
      entry.c_str(), "GEOM", strdup( getName( theObj ) ) ), forced, updateViewer );
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Redisplay
 *  Redisplay (erase and then display again) interactive object
 *  in the current viewer
 */
//=================================================================
void GEOM_Displayer::Redisplay( const Handle(SALOME_InteractiveObject)& theIO,
                                const bool updateViewer )
{
  // Remove the object permanently (<forced> == true)
  
  QAD_Study* activeStudy = getActiveStudy();
  if ( !activeStudy )
    return;
  
  int aNbFrames = activeStudy->getStudyFramesCount();
  for (int i = 0; i < aNbFrames; i++)
    {
      QAD_StudyFrame* sf = activeStudy->getStudyFrame(i);
      if ( !sf )
	continue;
      
      QAD_ViewFrame* vf = sf->getRightFrame()->getViewFrame();
      if ( !vf )
	continue;
      
      if ( vf->isVisible(theIO) || vf == GetActiveView() )
	{
	  Erase( theIO, true, false, vf );
	  Display( theIO, updateViewer, vf );
	}
    }
}

//=================================================================
/*!
 *  GEOM_Displayer::Display
 *  Calls Display() method for each object in the given list
 */
//=================================================================
void GEOM_Displayer::Display( const SALOME_ListIO& theIOList, const bool updateViewer )
{
  SALOME_ListIteratorOfListIO Iter( theIOList );
  for ( ; Iter.More(); Iter.Next() ) {
    Display( Iter.Value(), false );
  }
  if ( updateViewer )
    UpdateViewer();
}

//=================================================================
/*!
 *  GEOM_Displayer::Erase
 *  Calls Erase() method for each object in the given list
 */
//=================================================================
void GEOM_Displayer::Erase( const SALOME_ListIO& theIOList,
                            const bool forced,
                            const bool updateViewer )
{
  SALOME_ListIteratorOfListIO Iter( theIOList );
  for ( ; Iter.More(); Iter.Next() )
    Erase( Iter.Value(), forced, false );

  if ( updateViewer )
    UpdateViewer();
}

//=================================================================
/*!
 *  GEOM_Displayer::Redisplay
 *  Calls Redisplay() method for each object in the given list
 */
//=================================================================
void GEOM_Displayer::Redisplay( const SALOME_ListIO& theIOList, const bool updateViewer )
{
  SALOME_ListIteratorOfListIO Iter( theIOList );
  for ( ; Iter.More(); Iter.Next() ) {
    Redisplay( Iter.Value(), false );
  }
  if ( updateViewer )
    UpdateViewer();
}

//=================================================================
/*!
 *  GEOM_Displayer::UpdateViewer
 *  Update current viewer
 */
//=================================================================
void GEOM_Displayer::UpdateViewer()
{
  QAD_ViewFrame* vf = GetActiveView();
  if ( vf )
    vf->Repaint();
}

//=================================================================
/*!
 *  GEOM_Displayer::Update
 *  Update OCC presentaion
 *  [ Reimplemented from SALOME_Displayer ]
 */
//=================================================================
void GEOM_Displayer::Update( SALOME_OCCPrs* prs )
{
  OCCViewer_Prs* occPrs = dynamic_cast<OCCViewer_Prs*>( prs );
  if ( !occPrs )
    return;

  OCCViewer_ViewFrame* vf = dynamic_cast<OCCViewer_ViewFrame*>( myViewFrame );
  if ( !vf )
    return;

  if ( myType == GEOM_MARKER && !myShape.IsNull() && myShape.ShapeType() == TopAbs_FACE )
  {
    TopoDS_Face aFace = TopoDS::Face( myShape );
    Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( aFace ) );
    if ( !aPlane.IsNull() )
    {
      gp_Ax3 aPos = aPlane->Pln().Position();
      Handle(Geom_Axis2Placement) aPlc = new Geom_Axis2Placement( aPos.Ax2() );

      Handle(GEOM_AISTrihedron) aTrh;

      if ( occPrs->IsNull() )
      {
        aTrh = new GEOM_AISTrihedron( aPlc );

        if ( HasColor() )  
          aTrh->SetColor( (Quantity_NameOfColor)GetColor() );

        if ( HasWidth() )
          aTrh->SetWidth( GetWidth() );

        if ( !myIO.IsNull() )
        {
          aTrh->setIO( myIO );
          aTrh->SetOwner( myIO );
        }

        occPrs->AddObject( aTrh );
      }
      else
      {
        AIS_ListOfInteractive aList;
        occPrs->GetObjects( aList );
        AIS_ListIteratorOfListOfInteractive anIter( aList );
        for ( ; anIter.More(); anIter.Next() )
        {
          aTrh = Handle(GEOM_AISTrihedron)::DownCast( anIter.Value() );
          if ( !aTrh.IsNull() )
          {
            aTrh->SetComponent( aPlc );
            aTrh->SetToUpdate();            
          }
        }
      }

      occPrs->SetToActivate( ToActivate() );
    }
  }
  else
  {
    // if presentation is empty we try to create new one
    if ( occPrs->IsNull() )
    {
      if ( !myShape.IsNull() )
      {
        Handle(GEOM_AISShape) AISShape = new GEOM_AISShape( myShape, "" );
        // Temporary staff: vertex must be infinite for correct visualization
        AISShape->SetInfiniteState( myShape.Infinite() || myShape.ShapeType() == TopAbs_VERTEX );

        // Setup shape properties here ..., e.g. display mode, color, transparency, etc
        AISShape->SetShadingColor( myShadingColor );
        if ( HasColor() )
        {
          if ( myShape.ShapeType() == TopAbs_VERTEX )
          {
            AISShape->SetColor( (Quantity_NameOfColor)GetColor() );
            Handle(Prs3d_PointAspect) anAspect = AISShape->Attributes()->PointAspect();
            anAspect->SetColor( (Quantity_NameOfColor)GetColor() );
            AISShape->Attributes()->SetPointAspect( anAspect );
          }
          else
            AISShape->SetColor( (Quantity_NameOfColor)GetColor() );
        }

        if ( HasWidth() )
          AISShape->SetWidth( GetWidth() );

        if ( !myIO.IsNull() )
        {
          AISShape->setIO( myIO );
          AISShape->SetOwner( myIO );
        }
        else if ( !myName.empty() )
        {
          // Workaround to allow selection of temporary objects
          static int tempId = 0;
          char buf[50];
          sprintf( buf, "TEMP_%d", tempId++ );
          Handle( SALOME_InteractiveObject ) anObj =
            new SALOME_InteractiveObject( buf, "GEOM", myName.c_str() );
          AISShape->setIO( anObj );
          AISShape->SetOwner( anObj );
        }
        // AISShape->SetName(???); ??? necessary to set name ???
        occPrs->AddObject( AISShape );

        // In accordance with ToActivate() value object will be activated/deactivated
        // when it will be displayed
        occPrs->SetToActivate( ToActivate() );
      }
    }
    // if presentation is found -> set again shape for it
    else
    {
      if ( !myShape.IsNull() )
      {
        AIS_ListOfInteractive IOList;
        occPrs->GetObjects( IOList );
        AIS_ListIteratorOfListOfInteractive Iter( IOList );
        for ( ; Iter.More(); Iter.Next() )
        {
          Handle(GEOM_AISShape) AISShape = Handle(GEOM_AISShape)::DownCast( Iter.Value() );
          if ( AISShape.IsNull() )
            continue;
          if ( AISShape->Shape() != myShape )
          {
            AISShape->Set( myShape );
            AISShape->UpdateSelection();
            AISShape->SetToUpdate();
          }
          if ( !myIO.IsNull() )
          {
            AISShape->setIO( myIO );
            AISShape->SetOwner( myIO );
          }
        }
      }
    }
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Update
 *  Update VTK presentaion
 *  [ Reimplemented from SALOME_Displayer ]
 */
//=================================================================
void GEOM_Displayer::Update( SALOME_VTKPrs* prs )
{
  VTKViewer_Prs* vtkPrs = dynamic_cast<VTKViewer_Prs*>( prs );
  if ( !vtkPrs || myShape.IsNull() )
    return;

  vtkActorCollection* theActors = 0;
  
  if ( myType == GEOM_MARKER && !myShape.IsNull() && myShape.ShapeType() == TopAbs_FACE )
  {
    myToActivate = false;
    GEOM_VTKTrihedron* aTrh = GEOM_VTKTrihedron::New();
    
    if ( HasColor() )
    {
      Quantity_Color aColor( (Quantity_NameOfColor)GetColor() );
      aTrh->SetColor( aColor.Red(), aColor.Green(), aColor.Blue() );
    }

    Handle(Geom_Plane) aPlane =
      Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( TopoDS::Face( myShape ) ) );
    if ( aPlane.IsNull() )
      return;

    gp_Ax2 anAx2 = aPlane->Pln().Position().Ax2();
    aTrh->SetPlacement( new Geom_Axis2Placement( anAx2 ) );

    if ( VTKViewer_ViewFrame* vf = dynamic_cast<VTKViewer_ViewFrame*>( GetActiveView() ) )
      aTrh->SetSize( 0.5 * vf->GetTrihedronSize() );

    vtkPrs->AddObject( aTrh );
      
    theActors = vtkActorCollection::New();
    theActors->AddItem( aTrh );
  }
  else
    theActors = GEOM_AssemblyBuilder::BuildActors( myShape, 0, 0, Standard_True );
    
  theActors->InitTraversal();
  
  vtkActor* anActor = (vtkActor*)theActors->GetNextActor();

  vtkProperty* aProp = 0;

  if ( HasColor() || HasWidth() )
  {
    aProp = vtkProperty::New();
    aProp->SetRepresentationToWireframe();
  }
  
  if ( HasColor() )
  {
    Quantity_Color aColor( (Quantity_NameOfColor)GetColor() );
    aProp->SetColor( aColor.Red(), aColor.Green(), aColor.Blue() );
  }

  if ( HasWidth() )
  {
    aProp->SetLineWidth( GetWidth() );
  }

  while ( anActor != NULL )
  {
    SALOME_Actor* GActor = SALOME_Actor::SafeDownCast( anActor );
    
    GActor->setIO( myIO );

    if ( aProp )
    {
      GActor->SetProperty( aProp );
      GActor->SetPreviewProperty( aProp );

      GEOM_Actor* aGeomGActor = GEOM_Actor::SafeDownCast( anActor );
      if ( aGeomGActor != 0 )
      {
        aGeomGActor->SetShadingProperty( aProp );
        aGeomGActor->SetWireframeProperty( aProp );
      }
    }

    if ( myToActivate )
      GActor->PickableOn();
    else
      GActor->PickableOff();
      
    vtkPrs->AddObject( GActor );
    
    anActor = (vtkActor*)theActors->GetNextActor();
  }

  if ( aProp )
    aProp->Delete();

  theActors->Delete();
}

//=================================================================
/*!
 *  GEOM_Displayer::BuildPrs
 *  Build presentation accordint to the current viewer type
 */
//=================================================================
SALOME_Prs* GEOM_Displayer::BuildPrs( GEOM::GEOM_Object_ptr theObj )
{
  if ( theObj->_is_nil() )
    return 0;

  GeometryGUI* geomGUI = GeometryGUI::GetGeomGUI();

  myViewFrame = GetActiveView();
  if ( myViewFrame == 0 )
    return 0;
  
  SALOME_Prs* aPrs = myViewFrame->CreatePrs();
  if ( aPrs == 0 )
    return 0;

  internalReset();  
  setShape( geomGUI->GetShapeReader().GetShape( geomGUI->GetGeomGen(), theObj ) );
  myType = theObj->GetType();

  // Update presentation
  UpdatePrs( aPrs );  

  return aPrs;
}

//=================================================================
/*!
 *  GEOM_Displayer::BuildPrs
 *  Build presentation accordint to the current viewer type
 */
//=================================================================
SALOME_Prs* GEOM_Displayer::BuildPrs( const TopoDS_Shape& theShape )
{
  myViewFrame = GetActiveView();
  if ( theShape.IsNull() || myViewFrame == 0 )
    return 0;

  SALOME_Prs* aPrs = myViewFrame->CreatePrs();
  if ( aPrs == 0 )
    return 0;

  internalReset();
  setShape( theShape );
  myType = -1;

  UpdatePrs( aPrs );

  return aPrs;
}

//=================================================================
/*!
 *  GEOM_Displayer::buildPresentation
 *  Builds/finds object's presentation for the current viewer
 *  Calls corresponding Update() method by means of double dispatch
 *  [ internal ]      
 */
//=================================================================
SALOME_Prs* GEOM_Displayer::buildPresentation( const Handle(SALOME_InteractiveObject)& theIO, 
					       QAD_ViewFrame* theViewFrame )
{
  SALOME_Prs* prs = 0;
  internalReset();

  GeometryGUI* geomGUI = GeometryGUI::GetGeomGUI();

  myViewFrame = theViewFrame;  // no dynamic_cast here to use double dispatch
  if ( !myViewFrame )
    myViewFrame = GetActiveView();
  
  if ( myViewFrame ) {
    prs = myViewFrame->CreatePrs( !theIO.IsNull() ? theIO->getEntry() : 0 );
    if ( prs ) {
      if ( !theIO.IsNull() ) {
	// set interactive object
	setIO( theIO );
	//  Find SOBject (because shape should be published previously)
	SALOMEDS::SObject_var SO = getActiveStudy()->getStudyDocument()->FindObjectID( theIO->getEntry() );
	if ( !SO->_is_nil() ) {
	  // get CORBA reference to data object
	  CORBA::Object_var object = SO->GetObject();
	  if ( !CORBA::is_nil( object ) ) {
	    // downcast to GEOM object
	    GEOM::GEOM_Object_var GeomObject = GEOM::GEOM_Object::_narrow( object );
	    if ( !GeomObject->_is_nil() ) {
	      // finally set shape
	      setShape( geomGUI->GetShapeReader().GetShape( geomGUI->GetGeomGen(), GeomObject ) );
	      myType = GeomObject->GetType();
	    }
	  }
	}
      }
      UpdatePrs( prs );  // Update presentation by using of the double dispatch
    }
  }
  return prs;
}

//=================================================================
/*!
 *  GEOM_Displayer::internalReset
 *  Resets internal data
 *  [internal]
 */
//=================================================================
void GEOM_Displayer::internalReset()
{
  myIO.Nullify();
  myShape.Nullify();
}

//=================================================================
/*!
 *  GEOM_Displayer::LocalSelection
 *  Activate selection of CAD shapes with activisation of selection
 *  of their sub-shapes (with opened local context for OCC viewer)
 */
//=================================================================
void GEOM_Displayer::LocalSelection( const Handle(SALOME_InteractiveObject)& theIO, const int theMode )
{
  // remove all filters from selection
  SALOME_Selection* aSel = SALOME_Selection::Selection(
    QAD_Application::getDesktop()->getActiveStudy()->getSelection() );

  aSel->ClearFilters();

  QAD_ViewFrame* vf = GetActiveView();
  if ( vf )
  {
    SALOME_Prs* prs = vf->CreatePrs( theIO.IsNull() ? 0 : theIO->getEntry() );
    ((SALOME_View*)vf)->LocalSelection( prs, theMode );
    delete prs;  // delete presentation because displayer is its owner
  }  
}

//=================================================================
/*!
 *  GEOM_Displayer::globalSelection
 *  Activate selection of CAD shapes without activisation of selection
 *  of their sub-shapes (without opened local context for OCC viewer)
 */
//=================================================================
void GEOM_Displayer::GlobalSelection( const int theMode, const bool update )
{
  TColStd_MapOfInteger aModes;
  aModes.Add( theMode );
  GlobalSelection( aModes, update );
}

//=================================================================
/*!
 *  GEOM_Displayer::globalSelection
 *  Activate selection of CAD shapes without activisation of selection
 *  of their sub-shapes (without opened local context for OCC viewer)
 */
//=================================================================
void GEOM_Displayer::GlobalSelection( const TColStd_MapOfInteger& theModes,
				      const bool update )
{
  QAD_ViewFrame* vf = GetActiveView();
  if ( vf == 0 )
    return;

  // Close local context
  vf->GlobalSelection( update );

  // Set selection filters in accordance with current mode
  SALOME_Selection* aSel = SALOME_Selection::Selection(
    QAD_Application::getDesktop()->getActiveStudy()->getSelection() );

  // Remove from selection temporary objects if necessary
  if ( !theModes.Contains( GEOM_PREVIEW ) )
    clearTemporary( aSel );

  aSel->ClearIndex();
    
  aSel->ClearFilters();

  if ( theModes.Contains( GEOM_ALLOBJECTS ) )
    return;

  Handle(SALOME_Filter) aFilter;
  if ( theModes.Extent() == 1 )
  {
    int aMode = TColStd_MapIteratorOfMapOfInteger( theModes ).Key();
    aFilter = getFilter( aMode );
  }
  else if ( theModes.Extent() > 1 )
  {
    TColStd_MapOfInteger aTopAbsModes;
    TColStd_MapIteratorOfMapOfInteger anIter( theModes );
    GEOM_ListOfFilter aListOfFilters;
    for ( ; anIter.More(); anIter.Next() )
    {
      Handle(SALOME_Filter) aFilter = getFilter( anIter.Key() );
      if ( !aFilter.IsNull() )
        aListOfFilters.Append( aFilter );
    }

    aFilter = new GEOM_LogicalFilter( aListOfFilters, GEOM_LogicalFilter::LO_OR );
  }
  else
    return;

  if ( !aFilter.IsNull() )
    aSel->AddFilter( aFilter );
}

//=================================================================
/*!
 *  GEOM_Displayer::LocalSelection
 *  Activate selection of CAD shapes with activisation of selection
 *  of their sub-shapes (with opened local context for OCC viewer)
 */
//=================================================================
void GEOM_Displayer::LocalSelection( const SALOME_ListIO& theIOList, const int theMode )
{
  SALOME_ListIteratorOfListIO Iter( theIOList );
  for ( ; Iter.More(); Iter.Next() ) 
    LocalSelection( Iter.Value(), theMode );
}

//=================================================================
/*!
 *  GEOM_Displayer::BeforeDisplay
 *  Called before displaying of pars. Close local context
 *  [ Reimplemented from SALOME_Displayer ]
 */
//=================================================================
void GEOM_Displayer::BeforeDisplay( SALOME_View* v, const SALOME_OCCViewType& )
{
  OCCViewer_ViewFrame* vf = dynamic_cast<OCCViewer_ViewFrame*>( v );
  if ( vf )
  {
    Handle(AIS_InteractiveContext) ic = vf->getViewer()->getAISContext();
    if ( !ic.IsNull() )
    {
      if ( ic->HasOpenedContext() )
      ic->CloseAllContexts();
    }
  }
}

void GEOM_Displayer::AfterDisplay( SALOME_View*, const SALOME_OCCViewType& )
{
}


//=================================================================
/*!
 *  GEOM_Displayer::SetColor
 *  Set color for shape displaying. If it is equal -1 then default color is used.
 *  Available values are from Quantity_NameOfColor enumeration
 */
//=================================================================
void GEOM_Displayer::SetColor( const int color )
{
  myColor = color;
}

int GEOM_Displayer::GetColor() const
{
  return myColor;
}

bool GEOM_Displayer::HasColor() const
{
  return myColor != -1;
}

void GEOM_Displayer::UnsetColor()
{
  myColor = -1;
}

//=================================================================
/*!
 *  GEOM_Displayer::SetWidth
 *  Set width of shape displaying. If it is equal -1 then default width is used.
 */
//=================================================================
void GEOM_Displayer::SetWidth( const double width )
{
  myWidth = width;
}

double GEOM_Displayer::GetWidth() const
{
  return myWidth;
}

bool GEOM_Displayer::HasWidth() const
{
  return myWidth != -1;
}

void GEOM_Displayer::UnsetWidth()
{
  myWidth = -1;
}

//=================================================================
/*!
 *  GEOM_Displayer::SetToActivate
 *  This method is used for activisation/deactivisation of objects to be displayed
 */
//=================================================================
void GEOM_Displayer::SetToActivate( const bool toActivate )
{
  myToActivate = toActivate;
}
bool GEOM_Displayer::ToActivate() const
{
  return myToActivate;
}

//=================================================================
/*!
 *  GEOM_Displayer::clearTemporary
 *  Removes from selection temporary objects 
 */
//=================================================================
void GEOM_Displayer::clearTemporary( SALOME_Selection* theSel )
{
  SALOME_ListIO toBeRemoved;
  SALOME_ListIteratorOfListIO anIter( theSel->StoredIObjects() );
  for ( ; anIter.More(); anIter.Next() )
  {
    Handle(SALOME_InteractiveObject) anIO = anIter.Value();
    if ( anIO->hasEntry() && ( strncmp( anIO->getEntry(), "TEMP_", 5 ) == 0 ) )
    toBeRemoved.Append( anIO );
  }

  SALOME_ListIteratorOfListIO anIter2( toBeRemoved );
  for ( ; anIter2.More(); anIter2.Next() )
    theSel->RemoveIObject( anIter2.Value(), false );
}
























