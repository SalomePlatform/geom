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
// File   : MeasureGUI_DimensionFilter.cxx
// Author : Anton POLETAEV, Open CASCADE S.A.S.

#include "MeasureGUI_DimensionFilter.h"

// GEOM includes
#include <GEOM_AISDimension.hxx>

// SALOME includes
#include <SALOME_InteractiveObject.hxx>

OCCT_IMPLEMENT_STANDARD_RTTIEXT(MeasureGUI_DimensionFilter, SelectMgr_Filter)

//=======================================================================
// function : Constructor
// purpose  : 
//=======================================================================
MeasureGUI_DimensionFilter::MeasureGUI_DimensionFilter( const Handle(AIS_InteractiveObject)& theIO )
: SelectMgr_Filter(), 
  myFilterIO( theIO ),
  myFilterEntry(),
  myFilterId( 0 ),
  myIsFilterIO( Standard_True ),
  myIsFilterEntry( Standard_False ),
  myIsFilterId( Standard_False )
{
}

//=======================================================================
// function : Constructor
// purpose  : 
//=======================================================================
MeasureGUI_DimensionFilter::MeasureGUI_DimensionFilter( const int theId )
: SelectMgr_Filter(), 
  myFilterIO(),
  myFilterEntry(),
  myFilterId( theId ),
  myIsFilterIO( Standard_False ),
  myIsFilterEntry( Standard_False ),
  myIsFilterId( Standard_True )
{
}

//=======================================================================
// function : Constructor
// purpose  : 
//=======================================================================
MeasureGUI_DimensionFilter::MeasureGUI_DimensionFilter( const std::string& theEntry )
: SelectMgr_Filter(),
  myFilterIO( NULL ),
  myFilterEntry( theEntry ),
  myFilterId( 0 ),
  myIsFilterIO( Standard_False ),
  myIsFilterEntry( Standard_True ),
  myIsFilterId( Standard_False )
{
}

//=======================================================================
// function : Constructor
// purpose  : 
//=======================================================================
MeasureGUI_DimensionFilter::MeasureGUI_DimensionFilter( const std::string& theEntry, const int theId )
: SelectMgr_Filter(),
  myFilterIO( NULL ),
  myFilterEntry( theEntry ),
  myFilterId( theId ),
  myIsFilterIO( Standard_False ),
  myIsFilterEntry( Standard_True ),
  myIsFilterId( Standard_True )
{
}

//=======================================================================
// function : SetFilterIO
// purpose  : 
//=======================================================================
void MeasureGUI_DimensionFilter::SetFilterIO( const Handle(AIS_InteractiveObject)& theIO )
{
  myFilterIO = theIO;
  myIsFilterIO = Standard_True;
}

//=======================================================================
// function : SetFilterEntry
// purpose  : 
//=======================================================================
void MeasureGUI_DimensionFilter::SetFilterEntry( const std::string& theEntry )
{
  myFilterEntry = theEntry;
  myIsFilterEntry = Standard_True;
}

//=======================================================================
// function : SetFilterId
// purpose  : 
//=======================================================================
void MeasureGUI_DimensionFilter::SetFilterId( const int theId )
{
  myFilterId = theId;
  myIsFilterId = Standard_True;
}

//=======================================================================
// function : IsOk
// purpose  : 
//=======================================================================
Standard_Boolean MeasureGUI_DimensionFilter::IsOk( const Handle(SelectMgr_EntityOwner)& theEntity ) const
{
  Handle(AIS_InteractiveObject) anAIS = Handle(AIS_InteractiveObject)::DownCast( theEntity->Selectable() );

  if ( anAIS.IsNull() || !anAIS->HasOwner() )
  {
    return Standard_False;
  }
  
  if ( !anAIS->IsKind( STANDARD_TYPE( AIS_LengthDimension ) )
    && !anAIS->IsKind( STANDARD_TYPE( AIS_DiameterDimension ) )
    && !anAIS->IsKind( STANDARD_TYPE( AIS_AngleDimension ) ) )
  {
    return Standard_False;
  }

  if ( myIsFilterIO )
  {
    if ( anAIS != myFilterIO )
    {
      return Standard_False;
    }
  }

  if ( myIsFilterEntry )
  {
    Handle(SALOME_InteractiveObject) anIO = Handle(SALOME_InteractiveObject)::DownCast( anAIS->GetOwner() );
    if ( anIO.IsNull() )
    {
      return Standard_False;
    }

    if ( std::string( anIO->getEntry() ) != myFilterEntry )
    {
      return Standard_False;
    }
  }

  if ( myIsFilterId )
  {
    Handle(GEOM_AISLength) aLength = Handle(GEOM_AISLength)::DownCast( anAIS );
    if ( !aLength.IsNull() && aLength->GetId() != myFilterId )
    {
      return Standard_False;
    }

    Handle(GEOM_AISDiameter) aDiameter = Handle(GEOM_AISDiameter)::DownCast( anAIS );
    if ( !aDiameter.IsNull() && aDiameter->GetId() != myFilterId )
    {
      return Standard_False;
    }

    Handle(GEOM_AISAngle) anAngle = Handle(GEOM_AISAngle)::DownCast( anAIS );
    if ( !anAngle.IsNull() && anAngle->GetId() != myFilterId )
    {
      return Standard_False;
    }
  }

  return Standard_True;
}
