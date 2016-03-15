// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:        CurveCreator_PosPoint.hxx
// Author:      Alexander KOVALEV

#ifndef _CurveCreator_PosPoint_HeaderFile
#define _CurveCreator_PosPoint_HeaderFile

#include "CurveCreator.hxx"

struct CurveCreator_PosPoint
{
public:
  CurveCreator_PosPoint( int theID, CurveCreator::Coordinates theCoords ) 
    : myID( theID ), myCoords( theCoords )
  { };

  ////! Overloaded operator to use sorting.
  //bool operator < (CurveCreator_PosPoint const & thePosPoint) const 
  //{
  //  return myID < thePosPoint.myID;
  //}

  int                       myID;     // point ID
  CurveCreator::Coordinates myCoords; // point coordinates

};

#endif
