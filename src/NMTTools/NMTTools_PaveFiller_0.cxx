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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:	NMTTools_PaveFiller_1.cxx
// Created:	Mon Dec  8 11:47:55 2003
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <NMTTools_PaveFiller.ixx>

#include <TopAbs_ShapeEnum.hxx>
#include <BOPTools_ListOfCoupleOfInteger.hxx>
#include <NMTDS_ShapesDataStructure.hxx>
#include <BooleanOperations_OnceExplorer.hxx>

//=======================================================================
//function : SortTypes
//purpose  : 
//=======================================================================
  void NMTTools_PaveFiller::SortTypes(Standard_Integer& theWhat,
				      Standard_Integer& theWith)const 
{ 
  Standard_Boolean aReverseFlag=Standard_True;

  TopAbs_ShapeEnum aType1= myDS->GetShapeType(theWhat),
                   aType2= myDS->GetShapeType(theWith);
  
  if (aType1==aType2)
    return;
  
  if (aType1==TopAbs_EDGE && aType2==TopAbs_FACE){
    aReverseFlag=Standard_False;
  }

  if (aType1==TopAbs_VERTEX && 
      (aType2==TopAbs_FACE || aType2==TopAbs_EDGE)) {
    aReverseFlag=Standard_False;
  }
  
  Standard_Integer aWhat, aWith;
  aWhat=(aReverseFlag) ? theWith : theWhat;
  aWith=(aReverseFlag) ? theWhat : theWith;
  
  theWhat=aWhat;
  theWith=aWith;
}
//=======================================================================
// function:  ExpectedPoolLength
// purpose: 
//=======================================================================
  Standard_Integer NMTTools_PaveFiller::ExpectedPoolLength()const
{
  Standard_Integer aNbIIs;
  Standard_Real aCfPredict=.5;
  // Modified  Thu Sep 14 14:35:18 2006 
  // Contribution of Samtech www.samcef.com BEGIN
  //const BOPTools_ListOfCoupleOfInteger& aLC=myDSIt.ListOfCouple();
  //aNbIIs=aLC.Extent();
  aNbIIs=myDSIt.ExpectedLength();
  // Contribution of Samtech www.samcef.com END
  //
  if (aNbIIs==1) {
    return aNbIIs;
  }
  //
  aNbIIs=(Standard_Integer) (aCfPredict*(Standard_Real)aNbIIs);
  
  return aNbIIs;
}
//=======================================================================
// function:IsSuccesstorsComputed
// purpose: 
//=======================================================================
  Standard_Boolean NMTTools_PaveFiller::IsSuccesstorsComputed(const Standard_Integer aN1,
							      const Standard_Integer aN2)const
{
  Standard_Integer nSuc, n1, n2;

  BooleanOperations_OnceExplorer aExp(*myDS);
  TopAbs_ShapeEnum aType=myDS->GetShapeType(aN1);

  n1=aN1;
  n2=aN2;

  if (aType!=TopAbs_VERTEX) {
    Standard_Integer ntmp=n1;
    n1=n2;
    n2=ntmp;
  }

  aType=myDS->GetShapeType(n2);
  if (aType==TopAbs_EDGE) {
    aExp.Init(n2, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      nSuc=aExp.Current();
      if (myIntrPool->IsComputed(n1, nSuc)) {
	return Standard_True;
      }
    }
  return Standard_False;
  }

  else if (aType==TopAbs_FACE) {
    aExp.Init(n2, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      nSuc=aExp.Current();
      if (myIntrPool->IsComputed(n1, nSuc)) {
	return Standard_True;
      }
    }

    aExp.Init(n2, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      nSuc=aExp.Current();
      if (myIntrPool->IsComputed(n1, nSuc)) {
	return Standard_True;
      }
    }
    return Standard_False;
  }

  return Standard_False;
}
