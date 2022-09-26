// Copyright (C) 2013-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include "GEOMImpl_IConformity.hxx"

//=============================================================================
/*!
 *  SetListOfShapesIndices
 *  Input 2D array of indexes converted into 1D array
 */
 //=============================================================================
void GEOMImpl_IConformity::SetListOfShapesIndices(const Handle(TColStd_HArray2OfInteger)& theArrayOfIndexes)
{
  Handle(TColStd_HArray1OfInteger) anArray = new TColStd_HArray1OfInteger(1, theArrayOfIndexes->Size());

  for (Standard_Integer aRow = 1; aRow <= theArrayOfIndexes->NbRows(); ++aRow)
  {
    anArray->SetValue(2 * aRow - 1, theArrayOfIndexes->Value(aRow, 1));
    anArray->SetValue(2 * aRow, theArrayOfIndexes->Value(aRow, 2));
  }

  _func->SetIntegerArray(CHECKCONFORMITY_RET_LIST_OF_INDEXES, anArray);
}

//=============================================================================
/*!
 *  GetListOfShapesIndices
 *  Get cached 1D array of indexes and converted into 2D array of indexes
 */
 //=============================================================================
Handle(TColStd_HArray2OfInteger) GEOMImpl_IConformity::GetListOfShapesIndices()
{
  Handle(TColStd_HArray1OfInteger) anArray = _func->GetIntegerArray(CHECKCONFORMITY_RET_LIST_OF_INDEXES);

  Standard_Integer aLength = anArray->Size() / 2;
  Handle(TColStd_HArray2OfInteger) aResArray = new TColStd_HArray2OfInteger(1, aLength, 1, 2);

  for (Standard_Integer anIndex = 1; anIndex <= aLength; ++anIndex)
  {
    aResArray->SetValue(anIndex, 1, anArray->Value(2 * anIndex - 1));
    aResArray->SetValue(anIndex, 2, anArray->Value(2 * anIndex));
  }

  return aResArray;
}