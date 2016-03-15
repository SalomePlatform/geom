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

#include "GEOM_TypeFilter.h"

//=======================================================================
// function : GEOM_TypeFilter
// purpose  : 
//=======================================================================
GEOM_TypeFilter::GEOM_TypeFilter( SalomeApp_Study* study, const int type, const bool isShapeType )
:GEOM_SelectionFilter( study ),
 myType( type ),
 myIsShapeType(isShapeType)
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
    {
      if (!myIsShapeType)
        return obj->GetType() == type();
      else
        return obj->GetShapeType() == type();
    }
  
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
