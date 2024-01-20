// Copyright (C) 2013-2024  CEA, EDF, OPEN CASCADE
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

// File:        CurveCreator_Section.cxx
// Author:      Aleksandr BOBKOV

#include "CurveCreator_Section.hxx"

#include <vector>

const double POINT_CONFUSION = 1e-4;

//=======================================================================
// function: GetDifferentPoints
// purpose:
//=======================================================================
Handle(TColgp_HArray1OfPnt) CurveCreator_Section::GetDifferentPoints( int theDimension ) const
{
  Handle(TColgp_HArray1OfPnt) points;

  std::vector<gp_Pnt> aTmpPoints;

  CurveCreator::Coordinates::const_iterator aPIt = myPoints.begin();
  CurveCreator::Coordinates::const_iterator aPItLast = myPoints.end();

  gp_Pnt aFirstPoint;
  gp_Pnt aPoint;

  if( myPoints.size() > 0 )
  {
    aFirstPoint = gp_Pnt(*aPIt, *(aPIt + 1), (theDimension == 2) ? 0 : *(aPIt + 2));
    aPoint = aFirstPoint;
    aTmpPoints.push_back(aPoint);
  }

  for (; aPIt != aPItLast; aPIt += theDimension)
  {
    const gp_Pnt aPoint2(
      *aPIt, *(aPIt + 1), (theDimension == 2) ? 0 : *(aPIt + 2));
    if (!aPoint.IsEqual(aPoint2, POINT_CONFUSION))
    {
      aPoint = aPoint2;
      aTmpPoints.push_back(aPoint);
    }
  }

  int aPointCount = aTmpPoints.size();
  if (myIsClosed)
  {
    while (aPointCount > 1 &&
      aFirstPoint.IsEqual(aTmpPoints[aPointCount - 1], POINT_CONFUSION))
    {
      --aPointCount;
    }
  }

  points = new TColgp_HArray1OfPnt(1, aPointCount);
  for (int aPI = 0; aPI < aPointCount; ++aPI)
  {
    points->SetValue(aPI + 1, aTmpPoints[aPI]);
  }

  return points;
}
