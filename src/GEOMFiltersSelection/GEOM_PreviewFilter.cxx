#include "GEOM_PreviewFilter.h"

#include <LightApp_DataOwner.h>

//=======================================================================
// function : GEOM_SelectionFilter
// purpose  : 
//=======================================================================
GEOM_PreviewFilter::GEOM_PreviewFilter( SalomeApp_Study* study )
: SalomeApp_Filter( study )
{
}

//=======================================================================
// function : ~GEOM_SelectionFilter
// purpose  : 
//=======================================================================
GEOM_PreviewFilter::~GEOM_PreviewFilter()
{
}

//=======================================================================
// function : isOk
// purpose  : 
//=======================================================================
bool GEOM_PreviewFilter::isOk( const SUIT_DataOwner* sOwner ) const
{
  const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*> ( sOwner );
  return ( owner && strstr( owner->entry(), "TEMP" ) );
}

