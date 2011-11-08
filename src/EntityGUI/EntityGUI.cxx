// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : EntityGUI.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "EntityGUI.h"

#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>
#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>

#include <QMouseEvent>
#include <QApplication>

#include "EntityGUI_SketcherDlg.h"        // Sketcher
#include "EntityGUI_3DSketcherDlg.h"      // Sketcher
#include "EntityGUI_SubShapeDlg.h"        // Method SUBSHAPE
#include "EntityGUI_FeatureDetectorDlg.h" // Feature Detection
#include "EntityGUI_PictureImportDlg.h"   // Import Picture in viewer

//=======================================================================
// function : EntityGUI()
// purpose  : Constructor
//=======================================================================
EntityGUI::EntityGUI( GeometryGUI* parent ) :  GEOMGUI( parent )
{
  mySimulationShape1 = new AIS_Shape( TopoDS_Shape() );
  mySimulationShape2 = new AIS_Shape( TopoDS_Shape() );
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
bool EntityGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();
  QDialog* aDlg = NULL;

  switch ( theCommandID ) {
  case GEOMOp::Op2dSketcher: // 2D SKETCHER
    getGeometryGUI()->ActiveWorkingPlane();
    aDlg = new EntityGUI_SketcherDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::Op3dSketcher: // 3D SKETCHER
    aDlg = new EntityGUI_3DSketcherDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpExplode:    // EXPLODE
    aDlg = new EntityGUI_SubShapeDlg( getGeometryGUI(), parent );
    break;
#ifdef WITH_OPENCV
  case GEOMOp::OpFeatureDetect:    // FEATURE DETECTION
    aDlg = new EntityGUI_FeatureDetectorDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpPictureImport:    // IMPORT PICTURE IN VIEWER
    aDlg = new EntityGUI_PictureImportDlg( getGeometryGUI(), parent );
    break;
#endif
  default:
    app->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) );
    break;
  }
  if ( aDlg )
    aDlg->show();
  
  return true;
}

//=================================================================================
// function : 0nMousePress()
// purpose  : [static] manage mouse events
//=================================================================================
bool EntityGUI::OnMousePress( QMouseEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* theViewWindow )
{
  MESSAGE("EntityGUI::OnMousePress")
  QDialog* aDlg = getGeometryGUI()->GetActiveDialogBox();

  // Create Point dialog, OCC viewer 
  if ( aDlg && 
       theViewWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() &&
       pe->modifiers() != Qt::ControlModifier ) {
    
    gp_Pnt aPnt;
  
    if ( QString( aDlg->metaObject()->className() ).compare( "EntityGUI_SketcherDlg" ) == 0 ) 
    { 
      EntityGUI_SketcherDlg* aSketcherDlg = (EntityGUI_SketcherDlg*) aDlg;
      if ( aSketcherDlg->acceptMouseEvent() ) {
        OCCViewer_Viewer* anOCCViewer =
          ( (OCCViewer_ViewManager*)( theViewWindow->getViewManager() ) )->getOCCViewer();
        Handle(AIS_InteractiveContext) ic = anOCCViewer->getAISContext();
    
        ic->InitSelected();
        if ( pe->modifiers() == Qt::ShiftModifier )
          ic->ShiftSelect();  // Append selection
        else
          ic->Select();       // New selection

        ic->InitSelected();
        if ( ic->MoreSelected() ) {
          TopoDS_Shape aShape = ic->SelectedShape();
          if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
            aPnt = BRep_Tool::Pnt( TopoDS::Vertex( ic->SelectedShape() ) );
        }
        else {
          OCCViewer_ViewPort3d* vp =  ((OCCViewer_ViewWindow*)theViewWindow)->getViewPort();
          aPnt = ConvertClickToPoint( pe->x(), pe->y(), vp->getView() );
        }
        
        Qt::KeyboardModifiers modifiers = pe->modifiers();
        aSketcherDlg->OnPointSelected( modifiers, aPnt );  // "feed" the point to point construction dialog
      } // acceptMouseEvent()
    }
    if (  QString( aDlg->metaObject()->className() ).compare( "EntityGUI_FeatureDetectorDlg" ) == 0 ) 
    {
      EntityGUI_FeatureDetectorDlg* aCornerDlg = (EntityGUI_FeatureDetectorDlg*) aDlg;
      if ( aCornerDlg->acceptMouseEvent() ) {
        OCCViewer_Viewer* anOCCViewer =
          ( (OCCViewer_ViewManager*)( theViewWindow->getViewManager() ) )->getOCCViewer();
        Handle(AIS_InteractiveContext) ic = anOCCViewer->getAISContext();   

        ic->InitSelected();
        ic->Select();       // New selection

        ic->InitSelected();
        if ( ic->MoreSelected() ) {
          TopoDS_Shape aShape = ic->SelectedShape();
          if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
            aPnt = BRep_Tool::Pnt( TopoDS::Vertex( ic->SelectedShape() ) );
        }
        else {
          OCCViewer_ViewPort3d* vp =  ((OCCViewer_ViewWindow*)theViewWindow)->getViewPort();
          aPnt = ConvertClickToPoint( pe->x(), pe->y(), vp->getView() );
        }
        
//         aCornerDlg->OnPointSelected( aPnt );  // "feed" the point to corner detection dialog
        
        QPoint start = QPoint(pe->x(),pe->y());
        aCornerDlg->setStartPnt( start );
      } // acceptMouseEvent()
      
    }
  }
  return false;
}

