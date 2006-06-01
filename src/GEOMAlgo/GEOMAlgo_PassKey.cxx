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
// File:	GEOMAlgo_PassKey.cxx
// Created:	
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <GEOMAlgo_PassKey.ixx>

#include <stdio.h>
#include <string.h>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#ifdef WNT
#pragma warning( disable : 4101) 
#endif

static 
  void SortShell(const int n, int* a); 
static
  Standard_Integer NormalizedId(const Standard_Integer aId,
				const Standard_Integer aDiv);

//=======================================================================
//function :
//purpose  : 
//=======================================================================
  GEOMAlgo_PassKey::GEOMAlgo_PassKey()
{
  Clear();
}
//=======================================================================
//function :Assign
//purpose  : 
//=======================================================================
  GEOMAlgo_PassKey& GEOMAlgo_PassKey::Assign(const GEOMAlgo_PassKey& anOther)
{
  myNbIds=anOther.myNbIds;
  myNbMax=anOther.myNbMax;
  mySum=anOther.mySum;
  memcpy(myIds, anOther.myIds, sizeof(myIds));
  return *this;
}
//=======================================================================
//function :Clear
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::Clear()
{
  Standard_Integer i;
  //
  myNbIds=0;
  myNbMax=8;
  mySum=0;
  for (i=0; i<myNbMax; ++i) {
    myIds[i]=0;
  }
}
//=======================================================================
//function :SetIds
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::SetIds(const Standard_Integer anId1)
			       
{
  myNbIds=1;
  myIds[myNbMax-1]=anId1;
  mySum=anId1;
}
//=======================================================================
//function :SetIds
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::SetIds(const Standard_Integer anId1,
				const Standard_Integer anId2)
{
  Standard_Integer aIdN1, aIdN2;
  //
  myNbIds=2;
  aIdN1=NormalizedId(anId1, myNbIds);
  aIdN2=NormalizedId(anId2, myNbIds);
  mySum=aIdN1+aIdN2;
  //
  if (anId1<anId2) {
    myIds[myNbMax-2]=anId1;
    myIds[myNbMax-1]=anId2;
    return;
  }
  myIds[myNbMax-2]=anId2;
  myIds[myNbMax-1]=anId1;
}

//=======================================================================
//function :SetIds
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::SetIds(const Standard_Integer anId1,
				const Standard_Integer anId2,
				const Standard_Integer anId3)
{
  Standard_Integer aIdN1, aIdN2, aIdN3;
  //
  myNbIds=3;
  aIdN1=NormalizedId(anId1, myNbIds);
  aIdN2=NormalizedId(anId2, myNbIds);
  aIdN3=NormalizedId(anId3, myNbIds);
  mySum=aIdN1+aIdN2+aIdN3;
  //
  myIds[myNbMax-3]=anId1;
  myIds[myNbMax-2]=anId2;
  myIds[myNbMax-1]=anId3;
  //
  Compute();
}
//=======================================================================
//function :SetIds
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::SetIds(const Standard_Integer anId1,
				const Standard_Integer anId2,
				const Standard_Integer anId3,
				const Standard_Integer anId4)
{
  Standard_Integer aIdN1, aIdN2, aIdN3, aIdN4;
  //
  myNbIds=4;
  aIdN1=NormalizedId(anId1, myNbIds);
  aIdN2=NormalizedId(anId2, myNbIds);
  aIdN3=NormalizedId(anId3, myNbIds);
  aIdN4=NormalizedId(anId4, myNbIds);
  mySum=aIdN1+aIdN2+aIdN3+aIdN4;
  //
  myIds[myNbMax-4]=anId1;
  myIds[myNbMax-3]=anId2;
  myIds[myNbMax-2]=anId3;
  myIds[myNbMax-1]=anId4;
  //
  Compute();
}
//=======================================================================
//function :SetIds
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::SetIds(const TColStd_ListOfInteger& aLI)
{
  Standard_Integer aNb, i, anId, aIdN;
  TColStd_ListIteratorOfListOfInteger aIt;
  //
  aNb=aLI.Extent();
  if (!aNb || aNb > myNbMax) {
    return;
  }
  //
  myNbIds=aNb;
  mySum=0;
  i=myNbMax-myNbIds;
  aIt.Initialize(aLI);
  for (; aIt.More(); aIt.Next(), ++i) {
    anId=aIt.Value();
    myIds[i]=anId;
    aIdN=NormalizedId(anId, myNbIds);
    mySum+=aIdN;
  }
  //
  Compute();
}
//=======================================================================
//function :Id
//purpose  : 
//=======================================================================
  Standard_Integer GEOMAlgo_PassKey::Id(const Standard_Integer aIndex)const
{
  if (aIndex < 0 || aIndex >= myNbMax) {
    return 0;
  }
  return myIds[aIndex];
}
//=======================================================================
//function :NbMax
//purpose  : 
//=======================================================================
  Standard_Integer GEOMAlgo_PassKey::NbMax()const
{
  return myNbMax;
}
//=======================================================================
//function :Compute
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::Compute()
{
  SortShell(myNbIds, myIds+myNbMax-myNbIds);
}
//=======================================================================
//function :IsEqual
//purpose  : 
//=======================================================================
  Standard_Boolean GEOMAlgo_PassKey::IsEqual(const GEOMAlgo_PassKey& anOther) const
{
  Standard_Integer iIsEqual;
  Standard_Boolean bIsEqual;
  //
  iIsEqual=memcmp(myIds, anOther.myIds, sizeof(myIds));
  bIsEqual=Standard_False;
  if (!iIsEqual) {
    bIsEqual=!bIsEqual;
  }
  return bIsEqual;
}
//=======================================================================
//function :Key
//purpose  : 
//=======================================================================
  Standard_Address GEOMAlgo_PassKey::Key()const
{
  return (Standard_Address)myIds;
}
//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================
  Standard_Integer GEOMAlgo_PassKey::HashCode(const Standard_Integer Upper) const
{
  //return (mySum % Upper);
  return ::HashCode(mySum, Upper);
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::Dump()const
{
  Standard_Integer i;
  //
  printf(" PassKey: {");
  for (i=0; i<myNbMax; ++i) {
    printf(" %d", myIds[i]);
  }
  printf(" }");
}
//=======================================================================
// function: NormalizedId
// purpose : 
//=======================================================================
Standard_Integer NormalizedId(const Standard_Integer aId,
			      const Standard_Integer aDiv)
{
  Standard_Integer aMax, aTresh, aIdRet;
  //
  aIdRet=aId;
  aMax=::IntegerLast();
  aTresh=aMax/aDiv;
  if (aId>aTresh) {
    aIdRet=aId%aTresh;
  }
  return aIdRet;
}
//=======================================================================
// function: SortShell
// purpose : 
//=======================================================================
void SortShell(const int n, int* a) 
{
  int  x, nd, i, j, l, d=1;
  //
  while(d<=n) {
    d*=2;
  }
  //
  while (d) {
    d=(d-1)/2;
    //
    nd=n-d;
    for (i=0; i<nd; ++i) {
      j=i;
    m30:;
      l=j+d;
      if (a[l] < a[j]){
	x=a[j];
	a[j]=a[l];
	a[l]=x;
	j-=d;
	if (j > -1) goto m30;
      }//if (a[l] < a[j]){
    }//for (i=0; i<nd; ++i) 
  }//while (1)
}
