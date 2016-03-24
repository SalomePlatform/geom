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


#include "GEOMImpl_IPolyline2D.hxx"

#include <TColStd_HArray1OfInteger.hxx>


//=============================================================================
/*!
 *  SetCoords
 */
//=============================================================================
void GEOMImpl_IPolyline2D::SetCoords
                  (const std::list <std::list <double> > &theValue)
{
  const Standard_Integer aNbSec = theValue.size();

  if (aNbSec > 0) {
    // Compute the total number of points and fill the array of start indices.
    Standard_Integer                                i;
    Standard_Integer                                aNbCoords = 0;
    Handle(TColStd_HArray1OfInteger)                anIndices =
      new TColStd_HArray1OfInteger(1, aNbSec);
    Handle(TColStd_HArray1OfReal)                   aCoords;
    std::list <std::list <double> >::const_iterator aSecIter = theValue.begin();

    for (i = 1; aSecIter != theValue.end(); ++aSecIter, ++i) {
      anIndices->SetValue(i, aNbCoords + 1);
      aNbCoords += aSecIter->size();
    }

    if (aNbCoords > 0) {
      // Fill the array of coordinates.
      std::list<double>::const_iterator aCIter;

      aCoords  = new TColStd_HArray1OfReal(1, aNbCoords);
      aSecIter = theValue.begin();

      for (i = 1; aSecIter != theValue.end(); ++aSecIter) {
        for (aCIter = aSecIter->begin(); aCIter != aSecIter->end(); ++aCIter) {
          aCoords->SetValue(i++, *aCIter);
        }
      }
    }

    // Store the coordinates.
    if (aCoords.IsNull() == Standard_False) {
      _func->SetRealArray(POLY_ARG_COORDS, aCoords);
    }

    _func->SetIntegerArray(POLY_ARG_START_INDICES, anIndices);
  }
}

//=============================================================================
/*!
 *  GetCoords
 */
//=============================================================================
void GEOMImpl_IPolyline2D::GetCoords(std::list <std::list <double> > &theValue)
{
  theValue.clear();

  Handle(TColStd_HArray1OfReal)    aCoords   =
    _func->GetRealArray(POLY_ARG_COORDS);
  Handle(TColStd_HArray1OfInteger) anIndices =
    _func->GetIntegerArray(POLY_ARG_START_INDICES);

  if (anIndices.IsNull() == Standard_False) {
    const Standard_Integer aNbSec = anIndices->Length();

    // Create an empty sections.
    theValue.resize(aNbSec);

    if (aCoords.IsNull() == Standard_False) {
      Standard_Integer i;
      Standard_Integer j;
      std::list <std::list <double> >::iterator anIt = theValue.begin();

      for (i = anIndices->Lower(); i <= anIndices->Upper(); ++i, ++anIt) {
        const Standard_Integer iCoord1 = anIndices->Value(i);
        const Standard_Integer iCoord2 = i + 1 > anIndices->Upper() ?
          aCoords->Upper() + 1 : anIndices->Value(i + 1);

        for (j = iCoord1; j < iCoord2; ++j) {
          anIt->push_back(aCoords->Value(j));
        }
      }
    }
  }
}
