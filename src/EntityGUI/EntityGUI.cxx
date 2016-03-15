// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#include "GeometryGUI.h"
#include "GeometryGUI_Operations.h"
#include "GEOMUtils.hxx"

#include <LightApp_SelectionMgr.h>
#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewPort3d.h>
#include <OCCViewer_ViewWindow.h>
#include <SALOME_ListIO.hxx>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include "utilities.h"

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>

#include <QMouseEvent>
#include <QApplication>

#include "EntityGUI_SketcherDlg.h"        // Sketcher
#include "EntityGUI_3DSketcherDlg.h"      // Sketcher
#include "EntityGUI_IsolineDlg.h"         // Isoline
#include "EntityGUI_SurfFromFaceDlg.h"    // Surface From Face
#include "EntityGUI_SubShapeDlg.h"        // Method SUBSHAPE
#include "EntityGUI_FeatureDetectorDlg.h" // Feature Detection
#include "EntityGUI_PictureImportDlg.h"   // Import Picture in viewer
#include "EntityGUI_FieldDlg.h"           // Create/Edit Field
#include "EntityGUI_PolylineDlg.h"        // Create/Edit 2d polyline

#include "GEOMImpl_Types.hxx"


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
  case GEOMOp::OpIsoline:    // ISOLINE
    aDlg = new EntityGUI_IsolineDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpSurfaceFromFace:    // SURFACE FROM FACE
    aDlg = new EntityGUI_SurfFromFaceDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpExplode:    // EXPLODE
    aDlg = new EntityGUI_SubShapeDlg( getGeometryGUI(), parent );
    break;
#ifdef WITH_OPENCV
  case GEOMOp::OpFeatureDetect:    // FEATURE DETECTION
    aDlg = new EntityGUI_FeatureDetectorDlg( getGeometryGUI(), parent );
    break;
#endif
  case GEOMOp::OpPictureImport:    // IMPORT PICTURE IN VIEWER
    aDlg = new EntityGUI_PictureImportDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpCreateField: // CREATE FIELD
    aDlg = new EntityGUI_FieldDlg (getGeometryGUI(), GEOM::GEOM_Field::_nil(), 0,
                                   parent);
    break;
  case GEOMOp::OpEditField: // EDIT FIELD
  case GEOMOp::OpEditFieldPopup:
  {
    SALOME_ListIO aList;
    aList.Clear();

    LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
    if (aSelMgr)
      aSelMgr->selectedObjects(aList);
    SALOME_ListIteratorOfListIO anIter (aList);

    GEOM::GEOM_Field_var     field;
    GEOM::GEOM_FieldStep_var step;

    SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
    if ( aList.Extent() > 0 && study ) {
      for ( ; anIter.More(); anIter.Next() )
      {
        Handle(SALOME_InteractiveObject) anIObj = anIter.Value();
        if ( !anIObj.IsNull() && anIObj->hasEntry() )
          if ( _PTR(SObject) obj = study->studyDS()->FindObjectID( anIObj->getEntry() ))
          {
            CORBA::Object_var corbaObj = GeometryGUI::ClientSObjectToObject( obj );
            GEOM::GEOM_BaseObject_var bo = GEOM::GEOM_BaseObject::_narrow( corbaObj );
            GEOM::GEOM_Field_var     f;
            GEOM::GEOM_FieldStep_var s;
            switch ( bo->GetType() ) {
            case GEOM_FIELD:
              f = GEOM::GEOM_Field::_narrow( corbaObj ); break;
            case GEOM_FIELD_STEP:
              step = GEOM::GEOM_FieldStep::_narrow( corbaObj );
              if ( !step->_is_nil() )
                f = step->GetField();
              break;
            default:
              continue;
            }
            if ( !f->_is_nil() )
            {
              if ( !field->_is_nil() && !f->_is_equivalent( field ))
              {
                field = GEOM::GEOM_Field::_nil(); // several field selected
                break;
              }
              field = f;
            }
          }
      }
    }

    if ( !field->_is_nil()) {
      int stepID;
      if ( !step->_is_nil() ) {
        stepID = step->GetID();
      }
      else {
        GEOM::ListOfLong_var stepIDs = field->GetSteps();
        if ( stepIDs->length() > 0 )
          stepID = stepIDs[0];
        else
          stepID = 0;
      }
      aDlg = new EntityGUI_FieldDlg (getGeometryGUI(), field, stepID, parent); 
      break;
    }
    SUIT_MessageBox::warning(parent, tr("WRN_WARNING"), tr("NO_FIELD"));
    break;
  }
  case GEOMOp::Op2dPolylineEditor: // POLYLINE EDITOR
    getGeometryGUI()->ActiveWorkingPlane();
    aDlg = new EntityGUI_PolylineDlg( getGeometryGUI(), parent );
    break;
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
  QDialog* aDlg = getGeometryGUI()->GetActiveDialogBox();

  // Create Point dialog, OCC viewer 
  if ( aDlg && 
       theViewWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() &&
       pe->modifiers() != Qt::ControlModifier ) {
    
    gp_Pnt aPnt;
  
    if ( QString( aDlg->metaObject()->className() ).compare( "EntityGUI_SketcherDlg" ) == 0 ) 
    { 
      EntityGUI_SketcherDlg* aSketcherDlg = (EntityGUI_SketcherDlg*) aDlg;
      ((OCCViewer_ViewWindow*)theViewWindow)->setSketcherStyle(true);
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
          aPnt = GEOMUtils::ConvertClickToPoint( pe->x(), pe->y(), vp->getView() );
        }
        
        Qt::KeyboardModifiers modifiers = pe->modifiers();
        aSketcherDlg->OnPointSelected( modifiers, aPnt );  // "feed" the point to point construction dialog
      } // acceptMouseEvent()
    }
