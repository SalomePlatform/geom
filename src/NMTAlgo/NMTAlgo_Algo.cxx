// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
// File:	NMTAlgo_Algo.cxx
// Created:	Tue Jan 27 14:58:21 2004
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTAlgo_Algo.ixx>
#include <stdlib.h>
//=======================================================================
// function: NMTAlgo_Algo::NMTAlgo_Algo
// purpose: 
//=======================================================================
  NMTAlgo_Algo::NMTAlgo_Algo()
:
  myDSFiller(NULL),
  myIsDone(Standard_False),
  myIsComputed(Standard_False),
  myErrorStatus(1)
{
  myDraw=0;
  //
  char* xr=getenv("MDISP");
  if (xr!=NULL) {
    if (!strcmp (xr, "yes")) {
      myDraw=1;
    }
  }
}
//=======================================================================
// function: NMTAlgo_Algo
// purpose: 
//=======================================================================
  NMTAlgo_Algo::~NMTAlgo_Algo()
{
  Clear();
}
//=======================================================================
// function: Clear
// purpose: 
//=======================================================================
  void NMTAlgo_Algo::Clear()
{
  myIsDone=Standard_False;
  myIsComputed=Standard_False;
  myErrorStatus=1;
}
//=======================================================================
// function: SetFiller
// purpose: 
//=======================================================================
  void NMTAlgo_Algo::SetFiller(const NMTTools_DSFiller& aDSF)
{
  myDSFiller=(NMTTools_DSFiller *)&aDSF;
}
//=======================================================================
// function: Filler
// purpose: 
//=======================================================================
  const NMTTools_DSFiller& NMTAlgo_Algo::Filler()const
{
  return *myDSFiller;
}
//=======================================================================
// function: ComputeWithFiller
// purpose: 
//=======================================================================
  void NMTAlgo_Algo::ComputeWithFiller(const NMTTools_DSFiller& aDSF)
{
  SetFiller(aDSF);
}
//=======================================================================
// function: Shape
// purpose: 
//=======================================================================
  const TopoDS_Shape& NMTAlgo_Algo::Shape()const
{
  return myShape;
}
//=======================================================================
// function: IsDone
// purpose: 
//=======================================================================
  Standard_Boolean NMTAlgo_Algo::IsDone()const
{
  return myIsDone;
}
//=======================================================================
// function: ErrorStatus
// purpose: 
//=======================================================================
  Standard_Integer NMTAlgo_Algo::ErrorStatus()const
{
  return myErrorStatus;
}
/*
//=======================================================================
// function: Compute
// purpose: 
//=======================================================================
  void NMTAlgo_Algo::Compute()
{
}
//=======================================================================
// function: SetCompositeShape
// purpose: 
//=======================================================================
  void NMTAlgo_Algo::SetCompositeShape(const TopoDS_Shape& aS)
{
  myCompositeShape=aS;
}
//=======================================================================
// function: CompositeShape
// purpose: 
//=======================================================================
  const TopoDS_Shape& NMTAlgo_Algo::CompositeShape()const
{
  return myCompositeShape;
}
*/
