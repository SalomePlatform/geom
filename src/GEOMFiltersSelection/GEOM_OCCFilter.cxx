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

#include "GEOM_OCCFilter.h"

#include <LightApp_SelectionMgr.h> 
#include <LightApp_DataOwner.h>

#include <SALOME_InteractiveObject.hxx>

#include <AIS_InteractiveObject.hxx>
#include <SelectMgr_EntityOwner.hxx>

OCCT_IMPLEMENT_STANDARD_RTTIEXT(GEOM_OCCFilter, SelectMgr_Filter)

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