#ifdef WITH_OPENCV
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
        TopoDS_Shape aShape;
        if ( ic->MoreSelected() ) 
          aShape = ic->SelectedShape();
        if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
            aPnt = BRep_Tool::Pnt( TopoDS::Vertex( ic->SelectedShape() ) );
        else 
        {
          OCCViewer_ViewPort3d* vp =  ((OCCViewer_ViewWindow*)theViewWindow)->getViewPort();
          aPnt = GEOMUtils::ConvertClickToPoint( pe->x(), pe->y(), vp->getView() );
        }
        
//         aCornerDlg->OnPointSelected( aPnt );  // "feed" the point to corner detection dialog
        
//         QPoint start = QPoint(pe->x(),pe->y());
        aCornerDlg->setStartPnt( aPnt );
      } // acceptMouseEvent()
      
    }
#endif
  }

  return false;
}

//=================================================================================
// function : 0nMouseRelease()
// purpose  : [static] manage mouse events
//=================================================================================
bool EntityGUI::OnMouseRelease( QMouseEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* theViewWindow )
{
  ((OCCViewer_ViewWindow*)theViewWindow)->setSketcherStyle(false);
  QDialog* aDlg = getGeometryGUI()->GetActiveDialogBox();
#ifdef WITH_OPENCV
  if ( aDlg && ( QString( aDlg->metaObject()->className() ).compare( "EntityGUI_FeatureDetectorDlg" ) == 0 ) &&
       theViewWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() &&
       pe->modifiers() != Qt::ControlModifier ) 
  {   
    EntityGUI_FeatureDetectorDlg* aCornerDlg = (EntityGUI_FeatureDetectorDlg*) aDlg;
   
    gp_Pnt aPnt; 
      
    if ( aCornerDlg->acceptMouseEvent() )
    {
//       QPoint end = QPoint(pe->x(),pe->y());
      OCCViewer_ViewPort3d* vp =  ((OCCViewer_ViewWindow*)theViewWindow)->getViewPort();
      aPnt = GEOMUtils::ConvertClickToPoint( pe->x(), pe->y(), vp->getView() );
      aCornerDlg->setEndPnt( aPnt );
    }    
  }
#endif
  return false;
}

//=================================================================================
// function : 0nMouseMove()
// purpose  : [static] manage mouse events
//=================================================================================
bool EntityGUI::OnMouseMove( QMouseEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* theViewWindow )
{
  QDialog* aDlg = getGeometryGUI()->GetActiveDialogBox();
  
  if ( aDlg && QString( aDlg->metaObject()->className() ).compare( "EntityGUI_SketcherDlg" ) == 0 &&
       theViewWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() ) 
  { 
    EntityGUI_SketcherDlg* aSketcherDlg = (EntityGUI_SketcherDlg*) aDlg;
    if ( aSketcherDlg->acceptMouseEvent() ) 
    {    
      OCCViewer_ViewPort3d* vp =  ((OCCViewer_ViewWindow*)theViewWindow)->getViewPort();
      gp_Pnt aPnt = GEOMUtils::ConvertClickToPoint( pe->x(), pe->y(), vp->getView() );
  
      Qt::KeyboardModifiers modifiers = pe->modifiers();
      if (QApplication::mouseButtons() == Qt::LeftButton )
        aSketcherDlg->OnPointSelected( modifiers, aPnt, false );  // "feed" the point to point construction dialog
    }
  }
    
  return false;
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
      ic->Erase( mySimulationShape1, Standard_True );
      ic->ClearPrs( mySimulationShape1 );

      mySimulationShape1 = new AIS_Shape( TopoDS_Shape() );
      mySimulationShape1->Set( S1 );
      mySimulationShape1->SetColor( Quantity_NOC_RED );

      ic->Deactivate( mySimulationShape1 );
      ic->Display( mySimulationShape1, Standard_False );
      mySimulationShape1->UnsetColor();
    }
    if ( !S2.IsNull() ) {
      ic->Erase( mySimulationShape2, Standard_True );
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
      ic->Erase( mySimulationShape1, Standard_True );
      ic->Erase( mySimulationShape2, Standard_True );
      ic->ClearPrs( mySimulationShape1 );
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
