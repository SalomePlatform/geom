//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// GEOM GEOMGUI : GUI for Geometry component
// File   : EntityGUI.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "EntityGUI.h"

#include <GeometryGUI.h>

#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>
#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewManager.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>

#include <TopoDS_Shape.hxx>

#include "EntityGUI_SketcherDlg.h" // Sketcher
#include "EntityGUI_3DSketcherDlg.h" // Sketcher
#include "EntityGUI_SubShapeDlg.h" // Method SUBSHAPE

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
  case 404: // SKETCHER
    getGeometryGUI()->ActiveWorkingPlane();
    aDlg = new EntityGUI_SketcherDlg( getGeometryGUI(), parent );
    break;
  case 405: // 3D SKETCHER
    aDlg = new EntityGUI_3DSketcherDlg( getGeometryGUI(), parent );
    break;
  case 407: // EXPLODE : use ic
    aDlg = new EntityGUI_SubShapeDlg( getGeometryGUI(), parent );
    break;
  default:
    app->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) );
    break;
  }
  if ( aDlg )
    aDlg->show();
  
  return true;
}


//=====================================================================================
// function : DisplaySimulationShape() 
// purpose  : Displays 'this->mySimulationShape' a pure graphical shape from a TopoDS_Shape
//=====================================================================================
void EntityGUI::DisplaySimulationShape( const TopoDS_Shape& S1, const TopoDS_Shape& S2 ) 
{
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