//=================================================================================
// function : 0nMouseMove()
// purpose  : [static] manage mouse events
//=================================================================================
bool EntityGUI::OnMouseMove( QMouseEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* theViewWindow )
{
//   MESSAGE("EntityGUI::OnMouseMove")
  QDialog* aDlg = getGeometryGUI()->GetActiveDialogBox();
  if ( aDlg && ( QString( aDlg->metaObject()->className() ).compare( "EntityGUI_FeatureDetectorDlg" ) == 0 ) &&
       theViewWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() &&
       pe->modifiers() != Qt::ControlModifier ) {
    
    EntityGUI_FeatureDetectorDlg* aCornerDlg = (EntityGUI_FeatureDetectorDlg*) aDlg;
   
    gp_Pnt aPnt; 
      
    if ( QApplication::mouseButtons() == Qt::LeftButton && 
         aCornerDlg->acceptMouseEvent() )
    {
      QPoint end = QPoint(pe->x(),pe->y());
      aCornerDlg->setEndPnt( end );
    }    
  }
  return false;
}

//=======================================================================
// function : ConvertClickToPoint()
// purpose  : Returns the point clicked in 3D view
//=======================================================================
gp_Pnt EntityGUI::ConvertClickToPoint( int x, int y, Handle(V3d_View) aView )
{
  V3d_Coordinate XEye, YEye, ZEye, XAt, YAt, ZAt;
  aView->Eye( XEye, YEye, ZEye );

  aView->At( XAt, YAt, ZAt );
  gp_Pnt EyePoint( XEye, YEye, ZEye );
  gp_Pnt AtPoint( XAt, YAt, ZAt );

  gp_Vec EyeVector( EyePoint, AtPoint );
  gp_Dir EyeDir( EyeVector );

  gp_Pln PlaneOfTheView = gp_Pln( AtPoint, EyeDir );
  Standard_Real X, Y, Z;
  aView->Convert( x, y, X, Y, Z );
  gp_Pnt ConvertedPoint( X, Y, Z );

  gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project( PlaneOfTheView, ConvertedPoint );
  gp_Pnt ResultPoint = ElSLib::Value( ConvertedPointOnPlane.X(), ConvertedPointOnPlane.Y(), PlaneOfTheView );
  return ResultPoint;
}

