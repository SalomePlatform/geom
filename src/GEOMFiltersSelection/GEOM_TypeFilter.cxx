#include "GEOM_TypeFilter.h"

//=======================================================================
// function : GEOM_TypeFilter
// purpose  : 
//=======================================================================
GEOM_TypeFilter::GEOM_TypeFilter( SalomeApp_Study* study, const int type )
:GEOM_SelectionFilter( study ),
myType( type )
{
}

//=======================================================================
// function : ~GEOM_TypeFilter
// purpose  : 
//=======================================================================
GEOM_TypeFilter::~GEOM_TypeFilter()
{
}

//=======================================================================
// function : isOk
// purpose  : 
//=======================================================================
bool GEOM_TypeFilter::isOk( const SUIT_DataOwner* sOwner ) const
{
  GEOM::GEOM_Object_var obj = getObject( sOwner );
  if ( !CORBA::is_nil( obj ) )
    return obj->GetType() == type();
  
  return false;
}

//=======================================================================
// function : type
// purpose  : 
//=======================================================================
int GEOM_TypeFilter::type() const
{
  return myType;
}
