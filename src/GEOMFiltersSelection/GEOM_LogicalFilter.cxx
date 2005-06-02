#include "GEOM_LogicalFilter.h"

//=======================================================================
// function : GEOM_LogicalFilter
// purpose  : 
//=======================================================================
GEOM_LogicalFilter::GEOM_LogicalFilter( const QPtrList<SUIT_SelectionFilter>& lst, const int op )
: SUIT_SelectionFilter()
{
  setFilters( lst ); 
  setOperation( op );
}

//=======================================================================
// function : ~GEOM_LogicalFilter
// purpose  : 
//=======================================================================
GEOM_LogicalFilter::~GEOM_LogicalFilter()
{
}

//=======================================================================
// function : isOk
// purpose  : 
//=======================================================================
bool GEOM_LogicalFilter::isOk( const SUIT_DataOwner* owner ) const
{
  GEOM_LogicalFilter* non_const_this = (GEOM_LogicalFilter*)this;
  for ( SUIT_SelectionFilter* filter = non_const_this->myFilters.first(); filter; filter = non_const_this->myFilters.next() )
  {
    if ( myOperation == LO_OR && filter->isOk( owner ) )
      return true;
    if ( myOperation == LO_AND && !filter->isOk( owner ) )
      return false;
    if ( myOperation == LO_NOT )
      return !filter->isOk( owner );
  }

  return ( myOperation != LO_OR );
}

//=======================================================================
// function : setFilters
// purpose  : 
//=======================================================================
void GEOM_LogicalFilter::setFilters( const QPtrList<SUIT_SelectionFilter>& lst )
{
  myFilters = lst;
}

//=======================================================================
// function : setOperation
// purpose  : 
//=======================================================================
void GEOM_LogicalFilter::setOperation( const int op ) 
{
  myOperation = op;
}

//=======================================================================
// function : getFilters
// purpose  : 
//=======================================================================
QPtrList<SUIT_SelectionFilter> GEOM_LogicalFilter::getFilters() const 
{
  return myFilters;
}

//=======================================================================
// function : getOperation
// purpose  : 
//=======================================================================
int GEOM_LogicalFilter::getOperation() const
{
  return myOperation;
}
