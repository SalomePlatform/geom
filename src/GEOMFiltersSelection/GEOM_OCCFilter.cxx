#include "GEOM_OCCFilter.h"

#include <LightApp_SelectionMgr.h> 
#include <LightApp_DataOwner.h>

#include <SALOME_InteractiveObject.hxx>

#include <AIS_InteractiveObject.hxx>
#include <SelectMgr_EntityOwner.hxx>

IMPLEMENT_STANDARD_HANDLE(GEOM_OCCFilter, SelectMgr_Filter)
IMPLEMENT_STANDARD_RTTIEXT(GEOM_OCCFilter, SelectMgr_Filter)

//=======================================================================
// function : GEOM_OCCFilter
// purpose  : 
//=======================================================================
GEOM_OCCFilter::GEOM_OCCFilter( LightApp_SelectionMgr* theSelMgr )
  : SelectMgr_Filter()
{
  mySelMgr = theSelMgr;
}

//=======================================================================
// function : ~GEOM_OCCFilter
// purpose  : 
//=======================================================================
GEOM_OCCFilter::~GEOM_OCCFilter()
{
}

//=======================================================================
// function : IsOk
// purpose  : 
//=======================================================================
Standard_Boolean GEOM_OCCFilter::IsOk( const Handle(SelectMgr_EntityOwner)& anObj ) const
{
  Handle(AIS_InteractiveObject) anAIS = Handle(AIS_InteractiveObject)::DownCast( anObj->Selectable() );
  if ( anAIS.IsNull() || !anAIS->HasOwner() )
    return false;
  
  Handle(SALOME_InteractiveObject) anIO = Handle(SALOME_InteractiveObject)::DownCast(anAIS->GetOwner()); 
  if ( anIO.IsNull() ) return false;
  
  return mySelMgr->isOk( new LightApp_DataOwner( QString( anIO->getEntry() ) ) );
}