//=====================================================================================
// function : DisplaySimulationShape() 
// purpose  : Displays 'this->mySimulationShape' a pure graphical shape from a TopoDS_Shape
//=====================================================================================
void EntityGUI::DisplaySimulationShape( const TopoDS_Shape& S1, const TopoDS_Shape& S2 ) 
{
  MESSAGE("EntityGUI::DisplaySimulationShape")
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return;

  SUIT_ViewManager* aVM = app->desktop()->activeWindow()->getViewManager();
  if ( aVM->getType() != OCCViewer_Viewer::Type() )
    return;

  OCCViewer_Viewer* v3d = ( (OCCViewer_ViewManager*)aVM )->getOCCViewer();
  Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
  try {
    if ( !S1.IsNull() ) {
      /* erase any previous */
      ic->Erase( mySimulationShape1, Standard_True, Standard_False );
      ic->ClearPrs( mySimulationShape1 );

      mySimulationShape1 = new AIS_Shape( TopoDS_Shape() );
      mySimulationShape1->Set( S1 );
      mySimulationShape1->SetColor( Quantity_NOC_RED );

      ic->Deactivate( mySimulationShape1 );
      ic->Display( mySimulationShape1, Standard_False );
      mySimulationShape1->UnsetColor();
    }
    if ( !S2.IsNull() ) {
      ic->Erase( mySimulationShape2, Standard_True, Standard_False );
      ic->ClearPrs( mySimulationShape2 );

      mySimulationShape2 = new AIS_Shape( TopoDS_Shape() );
      mySimulationShape2->Set( S2 );
      mySimulationShape2->SetColor( Quantity_NOC_VIOLET );

      ic->Deactivate( mySimulationShape2 );
      ic->Display( mySimulationShape2, Standard_False );
      mySimulationShape2->UnsetColor();
    }
    ic->UpdateCurrentViewer();
  }
  catch( Standard_Failure ) {
    MESSAGE( "Exception catched in EntityGUI::DisplaySimulationShape" );
  } 
}

//==================================================================================
// function : EraseSimulationShape()
// purpose  : Clears the display of 'mySimulationShape' a pure graphical shape
//==================================================================================
void EntityGUI::EraseSimulationShape()
{
  MESSAGE("EntityGUI::EraseSimulationShape")
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return;

  // get all view windows at the desktop
  QList<SUIT_ViewWindow*> aWndLst = app->desktop()->windows();
  //get all view windows, which belong to the active study
  QList<SUIT_ViewWindow*> aWndLstAS;
  SUIT_ViewWindow* vw;

  QListIterator<SUIT_ViewWindow*> itWL( aWndLst );
  while ( itWL.hasNext() && ( vw = itWL.next() ) )
    if ( vw->getViewManager()->study() == app->activeStudy() )
      aWndLstAS.append( vw );

  QListIterator<SUIT_ViewWindow*> itWLAS( aWndLstAS );
  while ( itWLAS.hasNext() && ( vw = itWLAS.next() ) ) {
    if ( vw->getViewManager()->getType() == OCCViewer_Viewer::Type() ) {
      OCCViewer_Viewer* v3d = ( (OCCViewer_ViewManager*)( vw->getViewManager() ) )->getOCCViewer();
      Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
      ic->Erase( mySimulationShape1, Standard_True, Standard_False );
      ic->ClearPrs( mySimulationShape1 );
      ic->Erase( mySimulationShape2, Standard_True, Standard_False );
      ic->ClearPrs( mySimulationShape2 );
      ic->UpdateCurrentViewer();
    } 
  }
}

//=====================================================================================
// function : SObjectExist()
// purpose  :
//=====================================================================================
bool EntityGUI::SObjectExist( const _PTR(SObject)& theFatherObject, const char* IOR )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !appStudy ) return false;

  _PTR(Study) aStudy = appStudy->studyDS();
  _PTR(ChildIterator) it ( aStudy->NewChildIterator( theFatherObject ) );
  _PTR(SObject) RefSO;
  _PTR(GenericAttribute) anAttr;
  for ( ; it->More();it->Next() ) {
    _PTR(SObject) SO ( it->Value() );
    if ( SO->FindAttribute( anAttr, "AttributeIOR" ) ) {
      _PTR(AttributeIOR) anIOR ( anAttr  );
      if ( strcmp( anIOR->Value().c_str(), IOR ) == 0 )
        return true;
    }
    if ( SO->ReferencedObject( RefSO ) ) {
      if ( RefSO->FindAttribute( anAttr, "AttributeIOR" ) ) {
        _PTR(AttributeIOR) anIOR ( anAttr );
        if ( strcmp( anIOR->Value().c_str(), IOR ) == 0 )
          return true;
      }
    }
  }
  return false;
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
#ifdef WIN32
  __declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new EntityGUI( parent );
  }
}
