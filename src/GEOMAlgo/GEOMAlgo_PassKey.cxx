// File:	GEOMAlgo_PassKey.cxx
// Created:	
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <GEOMAlgo_PassKey.ixx>

#include <stdio.h>
#include <string.h>

#include <TopTools_ListIteratorOfListOfShape.hxx>

#ifdef WNT
#pragma warning( disable : 4101) 
#endif

static 
  void SortShell(const int n, int* a); 

//=======================================================================
//function :
//purpose  : 
//=======================================================================
  GEOMAlgo_PassKey::GEOMAlgo_PassKey()
{
  Clear();
}
//=======================================================================
//function :Clear
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::Clear()
{
  myNbIds=0;
  myNbMax=8;
  mySum=0;
  myIds[0]=0;  myIds[1]=0;  myIds[2]=0;  myIds[3]=0;
  myIds[4]=0;  myIds[5]=0;  myIds[6]=0;  myIds[7]=0;
  myUpper=432123;//2147483647;
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
//function :SetIds
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::SetIds(const TopoDS_Shape& aS1)
			       
{
  Standard_Integer anId1;
  //
  anId1=aS1.HashCode(myUpper);
  //
  myNbIds=1;
  myIds[7]=anId1;
  mySum=anId1;
}
//=======================================================================
//function :SetIds
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::SetIds(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2)
{
  Standard_Integer anId1, anId2;
  //
  anId1=aS1.HashCode(myUpper);
  anId2=aS2.HashCode(myUpper);
  //
  myNbIds=2;
  mySum=anId1+anId2;
  if (anId1<anId2) {
    myIds[6]=anId1;
    myIds[7]=anId2;
    return;
  }
  myIds[6]=anId2;
  myIds[7]=anId1;
}
//=======================================================================
//function :SetIds
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::SetIds(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2,
				const TopoDS_Shape& aS3)
{
  Standard_Integer anId1, anId2, anId3;
  //
  anId1=aS1.HashCode(myUpper);
  anId2=aS2.HashCode(myUpper);
  anId3=aS3.HashCode(myUpper);
  //
  myNbIds=3;
  myIds[5]=anId1;
  myIds[6]=anId2;
  myIds[7]=anId3;
  mySum=anId1+anId2+anId3;
  Compute();
}
//=======================================================================
//function :SetIds
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::SetIds(const TopoDS_Shape& aS1,
				const TopoDS_Shape& aS2,
				const TopoDS_Shape& aS3,
				const TopoDS_Shape& aS4)
{
  Standard_Integer anId1, anId2, anId3, anId4;
  //
  anId1=aS1.HashCode(myUpper);
  anId2=aS2.HashCode(myUpper);
  anId3=aS3.HashCode(myUpper);
  anId4=aS4.HashCode(myUpper);
  //
  myNbIds=4;
  myIds[4]=anId1;
  myIds[5]=anId2;
  myIds[6]=anId3;
  myIds[7]=anId4;
  mySum=anId1+anId2+anId3+anId4;
  Compute();
}
//=======================================================================
//function :SetIds
//purpose  : 
//=======================================================================
  void GEOMAlgo_PassKey::SetIds(const TopTools_ListOfShape& aLS)
{
  Standard_Integer aNb, i, anId;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  aNb=aLS.Extent();
  if (aNb<1 || aNb > myNbMax) {
    return;
  }
  //
  myNbIds=aNb;
  mySum=0;
  i=myNbMax-myNbIds;
  aIt.Initialize(aLS);
  for (; aIt.More(); aIt.Next(), ++i) {
    const TopoDS_Shape& aS=aIt.Value();
    anId=aS.HashCode(myUpper);
    myIds[i]=anId;
    mySum+=anId;
  }
  //
  Compute();
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
  return (mySum % Upper);
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
