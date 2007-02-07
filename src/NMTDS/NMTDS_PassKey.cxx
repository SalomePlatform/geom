// Copyright (C) 2006 SAMTECH
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
// File:	NMTDS_PassKey.cxx
// Created:	
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTDS_PassKey.ixx>

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
  NMTDS_PassKey::NMTDS_PassKey()
{
  Clear();
}
//=======================================================================
//function :Assign
//purpose  : 
//=======================================================================
  NMTDS_PassKey& NMTDS_PassKey::Assign(const NMTDS_PassKey& anOther)
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
  void NMTDS_PassKey::Clear()
{
  Standard_Integer i;
  //
  myNbIds=0;
  myNbMax=2;
  mySum=0;
  for (i=0; i<myNbMax; ++i) {
    myIds[i]=0;
  }
}
//=======================================================================
//function :SetIds
//purpose  : 
//=======================================================================
  void NMTDS_PassKey::SetIds(const Standard_Integer anId1,
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
//function :Ids
//purpose  : 
//=======================================================================
  void NMTDS_PassKey::Ids(Standard_Integer& aId1,
			  Standard_Integer& aId2)const
{
  aId1=myIds[0];
  aId2=myIds[1];
}
//=======================================================================
//function :NbMax
//purpose  : 
//=======================================================================
  Standard_Integer NMTDS_PassKey::NbMax()const
{
  return myNbMax;
}
//=======================================================================
//function :Compute
//purpose  : 
//=======================================================================
  void NMTDS_PassKey::Compute()
{
  SortShell(myNbIds, myIds+myNbMax-myNbIds);
}
//=======================================================================
//function :IsEqual
//purpose  : 
//=======================================================================
  Standard_Boolean NMTDS_PassKey::IsEqual(const NMTDS_PassKey& anOther) const
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
  Standard_Address NMTDS_PassKey::Key()const
{
  return (Standard_Address)myIds;
}
//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================
  Standard_Integer NMTDS_PassKey::HashCode(const Standard_Integer Upper) const
{
  return ::HashCode(mySum, Upper);
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
  void NMTDS_PassKey::Dump()const
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
