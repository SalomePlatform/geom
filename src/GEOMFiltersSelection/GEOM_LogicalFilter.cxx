//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
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
//  File   : GEOM_LogicalFilter.cxx
//  Author : Sergey LITONIN
//  Module : GEOM

#include "GEOM_LogicalFilter.hxx"

/*
  Class       : GEOM_LogicalFilter
  Description : Filter for combaining several filters with logical operation (OR or AND)
*/

IMPLEMENT_STANDARD_HANDLE( GEOM_LogicalFilter, SALOME_Filter )
IMPLEMENT_STANDARD_RTTIEXT( GEOM_LogicalFilter, SALOME_Filter )

//=======================================================================
// name    : GEOM_LogicalFilter::GEOM_LogicalFilter
// Purpose : Constructor
//=======================================================================
GEOM_LogicalFilter::GEOM_LogicalFilter( const GEOM_ListOfFilter& theFilters,
                                        const int                theLogOp )
{
  myFilters = theFilters;
  myLogOp = theLogOp;
}

//=======================================================================
// name    : GEOM_LogicalFilter::GEOM_LogicalFilter
// Purpose : Constructor
//=======================================================================
GEOM_LogicalFilter::GEOM_LogicalFilter( const Handle(SALOME_Filter)& theFilter,
                                        const int                    theLogOp )
{
  myFilters.Append( theFilter );
  myLogOp = theLogOp;
}

//=======================================================================
// name    : GEOM_LogicalFilter::~GEOM_LogicalFilter
// Purpose : Destructor
//=======================================================================
GEOM_LogicalFilter::~GEOM_LogicalFilter()
{
}

//=======================================================================
// name    : GEOM_LogicalFilter::IsOk
// Purpose : Verify validity of entry object
//=======================================================================
Standard_Boolean GEOM_LogicalFilter::IsOk( const Handle(SALOME_InteractiveObject)& theIO ) const
{
  GEOM_ListOfFilter::Iterator anIter( myFilters );
  for ( ; anIter.More(); anIter.Next() )
  {
    Handle(SALOME_Filter) aFilter = anIter.Value();
    if ( !aFilter.IsNull() )
    {
      if ( myLogOp == LO_OR && anIter.Value()->IsOk( theIO ) )
        return true;
      if ( myLogOp == LO_AND && !anIter.Value()->IsOk( theIO ) )
        return false;
      if ( myLogOp == LO_NOT )
        return !anIter.Value()->IsOk( theIO );
    }
  }

  return myLogOp == LO_OR ? false : true;
}

//=======================================================================
// name    : GEOM_LogicalFilter::SetFilters
// Purpose : Set new list of filters. Old wilters are removed
//=======================================================================
void GEOM_LogicalFilter::SetFilters( const GEOM_ListOfFilter& theFilters )
{
  myFilters = theFilters;
}

//=======================================================================
// name    : GEOM_LogicalFilter::SetLogOp
// Purpose : Set logical operation
//=======================================================================
void GEOM_LogicalFilter::SetLogOp( const int theLogOp )
{
  myLogOp = theLogOp;
}

//=======================================================================
// name    : GEOM_LogicalFilter::GetFilters
// Purpose : Get list of filters
//=======================================================================
const GEOM_ListOfFilter& GEOM_LogicalFilter::GetFilters() const
{
  return myFilters;
}

//=======================================================================
// name    : GEOM_LogicalFilter::GetLogOp
// Purpose : Get logical operation
//=======================================================================
int GEOM_LogicalFilter::GetLogOp() const
{
  return myLogOp;
}



































