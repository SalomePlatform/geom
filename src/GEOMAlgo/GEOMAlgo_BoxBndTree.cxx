// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

// File:        GEOMAlgo_BoxBndTree.cxx
// Created:     Tue Oct 17 13:04:11 2006
// Author:      Peter KURNEV
//              <pkv@irinox>
//
#include <GEOMAlgo_BoxBndTree.hxx>
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  GEOMAlgo_BoxBndTreeSelector::GEOMAlgo_BoxBndTreeSelector()
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  GEOMAlgo_BoxBndTreeSelector::~GEOMAlgo_BoxBndTreeSelector()
{
}
//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================
  Standard_Boolean GEOMAlgo_BoxBndTreeSelector::Reject (const Bnd_Box& aBox) const
{
  Standard_Boolean bRet;
  //
  bRet=myBox.IsOut(aBox);
  return bRet;
}
//=======================================================================
//function : Accept
//purpose  : 
//=======================================================================
  Standard_Boolean GEOMAlgo_BoxBndTreeSelector::Accept (const Standard_Integer& aIndex)
{
  Standard_Boolean bRet=Standard_False;
  //
  if (myFence.Add(aIndex)) {
    myIndices.Append(aIndex);
    bRet=!bRet;
  }
  return bRet;
}
//=======================================================================
//function : SetBox
//purpose  : 
//=======================================================================
  void GEOMAlgo_BoxBndTreeSelector::SetBox(const Bnd_Box& aBox)
{
  myBox=aBox;
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
  void GEOMAlgo_BoxBndTreeSelector::Clear()
{
  myFence.Clear();
  myIndices.Clear();
}
//=======================================================================
//function : Indices
//purpose  : 
//=======================================================================
  const TColStd_ListOfInteger& GEOMAlgo_BoxBndTreeSelector::Indices() const
{
  return myIndices;
}
