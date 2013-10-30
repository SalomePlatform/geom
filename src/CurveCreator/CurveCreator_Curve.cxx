// Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File:        CurveCreator_Curve.cxx
// Author:      Sergey KHROMOV

#include "CurveCreator_Curve.hxx"
#include "CurveCreator_Section.hxx"
#include "CurveCreator_Listener.hxx"

#include <stdio.h>

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
CurveCreator_Curve::CurveCreator_Curve
                (const CurveCreator::Dimension theDimension)
: CurveCreator_ICurve(theDimension)
{
}

//=======================================================================
// function: addPoints
// purpose:
//=======================================================================
void CurveCreator_Curve::addPoints
   (const CurveCreator::Coordinates &thePoints, const int theISection)
{
  CurveCreator_Section *aSection =
    (theISection == -1 ? mySections.back() : mySections.at(theISection));

  aSection->myPoints.insert(aSection->myPoints.end(),
                            thePoints.begin(), thePoints.end());
  if( myListener )
    myListener->pointInserted( theISection, -1 );
}
