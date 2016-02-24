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

//  GEOM OBJECT : interactive object for Geometry entities visualization
//  File   : GEOM_AISDimension.cxx
//  Module : GEOM

#include "GEOM_AISDimension.hxx"

OCCT_IMPLEMENT_STANDARD_RTTIEXT(GEOM_AISLength,   AIS_LengthDimension)
OCCT_IMPLEMENT_STANDARD_RTTIEXT(GEOM_AISDiameter, AIS_DiameterDimension)
OCCT_IMPLEMENT_STANDARD_RTTIEXT(GEOM_AISAngle,    AIS_AngleDimension)

/* ****************************************************************************** *
 *                                                                                *
 *                                GEOM_AISLength                                  *
 *                                                                                *
 * ****************************************************************************** */

//=================================================================================
// function : Constructor
// purpose  : 
//=================================================================================
GEOM_AISLength::GEOM_AISLength( const Standard_Integer theId ) 
: AIS_LengthDimension( gp::Origin(),
                       gp::Origin(),
                       gp_Pln( gp::XOY() ) ),
  myId( theId )
{
}

//=================================================================================
// function : GetId
// purpose  : 
//=================================================================================
Standard_Integer GEOM_AISLength::GetId() const
{
  return myId;
}

//=================================================================================
// function : SetId
// purpose  : 
//=================================================================================
void GEOM_AISLength::SetId( const Standard_Integer theId )
{
  myId = theId;
}


/* ****************************************************************************** *
 *                                                                                *
 *                              GEOM_AISDiameter                                  *
 *                                                                                *
 * ****************************************************************************** */

//=================================================================================
// function : Constructor
// purpose  : 
//=================================================================================
GEOM_AISDiameter::GEOM_AISDiameter( const Standard_Integer theId )
: AIS_DiameterDimension( gp_Circ( gp_Ax2( gp::Origin(), gp::DZ() ), 0.0 ) ),
  myId( theId )
{
}

//=================================================================================
// function : GetId
// purpose  : 
//=================================================================================
Standard_Integer GEOM_AISDiameter::GetId() const
{
  return myId;
}

//=================================================================================
// function : SetId
// purpose  : 
//=================================================================================
void GEOM_AISDiameter::SetId( const Standard_Integer theId )
{
  myId = theId;
}

/* ****************************************************************************** *
 *                                                                                *
 *                              GEOM_AISAngle                                     *
 *                                                                                *
 * ****************************************************************************** */

//=================================================================================
// function : Constructor
// purpose  : 
//=================================================================================
GEOM_AISAngle::GEOM_AISAngle( const Standard_Integer theId ) 
: AIS_AngleDimension( gp::Origin(),
                      gp::Origin(),
                      gp::Origin() ),
  myId( theId )
{
}

//=================================================================================
// function : GetId
// purpose  : 
//=================================================================================
Standard_Integer GEOM_AISAngle::GetId() const
{
  return myId;
}

//=================================================================================
// function : SetId
// purpose  : 
//=================================================================================
void GEOM_AISAngle::SetId( const Standard_Integer theId )
{
  myId = theId;
}
