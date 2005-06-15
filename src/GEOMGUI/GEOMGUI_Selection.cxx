#include <OCCViewer_ViewModel.h>

#include "GEOMGUI_Selection.h"
#include "GEOM_Displayer.h"
#include "GEOMImpl_Types.hxx"

#include <SalomeApp_DataOwner.h>
#include <SalomeApp_Study.h>

#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewModel.h>

#include <SALOMEDSClient_SObject.hxx>
#include <SALOMEDSClient_Study.hxx>
#include <SALOMEDS_SObject.hxx>
#include <SALOME_Prs.h>
#include <SALOME_InteractiveObject.hxx>

#include <SOCC_Prs.h>
#include <SVTK_Prs.h>
#include <SALOME_Actor.h>
#include <vtkActorCollection.h>

#include <OCCViewer_ViewModel.h>

#include <AIS.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListOfInteractive.hxx>

GEOMGUI_Selection::GEOMGUI_Selection()
{
}

GEOMGUI_Selection::~GEOMGUI_Selection()
{
}

QtxValue GEOMGUI_Selection::param( const int ind, const QString& p ) const
{
  QtxValue val( SalomeApp_Selection::param( ind, p ) );
  if ( !val.isValid() ) {
    if      ( p == "isVisible"   )    val = QtxValue( isVisible( ind ) );
    else if ( p == "isOCC"       )    val = QtxValue( isOCC() );
    else if ( p == "type"        )    val = QtxValue( typeName( ind ) );
    else if ( p == "displaymode" )    val = QtxValue( displayMode( ind ) );
    else if ( p == "isActiveViewer" ) val = QtxValue( isActiveViewer() );
  }

  //printf( "--> param() : [%s] = %s\n", p.latin1(), val.toString ().latin1() );

  return val;
}

QString GEOMGUI_Selection::typeName( const int index ) const
{
  if ( isComponent( index ) )
    return "Component";
  GEOM::GEOM_Object_var anObj = getObject( index );
  if ( !CORBA::is_nil( anObj ) ) {
    const int aGeomType = anObj->GetType();
    if ( aGeomType == GEOM_GROUP )
      return "Group";
    else
      return "Shape";
  }
  return "Unknown";
}

bool GEOMGUI_Selection::isVisible( const int index ) const
{
  GEOM::GEOM_Object_var obj = getObject( index );
  SALOME_View* view = GEOM_Displayer::GetActiveView();
  if ( !CORBA::is_nil( obj ) && view ) {
    Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject( entry( index ).latin1(), "GEOM", "TEMP_IO" );
    return view->isVisible( io );
  }
  return false;
}

bool GEOMGUI_Selection::isOCC() const
{
  SUIT_ViewWindow* window = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  return ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );
}

QString GEOMGUI_Selection::displayMode( const int index ) const
{
  SALOME_View* view = GEOM_Displayer::GetActiveView();
  if ( view ) {
    SALOME_Prs* prs = view->CreatePrs( entry( index ) );
    if ( prs ) {
      if ( isOCC() ) { // assuming OCC
	SOCC_Prs* occPrs = (SOCC_Prs*) prs;
	AIS_ListOfInteractive lst;
	occPrs->GetObjects( lst );
	if ( lst.Extent() ) {
	  Handle(AIS_InteractiveObject) io = lst.First();
	  if ( !io.IsNull() ) {
	    int dm = io->DisplayMode();
            if ( dm == AIS_WireFrame )
	      return "Wireframe";
	    else if ( dm == AIS_Shaded )
	      return "Shading";
	    else { // return default display mode of AIS_InteractiveContext
	      OCCViewer_Viewer* occViewer = (OCCViewer_Viewer*) SUIT_Session::session()->activeApplication()->desktop(
                                            )->activeWindow()->getViewManager()->getViewModel();
	      Handle(AIS_InteractiveContext) ic = occViewer->getAISContext();
	      dm = ic->DisplayMode();
	      if ( dm == AIS_WireFrame )
		return "Wireframe";
	      else if ( dm == AIS_Shaded )
		return "Shading";
	    }
	  }
	}
      } 
      else { // assuming VTK
	SVTK_Prs* vtkPrs = (SVTK_Prs*) prs;
	vtkActorCollection* lst = vtkPrs->GetObjects();
	if ( lst ) {
	  lst->InitTraversal();
	  vtkActor* actor = lst->GetNextActor();
	  if ( actor ) {
	    SALOME_Actor* salActor = dynamic_cast<SALOME_Actor*>( actor );
	    if ( salActor ) {
	      int dm = salActor->getDisplayMode();
	      if ( dm == 0 )
		return "Wireframe";
	      else if ( dm == 1 ) 
		return "Shading";
	    } // if ( salome actor )
	  } // if ( actor )
	} // if ( lst == vtkPrs->GetObjects() )
      } // if VTK
    }
  }
  return "";
}

bool GEOMGUI_Selection::isComponent( const int index ) const
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
    (SUIT_Session::session()->activeApplication()->activeStudy());

  if ( appStudy && index >= 0 && index < count() )  {
    _PTR(Study) study = appStudy->studyDS();
    QString anEntry = entry( index );

    if ( study && !anEntry.isNull() ) { 
      _PTR(SObject) aSO( study->FindObjectID( anEntry.latin1() ) );
      if ( aSO && aSO->GetFatherComponent() ) 
	return aSO->GetFatherComponent()->GetIOR() == aSO->GetIOR();
    }
  }
  return false;
}

GEOM::GEOM_Object_ptr GEOMGUI_Selection::getObject( const int index ) const
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
    (SUIT_Session::session()->activeApplication()->activeStudy());

  if ( appStudy && index >= 0 && index < count() )  {
    _PTR(Study) study = appStudy->studyDS();
    QString anEntry = entry( index );

    if ( study && !anEntry.isNull() ) { 
      _PTR(SObject) aSO( study->FindObjectID( anEntry.latin1() ) );
      if ( aSO ) {
	SALOMEDS_SObject* aDSObj = dynamic_cast<SALOMEDS_SObject*>( aSO.get() );
	return GEOM::GEOM_Object::_narrow( aDSObj->GetObject() );
      }
    }
  }
  return GEOM::GEOM_Object::_nil();
}

bool GEOMGUI_Selection::isActiveViewer() const
{
  return ( SUIT_Session::session()->activeApplication()->desktop()->activeWindow() != 0 );
}
